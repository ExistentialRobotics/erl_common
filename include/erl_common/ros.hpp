#pragma once

#include "logging.hpp"

#include <filesystem>
#include <unordered_map>
#include <vector>

#ifdef ERL_USE_OPENCV
    #include <opencv2/core.hpp>
#endif

#ifdef ERL_ROS_VERSION_1
    #include <ros/ros.h>

namespace erl::common::ros_params {
    // Functionality for loading ROS parameters

    [[nodiscard]] inline std::string
    GetRos1ParamPath(const std::string &prefix, const std::string &name) {
        if (prefix.empty()) {
            return name;
        } else if (prefix.back() == '/') {
            return prefix + name;
        } else {
            return prefix + "/" + name;
        }
    }

    // Type trait for types natively supported by ROS1 nh.param<T>().
    // These are passed directly to nh.param<T>().
    // All other types fall back to YAML string serialization.
    template<typename T>
    inline constexpr bool is_ros1_native_param_v = std::is_same_v<T, bool> ||                 //
                                                   std::is_same_v<T, int> ||                  //
                                                   std::is_same_v<T, double> ||               //
                                                   std::is_same_v<T, std::string> ||          //
                                                   std::is_same_v<T, std::vector<bool>> ||    //
                                                   std::is_same_v<T, std::vector<int>> ||     //
                                                   std::is_same_v<T, std::vector<double>> ||  //
                                                   std::is_same_v<T, std::vector<std::string>>;

    template<typename T>
    struct LoadRos1Param {
        static void
        Run(ros::NodeHandle &nh, const std::string &param_name, T &member) {
            if constexpr (is_ros1_native_param_v<T>) {
                nh.param<T>(param_name, member, member);
            } else {
                // YAML fallback: serialize as a YAML string
                using yaml_convert = YAML::convert<T>;
                std::string value_str = yaml_convert::encode(member).template as<std::string>();
                if (!nh.param<std::string>(param_name, value_str, value_str)) { return; }
                ERL_ASSERT(yaml_convert::decode(YAML::Node(value_str), member));
            }
        }
    };

    // Keep specializations that map to different native ROS1 types for better introspection.

    template<>
    struct LoadRos1Param<float> {
        static void
        Run(ros::NodeHandle &nh, const std::string &param_name, float &member) {
            double temp = static_cast<double>(member);
            if (!nh.param<double>(param_name, temp, temp)) { return; }
            if (temp < -std::numeric_limits<float>::max() ||
                temp > std::numeric_limits<float>::max()) {
                ERL_WARN(
                    "Parameter {} has value {} outside the range of float type",
                    param_name,
                    temp);
            }
            member = static_cast<float>(temp);
        }
    };

    template<>
    struct LoadRos1Param<std::vector<float>> {
        static void
        Run(ros::NodeHandle &nh, const std::string &param_name, std::vector<float> &member) {
            std::vector<double> temp;
            temp.reserve(member.size());
            std::transform(member.begin(), member.end(), temp.begin(), [](float v) {
                return static_cast<double>(v);
            });
            if (!nh.param<std::vector<double>>(param_name, temp, temp)) { return; }
            member.resize(temp.size());
            for (std::size_t i = 0; i < temp.size(); ++i) {
                member[i] = static_cast<float>(temp[i]);
            }
        }
    };

    template<typename Scalar_, int Rows_, int Cols_, int Options_, int MaxRows_, int MaxCols_>
    struct LoadRos1Param<Eigen::Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>> {
        using Mat = Eigen::Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>;

        static void
        Run(ros::NodeHandle &nh, const std::string &param_name, Mat &member) {
            long n_rows = Rows_;
            long n_cols = Cols_;
            if (Rows_ == Eigen::Dynamic) {
                LoadRos1Param<long>::Run(nh, GetRos1ParamPath(param_name, "rows"), n_rows);
            }
            if (Cols_ == Eigen::Dynamic) {
                LoadRos1Param<long>::Run(nh, GetRos1ParamPath(param_name, "cols"), n_cols);
            }

            std::vector<Scalar_> values;
            LoadRos1Param<std::vector<Scalar_>>::Run(nh, param_name, values);
            if (values.empty()) { return; }

            if (Rows_ > 0 && Cols_ > 0) {
                ERL_ASSERTM(
                    values.size() == static_cast<std::size_t>(Rows_ * Cols_),
                    "expecting {} values for param {}, got {}",
                    Rows_ * Cols_,
                    param_name,
                    values.size());
                member = Eigen::Map<Mat>(values.data(), Rows_, Cols_);
                return;
            }
            if (Rows_ > 0) {
                ERL_ASSERTM(
                    values.size() % Rows_ == 0,
                    "expecting multiple of {} values for param {} ({} x -1), got {}",
                    Rows_,
                    param_name,
                    Rows_,
                    values.size());
                const int cols = static_cast<int>(values.size()) / Rows_;
                ERL_ASSERTM(
                    n_cols <= 0 || cols == n_cols,
                    "mismatched number of columns: {} vs {}",
                    cols,
                    n_cols);
                member.resize(Rows_, cols);
                member = Eigen::Map<Mat>(values.data(), Rows_, cols);
                return;
            }
            if (Cols_ > 0) {
                ERL_ASSERTM(
                    values.size() % Cols_ == 0,
                    "expecting multiple of {} values for param {} (-1 x {}), got {}",
                    Cols_,
                    param_name,
                    Cols_,
                    values.size());
                const int rows = static_cast<int>(values.size()) / Cols_;
                ERL_ASSERTM(
                    n_rows <= 0 || rows == n_rows,
                    "mismatched number of rows: {} vs {}",
                    rows,
                    n_rows);
                member.resize(rows, Cols_);
                member = Eigen::Map<Mat>(values.data(), rows, Cols_);
                return;
            }
            ERL_ASSERTM(
                n_rows > 0 && n_cols > 0,
                "For param {} with fully dynamic size (-1 x -1), both rows and cols must be "
                "specified and > 0",
                param_name);
            const int size = static_cast<int>(values.size());
            ERL_ASSERTM(
                size == n_rows * n_cols,
                "expecting {} values for param {} ({} x {}), got {}",
                n_rows * n_cols,
                param_name,
                n_rows,
                n_cols,
                size);
            member.resize(n_rows, n_cols);
            member = Eigen::Map<Mat>(values.data(), n_rows, n_cols);
            return;
        }
    };

    #ifdef ERL_USE_OPENCV

    template<>
    struct LoadRos1Param<cv::Scalar> {
        static void
        Run(ros::NodeHandle &nh, const std::string &param_name, cv::Scalar &member) {
            std::vector<double> values;
            if (!nh.param<std::vector<double>>(param_name, values, values)) { return; }
            if (values.empty()) { return; }
            ERL_ASSERTM(
                values.size() <= 4,
                "expecting up to 4 values for param {}, got {}",
                param_name,
                values.size());
            for (std::size_t i = 0; i < values.size(); ++i) { member[i] = values[i]; }
        }
    };

    #endif

}  // namespace erl::common::ros_params

#endif

#ifdef ERL_ROS_VERSION_2
    #include <rclcpp/rclcpp.hpp>

namespace erl::common::ros_params {
    // Functionality for loading ROS2 parameters

    [[nodiscard]] inline std::string
    GetRos2ParamPath(const std::string &prefix, const std::string &name) {
        if (prefix.empty()) { return name; }
        return prefix + "." + name;
    }

    // Type trait for types natively supported by rclcpp parameters.
    // These are passed directly to declare_parameter<T>/get_parameter<T>.
    // All other types fall back to YAML string serialization.
    template<typename T>
    inline constexpr bool is_ros2_native_param_v = std::is_same_v<T, bool> ||                  //
                                                   std::is_same_v<T, int64_t> ||               //
                                                   std::is_same_v<T, double> ||                //
                                                   std::is_same_v<T, std::string> ||           //
                                                   std::is_same_v<T, std::vector<uint8_t>> ||  //
                                                   std::is_same_v<T, std::vector<bool>> ||     //
                                                   std::is_same_v<T, std::vector<int64_t>> ||  //
                                                   std::is_same_v<T, std::vector<double>> ||   //
                                                   std::is_same_v<T, std::vector<std::string>>;

    template<typename T>
    struct LoadRos2Param {
        static void
        Run(rclcpp::Node *node, const std::string &param_name, T &member) {
            if constexpr (is_ros2_native_param_v<T>) {
                // LoadRos2Param may be called multiple times. For example, it can be called first
                // in a base node class to load some parameters, and then called again in a derived
                // node class to load more parameters. In that case, we should not declare the same
                // parameter again, because rclcpp will throw an exception if we try to declare a
                // parameter that already exists.
                if (!node->has_parameter(param_name)) {
                    node->declare_parameter<T>(param_name, member);
                }
                node->get_parameter<T>(param_name, member);
            } else {
                // YAML fallback: serialize as a YAML string
                using yaml_convert = YAML::convert<T>;
                std::string value_str = yaml_convert::encode(member).template as<std::string>();
                if (!node->has_parameter(param_name)) {
                    node->declare_parameter<std::string>(param_name, value_str);
                }
                if (!node->get_parameter<std::string>(param_name, value_str)) { return; }
                ERL_ASSERT(yaml_convert::decode(YAML::Node(value_str), member));
            }
        }
    };

    // Keep specializations that map to different native ROS2 types for better introspection.

    template<>
    struct LoadRos2Param<std::vector<float>> {
        static void
        Run(rclcpp::Node *node, const std::string &param_name, std::vector<float> &member) {
            std::vector<double> temp;
            temp.reserve(member.size());
            std::transform(member.begin(), member.end(), temp.begin(), [](float v) {
                return static_cast<double>(v);
            });
            if (!node->has_parameter(param_name)) {
                node->declare_parameter<std::vector<double>>(param_name, temp);
            }
            if (!node->get_parameter(param_name, temp)) { return; }
            member.resize(temp.size());
            for (std::size_t i = 0; i < temp.size(); ++i) {
                member[i] = static_cast<float>(temp[i]);
            }
        }
    };

    template<>
    struct LoadRos2Param<std::vector<int>> {
        static void
        Run(rclcpp::Node *node, const std::string &param_name, std::vector<int> &member) {
            std::vector<long> temp;
            temp.reserve(member.size());
            std::transform(member.begin(), member.end(), temp.begin(), [](int v) {
                return static_cast<long>(v);
            });
            if (!node->has_parameter(param_name)) {
                node->declare_parameter<std::vector<long>>(param_name, temp);
            }
            if (!node->get_parameter(param_name, temp)) { return; }
            member.resize(temp.size());
            for (std::size_t i = 0; i < temp.size(); ++i) { member[i] = static_cast<int>(temp[i]); }
        }
    };

    template<typename Scalar_, int Rows_, int Cols_, int Options_, int MaxRows_, int MaxCols_>
    struct LoadRos2Param<Eigen::Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>> {
        using Mat = Eigen::Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>;

        static void
        Run(rclcpp::Node *node, const std::string &param_name, Mat &member) {
            long n_rows = Rows_;
            long n_cols = Cols_;
            if (Rows_ == Eigen::Dynamic) {
                LoadRos2Param<long>::Run(node, GetRos2ParamPath(param_name, "rows"), n_rows);
            }
            if (Cols_ == Eigen::Dynamic) {
                LoadRos2Param<long>::Run(node, GetRos2ParamPath(param_name, "cols"), n_cols);
            }

            std::vector<Scalar_> values;
            LoadRos2Param<std::vector<Scalar_>>::Run(node, param_name, values);
            if (values.empty()) { return; }

            if (Rows_ > 0 && Cols_ > 0) {
                ERL_ASSERTM(
                    values.size() == static_cast<std::size_t>(Rows_ * Cols_),
                    "expecting {} values for param {}, got {}",
                    Rows_ * Cols_,
                    param_name,
                    values.size());
                member = Eigen::Map<Mat>(values.data(), Rows_, Cols_);
                return;
            }
            if (Rows_ > 0) {
                ERL_ASSERTM(
                    values.size() % Rows_ == 0,
                    "expecting multiple of {} values for param {} ({} x -1), got {}",
                    Rows_,
                    param_name,
                    Rows_,
                    values.size());
                const int cols = static_cast<int>(values.size()) / Rows_;
                ERL_ASSERTM(
                    n_cols <= 0 || cols == n_cols,
                    "mismatched number of columns: {} vs {}",
                    cols,
                    n_cols);
                member.resize(Rows_, cols);
                member = Eigen::Map<Mat>(values.data(), Rows_, cols);
                return;
            }
            if (Cols_ > 0) {
                ERL_ASSERTM(
                    values.size() % Cols_ == 0,
                    "expecting multiple of {} values for param {} (-1 x {}), got {}",
                    Cols_,
                    param_name,
                    Cols_,
                    values.size());
                const int rows = static_cast<int>(values.size()) / Cols_;
                ERL_ASSERTM(
                    n_rows <= 0 || rows == n_rows,
                    "mismatched number of rows: {} vs {}",
                    rows,
                    n_rows);
                member.resize(rows, Cols_);
                member = Eigen::Map<Mat>(values.data(), rows, Cols_);
                return;
            }
            ERL_ASSERTM(
                n_rows > 0 && n_cols > 0,
                "For param {} with fully dynamic size (-1 x -1), both rows and cols must be "
                "specified and > 0",
                param_name);
            const int size = static_cast<int>(values.size());
            ERL_ASSERTM(
                size == n_rows * n_cols,
                "expecting {} values for param {} ({} x {}), got {}",
                n_rows * n_cols,
                param_name,
                n_rows,
                n_cols,
                size);
            member.resize(n_rows, n_cols);
            member = Eigen::Map<Mat>(values.data(), n_rows, n_cols);
            return;
        }
    };

    #ifdef ERL_USE_OPENCV

    template<>
    struct LoadRos2Param<cv::Scalar> {
        static void
        Run(rclcpp::Node *node, const std::string &param_name, cv::Scalar &member) {
            std::vector<double> values;
            if (!node->has_parameter(param_name)) {
                node->declare_parameter<std::vector<double>>(param_name, values);
            }
            if (!node->get_parameter<std::vector<double>>(param_name, values)) { return; }
            if (values.empty()) { return; }
            ERL_ASSERTM(
                values.size() <= 4,
                "expecting up to 4 values for param {}, got {}",
                param_name,
                values.size());
            for (std::size_t i = 0; i < values.size(); ++i) { member[i] = values[i]; }
        }
    };

    #endif

}  // namespace erl::common::ros_params

#endif
