#include "erl_common/test_helper.hpp"

TEST(Logging, DateTime) {
    std::cout << "date: " << erl::common::Logging::GetDateStr() << std::endl
              << "time: " << erl::common::Logging::GetTimeStr() << std::endl
              << "date and time: " << erl::common::Logging::GetDateTimeStr() << std::endl
              << "timestamp: " << erl::common::Logging::GetTimeStamp() << std::endl;
}

TEST(Logging, Print) {
    erl::common::Logging::Info("info message\n");
    erl::common::Logging::Info("{}\n", 10);
    erl::common::Logging::Info("{:d}\n", 10);
    erl::common::Logging::Info("{:f}\n", 10.0);
    erl::common::Logging::Info("{:.3f}\n", 2.0 / 3.0);
    erl::common::Logging::Debug("debug message:{}:{}\n", __FILE__, __LINE__);
    erl::common::Logging::Warn("warn message\n");
    erl::common::Logging::Error("error message\n");
    erl::common::Logging::Success("success message\n");
    const std::string failure_msg = erl::common::Logging::Failure("failure message\n");
    EXPECT_EQ(failure_msg, "failure message\n");
}

TEST(Logging, Macro) {
    ERL_INFO("info message, 2 / 3 = {:.3f}", 2.0 / 3.0);
    ERL_INFO_ONCE("once info message, 2 / 3 = {:.3f}", 2.0 / 3.0);
    ERL_DEBUG("debug message, 2 / 3 = {:.3f}", 2.0 / 3.0);
    try {
        ERL_DEBUG_ASSERT(1 == 2, "message");
    } catch (std::runtime_error &e) { EXPECT_STREQ(e.what(), ("assertion (1 == 2) at " + std::string(__FILE__) + ":22: message\n").c_str()); }
    ERL_WARN("warn message, 2 / 3 = {:.3f}", 2.0 / 3.0);
    ERL_WARN_ONCE("once warn message, 2 / 3 = {:.3f}", 2.0 / 3.0);
    ERL_WARN_COND(true, "warn message, 2 / 3 = {:.3f}", 2.0 / 3.0);
    ERL_ERROR("error meesaage, 2 / 3 = {:.3f}", 2.0 / 3.0);
    try {
        ERL_ASSERTM(1 == 2, "message");
    } catch (std::runtime_error &e) { EXPECT_STREQ(e.what(), ("assertion (1 == 2) at " + std::string(__FILE__) + ":31: message\n").c_str()); }
    // should exit
    ASSERT_DEATH({ ERL_FATAL("fatal message, 2 / 3 = {:.3f}", 2.0 / 3.0); }, ".*");
}
