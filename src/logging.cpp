#include "erl_common/logging.hpp"

namespace erl::common {
    Logging::Level Logging::s_level_ = kInfo;
    std::mutex Logging::g_print_mutex;

    void
    Logging::SetLevel(const Level level) {
        s_level_ = level;
    }

    Logging::Level
    Logging::GetLevel() {
        return s_level_;
    }

    std::string
    Logging::GetDateStr() {
        return fmt::format("{:%Y-%m-%d}", fmt::localtime(std::time(nullptr)));
    }

    std::string
    Logging::GetTimeStr() {
        return fmt::format("{:%X}", fmt::localtime(std::time(nullptr)));
    }

    std::string
    Logging::GetDateTimeStr() {
        return fmt::format("{:%Y-%m-%d %X}", fmt::localtime(std::time(nullptr)));
    }

    std::string
    Logging::GetTimeStamp() {
        return fmt::format("{:%Y%m%d-%H%M%S}", fmt::localtime(std::time(nullptr)));
    }

}  // namespace erl::common
