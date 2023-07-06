#pragma once

#include <Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "assert.hpp"
#include "grid_map_info.hpp"

namespace erl::common {
    cv::Mat
    AlphaBlending(const cv::Mat &foreground, const cv::Mat &background);

    cv::Mat &
    DrawTrajectoryInplace(
        cv::Mat &map,
        const Eigen::Ref<const Eigen::Matrix2Xd> &trajectory,
        const std::shared_ptr<GridMapInfo2D> &grid_map_info,
        const cv::Scalar &color,
        int thickness,
        bool pixel_based);

    inline cv::Mat &
    DrawLidarScanAreaInplace(
        cv::Mat &map,
        const Eigen::Ref<const Eigen::Vector2d> &position,
        const Eigen::Ref<const Eigen::VectorXd> &angles_in_world,
        const Eigen::Ref<const Eigen::VectorXd> &ranges,
        const std::shared_ptr<GridMapInfo2D> &grid_map_info,
        const cv::Scalar &color) {

        long num_points = angles_in_world.size();
        if (num_points == 0) { return map; }

        std::vector<std::vector<cv::Point2i>> contours(1);
        auto &contour = contours[0];
        contour.reserve(num_points + 1);
        contour.emplace_back(grid_map_info->MeterToGridForValue(position[1], 1), grid_map_info->MeterToGridForValue(position[0], 0));
        for (long i = 0; i < num_points; ++i) {
            const double &kAngle = angles_in_world[i];
            const double &kRange = ranges[i];
            contour.emplace_back(
                grid_map_info->MeterToGridForValue(position[1] + kRange * std::sin(kAngle), 1),
                grid_map_info->MeterToGridForValue(position[0] + kRange * std::cos(kAngle), 0));
        }
        cv::fillPoly(map, contours, color);

        return map;
    }

    inline cv::Mat &
    DrawLidarRaysInplace(
        cv::Mat &map,
        const Eigen::Ref<const Eigen::Vector2d> &position,
        const Eigen::Ref<const Eigen::VectorXd> &angles_in_world,
        const Eigen::Ref<const Eigen::VectorXd> &ranges,
        const std::shared_ptr<GridMapInfo2D> &grid_map_info,
        const cv::Scalar &color,
        int ray_thickness) {

        long num_points = angles_in_world.size();
        if (num_points == 0) { return map; }

        std::vector<cv::Point2i> points;
        points.reserve(num_points * 2);
        cv::Point2i start_point(grid_map_info->MeterToGridForValue(position[1], 1), grid_map_info->MeterToGridForValue(position[0], 0));
        for (long i = 0; i < num_points; ++i) {
            const double &kAngle = angles_in_world[i];
            const double &kRange = ranges[i];
            points.push_back(start_point);
            points.emplace_back(
                grid_map_info->MeterToGridForValue(position[1] + kRange * std::sin(kAngle), 1),
                grid_map_info->MeterToGridForValue(position[0] + kRange * std::cos(kAngle), 0));
        }
        cv::polylines(map, points, false, color, ray_thickness);

        return map;
    }

    inline cv::Mat
    MergeColoredMask(const std::vector<cv::Mat> &masks, const std::vector<cv::Scalar> &colors) {
        ERL_ASSERTM(masks.size() == colors.size(), "The number of masks and colors must be the same.\n");
        ERL_ASSERTM(!masks.empty(), "The number of masks and colors must be greater than 0.\n");

        cv::Mat merged_mask = cv::Mat::zeros(masks[0].size(), CV_8UC4);
        for (size_t i = 0; i < masks.size(); ++i) {
            cv::Mat colored_mask;
            cv::cvtColor(masks[i], colored_mask, cv::COLOR_GRAY2RGBA);
            cv::multiply(colored_mask, colors[i], colored_mask);
            cv::add(merged_mask, colored_mask, merged_mask);
        }
        return merged_mask;
    }
}  // namespace erl::common
