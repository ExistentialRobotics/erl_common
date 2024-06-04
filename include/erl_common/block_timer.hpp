#pragma once

#include "progress_bar.hpp"

#include <fmt/format.h>

#include <chrono>

namespace erl::common {

    template<typename Duration>
    struct BlockTimer {
        const char *label;
        std::chrono::time_point<std::chrono::high_resolution_clock> t1;

        explicit BlockTimer(const char *label)
            : label(label),
              t1(std::chrono::high_resolution_clock::now()) {}

        ~BlockTimer() {
            auto &&t2 = std::chrono::high_resolution_clock::now();
            double &&dt = std::chrono::duration<double, typename Duration::period>(t2 - t1).count();

            std::string unit;
            if (std::is_same_v<Duration, std::chrono::nanoseconds>) {
                unit = " ns";
            } else if (std::is_same_v<Duration, std::chrono::microseconds>) {
                unit = " us";
            } else if (std::is_same_v<Duration, std::chrono::milliseconds>) {
                unit = " ms";
            } else if (std::is_same_v<Duration, std::chrono::seconds>) {
                unit = " s";
            } else if (std::is_same_v<Duration, std::chrono::minutes>) {
                unit = " min";
            } else if (std::is_same_v<Duration, std::chrono::hours>) {
                unit = " hrs";
            }

            std::string msg = fmt::format("{}: {:.3f}{}", label, dt, unit);
            if (ProgressBar::GetNumBars() == 0) { msg += "\n"; }
            ProgressBar::Write(msg);
        }
    };
}  // namespace erl::common
