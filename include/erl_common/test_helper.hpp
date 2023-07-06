#pragma once

#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "binary_file.hpp"
#include "color_print.hpp"
#include "eigen.hpp"

constexpr double kAbsTol = 1.e-6;
constexpr double kRelTol = 1.e-6;

namespace erl::common {

    template<bool is_integral, typename T>
    bool
    CheckValue(const char *question_str, T ans, T gt, bool assert_diff = true) {
        if (is_integral && ans == gt) {
            std::cout << question_str << ": " << ans << ", " << PrintSuccess("[PASSED]") << std::endl;
            return true;
        } else {
            auto diff = std::fabs(ans - gt) - (kAbsTol + kRelTol * std::fabs(gt));
            if (diff > 0) {
                std::cout << question_str << ": " << ans << " and " << gt << ", diff = " << ans - gt << PrintError("[FAILED]") << std::endl;
                if (assert_diff) { throw std::runtime_error(std::string(question_str) + " failed."); }
                return false;
            } else {
                std::cout << question_str << ": " << ans << ", " << PrintSuccess("[PASSED]") << std::endl;
            }
            return true;
        }
    }

    template<typename T>
    bool CheckIntegralValue(const char *question_str, T ans, T gt, bool assert_diff = true) {
        return CheckValue<true>(question_str, ans, gt, assert_diff);
    }

    template<typename T>
    bool CheckFloatingValue(const char *question_str, T ans, T gt, bool assert_diff = true) {
        return CheckValue<false>(question_str, ans, gt, assert_diff);
    }

    template<typename T1, typename T2>
    bool
    CheckAnswers(const char *question_str, T1 ans, T2 gt, bool assert_diff = true) {

        if ((size_t) ans.size() != (size_t) gt.size()) {
            std::cout << question_str << ": GetSize is different, " << ans.size() << " and " << gt.size() << PrintError(" [FAILED]") << std::endl;
            return false;
        }
        bool fail = false;
        auto ans_ptr = ans.data();
        auto gt_ptr = gt.data();
        for (decltype(ans.size()) i = 0; i < ans.size(); ++i) {
            auto diff = std::fabs(ans_ptr[i] - gt_ptr[i]) - (kAbsTol + kRelTol * std::fabs(gt_ptr[i]));
            if (diff > 0) {
                std::cout << "element " << i << " is different: " << ans_ptr[i] << " and " << gt_ptr[i] << ", diff = " << std::scientific
                          << std::setprecision(4) << std::fabs(ans_ptr[i] - gt_ptr[i]) << std::endl
                          << "GetSize is " << ans.size() << std::endl;
                fail = true;
                if (assert_diff) { throw std::runtime_error(std::string(question_str) + " failed."); }
            }
        }

        if (fail) {
            std::cout << question_str << ": ";
            std::cout << PrintError("[FAILED]") << std::endl;
            return false;
        } else {
            std::cout << question_str << ": ";
            std::cout << PrintSuccess("[PASSED]") << std::endl;
            return true;
        }
    }

    template<typename T1, typename T2>
    void
    GtestAssertSequenceEqual(T1 ans, T2 gt) {
        auto ans_ptr = ans.data();
        auto gt_ptr = gt.data();
        for (decltype(ans.size()) i = 0; i < ans.size(); ++i) { ASSERT_EQ(ans_ptr[i], gt_ptr[i]); }
    }

    template<typename T1, typename T2>
    void
    GTestAssertSequenceNear(T1 ans, T2 gt) {
        auto ans_ptr = ans.data();
        auto gt_ptr = gt.data();
        for (decltype(ans.size()) i = 0; i < ans.size(); ++i) { ASSERT_NEAR(ans_ptr[i], gt_ptr[i], kAbsTol + kRelTol * std::fabs(gt_ptr[i])); }
    }

    template<typename duration, typename F, typename... Args>
    long
    ReportTime(const char *label, int repeat, bool print_all_repetitions, F func, Args &&...args) {
        std::string unit;
        if (std::is_same<duration, std::chrono::nanoseconds>::value) {
            unit = " ns";
        } else if (std::is_same<duration, std::chrono::microseconds>::value) {
            unit = " us";
        } else if (std::is_same<duration, std::chrono::milliseconds>::value) {
            unit = " ms";
        } else if (std::is_same<duration, std::chrono::seconds>::value) {
            unit = " s";
        } else if (std::is_same<duration, std::chrono::minutes>::value) {
            unit = " min";
        } else if (std::is_same<duration, std::chrono::hours>::value) {
            unit = " hrs";
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        func(std::forward<Args>(args)...);
        auto t2 = std::chrono::high_resolution_clock::now();
        long dt_mean = std::chrono::duration_cast<duration>(t2 - t1).count();
        long dt_square_mean = dt_mean * dt_mean;
        long dt_max = dt_mean;
        long dt_min = dt_mean;
        if (print_all_repetitions) { std::cout << label << "[0]: " << dt_mean << unit << std::endl; }
        for (int i = 0; i < repeat; ++i) {
            t1 = std::chrono::high_resolution_clock::now();
            func(std::forward<Args>(args)...);
            t2 = std::chrono::high_resolution_clock::now();
            long dt = std::chrono::duration_cast<duration>(t2 - t1).count();
            if (print_all_repetitions) { std::cout << label << '[' << i + 1 << "]: " << dt << unit << std::endl; }
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
            auto dt_std = (long) std::sqrt(dt_square_mean - dt_mean * dt_mean);
            std::cout << label << ": " << dt_mean << " +- " << dt_std << unit << std::endl;
        }

        return dt_mean;
    }
}  // namespace erl::common
