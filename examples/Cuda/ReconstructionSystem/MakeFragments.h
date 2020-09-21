//
// Created by wei on 2/4/19.
// Edited by Shank for Jetscan.
//

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <Cuda/Open3DCuda.h>
#include <Open3D/Open3D.h>
#include <Open3D/Registration/GlobalOptimization.h>

#include "DatasetConfig.h"
#include "ORBPoseEstimation.h"

using namespace open3d;
using namespace open3d::utility;
using namespace open3d::io;
using namespace open3d::odometry;
using namespace open3d::registration;
using namespace open3d::geometry;

namespace MakeFragment {
void MakePoseGraphForFragment(int fragment_id, DatasetConfig &config) {
    cuda::RGBDOdometryCuda<3> odometry;
    odometry.SetIntrinsics(config.intrinsic_);
    odometry.SetParameters(OdometryOption({20, 10, 5}, config.max_depth_diff_,
                                          config.min_depth_, config.max_depth_),
                           0.968f);

    cuda::RGBDImageCuda rgbd_source((float)config.max_depth_,
                                    (float)config.depth_factor_);
    cuda::RGBDImageCuda rgbd_target((float)config.max_depth_,
                                    (float)config.depth_factor_);

    const int begin = fragment_id * config.n_frames_per_fragment_;
    const int end = std::min((fragment_id + 1) * config.n_frames_per_fragment_,
                             (int)config.color_files_.size());

    // world_to_source
    Eigen::Matrix4d trans_odometry = Eigen::Matrix4d::Identity();
    PoseGraph pose_graph;
    pose_graph.nodes_.emplace_back(PoseGraphNode(trans_odometry));

    /** Add odometry and keyframe info **/
#ifdef USE_OPENCV
    std::vector<ORBPoseEstimation::KeyframeInfo> keyframe_infos;
    cv::Ptr<cv::ORB> orb = cv::ORB::create(100);
#endif

    for (int s = begin; s < end; ++s) {
        Image depth, color;

        ReadImage(config.depth_files_[s], depth);
        ReadImage(config.color_files_[s], color);
        rgbd_source.Upload(depth, color);

        /** Insert a keyframe **/
#ifdef USE_OPENCV
        if (config.with_opencv_ && s % config.n_keyframes_per_n_frame_ == 0) {
            cv::Mat im = cv::imread(config.color_files_[s]);
            cv::cvtColor(im, im, cv::COLOR_BGR2GRAY);
            std::vector<cv::KeyPoint> kp;
            cv::Mat desc;
            orb->detectAndCompute(im, cv::noArray(), kp, desc);

            ORBPoseEstimation::KeyframeInfo keyframe_info;
            keyframe_info.idx = s;
            keyframe_info.descriptor = desc;
            keyframe_info.keypoints = kp;
            keyframe_info.color = im;
            keyframe_info.depth = rgbd_source.depth_.DownloadMat();
            keyframe_infos.emplace_back(keyframe_info);
        }
#endif  // USE_OPENCV

        int t = s + 1;
        if (t >= end) break;

        ReadImage(config.depth_files_[t], depth);
        ReadImage(config.color_files_[t], color);
        rgbd_target.Upload(depth, color);

        LogDebug("RGBD Odometry between ({} {})", s, t);
        odometry.transform_source_to_target_ = Eigen::Matrix4d::Identity();
        odometry.Initialize(rgbd_source, rgbd_target);
        odometry.ComputeMultiScale();

        Eigen::Matrix4d trans = odometry.transform_source_to_target_;
        Eigen::Matrix6d information = odometry.ComputeInformationMatrix();

        // source_to_target * world_to_source = world_to_target
        trans_odometry = trans * trans_odometry;

        // target_to_world
        Eigen::Matrix4d trans_odometry_inv = trans_odometry.inverse();

        pose_graph.nodes_.emplace_back(PoseGraphNode(trans_odometry_inv));
        pose_graph.edges_.emplace_back(
                PoseGraphEdge(s - begin, t - begin, trans, information, false));
    }

#ifdef USE_OPENCV
    /** Add Loop closures **/
    if (config.with_opencv_) {
        for (int i = 0; i < keyframe_infos.size() - 1; ++i) {
            for (int j = i + 1; j < keyframe_infos.size(); ++j) {
                int s = keyframe_infos[i].idx;
                int t = keyframe_infos[j].idx;
                LogInfo("RGBD Loop closure between ({} {})", s, t);

                bool is_success;
                Eigen::Matrix4d trans_source_to_target;

                std::tie(is_success, trans_source_to_target) =
                        ORBPoseEstimation::PoseEstimation(keyframe_infos[i],
                                                          keyframe_infos[j],
                                                          config.intrinsic_);

                if (is_success) {
                    odometry.transform_source_to_target_ =
                            trans_source_to_target;

                    Image depth, color;

                    ReadImage(config.depth_files_[s], depth);
                    ReadImage(config.color_files_[s], color);
                    rgbd_source.Upload(depth, color);

                    ReadImage(config.depth_files_[t], depth);
                    ReadImage(config.color_files_[t], color);
                    rgbd_target.Upload(depth, color);

                    odometry.Initialize(rgbd_source, rgbd_target);
                    auto result = odometry.ComputeMultiScale();

                    if (std::get<0>(result)) {
                        Eigen::Matrix4d trans =
                                odometry.transform_source_to_target_;
                        Eigen::Matrix6d information =
                                odometry.ComputeInformationMatrix();

                        pose_graph.edges_.emplace_back(
                                PoseGraphEdge(s - begin, t - begin, trans,
                                              information, true));
                    }
                }
            }
        }
    }
#endif  // USE_OPENCV
    WritePoseGraph(config.GetPoseGraphFileForFragment(fragment_id, false),
                   pose_graph);
}

void OptimizePoseGraphForFragment(int fragment_id, DatasetConfig &config) {
    PoseGraph pose_graph;
    ReadPoseGraph(config.GetPoseGraphFileForFragment(fragment_id, false),
                  pose_graph);

    GlobalOptimizationConvergenceCriteria criteria;
    GlobalOptimizationOption option(config.max_depth_diff_, 0.25,
                                    config.preference_loop_closure_odometry_,
                                    0);
    GlobalOptimizationLevenbergMarquardt optimization_method;
    GlobalOptimization(pose_graph, optimization_method, criteria, option);

    auto pose_graph_prunned =
            CreatePoseGraphWithoutInvalidEdges(pose_graph, option);

    WritePoseGraph(config.GetPoseGraphFileForFragment(fragment_id, true),
                   *pose_graph_prunned);
}

void IntegrateForFragment(int fragment_id, DatasetConfig &config) {
    PoseGraph pose_graph;
    ReadPoseGraph(config.GetPoseGraphFileForFragment(fragment_id, true),
                  pose_graph);

    float voxel_length = config.tsdf_cubic_size_ / 512.0;

    cuda::PinholeCameraIntrinsicCuda intrinsic(config.intrinsic_);
    cuda::TransformCuda trans = cuda::TransformCuda::Identity();
    cuda::ScalableTSDFVolumeCuda tsdf_volume(
            8, voxel_length, (float)config.tsdf_truncation_, trans,config.bucket_count_tsdf_,config.value_cap_tsdf_);

    cuda::RGBDImageCuda rgbd((float)config.max_depth_,
                             (float)config.depth_factor_);

    const int begin = fragment_id * config.n_frames_per_fragment_;
    const int end = std::min((fragment_id + 1) * config.n_frames_per_fragment_,
                             (int)config.color_files_.size());

    for (int i = begin; i < end; ++i) {
        LogDebug("Integrating frame {} ...", i);

        Image depth, color;
        ReadImage(config.depth_files_[i], depth);
        ReadImage(config.color_files_[i], color);
        rgbd.Upload(depth, color);

        /* Use ground truth trajectory */
        Eigen::Matrix4d pose = pose_graph.nodes_[i - begin].pose_;
        trans.FromEigen(pose);

        tsdf_volume.Integrate(rgbd, intrinsic, trans);
    }

    tsdf_volume.GetAllSubvolumes();
    WriteScalableTSDFVolumeToBIN(config.GetBinFileForFragment(fragment_id),
                                 tsdf_volume);

    cuda::ScalableMeshVolumeCuda mesher(
            cuda::VertexWithNormalAndColor, 8,
            tsdf_volume.active_subvolume_entry_array_.size(),config.max_vertices_mesh_,config.max_triangle_mesh_);
    mesher.MarchingCubes(tsdf_volume);
    auto mesh = mesher.mesh().Download();

    PointCloud pcl;
    pcl.points_ = mesh->vertices_;
    pcl.normals_ = mesh->vertex_normals_;
    pcl.colors_ = mesh->vertex_colors_;

    /** Write original fragments **/
    WritePointCloudToPLY(config.GetPlyFileForFragment(fragment_id), pcl);

    /** Write downsampled thumbnail fragments **/
    auto pcl_downsampled = pcl.VoxelDownSample(config.voxel_size_);
    WritePointCloudToPLY(config.GetThumbnailPlyFileForFragment(fragment_id),
                         *pcl_downsampled);
}

static int Run(DatasetConfig &config) {
    Timer timer;
    timer.Start();

    filesystem::MakeDirectoryHierarchy(config.path_dataset_ +
                                       "/fragments/thumbnails");

    const int num_fragments = DIV_CEILING(config.color_files_.size(),
                                          config.n_frames_per_fragment_);

    for (int i = 0; i < num_fragments; ++i) {
        LogInfo("Processing fragment {} / {}", i, num_fragments - 1);
        MakePoseGraphForFragment(i, config);
        OptimizePoseGraphForFragment(i, config);
        IntegrateForFragment(i, config);
    }
    timer.Stop();
    LogInfo("\n");
    LogInfo("================================");
    LogInfo("MakeFragment takes {} s", timer.GetDuration() * 1e-3);
}
};  // namespace MakeFragment
