#pragma once

#include "progress_bar.hpp"

#include <fmt/format.h>

#include <chrono>

namespace erl::common {

    template<typename Duration>
    struct BlockTimer {
        const char *label;
        double *dt;
        bool verbose;
        std::chrono::time_point<std::chrono::high_resolution_clock> t1;

        explicit BlockTimer(const char *label, double *dt = nullptr, const bool verbose = true)
            : label(label),
              dt(dt),
              verbose(verbose),
              t1(std::chrono::high_resolution_clock::now()) {}

        ~BlockTimer() {
            if (this->dt == nullptr && !verbose) { return; }  // No need to measure time and print message

            auto &&t2 = std::chrono::high_resolution_clock::now();
            double &&t_diff = std::chrono::duration<double, typename Duration::period>(t2 - t1).count();
            if (this->dt != nullptr) { *this->dt = t_diff; }

            if (verbose) {
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

                std::string msg = fmt::format("{}: {:.3f}{}", label, t_diff, unit);
                if (ProgressBar::GetNumBars() == 0) { msg += "\n"; }
                ProgressBar::Write(msg);
            }
        }
    };
}  // namespace erl::common
