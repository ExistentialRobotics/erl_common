#pragma once
#include "eigen.hpp"

#include <cmath>

namespace erl::common {
    /**
     * Wrap angle to [-pi, pi)
     * @param angle
     * @return
     */
    inline double
    WrapAnglePi(double angle) {
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

    inline double
    WrapAngleTwoPi(double angle) {
        angle = std::fmod(angle, 2.0 * M_PI);
        if (angle < 0.0) { angle += 2.0 * M_PI; }
        return angle;
        // if (angle < 0 && angle > -1.e-7) {
        //     return 0;
        // }
        // double t = std::floor(angle / (2 * M_PI));
        // angle -= t * 2 * M_PI;
        // if (2 * M_PI - angle < 5.e-7) {
        //     return 0;
        // }
        // return angle;
    }

    inline double
    DegreeToRadian(const double degree) {
        return degree * M_PI / 180.0;
    }

    inline double
    RadianToDegree(const double radian) {
        return radian * 180.0 / M_PI;
    }

    /**
     * Convert azimuth-elevation to 3d direction vector
     * @param azimuth: [0, 2pi), angle between x-axis and the projection of the direction vector on x-y plane
     * @param elevation: [-pi/2, pi/2], angle between x-y plane and the direction vector
     * @return
     */
    inline Eigen::Vector3d
    AzimuthElevationToDirection(const double azimuth, const double elevation) {
        const double sin_azimuth = std::sin(azimuth);
        const double cos_azimuth = std::cos(azimuth);
        const double sin_elevation = std::sin(elevation);
        const double cos_elevation = std::cos(elevation);
        return {cos_azimuth * cos_elevation, sin_azimuth * cos_elevation, sin_elevation};
    }

    inline void
    DirectionToAzimuthElevation(const Eigen::Ref<const Eigen::Vector3d> &direction, double &azimuth, double &elevation) {
        elevation = std::asin(direction[2]);               // [-pi/2, pi/2]
        azimuth = std::atan2(direction[1], direction[0]);  // [-pi, pi)
    }
}  // namespace erl::common
