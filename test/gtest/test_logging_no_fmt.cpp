#include "erl_common/logging_no_fmt.hpp"

int
main() {
    // Set logging level
    erl::common::LoggingNoFmt::SetLevel(erl::common::LoggingLevel::kDebug);

    // Test different logging levels (fmt-style "{}" placeholders)
    erl::common::LoggingNoFmt::Info("This is an info message: {} items processed", 42);
    erl::common::LoggingNoFmt::Debug("Debug message with values: x={}, y={}", 3.14, 2.71);
    erl::common::LoggingNoFmt::Warn("Warning message: {}", "something might be wrong");
    erl::common::LoggingNoFmt::Error("Error occurred in function: {}", __func__);
    erl::common::LoggingNoFmt::Success("Operation completed successfully!");

    // Format spec is accepted but ignored — value still prints via operator<<
    erl::common::LoggingNoFmt::Info("Float with spec: {:.3f}", 2.0 / 3.0);

    // Escapes
    erl::common::LoggingNoFmt::Info("Literal braces: {{}} and value={}", 7);

    // Demonstrate time functions
    erl::common::LoggingNoFmt::Info("Current date: {}", erl::common::LoggingNoFmt::GetDateStr());
    erl::common::LoggingNoFmt::Info("Current time: {}", erl::common::LoggingNoFmt::GetTimeStr());
    erl::common::LoggingNoFmt::Info(
        "Current datetime: {}",
        erl::common::LoggingNoFmt::GetDateTimeStr());
    erl::common::LoggingNoFmt::Info("Timestamp: {}", erl::common::LoggingNoFmt::GetTimeStamp());

    return 0;
}
