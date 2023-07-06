#pragma once

#include <iostream>

#include "color_print.hpp"
#include "string_utils.hpp"

#define ERL_ERROR(...)                                                                                                 \
    do {                                                                                                               \
        std::cout << erl::common::PrintError("[ERROR]: ", __FILE__, ':', __LINE__, ": ", __PRETTY_FUNCTION__) << ": "; \
        printf(__VA_ARGS__);                                                                                           \
        exit(1);                                                                                                       \
    } while (false)

#define ERL_WARNING(...)                                                                                                \
    do {                                                                                                                \
        std::cout << erl::common::PrintWarning("[WARN]: ", __FILE__, ':', __LINE__, ": ") << ": "; \
        printf(__VA_ARGS__);                                                                                            \
    } while (false)

#define ERL_WARNING_ONCE(...)         \
    do {                              \
        static bool warned = false;   \
        if (!warned) {                \
            warned = true;            \
            ERL_WARNING(__VA_ARGS__); \
        }                             \
    } while (false)

#define ERL_WARNING_IF(condition, ...)               \
    do {                                             \
        if (condition) { ERL_WARNING(__VA_ARGS__); } \
    } while (false)

#define ERL_WARNING_ONCE_IF(condition, ...) \
    do {                                    \
        static bool warned = false;         \
        if (!warned && (condition)) {       \
            warned = true;                  \
            ERL_WARNING(__VA_ARGS__);       \
        }                                   \
    } while (false)

#define ERL_INFO(...)                                                                                                \
    do {                                                                                                             \
        std::cout << erl::common::PrintInfo("[INFO]: ", __FILE__, ':', __LINE__, ": ") << ": "; \
        printf(__VA_ARGS__);                                                                                         \
    } while (false)

#ifndef NDEBUG
#define ERL_DEBUG(...)                                                                                                \
    do {                                                                                                              \
        std::cout << erl::common::PrintInfo("[DEBUG]: ", __FILE__, ':', __LINE__, ": ", __PRETTY_FUNCTION__) << ": "; \
        printf(__VA_ARGS__);                                                                                          \
    } while (false)
#else
#define ERL_DEBUG(...) ((void) 0)
#endif

#define ERL_ASSERTM(expr, ...)                   \
    do {                                         \
        if (!(expr)) {                           \
            std::stringstream ss;                \
            ss << erl::common::PrintError(       \
                "Assertion failed: (",           \
                #expr,                           \
                "), file ",                      \
                __FILE__,                        \
                ", line ",                       \
                __LINE__,                        \
                ": ",                            \
                __PRETTY_FUNCTION__,             \
                ":",                             \
                ERL_FORMAT_STRING(__VA_ARGS__)); \
            std::cout << std::flush;             \
            throw std::runtime_error(ss.str());  \
        }                                        \
    } while (false)

#define ERL_ASSERT(expr) ERL_ASSERTM(expr, "Assertion %s failed.", #expr)

#if !defined(NDEBUG)
#define ERL_DEBUG_ASSERT(expr, ...) ERL_ASSERTM(expr, __VA_ARGS__)
#else
#define ERL_DEBUG_ASSERT(expr, ...) (void) 0
#endif
