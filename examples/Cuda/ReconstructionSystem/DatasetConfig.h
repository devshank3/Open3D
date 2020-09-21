//
// Created by wei on 2/7/19.
// Edited by Shank for Jetscan.
//

#pragma once

#include <json/json.h>
#include <Open3D/Open3D.h>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace open3d {
struct Match {
    bool success;
    int s;
    int t;
    Eigen::Matrix4d trans_source_to_target;
    Eigen::Matrix6d information;
};

static const std::string kDefaultDatasetConfigDir =
    "config";

class DatasetConfig : public utility::IJsonConvertible {
public:
    std::string path_dataset_;
    std::string path_intrinsic_;

    bool use_data_association_;
    bool with_opencv_;
    bool run_all_;

    int n_frames_per_fragment_;
    int n_keyframes_per_n_frame_;

    int bucket_count_tsdf_;
    int value_cap_tsdf_;
    int max_vertices_mesh_;
    int max_triangle_mesh_;

    double min_depth_;
    double max_depth_;
    double depth_factor_;
    double voxel_size_;

    double max_depth_diff_;
    double preference_loop_closure_odometry_;
    double preference_loop_closure_registration_;
    double tsdf_cubic_size_;
    double tsdf_truncation_;

    camera::PinholeCameraIntrinsic intrinsic_;
    std::vector<std::string> color_files_;
    std::vector<std::string> depth_files_;
    std::vector<std::string> fragment_files_;
    std::vector<std::string> thumbnail_fragment_files_;

    bool GetColorAndDepthFilesForTUM() {
        std::string association_file_name = path_dataset_ +
            "/depth_rgb_association.txt";
        if (!utility::filesystem::FileExists(association_file_name)) {
            utility::LogError("Data association file not found for %s\n",
                       path_dataset_.c_str());
            return false;
        }

        std::ifstream in(association_file_name);
        std::string depth_file, color_file;
        while (in >> depth_file >> color_file) {
            color_files_.emplace_back(path_dataset_ + "/" + color_file);
            depth_files_.emplace_back(path_dataset_ + "/" + depth_file);
        }

        return true;
    }

    bool GetColorFiles() {
        const std::vector<std::string> color_folders = {
            "/color", "/rgb", "/image"
        };

        for (auto &color_folder : color_folders) {
            std::string color_directory = path_dataset_ + color_folder;
            if (utility::filesystem::DirectoryExists(color_directory)) {
                utility::filesystem::ListFilesInDirectory(color_directory,
                    color_files_);
                std::sort(color_files_.begin(), color_files_.end());
                return true;
            }
        }

        utility::LogError("No color image folder found in directory %s\n",
                   path_dataset_.c_str());
        return false;
    }

    bool GetDepthFiles() {
        std::string depth_directory = path_dataset_ + "/depth";
        if (!utility::filesystem::DirectoryExists(depth_directory)) {
            utility::LogError("No depth image folder found in directory %s\n",
                       depth_directory.c_str());
            return false;
        }
        utility::filesystem::ListFilesInDirectory(depth_directory,
            depth_files_);

        /* alphabetical order */
        std::sort(depth_files_.begin(), depth_files_.end());
        return true;
    }

    bool GetFragmentFiles() {
        std::string fragment_directory = path_dataset_ + "/fragments";
        if (!utility::filesystem::DirectoryExists(fragment_directory)) {
            utility::LogError("No fragment folder found in directory %s\n",
                       fragment_directory.c_str());
            return false;
        }

        utility::filesystem::ListFilesInDirectoryWithExtension(
            fragment_directory, "ply", fragment_files_);

        /* alphabetical order */
        std::sort(fragment_files_.begin(), fragment_files_.end());
        return true;
    }

    bool GetThumbnailFragmentFiles() {
        std::string fragment_directory =
            path_dataset_ + "/fragments/thumbnails";
        if (!utility::filesystem::DirectoryExists(fragment_directory)) {
            utility::LogError("No fragment thumbnail folder found in "
                               "directory %s\n",
                       fragment_directory.c_str());
            return false;
        }

        utility::filesystem::ListFilesInDirectoryWithExtension(
            fragment_directory, "ply", thumbnail_fragment_files_);

        /* alphabetical order */
        std::sort(thumbnail_fragment_files_.begin(),
                  thumbnail_fragment_files_.end());
        return true;
    }

    std::string GetPlyFileForFragment(int fragment_id) {
        std::stringstream ss;
        ss << path_dataset_ << "/fragments/fragment_";
        ss << std::setw(3) << std::setfill('0') << fragment_id;
        ss << ".ply";
        return ss.str();
    }

    std::string GetBinFileForFragment(int fragment_id) {
        std::stringstream ss;
        ss << path_dataset_ << "/fragments/fragment_";
        ss << std::setw(3) << std::setfill('0') << fragment_id;
        ss << ".bin";
        return ss.str();
    }

    std::string GetThumbnailPlyFileForFragment(int fragment_id) {
        std::stringstream ss;
        ss << path_dataset_ << "/fragments/thumbnails/fragment_";
        ss << std::setw(3) << std::setfill('0') << fragment_id;
        ss << ".ply";
        return ss.str();
    }

    std::string GetPoseGraphFileForFragment(
        int fragment_id, bool optimized) {

        std::stringstream ss;
        ss << path_dataset_ << "/fragments/fragment_";
        if (optimized) {
            ss << "optimized_";
        }
        ss << std::setw(3) << std::setfill('0') << fragment_id;
        ss << ".json";

        return ss.str();
    }

    std::string GetPoseGraphFileForScene(bool optimized) {
        std::stringstream ss;
        ss << path_dataset_ << "/scene/global_registration";
        if (optimized) {
            ss << "_optimized";
        }
        ss << ".json";

        return ss.str();
    }

    std::string GetPoseGraphFileForRefinedScene(bool optimized) {
        std::stringstream ss;
        ss << path_dataset_ << "/scene/refined_registration";
        if (optimized) {
            ss << "_optimized";
        }
        ss << ".json";

        return ss.str();
    }

    std::string GetReconstructedSceneFile() {
        std::stringstream ss;
        ss << path_dataset_ << "/scene/integrated.ply";

        return ss.str();
    }

    bool ConvertToJsonValue(Json::Value &value) const override {}

    bool ConvertFromJsonValue(const Json::Value &value) override {
        if (!value.isObject()) {
            utility::LogWarning("DatasetConfig read JSON failed: unsupported "
                              "json "
                         "format.\n");
            return false;
        }

        path_dataset_ = value.get("path_dataset", "").asString();
        path_intrinsic_ = value.get("path_intrinsic", "").asString();
        use_data_association_ = value.get("is_tum", false).asBool();
        with_opencv_ = value.get("with_opencv", true).asBool();

        run_all_ = value.get("run_all", true).asBool();

        n_frames_per_fragment_ = value.get(
            "n_frames_per_fragment", 100).asInt();
        n_keyframes_per_n_frame_ = value.get(
            "n_keyframes_per_n_frame", 5).asInt();

        min_depth_ = value.get("min_depth", 0.3).asDouble();
        max_depth_ = value.get("max_depth", 3.0).asDouble();
        depth_factor_ = value.get("depth_factor", 1000.0).asDouble();
        voxel_size_ = value.get("voxel_size", 0.05).asDouble();

        max_depth_diff_ = value.get("max_depth_diff", 0.07).asDouble();
        preference_loop_closure_odometry_ = value.get(
            "preference_loop_closure_odometry", 0.1).asDouble();
        preference_loop_closure_registration_ = value.get(
            "preference_loop_closure_registration", 5.0).asDouble();
        tsdf_cubic_size_ = value.get("tsdf_cubic_size", 3.0).asDouble();
        tsdf_truncation_ = value.get("tsdf_truncation", 0.04).asDouble();

        bucket_count_tsdf_ = value.get("bucket_count_tsdf",40000).asInt();
        value_cap_tsdf_ = value.get("value_cap_tsdf",60000).asInt();
        max_vertices_mesh_ = value.get("max_vertices_mesh",2000000).asInt();
        max_triangle_mesh_ = value.get("max_triangle_mesh",4000000).asInt();
        

        if (path_intrinsic_.empty()) {
            intrinsic_ = camera::PinholeCameraIntrinsic(
                camera::PinholeCameraIntrinsicParameters::PrimeSenseDefault);
        } else {
            bool is_success = io::ReadIJsonConvertible(path_intrinsic_,
                intrinsic_);
            if (!is_success) {
                utility::LogError("Unable to read camera intrinsics: %s!\n",
                           path_intrinsic_.c_str());
            }
        }

        if (!use_data_association_) {
            GetColorFiles();
            GetDepthFiles();
        } else {
            GetColorAndDepthFilesForTUM();
        }

        assert(color_files_.size() > 0);
        assert(color_files_.size() == depth_files_.size());

        return true;
    }
};
}