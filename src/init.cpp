#include "erl_common/init.hpp"

#include "erl_common/logging.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace erl::common {

    bool initialized = Init();

    namespace {
        // Parse ERL_LOG_LEVEL once and apply it. Logging::s_level_ is
        // constant-initialized to a safe default (kInfo in release, kDebug in
        // debug) so any logging call that fires during early dynamic
        // initialization, before Init() has run, still sees a valid level.
        void
        ApplyEnvLogLevel() {
#ifdef NDEBUG
            const char *env_p = std::getenv("ERL_LOG_LEVEL");
            const char *logging_level_names[] = {"debug", "info", "warn", "error", "silent"};
            if (env_p != nullptr) {
                const std::string level_str(env_p);
                if (level_str == "debug") {
                    Logging::SetLevel(kDebug);
                    return;
                }
                if (level_str == "info") {
                    Logging::SetLevel(kInfo);
                    return;
                }
                if (level_str == "warn") {
                    Logging::SetLevel(kWarn);
                    return;
                }
                if (level_str == "error") {
                    Logging::SetLevel(kError);
                    return;
                }
                if (level_str == "silent") {
                    Logging::SetLevel(kSilent);
                    return;
                }
                std::cout << "Invalid ERL_LOG_LEVEL: " << level_str << ", using the default level: "
                          << logging_level_names[Logging::GetLevel()] << "\n";
                return;
            }
            std::cout << "ERL_LOG_LEVEL not set, using the default level: "
                      << logging_level_names[Logging::GetLevel()] << "\n";
#endif
        }
    }  // namespace

    bool
    Init() {
        static bool initialized_ = false;
        if (initialized_) { return true; }

        ApplyEnvLogLevel();

        ERL_INFO("erl_common initialized");
        initialized_ = true;
        return true;
    }

}  // namespace erl::common