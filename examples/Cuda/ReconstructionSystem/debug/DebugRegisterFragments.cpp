//
// Created by wei on 2/4/19.
//

#include <string>
#include <vector>

#include <Cuda/Open3DCuda.h>
#include <Open3D/Open3D.h>
#include "../DatasetConfig.h"

using namespace open3d;
using namespace open3d::utility;
using namespace open3d::io;
using namespace open3d::registration;
using namespace open3d::visualization;

std::vector<Match> MatchFragments(DatasetConfig &config) {
    std::vector<Match> matches;

    for (int s = 0; s < config.thumbnail_fragment_files_.size() - 1; ++s) {
        auto source =
                CreatePointCloudFromFile(config.thumbnail_fragment_files_[s]);

        PoseGraph pose_graph_s;
        ReadPoseGraph(config.GetPoseGraphFileForFragment(s, true),
                      pose_graph_s);

        auto rbegin = pose_graph_s.nodes_.rbegin();
        Eigen::Matrix4d init_source_to_target = rbegin->pose_.inverse();

        int t = s + 1;
        auto target =
                CreatePointCloudFromFile(config.thumbnail_fragment_files_[t]);

        Match match;
        match.s = s;
        match.t = t;

        cuda::RegistrationCuda registration(
                TransformationEstimationType::ColoredICP);
        registration.Initialize(*source, *target, (float)config.voxel_size_,
                                init_source_to_target);
        registration.ComputeICP();
        match.trans_source_to_target = registration.transform_source_to_target_;
        match.information = registration.ComputeInformationMatrix();
        match.success = true;
        utility::LogDebug("Pair ({} {}) odometry computed.\n", match.s,
                          match.t);
        matches.push_back(match);

        DrawGeometries({source, target});
        source->Transform(match.trans_source_to_target);
        DrawGeometries({source, target});
    }

    return matches;
}

int main(int argc, char **argv) {
    DatasetConfig config;

    std::string config_path =
            argc > 1 ? argv[1]
                     : kDefaultDatasetConfigDir + "/stanford/lounge.json";

    bool is_success = io::ReadIJsonConvertible(config_path, config);
    if (!is_success) return 1;

    SetVerbosityLevel(VerbosityLevel::Debug);

    Timer timer;
    timer.Start();
    filesystem::MakeDirectory(config.path_dataset_ + "/scene");

    is_success = config.GetThumbnailFragmentFiles();
    if (!is_success) {
        LogError("Unable to get fragment files\n");
        return -1;
    }

    auto matches = MatchFragments(config);
    timer.Stop();

    LogInfo("RegisterFragments takes {} s\n", timer.GetDuration() * 1e-3);
    return 0;
}
