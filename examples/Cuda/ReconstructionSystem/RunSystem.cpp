

#include "DatasetConfig.h"

//#include "IntegrateScene.h"
#include "MakeFragments.h"
//#include "RefineRegistration.h"
//#include "RegisterFragments.h"

using namespace open3d;
using namespace open3d::io;
using namespace open3d::utility;

std::string SecondsToHMS(double seconds) {
    int minutes = int(seconds / 60);
    int hours = minutes / 60;

    double seconds_remain = seconds - minutes * 60;

    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
       << std::setfill('0') << (minutes % 60) << ":";

    ss.setf(std::ios::fixed);
    ss << std::setw(6) << std::setfill('0') << std::setprecision(3)
       << seconds_remain;

    return ss.str();
}

int main(int argc, char **argv) {
    DatasetConfig config;

    std::string config_path =
            argc > 1 ? argv[1] : kDefaultDatasetConfigDir + "/intel/test.json";

    bool is_success = ReadIJsonConvertible(config_path, config);
    if (!is_success) return 1;

    Timer timer_total;
    timer_total.Start();

    Timer timer;

    timer.Start();
    MakeFragment::Run(config);
    timer.Stop();
    std::string make_fragment_time = SecondsToHMS(timer.GetDuration() * 1e-3);

    /*timer.Start();
    RegisterFragments::Run(config);
    timer.Stop();
    std::string register_fragments_time =
            SecondsToHMS(timer.GetDuration() * 1e-3);

    timer.Start();
    RefineRegistration::Run(config);
    timer.Stop();
    std::string refine_registration_time =
            SecondsToHMS(timer.GetDuration() * 1e-3);

    timer.Start();
    IntegrateScene::Run(config);
    timer.Stop();
    std::string integrate_scene_time = SecondsToHMS(timer.GetDuration() * 1e-3);*/

    timer_total.Stop();
    std::string total_time = SecondsToHMS(timer_total.GetDuration() * 1e-3);

    LogInfo("================================\n");
    LogInfo(" - Make fragment      : {}\n", make_fragment_time.c_str());
    //LogInfo(" - Register fragments : {}\n", register_fragments_time.c_str());
    //LogInfo(" - Refine registration: {}\n", refine_registration_time.c_str());
    //LogInfo(" - Intergate scene    : {}\n", integrate_scene_time.c_str());
    LogInfo(" - Total              : {}\n", total_time.c_str());
    LogInfo("================================\n");

    //auto mesh = io::CreateMeshFromFile(config.GetReconstructedSceneFile());
    //visualization::DrawGeometries({mesh});

    return 0;
}
