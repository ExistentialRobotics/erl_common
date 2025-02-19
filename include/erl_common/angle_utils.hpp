#pragma once

#include "eigen.hpp"

#include <cmath>

namespace erl::common {
    /**
     * Wrap angle to [-pi, pi)
     * @param angle
     * @return
     */
    template<typename Dtype>
    Dtype
    WrapAnglePi(Dtype angle) {
        angle = std::fmod(angle, 2.0 * M_PI);
        if (angle < -M_PI) {
            angle += 2.0 * M_PI;
        } else if (angle >= M_PI) {
            angle -= 2.0 * M_PI;
        }
        return angle;
        // if (angle < -M_PI) {
        //     if (angle < -2 * M_PI) {
        //         double t = std::ceil(angle / (2 * M_PI));
        //         return angle - t * 2 * M_PI;
        //     } else {
        //         return angle + 2 * M_PI;
        //     }
        // } else if (angle >= M_PI) {
        //     double t = std::ceil(angle / (2 * M_PI));
        //     return angle - t * 2 * M_PI;
        // } else {
        //     return angle;
        // }
    }

    template<typename Dtype>
    Dtype
    WrapAngleTwoPi(Dtype angle) {
        angle = std::fmod(angle, 2.0 * M_PI);
        if (angle < 0.0) { angle += 2.0 * M_PI; }
        return angle;
        // if (angle < 0 && angle > -1.e-7) {
        //     return 0;
        // }
        // Dtype t = std::floor(angle / (2 * M_PI));
        // angle -= t * 2 * M_PI;
        // if (2 * M_PI - angle < 5.e-7) {
        //     return 0;
        // }
        // return angle;
    }

    template<typename Dtype>
    Dtype
    DegreeToRadian(const Dtype degree) {
        return degree * M_PI / 180.0;
    }

    template<typename Dtype>
    Dtype
    RadianToDegree(const Dtype radian) {
        return radian * 180.0 / M_PI;
    }

    /**
     * Convert azimuth-elevation to 3d direction vector
     * @param azimuth: [0, 2pi), angle between x-axis and the projection of the direction vector on x-y plane
     * @param elevation: [-pi/2, pi/2], angle between x-y plane and the direction vector
     * @return
     */
    template<typename Dtype>
    Eigen::Vector3<Dtype>
    AzimuthElevationToDirection(const Dtype azimuth, const Dtype elevation) {
        const Dtype sin_azimuth = std::sin(azimuth);
        const Dtype cos_azimuth = std::cos(azimuth);
        const Dtype sin_elevation = std::sin(elevation);
        const Dtype cos_elevation = std::cos(elevation);
        return {cos_azimuth * cos_elevation, sin_azimuth * cos_elevation, sin_elevation};
    }

    template<typename Dtype>
    void
    DirectionToAzimuthElevation(const Eigen::Ref<const Eigen::Vector3<Dtype>> &direction, Dtype &azimuth, Dtype &elevation) {
        elevation = std::asin(direction[2]);               // [-pi/2, pi/2]
        azimuth = std::atan2(direction[1], direction[0]);  // [-pi, pi)
    }
}  // namespace erl::common
