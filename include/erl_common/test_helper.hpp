#pragma once

#include "binary_file.hpp"
#include "eigen.hpp"
#include "logging.hpp"
#include "tracy.hpp"
#include "yaml.hpp"  // included before GTest headers to define operator<< for YamlableBase

#include <gtest/gtest-death-test.h>
#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>

namespace erl::common {

#define GTEST_PREPARE_OUTPUT_DIR()                                            \
    auto test_info = ::testing::UnitTest::GetInstance()->current_test_info(); \
    std::filesystem::path gtest_src_dir = __FILE__;                           \
    gtest_src_dir = gtest_src_dir.parent_path();                              \
    std::filesystem::path test_output_dir = test_info->test_suite_name();     \
    test_output_dir /= test_info->name();                                     \
    do {                                                                      \
        if (!std::filesystem::exists(test_output_dir)) {                      \
            std::filesystem::create_directories(test_output_dir);             \
        }                                                                     \
    } while (false)

#define ASSERT_EIGEN_MATRIX_EQUAL(question, ans, gt)                                         \
    do {                                                                                     \
        { ASSERT_EQ((ans).rows(), (gt).rows()); }                                            \
        { ASSERT_EQ((ans).cols(), (gt).cols()); }                                            \
        for (int _i = 0; _i < (ans).rows(); ++_i) {                                          \
            for (int _j = 0; _j < (ans).cols(); ++_j) {                                      \
                ASSERT_EQ((ans) (_i, _j), (gt) (_i, _j))                                     \
                    << (question) << ": element (" << _i << ", " << _j << ") is different."; \
            }                                                                                \
        }                                                                                    \
    } while (false)

#define ASSERT_EIGEN_MATRIX_NEAR(question, ans, gt, error)                                   \
    do {                                                                                     \
        { ASSERT_EQ((ans).rows(), (gt).rows()); }                                            \
        { ASSERT_EQ((ans).cols(), (gt).cols()); }                                            \
        for (int _i = 0; _i < (ans).rows(); ++_i) {                                          \
            for (int _j = 0; _j < (ans).cols(); ++_j) {                                      \
                ASSERT_NEAR((ans) (_i, _j), gt(_i, _j), error)                               \
                    << (question) << ": element (" << _i << ", " << _j << ") is different."; \
            }                                                                                \
        }                                                                                    \
    } while (false)

#define ASSERT_EIGEN_VECTOR_EQUAL(question, ans, gt)                       \
    do {                                                                   \
        { ASSERT_EQ((ans).size(), (gt).size()); }                          \
        for (int _i = 0; _i < (ans).size(); ++_i) {                        \
            ASSERT_EQ((ans)[_i], (gt)[_i])                                 \
                << (question) << ": element [" << _i << "] is different."; \
        }                                                                  \
    } while (false)

#define ASSERT_EIGEN_VECTOR_NEAR(question, ans, gt, error)                 \
    do {                                                                   \
        { ASSERT_EQ((ans).size(), (gt).size()); }                          \
        for (int _i = 0; _i < (ans).size(); ++_i) {                        \
            ASSERT_NEAR((ans)[_i], (gt)[_i], error)                        \
                << (question) << ": element [" << _i << "] is different."; \
        }                                                                  \
    } while (false)

#define ASSERT_STD_VECTOR_EQUAL(question, ans, gt)                         \
    do {                                                                   \
        { ASSERT_EQ((ans).size(), (gt).size()); }                          \
        for (std::size_t _i = 0; _i < (ans).size(); ++_i) {                \
            ASSERT_EQ((ans)[_i], (gt)[_i])                                 \
                << (question) << ": element [" << _i << "] is different."; \
        }                                                                  \
    } while (false)

    template<typename duration, typename F, typename... Args>
    double
    ReportTime(
        const char *label,
        int repeat,
        const bool print_all_repetitions,
        F func,
        Args &&...args) {
        std::string unit;
        if (std::is_same_v<duration, std::chrono::nanoseconds>) {
            unit = " ns";
        } else if (std::is_same_v<duration, std::chrono::microseconds>) {
            unit = " us";
        } else if (std::is_same_v<duration, std::chrono::milliseconds>) {
            unit = " ms";
        } else if (std::is_same_v<duration, std::chrono::seconds>) {
            unit = " s";
        } else if (std::is_same_v<duration, std::chrono::minutes>) {
            unit = " min";
        } else if (std::is_same_v<duration, std::chrono::hours>) {
            unit = " hrs";
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        func(std::forward<Args>(args)...);
        auto t2 = std::chrono::high_resolution_clock::now();
        double dt_mean = std::chrono::duration<double, typename duration::period>(t2 - t1).count();
        double dt_square_mean = dt_mean * dt_mean;
        double dt_max = dt_mean;
        double dt_min = dt_mean;
        if (print_all_repetitions) {
            std::cout << label << "[0]: " << dt_mean << unit << std::endl;
        }
        for (int i = 0; i < repeat; ++i) {
            t1 = std::chrono::high_resolution_clock::now();
            func(std::forward<Args>(args)...);
            t2 = std::chrono::high_resolution_clock::now();
            double dt = std::chrono::duration<double, typename duration::period>(t2 - t1).count();
            if (print_all_repetitions) {
                std::cout << label << '[' << i + 1 << "]: " << dt << unit << std::endl;
            }
            dt_mean += dt;
            dt_square_mean += dt * dt;
            dt_min = std::min(dt, dt_min);
            dt_max = std::max(dt, dt_max);
        }
        if (repeat++ > 0) {
            if (repeat >= 5) {
                dt_mean -= dt_min + dt_max;
                dt_square_mean -= dt_min * dt_min + dt_max * dt_max;
                repeat -= 2;
            }
            dt_mean /= repeat;
            dt_square_mean /= repeat;
            const long &dt_std = static_cast<long>(std::sqrt(dt_square_mean - dt_mean * dt_mean));
            std::cout << label << ": " << dt_mean << " +- " << dt_std << unit << std::endl;
        }

        return dt_mean;
    }

}  // namespace erl::common
