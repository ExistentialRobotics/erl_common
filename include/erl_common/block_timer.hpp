#pragma once

#include "logging.hpp"

#include <chrono>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>

namespace erl::common {

    struct BlockTimerRecords {

        struct Record {
            double current = 0.0;
            double total = 0.0;
            std::size_t count = 0;
        };

        inline static std::unordered_map<std::string, Record> records;
        inline static std::mutex records_mutex;
        inline static std::size_t max_label_length = 0;
        inline static std::vector<std::string> labels;  // for ordered printing

        static void
        AddRecord(const std::string &label, const double duration_ms) {
            std::scoped_lock<std::mutex> lock(records_mutex);

            max_label_length = std::max(max_label_length, label.size());

            auto &record = records[label];
            record.current = duration_ms;
            record.total += duration_ms;
            record.count += 1;
            if (record.count == 1) {
                labels.emplace_back(label);
                std::stable_sort(labels.begin(), labels.end());
            }
        }

        static void
        PrintRecords() {
            std::stringstream ss;
            ss << "Block Timer Records:\n";
            ss << std::left << std::setw(max_label_length + 2) << "Label" << std::right
               << std::setw(15) << "Current (ms)" << std::right << std::setw(15) << "Mean (ms)"
               << std::right << std::setw(15) << "Total (ms)" << std::right << std::setw(10)
               << "Count" << "\n";
            ss << std::string(max_label_length + 2 + 15 + 15 + 15 + 10, '-') << "\n";
            for (const auto &label: labels) {
                const auto &record = records[label];
                ss << std::left << std::setw(max_label_length + 2) << label << std::right
                   << std::setw(15) << record.current << std::right << std::setw(15)
                   << (record.count > 0 ? record.total / record.count : 0.0) << std::right
                   << std::setw(15) << record.total << std::right << std::setw(10) << record.count
                   << "\n";
            }
            ERL_INFO("{}", ss.str());
        }
    };

    template<typename Duration>
    struct BlockTimer {
        std::string label;
        double *dt;
        std::chrono::time_point<std::chrono::high_resolution_clock> t1;
        bool verbose;
        bool record;

        explicit BlockTimer(
            std::string label,
            double *dt = nullptr,
            const bool verbose_ = false,
            const bool record_ = true)
            : label(std::move(label)),
              dt(dt),
              t1(std::chrono::high_resolution_clock::now()),
              verbose(verbose_),
              record(record_) {}

        BlockTimer &
        SetVerbose(bool verbose_) {
            this->verbose = verbose_;
            return *this;
        }

        BlockTimer &
        SetRecord(bool record_) {
            this->record = record_;
            return *this;
        }

        template<typename T, typename Period>
        T
        Elapsed() const {
            auto &&t2 = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<T, Period>(t2 - t1).count();
        }

        ~BlockTimer() {
            const bool verbose_ = (Logging::GetLevel() <= LoggingLevel::kInfo) && this->verbose;
            // No need to measure time and print the message.
            if (this->dt == nullptr && !verbose_ && !record) { return; }

            auto &&t2 = std::chrono::high_resolution_clock::now();
            double &&t_diff =
                std::chrono::duration<double, typename Duration::period>(t2 - t1).count();
            if (this->dt != nullptr) { *this->dt = t_diff; }

            if (record) {
                // global record for this label
                BlockTimerRecords::AddRecord(
                    label,
                    std::chrono::duration<double, std::milli>(t2 - t1).count());
            }

            if (verbose_) {
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

                ERL_INFO("{}: {:.3f}{}", label, t_diff, unit);
            }
        }
    };

    using BlockTimerNs = BlockTimer<std::chrono::nanoseconds>;
    using BlockTimerUs = BlockTimer<std::chrono::microseconds>;
    using BlockTimerMs = BlockTimer<std::chrono::milliseconds>;
    using BlockTimerS = BlockTimer<std::chrono::seconds>;
}  // namespace erl::common

// with custom msg, no verbose, record by default
#define ERL_BLOCK_TIMER_MSG(msg)                erl::common::BlockTimerMs timer(msg, nullptr)
#define ERL_BLOCK_TIMER_MSG_TIME(msg, dt)       erl::common::BlockTimerMs timer(msg, &(dt))
#define ERL_BLOCK_TIMER_MICRO_MSG(msg)          erl::common::BlockTimerUs timer(msg, nullptr)
#define ERL_BLOCK_TIMER_MICRO_MSG_TIME(msg, dt) erl::common::BlockTimerUs timer(msg, &(dt))

// with function name as label, verbose, no record
#define ERL_BLOCK_TIMER() erl::common::BlockTimerMs timer(__PRETTY_FUNCTION__, nullptr, true, false)
#define ERL_BLOCK_TIMER_TIME(dt) \
    erl::common::BlockTimerMs timer(__PRETTY_FUNCTION__, &(dt), true, false)
#define ERL_BLOCK_TIMER_MICRO() \
    erl::common::BlockTimerUs timer(__PRETTY_FUNCTION__, nullptr, true, false)
#define ERL_BLOCK_TIMER_MICRO_TIME(dt) \
    erl::common::BlockTimerUs timer(__PRETTY_FUNCTION__, &(dt), true, false)

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
