//
// Created by wei on 3/1/19.
//

#include <string>
#include <vector>

#include <Open3D/Open3D.h>
#include <Cuda/Open3DCuda.h>

#include "Utils.h"

using namespace open3d;
using namespace open3d::utility;
using namespace open3d::io;
using namespace open3d::visualization;

int FastGlobalRegistrationForPointClouds(
    const std::string &source_ply_path,
    const std::string &target_ply_path) {

    SetVerbosityLevel(VerbosityLevel::Debug);

    auto source = CreatePointCloudFromFile(source_ply_path);
    auto target = CreatePointCloudFromFile(target_ply_path);

    auto source_down = source->VoxelDownSample(0.05);
    auto target_down = target->VoxelDownSample(0.05);

    auto source_cpu = *source_down;
    auto target_cpu = *target_down;

    /** Load data **/
    cuda::FastGlobalRegistrationCuda fgr;
    fgr.Initialize(*source_down, *target_down);

    /** Prepare visualizer **/
    VisualizerWithCudaModule visualizer;
    if (!visualizer.CreateVisualizerWindow("Fast Global Registration",
        640, 480,0, 0)) {
        LogWarning("Failed creating OpenGL window.\n");
        return -1;
    }
    visualizer.BuildUtilities();
    visualizer.UpdateWindowTitle();
    visualizer.AddGeometry(source_down);
    visualizer.AddGeometry(target_down);

    bool finished = false;
    int iter = 0, max_iter = 64;
    visualizer.RegisterKeyCallback(GLFW_KEY_SPACE, [&](Visualizer *vis) {
        if (finished) return false;

        /* FGR (1 iteration) */
        fgr.DoSingleIteration(iter++);

        /* Update geometry */
        *source_down = *fgr.source_.Download();
        if (source_cpu.HasColors()) {
            source_down->colors_ = source_cpu.colors_;
        }
        *target_down = *fgr.target_.Download();
        if (target_cpu.HasColors()) {
            target_down->colors_ = target_cpu.colors_;
        }
        vis->UpdateGeometry();

        if (iter == 1) {
            vis->ResetViewPoint(true);
        }

        /* Update flags */
        if (iter >= max_iter)
            finished = true;
        return !finished;
    });

    bool should_close = false;
    while (!should_close) {
        should_close = !visualizer.PollEvents();
    }
    visualizer.DestroyVisualizerWindow();

    return 0;
}

int main(int argc, char **argv) {
    std::string source_path, target_path;
    if (argc > 2) {
        source_path = argv[1];
        target_path = argv[2];
    } else {
        std::string test_data_path = "/media/wei/Data/data/redwood_simulated/livingroom1-clean/fragments";
        source_path = test_data_path + "/fragment_005.ply";
        target_path = test_data_path + "/fragment_008.ply";
    }

    FastGlobalRegistrationForPointClouds(source_path, target_path);

    return 0;
}