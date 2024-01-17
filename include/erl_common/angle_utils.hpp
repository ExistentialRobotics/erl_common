#pragma once
#include <cmath>

namespace erl::common {
    /**
     * Wrap angle to [-pi, pi)
     * @param angle
     * @return
     */
    inline double
    WrapAnglePi(double angle) {
        angle = std::fmod(angle, 2 * M_PI);
        if (angle < -M_PI) {
            angle += 2 * M_PI;
        } else if (angle >= M_PI) {
            angle -= 2 * M_PI;
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
        angle = std::fmod(angle, 2 * M_PI);
        if (angle < 0) { angle += 2 * M_PI; }
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
    DegreeToRadian(double degree) {
        return degree * M_PI / 180.0;
    }

    inline double
    RadianToDegree(double radian) {
        return radian * 180.0 / M_PI;
    }
}  // namespace erl::common
