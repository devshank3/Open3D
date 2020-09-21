//
// Created by wei on 2/4/19.
//

#include <vector>
#include <string>

#include <Open3D/Open3D.h>
#include <Open3D/Registration/GlobalOptimization.h>
#include <Cuda/Open3DCuda.h>

#include "../DatasetConfig.h"
#include "../ORBPoseEstimation.h"

using namespace open3d;
using namespace open3d::utility;
using namespace open3d::io;
using namespace open3d::registration;
using namespace open3d::odometry;
using namespace open3d::geometry;
using namespace open3d::visualization;

void DebugOdometryForFragment(int fragment_id, DatasetConfig &config) {
    cuda::RGBDOdometryCuda<3> odometry;
    odometry.SetIntrinsics(config.intrinsic_);
    odometry.SetParameters(OdometryOption({20, 10, 5},
                                          config.max_depth_diff_,
                                          config.min_depth_,
                                          config.max_depth_), 0.5f);

    cuda::RGBDImageCuda rgbd_source((float) config.max_depth_,
                                    (float) config.depth_factor_);
    cuda::RGBDImageCuda rgbd_target((float) config.max_depth_,
                                    (float) config.depth_factor_);

    const int begin = fragment_id * config.n_frames_per_fragment_;
    const int end = std::min((fragment_id + 1) * config.n_frames_per_fragment_,
                             (int) config.color_files_.size());

    // world_to_source
    Eigen::Matrix4d trans_odometry = Eigen::Matrix4d::Identity();
    PoseGraph pose_graph;
    pose_graph.nodes_.emplace_back(PoseGraphNode(trans_odometry));

    /** Add odometry and keyframe info **/
    std::vector<ORBPoseEstimation::KeyframeInfo> keyframe_infos;
    cv::Ptr<cv::ORB> orb = cv::ORB::create(100);

    for (int s = begin; s < end; ++s) {
        LogInfo("s: {}\n", s);
        Image depth, color;

        ReadImage(config.depth_files_[s], depth);
        ReadImage(config.color_files_[s], color);
        rgbd_source.Upload(depth, color);

        int t = s + 1;
        if (t >= end) break;
        ReadImage(config.depth_files_[t], depth);
        ReadImage(config.color_files_[t], color);
        rgbd_target.Upload(depth, color);

        odometry.transform_source_to_target_ = Eigen::Matrix4d::Identity();
        odometry.Initialize(rgbd_source, rgbd_target);
        auto result = odometry.ComputeMultiScale();

        std::shared_ptr<cuda::PointCloudCuda>
            pcl_source = std::make_shared<cuda::PointCloudCuda>(
            cuda::VertexWithColor, 640 * 480),
            pcl_target = std::make_shared<cuda::PointCloudCuda>(
            cuda::VertexWithColor, 640 * 480);

        int level = 2;
        pcl_source->Build(odometry.source_depth_[level],
                          odometry.source_intensity_[level],
                          odometry.device_->intrinsics_[level]);
        pcl_source->Build(odometry.target_depth_[level],
                          odometry.target_intensity_[level],
                          odometry.device_->intrinsics_[level]);
        pcl_source->Transform(std::get<1>(result));
        DrawGeometriesWithCudaModule({pcl_source, pcl_target});
    }

}


int main(int argc, char **argv) {
    Timer timer;
    timer.Start();

    DatasetConfig config;

    std::string config_path = argc > 1 ? argv[1] :
                              kDefaultDatasetConfigDir + "/test/config.json";

    bool is_success = ReadIJsonConvertible(config_path, config);
    if (!is_success) return 1;

    SetVerbosityLevel(VerbosityLevel::Debug);
    filesystem::MakeDirectory(config.path_dataset_ + "/fragments");

    config.with_opencv_ = true;
    const int num_fragments =
        DIV_CEILING(config.color_files_.size(),
                    config.n_frames_per_fragment_);

    for (int i = 19; i < 20; ++i) {
        LogInfo("Processing fragment {} / {}\n", i, num_fragments - 1);
        DebugOdometryForFragment(i, config);
    }
}