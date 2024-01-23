#pragma once

#include <cmath>

namespace erl::common {
    inline void
    CartesianToPolar(double x, double y, double &r, double &theta) {
        r = std::sqrt(x * x + y * y);
        theta = std::atan2(y, x);
    }
}  // namespace erl::common
