#pragma once

#include "eigen.hpp"
#include "grid_map_info.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <memory>

namespace erl::common {

    template <typename InfoDtype>
    struct GridMapDrawer2D {
        using _Matrix2X = Eigen::Matrix<InfoDtype, 2, Eigen::Dynamic>;
        using _Vector2 = Eigen::Vector<InfoDtype, 2>;
        typedef GridMapInfo2D<InfoDtype> _GridMapInfo;
        typedef std::shared_ptr<_GridMapInfo> _GridMapInfoPtr;
        _GridMapInfoPtr grid_map_info;
        cv::Mat image;

        // TODO: Do we need to specialize?
        explicit GridMapDrawer2D(const _GridMapInfoPtr &grid_map_info)
            : grid_map_info(grid_map_info),
              image(cv::Mat(grid_map_info->Height(), grid_map_info->Width(), CV_8UC3, cv::Scalar(0, 0, 0))) {}

        void
        ResetImage() {
            image = cv::Mat(grid_map_info->Height(), grid_map_info->Width(), CV_8UC3, cv::Scalar(0, 0, 0));
        }

        void
        DrawSegmentsInplace(
            cv::Mat &mat,
            const cv::Scalar &color,
            const int thickness,
            const Eigen::Ref<const _Matrix2X > &starts,
            const Eigen::Ref<const _Matrix2X > &ends) const {
            ERL_DEBUG_ASSERT(starts.cols() == ends.cols(), "starts and ends should have the same number of columns.");
            const long num_segments = starts.cols();
            for (long i = 0; i < num_segments; ++i) {
                const Eigen::Vector2i start = grid_map_info->MeterToPixelForPoints(starts.col(i));
                const Eigen::Vector2i end = grid_map_info->MeterToPixelForPoints(ends.col(i));
                cv::line(mat, cv::Point2i(start(0), start(1)), cv::Point2i(end(0), end(1)), color, thickness);
            }
        }

        [[nodiscard]] cv::Mat
        DrawSegments(
            const cv::Mat &mat,
            const cv::Scalar &color,
            const int thickness,
            const Eigen::Ref<const _Matrix2X> &starts,
            const Eigen::Ref<const _Matrix2X> &ends) const {

            cv::Mat result = mat.clone();
            DrawSegmentsInplace(result, color, thickness, starts, ends);
            return result;
        }

        void
        DrawRaysInplace(
            cv::Mat &mat,
            const cv::Scalar &color,
            const int thickness,
            const Eigen::Ref<const _Vector2> &start,
            const Eigen::Ref<const _Matrix2X> &ends) const {

            const Eigen::Vector2i start_pixel = grid_map_info->MeterToPixelForPoints(start);
            const cv::Point2i cv_start_pixel(start_pixel(0), start_pixel(1));

            const long num_ray_ends = ends.cols();
            for (long i = 0; i < num_ray_ends; ++i) {
                const Eigen::Vector2i end = grid_map_info->MeterToPixelForPoints(ends.col(i));
                cv::line(mat, cv_start_pixel, cv::Point2i(end(0), end(1)), color, thickness);
            }
        }

        [[nodiscard]] cv::Mat
        DrawRays(
            const cv::Mat &mat,
            const cv::Scalar &color,
            const int thickness,
            const Eigen::Ref<const _Vector2> &start,
            const Eigen::Ref<const _Matrix2X> &ends) const {
            cv::Mat result = mat.clone();
            DrawRaysInplace(result, color, thickness, start, ends);
            return result;
        }

        void
        DrawPolylineInplace(
            const cv::Mat &mat,
            const cv::Scalar &color,
            const int thickness,
            const bool closed,
            const Eigen::Ref<const _Matrix2X> &points) const {
            std::vector<cv::Point2i> cv_points;
            const long num_points = points.cols();
            cv_points.resize(num_points);
            Eigen::Map<Eigen::Matrix2Xi> map(reinterpret_cast<int *>(cv_points.data()), 2, num_points);
            for (long i = 0; i < num_points; ++i) { map.col(i) = grid_map_info->MeterToPixelForPoints(points.col(i)); }
            cv::polylines(mat, cv_points, closed, color, thickness, cv::LINE_8);
        }

        [[nodiscard]] cv::Mat
        DrawPolyline(const cv::Mat &mat, const cv::Scalar &color, const int thickness, const bool closed, const Eigen::Ref<const _Matrix2X> &points)
            const {
            cv::Mat result = mat.clone();
            DrawPolylineInplace(result, color, thickness, closed, points);
            return result;
        }

        [[nodiscard]] cv::Mat
        DrawContour(const cv::Mat &mat, const cv::Scalar &color, const int thickness, const Eigen::Ref<const _Matrix2X> &contour) const {
            cv::Mat result = mat.clone();
            DrawContourInplace(result, color, thickness, contour);
            return result;
        }

        void
        DrawContourInplace(cv::Mat &mat, const cv::Scalar &color, const int &thickness, const Eigen::Ref<const Eigen::Matrix2Xd> &contour) const {
            std::vector<std::vector<cv::Point2i>> points(1);
            const long num_points = contour.cols();
            points[0].resize(num_points);
            Eigen::Map<Eigen::Matrix2Xi> map(reinterpret_cast<int *>(points[0].data()), 2, num_points);
            for (long i = 0; i < num_points; ++i) { map.col(i) = grid_map_info->MeterToPixelForPoints(contour.col(i)); }
            cv::drawContours(mat, points, 0, color, thickness, cv::LINE_8);
        }

        void
        ShowImage(const std::string &title) const {
            cv::imshow(title, image);
            cv::waitKey(0);
        }
    };

}  // namespace erl::common
