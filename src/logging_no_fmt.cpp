#include "erl_common/logging_no_fmt.hpp"

#include <chrono>
#include <iomanip>
#include <ostream>
#include <sstream>

namespace erl::common {

    // Constant-initialized so the value is valid before any dynamic
    // initialization runs anywhere in the program. erl::common::Init() parses
    // ERL_LOG_LEVEL and overrides this default during library startup.
#ifndef NDEBUG
    LoggingLevel LoggingNoFmt::s_level_ = kDebug;
#else
    LoggingLevel LoggingNoFmt::s_level_ = kInfo;
#endif
    std::mutex LoggingNoFmt::g_print_mutex;

    void
    LoggingNoFmt::SetLevel(LoggingLevel level) {
        s_level_ = level;
    }

    LoggingLevel
    LoggingNoFmt::GetLevel() {
        return s_level_;
    }

    std::string
    LoggingNoFmt::GetDateStr() {
        const auto now = std::chrono::system_clock::now();
        const auto time_t = std::chrono::system_clock::to_time_t(now);
        const auto tm = *std::localtime(&time_t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d");
        return oss.str();
    }

    std::string
    LoggingNoFmt::GetTimeStr() {
        const auto now = std::chrono::system_clock::now();
        const auto time_t = std::chrono::system_clock::to_time_t(now);
        const auto tm = *std::localtime(&time_t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S");
        return oss.str();
    }

    std::string
    LoggingNoFmt::GetDateTimeStr() {
        const auto now = std::chrono::system_clock::now();
        const auto time_t = std::chrono::system_clock::to_time_t(now);
        const auto tm = *std::localtime(&time_t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    std::string
    LoggingNoFmt::GetTimeStamp() {
        const auto now = std::chrono::system_clock::now();
        const auto duration = now.time_since_epoch();
        const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

        return std::to_string(millis);
    }

}  // namespace erl::common
