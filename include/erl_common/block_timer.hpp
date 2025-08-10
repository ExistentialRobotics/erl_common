#pragma once

#include "logging.hpp"

#include <chrono>

namespace erl::common {

    template<typename Duration>
    struct BlockTimer {
        std::string label;
        double *dt;
        std::chrono::time_point<std::chrono::high_resolution_clock> t1;

        explicit BlockTimer(std::string label, double *dt = nullptr)
            : label(std::move(label)),
              dt(dt),
              t1(std::chrono::high_resolution_clock::now()) {}

        template<typename T, typename Period>
        T
        Elapsed() {
            auto &&t2 = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<T, Period>(t2 - t1).count();
        }

        ~BlockTimer() {
            const bool verbose = Logging::GetLevel() <= Logging::Level::kInfo;
            // No need to measure time and print the message.
            if (this->dt == nullptr && !verbose) { return; }

            auto &&t2 = std::chrono::high_resolution_clock::now();
            double &&t_diff =
                std::chrono::duration<double, typename Duration::period>(t2 - t1).count();
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

                erl::common::Logging::Info("{}: {:.3f}{}", label, t_diff, unit);
            }
        }
    };
}  // namespace erl::common

#define ERL_BLOCK_TIMER()                                     \
    erl::common::BlockTimer<std::chrono::milliseconds> timer( \
        LOGGING_LABELED_MSG(__PRETTY_FUNCTION__))
#define ERL_BLOCK_TIMER_TIME(dt)                              \
    erl::common::BlockTimer<std::chrono::milliseconds> timer( \
        LOGGING_LABELED_MSG(__PRETTY_FUNCTION__),             \
        &(dt))
#define ERL_BLOCK_TIMER_MSG(msg) \
    erl::common::BlockTimer<std::chrono::milliseconds> timer(LOGGING_LABELED_MSG(msg))
#define ERL_BLOCK_TIMER_MSG_TIME(msg, dt) \
    erl::common::BlockTimer<std::chrono::milliseconds> timer(LOGGING_LABELED_MSG(msg), &(dt))
#define ERL_BLOCK_TIMER_MICRO()                               \
    erl::common::BlockTimer<std::chrono::microseconds> timer( \
        LOGGING_LABELED_MSG(__PRETTY_FUNCTION__))
#define ERL_BLOCK_TIMER_MICRO_TIME(dt)                        \
    erl::common::BlockTimer<std::chrono::microseconds> timer( \
        LOGGING_LABELED_MSG(__PRETTY_FUNCTION__),             \
        &(dt))
#define ERL_BLOCK_TIMER_MICRO_MSG(msg) \
    erl::common::BlockTimer<std::chrono::microseconds> timer(LOGGING_LABELED_MSG(msg))
#define ERL_BLOCK_TIMER_MICRO_MSG_TIME(msg, dt) \
    erl::common::BlockTimer<std::chrono::microseconds> timer(LOGGING_LABELED_MSG(msg), &(dt))

#ifdef NDEBUG
    #define ERL_DEBUG_BLOCK_TIMER()                       (void) 0
    #define ERL_DEBUG_BLOCK_TIMER_TIME(dt)                (void) 0
    #define ERL_DEBUG_BLOCK_TIMER_MSG(msg)                (void) 0
    #define ERL_DEBUG_BLOCK_TIMER_MSG_TIME(msg, dt)       (void) 0
    #define ERL_DEBUG_BLOCK_TIMER_MICRO()                 (void) 0
    #define ERL_DEBUG_BLOCK_TIMER_MICRO_TIME(dt)          (void) 0
    #define ERL_DEBUG_BLOCK_TIMER_MICRO_MSG(msg)          (void) 0
    #define ERL_DEBUG_BLOCK_TIMER_MICRO_MSG_TIME(msg, dt) (void) 0
#else
    #define ERL_DEBUG_BLOCK_TIMER()                       ERL_BLOCK_TIMER()
    #define ERL_DEBUG_BLOCK_TIMER_TIME(dt)                ERL_BLOCK_TIMER_TIME(dt)
    #define ERL_DEBUG_BLOCK_TIMER_MSG(msg)                ERL_BLOCK_TIMER_MSG(msg)
    #define ERL_DEBUG_BLOCK_TIMER_MSG_TIME(msg, dt)       ERL_BLOCK_TIMER_MSG_TIME(msg, dt)
    #define ERL_DEBUG_BLOCK_TIMER_MICRO()                 ERL_BLOCK_TIMER_MICRO()
    #define ERL_DEBUG_BLOCK_TIMER_MICRO_TIME(dt)          ERL_BLOCK_TIMER_MICRO_TIME(dt)
    #define ERL_DEBUG_BLOCK_TIMER_MICRO_MSG(msg)          ERL_BLOCK_TIMER_MICRO_MSG(msg)
    #define ERL_DEBUG_BLOCK_TIMER_MICRO_MSG_TIME(msg, dt) ERL_BLOCK_TIMER_MICRO_MSG_TIME(msg, dt)
#endif
