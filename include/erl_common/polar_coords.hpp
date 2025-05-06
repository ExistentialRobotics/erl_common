#pragma once

#include <cmath>

namespace erl::common {
    template<typename Dtype>
    void
    CartesianToPolar(const Dtype x, const Dtype y, Dtype &r, Dtype &theta) {
        r = std::sqrt(x * x + y * y);
        theta = std::atan2(y, x);
    }
}  // namespace erl::common
