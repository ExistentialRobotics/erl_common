#pragma once

#include "logging_level.hpp"
#include "progress_bar.hpp"

#include <ctime>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>

#if __cplusplus >= 202002L && __has_include(<version>)
    #include <version>
#endif
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L && __has_include(<format>)
    #include <format>
    #define ERL_LOGGING_HAVE_STD_FORMAT 1
#endif

namespace erl::common::detail {

#ifdef ERL_LOGGING_HAVE_STD_FORMAT

    /**
     * C++20 std::format fast path. Honors full format-spec syntax — "{:.3f}",
     * "{:>10}", "{:%X}", etc. — the same way libfmt does.
     */
    template<typename... Args>
    inline std::string
    Format(std::string_view fmt_str, Args &&...args) {
        auto store = std::make_format_args(args...);
        return std::vformat(fmt_str, store);
    }

#else

    // Write the format string up to (and consuming) the next "{}" placeholder.
    // "{{" and "}}" become literal '{' / '}'. Anything between '{' and '}' is
    // ignored — i.e. format specs like "{:.2f}" are accepted but not honored.
    // Returns true if a placeholder was consumed (caller should write an arg),
    // false if the end of the format string was reached.
    inline bool
    ConsumeUntilPlaceholder(std::ostringstream &oss, std::string_view &fmt_str) {
        while (!fmt_str.empty()) {
            const char c = fmt_str.front();
            if (c == '{') {
                if (fmt_str.size() >= 2 && fmt_str[1] == '{') {
                    oss << '{';
                    fmt_str.remove_prefix(2);
                    continue;
                }
                const auto end = fmt_str.find('}');
                if (end == std::string_view::npos) {
                    oss << fmt_str;
                    fmt_str = {};
                    return false;
                }
                fmt_str.remove_prefix(end + 1);
                return true;
            }
            if (c == '}' && fmt_str.size() >= 2 && fmt_str[1] == '}') {
                oss << '}';
                fmt_str.remove_prefix(2);
                continue;
            }
            oss << c;
            fmt_str.remove_prefix(1);
        }
        return false;
    }

    template<typename T>
    inline void
    WriteOneFormatArg(std::ostringstream &oss, std::string_view &fmt_str, T &&arg) {
        if (ConsumeUntilPlaceholder(oss, fmt_str)) { oss << std::forward<T>(arg); }
        // Extra arg with no remaining "{}": silently dropped (matches fmt behavior).
    }

    /**
     * Minimal fmt-style formatter used when neither libfmt nor std::format is
     * available (pre-C++20 or older standard libraries).
     * Supports "{}" positional placeholders and "{{" / "}}" escapes. Format
     * specs like "{:.2f}" are accepted but ignored — the argument is streamed
     * via operator<< with default precision/width.
     */
    template<typename... Args>
    inline std::string
    Format(std::string_view fmt_str, Args &&...args) {
        std::ostringstream oss;
        (WriteOneFormatArg(oss, fmt_str, std::forward<Args>(args)), ...);
        // Drain any remaining placeholders (extra "{}" with no arg → empty).
        while (ConsumeUntilPlaceholder(oss, fmt_str)) { /* empty */ }
        return oss.str();
    }

#endif  // ERL_LOGGING_HAVE_STD_FORMAT

}  // namespace erl::common::detail

namespace erl::common {

    class LoggingNoFmt {
        static LoggingLevel s_level_;
        static std::mutex g_print_mutex;

        // ANSI color codes for terminal output
        static constexpr auto *COLOR_RESET = "\033[0m";
        static constexpr auto *COLOR_BLUE = "\033[1;36m";      // Deep sky blue + bold
        static constexpr auto *COLOR_ORANGE = "\033[1;33m";    // Orange + bold
        static constexpr auto *COLOR_RED = "\033[1;31m";       // Red + bold
        static constexpr auto *COLOR_DARK_RED = "\033[1;91m";  // Dark red + bold
        static constexpr auto *COLOR_GREEN = "\033[1;92m";     // Spring green + bold

        static std::string
        TimePrefix() {
            const time_t now = std::time(nullptr);
            const auto tm = *std::localtime(&now);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%X");
            return oss.str();
        }

        // Returns the formatted body (without the colored prefix or trailing
        // newline) so Failure() can hand it to the caller for exception text.
        template<typename... Args>
        static std::string
        WriteLine(
            const char *color,
            const char *level_tag,
            std::string_view fmt_str,
            Args &&...args) {
            const std::scoped_lock lock(g_print_mutex);
            std::string body = detail::Format(fmt_str, std::forward<Args>(args)...);
            std::string msg = color;
            msg += '[';
            msg += TimePrefix();
            msg += "][";
            msg += level_tag;
            msg += "]: ";
            msg += COLOR_RESET;
            msg += body;
            if (ProgressBar::GetNumBars() == 0) { msg += '\n'; }
            ProgressBar::Write(msg);
            return body;
        }

    public:
        static void
        SetLevel(LoggingLevel level);

        static LoggingLevel
        GetLevel();

        static std::string
        GetDateStr();

        static std::string
        GetTimeStr();

        static std::string
        GetDateTimeStr();

        static std::string
        GetTimeStamp();

        template<typename... Args>
        static void
        Info(std::string_view fmt_str, Args &&...args) {
            if (s_level_ > kInfo) { return; }
            WriteLine(COLOR_BLUE, "INFO", fmt_str, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void
        Debug(std::string_view fmt_str, Args &&...args) {
            if (s_level_ > kDebug) { return; }
            WriteLine(COLOR_ORANGE, "DEBUG", fmt_str, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void
        Warn(std::string_view fmt_str, Args &&...args) {
            if (s_level_ > kWarn) { return; }
            WriteLine(COLOR_ORANGE, "WARN", fmt_str, std::forward<Args>(args)...);
        }

        /**
         * Report the error but not fatal message when an exception is handled
         * properly and the program can continue.
         */
        template<typename... Args>
        static void
        Error(std::string_view fmt_str, Args &&...args) {
            if (s_level_ > kError) { return; }
            WriteLine(COLOR_RED, "ERROR", fmt_str, std::forward<Args>(args)...);
        }

        /** Report a fatal message ignoring the logging level. */
        template<typename... Args>
        static void
        Fatal(std::string_view fmt_str, Args &&...args) {
            WriteLine(COLOR_DARK_RED, "FATAL", fmt_str, std::forward<Args>(args)...);
        }

        /** Report a success message ignoring the logging level. */
        template<typename... Args>
        static void
        Success(std::string_view fmt_str, Args &&...args) {
            WriteLine(COLOR_GREEN, "SUCCESS", fmt_str, std::forward<Args>(args)...);
        }

        /**
         * Report a failure message ignoring the logging level. Returns the
         * formatted body (without the colored prefix) so callers can include
         * it in a thrown exception.
         */
        template<typename... Args>
        static std::string
        Failure(std::string_view fmt_str, Args &&...args) {
            return WriteLine(COLOR_RED, "FAILURE", fmt_str, std::forward<Args>(args)...);
        }

        static void
        Write(const std::string &msg) {
            const std::scoped_lock lock(g_print_mutex);
            ProgressBar::Write(msg);
        }
    };

}  // namespace erl::common
