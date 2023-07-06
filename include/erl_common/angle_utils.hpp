#pragma once
#include <cmath>

namespace erl::common {
    /**
     * Clip angle to [-pi, pi)
     * @param angle
     * @return
     */
    inline double
    ClipAngle(double angle) {
        if (angle < -M_PI) {
            return angle + 2 * M_PI;
        } else if (angle >= M_PI - 1.e-15) {
            return angle - 2 * M_PI;
        } else {
            return angle;
        }
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
