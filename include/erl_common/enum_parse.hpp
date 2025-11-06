#pragma once

#include "yaml.hpp"

#ifdef ERL_USE_FMT
    #include "fmt.hpp"

    #define ERL_FMT_FORMAT_ENUM(T)                                      \
        template<>                                                      \
        struct fmt::formatter<T> {                                      \
            template<typename ParseContext>                             \
            constexpr auto                                              \
            parse(ParseContext &ctx) {                                  \
                return ctx.begin();                                     \
            }                                                           \
                                                                        \
            template<typename FormatContext>                            \
            auto                                                        \
            format(const T &value, FormatContext &ctx) const {          \
                return format_to(                                       \
                    ctx.out(),                                          \
                    "{}",                                               \
                    YAML::convert<T>::encode(value).as<std::string>()); \
            }                                                           \
        }

#else
    #define ERL_FMT_FORMAT_ENUM(T) (void) 0
#endif  // ERL_USE_FMT

// aggregate macros for enum parsing in different contexts
#define ERL_PARSE_ENUM(T, N)        \
    ERL_ENUM_YAML_CONVERT(T, N);    \
    ERL_PARSE_BOOST_OPTION_ENUM(T); \
    ERL_FMT_FORMAT_ENUM(T);         \
    ERL_LOAD_ROS1_PARAM_ENUM(T);    \
    ERL_LOAD_ROS2_PARAM_ENUM(T)
