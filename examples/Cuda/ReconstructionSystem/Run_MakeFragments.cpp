//
// only execute make fragments
//

#include "DatasetConfig.h"
#include "MakeFragments.h"

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
            argc > 1 ? argv[1] : kDefaultDatasetConfigDir + "/intel/config.json";

    bool is_success = ReadIJsonConvertible(config_path, config);
    if (!is_success) return 1;

    Timer timer_total;
    timer_total.Start();

    Timer timer;

    timer.Start();
    MakeFragment::Run(config);
    timer.Stop();
    std::string make_fragment_time = SecondsToHMS(timer.GetDuration() * 1e-3);

    timer_total.Stop();
    std::string total_time = SecondsToHMS(timer_total.GetDuration() * 1e-3);

    LogInfo("\n");
    //LogInfo("================================");
    //LogInfo(" - Make fragment      : {}", make_fragment_time.c_str());
    LogInfo(" - Total              : {}", total_time.c_str());
    LogInfo("================================");
    LogInfo("\n");

    return 0;

}