#include "erl_common/logging_no_fmt.hpp"

int
main() {
    // Set logging level
    erl::common::LoggingNoFmt::SetLevel(erl::common::LoggingNoFmt::Level::kDebug);

    // Test different logging levels
    erl::common::LoggingNoFmt::Info("This is an info message: ", 42, " items processed");
    erl::common::LoggingNoFmt::Debug("Debug message with values: x=", 3.14, ", y=", 2.71);
    erl::common::LoggingNoFmt::Warn("Warning message: ", "something might be wrong");
    erl::common::LoggingNoFmt::Error("Error occurred in function: ", __func__);
    erl::common::LoggingNoFmt::Success("Operation completed successfully!");

    // Test macros
    ERL_NO_FMT_INFO("Using macro for info: ", "value=", 123);
    ERL_NO_FMT_WARN("Using macro for warning");
    ERL_NO_FMT_DEBUG("Debug macro with multiple args: ", "a=", 1, ", b=", 2);

    // Test once macros
    for (int i = 0; i < 3; ++i) {
        ERL_NO_FMT_INFO_ONCE("This should only appear once, iteration: ", i);
        ERL_NO_FMT_WARN_ONCE("This warning should only appear once");
    }

    // Test conditional macros
    bool condition = true;
    ERL_NO_FMT_WARN_COND(condition, "Conditional warning triggered");
    ERL_NO_FMT_WARN_COND(!condition, "This should not appear");

    // Test assertion (be careful with this in real code)
    int value = 10;
    ERL_NO_FMT_ASSERT(value > 5);  // This should pass

    // Demonstrate time functions
    erl::common::LoggingNoFmt::Info("Current date: ", erl::common::LoggingNoFmt::GetDateStr());
    erl::common::LoggingNoFmt::Info("Current time: ", erl::common::LoggingNoFmt::GetTimeStr());
    erl::common::LoggingNoFmt::Info(
        "Current datetime: ",
        erl::common::LoggingNoFmt::GetDateTimeStr());
    erl::common::LoggingNoFmt::Info("Timestamp: ", erl::common::LoggingNoFmt::GetTimeStamp());

    return 0;
}
