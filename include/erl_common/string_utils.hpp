#pragma once

#include <sstream>

namespace erl::common {

#define ERL_FORMAT_STRING(...)            \
    ([&]() {                          \
        char buffer[4096];            \
        std::snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
        return std::string(buffer);   \
    }())

    template<typename... Args>
    inline std::string
    AsString(Args... args) {
        std::stringstream ss;
        (ss << ... << args);  // https://en.cppreference.com/w/cpp/language/fold
        return ss.str();
    }

#define ERL_AS_STRING(x) #x
}  // namespace erl::common
