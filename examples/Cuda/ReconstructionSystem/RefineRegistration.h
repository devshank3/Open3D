//
// Created by wei on 2/4/19.
//

#include <Cuda/Open3DCuda.h>
#include <Open3D/Open3D.h>
#include <string>
#include <vector>

#include "DatasetConfig.h"

using namespace open3d;
using namespace open3d::utility;
using namespace open3d::io;
using namespace open3d::geometry;
using namespace open3d::registration;

namespace RefineRegistration {

std::tuple<Eigen::Matrix4d, Eigen::Matrix6d> MultiScaleICP(
        const PointCloud &source,
        const PointCloud &target,
        const Eigen::Matrix4d &init_trans,
        const float voxel_size,
        const std::vector<int> &iters = {50, 30, 14},
        const std::vector<float> &voxel_factors = {1.0, 2.0, 4.0}) {
    assert(iters.size() == voxel_factors.size());

    Eigen::Matrix4d transformation = init_trans;
    Eigen::Matrix6d information = Eigen::Matrix6d::Identity();

    for (int i = 0; i < iters.size(); ++i) {
        float voxel_size_level = voxel_size / voxel_factors[i];
        auto source_down = source.VoxelDownSample(voxel_size_level);
        auto target_down = target.VoxelDownSample(voxel_size_level);

        cuda::RegistrationCuda registration(
                TransformationEstimationType::ColoredICP);
        registration.Initialize(*source_down, *target_down,
                                voxel_size_level * 1.4f, transformation);
        registration.ComputeICP(iters[i]);

        transformation = registration.transform_source_to_target_;

        if (i == iters.size() - 1) {
            information = registration.ComputeInformationMatrix();
        }
    }

    return std::make_tuple(transformation, information);
}

std::vector<Match> MatchFragments(DatasetConfig &config) {
    PoseGraph pose_graph;
    ReadPoseGraph(config.GetPoseGraphFileForScene(true), pose_graph);

    std::vector<Match> matches;

    for (auto &edge : pose_graph.edges_) {
        Match match;
        match.s = edge.source_node_id_;
        match.t = edge.target_node_id_;
        match.success = true;

        auto source = CreatePointCloudFromFile(config.fragment_files_[match.s]);
        auto target = CreatePointCloudFromFile(config.fragment_files_[match.t]);

        std::tie(match.trans_source_to_target, match.information) =
                MultiScaleICP(*source, *target, edge.transformation_,
                              config.voxel_size_);

        LogInfo("Point cloud odometry ({} {})\n", match.s, match.t);

        matches.push_back(match);
    }

    return matches;
}

void MakePoseGraphForRefinedScene(const std::vector<Match> &matches,
                                  DatasetConfig &config) {
    PoseGraph pose_graph;

    /* world_to_frag0 */
    Eigen::Matrix4d trans_odometry = Eigen::Matrix4d::Identity();
    pose_graph.nodes_.emplace_back(registration::PoseGraphNode(trans_odometry));

    for (auto &match : matches) {
        if (!match.success) continue;
        if (match.t == match.s + 1) {
            /* world_to_fragi */
            trans_odometry = match.trans_source_to_target * trans_odometry;
            auto trans_odometry_inv = trans_odometry.inverse();

            pose_graph.nodes_.emplace_back(PoseGraphNode(trans_odometry_inv));
            pose_graph.edges_.emplace_back(PoseGraphEdge(
                    match.s, match.t, match.trans_source_to_target,
                    match.information, false));
        } else {
            pose_graph.edges_.emplace_back(PoseGraphEdge(
                    match.s, match.t, match.trans_source_to_target,
                    match.information, true));
        }
    }

    WritePoseGraph(config.GetPoseGraphFileForRefinedScene(false), pose_graph);
}

void OptimizePoseGraphForScene(DatasetConfig &config) {
    PoseGraph pose_graph;
    ReadPoseGraph(config.GetPoseGraphFileForRefinedScene(false), pose_graph);

    GlobalOptimizationConvergenceCriteria criteria;
    GlobalOptimizationOption option(
            config.voxel_size_ * 1.4, 0.25,
            config.preference_loop_closure_registration_, 0);
    GlobalOptimizationLevenbergMarquardt optimization_method;
    GlobalOptimization(pose_graph, optimization_method, criteria, option);

    auto pose_graph_prunned =
            CreatePoseGraphWithoutInvalidEdges(pose_graph, option);

    WritePoseGraph(config.GetPoseGraphFileForRefinedScene(true),
                   *pose_graph_prunned);
}

int Run(DatasetConfig &config) {
    Timer timer;
    timer.Start();

    filesystem::MakeDirectory(config.path_dataset_ + "/scene");

    bool is_success = config.GetFragmentFiles();
    if (!is_success) {
        LogError("Unable to get fragment files\n");
        return -1;
    }

    auto matches = MatchFragments(config);
    MakePoseGraphForRefinedScene(matches, config);
    OptimizePoseGraphForScene(config);

    timer.Stop();
    LogInfo("RefineRegistration takes {} s\n", timer.GetDuration() * 1e-3);
    return 0;
}
}  // namespace RefineRegistration
