//
// Created by wei on 2/4/19.
// Edited by Shank for Jetscan.
//

#include <string>
#include <vector>

#include <Cuda/Open3DCuda.h>
#include <Open3D/Open3D.h>

#include "DatasetConfig.h"

using namespace open3d;
using namespace open3d::utility;
using namespace open3d::io;
using namespace open3d::registration;

namespace IntegrateScene {
void IntegrateFragment(int fragment_id,
                       cuda::ScalableTSDFVolumeCuda &volume,
                       DatasetConfig &config) {
    PoseGraph global_pose_graph;
    ReadPoseGraph(config.GetPoseGraphFileForRefinedScene(true),
                  global_pose_graph);

    PoseGraph local_pose_graph;
    ReadPoseGraph(config.GetPoseGraphFileForFragment(fragment_id, true),
                  local_pose_graph);

    cuda::PinholeCameraIntrinsicCuda intrinsics(config.intrinsic_);
    cuda::RGBDImageCuda rgbd((float)config.max_depth_,
                             (float)config.depth_factor_);

    const int begin = fragment_id * config.n_frames_per_fragment_;
    const int end = std::min((fragment_id + 1) * config.n_frames_per_fragment_,
                             (int)config.color_files_.size());

    for (int i = begin; i < end; ++i) {
        LogInfo("Integrating frame {} ...", i);

        geometry::Image depth, color;
        ReadImage(config.depth_files_[i], depth);
        ReadImage(config.color_files_[i], color);
        rgbd.Upload(depth, color);

        /* Use ground truth trajectory */
        Eigen::Matrix4d pose = global_pose_graph.nodes_[fragment_id].pose_ *
                               local_pose_graph.nodes_[i - begin].pose_;
        cuda::TransformCuda trans;
        trans.FromEigen(pose);

        volume.Integrate(rgbd, intrinsics, trans);
    }
}

int Run(DatasetConfig &config) {
    SetVerbosityLevel(VerbosityLevel::Debug);

    Timer timer;
    timer.Start();

    /** Larger for integrating entire scene **/
    cuda::TransformCuda trans = cuda::TransformCuda::Identity();
    cuda::ScalableTSDFVolumeCuda tsdf_volume(
            8, (float)config.tsdf_cubic_size_ / 512,
            (float)config.tsdf_truncation_, trans, config.bucket_count_tsdf_,config.value_cap_tsdf_);

    bool is_success = config.GetFragmentFiles();
    if (!is_success) {
        utility::LogError("Unable to get fragment files");
        return -1;
    }
    for (int i = 0; i < config.fragment_files_.size(); ++i) {
        IntegrateFragment(i, tsdf_volume, config);
    }

    tsdf_volume.GetAllSubvolumes();

    /** Larger scene, larger points **/
    cuda::ScalableMeshVolumeCuda mesher(
            cuda::VertexWithNormalAndColor, 8,
            tsdf_volume.active_subvolume_entry_array_.size(), config.max_vertices_mesh_,config.max_triangle_mesh_);
    mesher.MarchingCubes(tsdf_volume);
    auto mesh = mesher.mesh().Download();

    WriteTriangleMeshToPLY(config.GetReconstructedSceneFile(), *mesh);
    timer.Stop();
    LogInfo("IntegrateScene takes {} s", timer.GetDuration() * 1e-3);

    return 0;
}
}  // namespace IntegrateScene
