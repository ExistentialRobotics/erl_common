#pragma once

#include "logging.hpp"

#include <istream>
#include <vector>

namespace erl::common {
    inline void
    SkipLine(std::istream &stream, const char delimiter = '\n') {
        char c;
        do { c = static_cast<char>(stream.get()); } while (stream.good() && c != delimiter);
    }

    template<typename T, typename TokenFunctionPair>
    bool
    WriteTokens(std::ostream &s, const T *obj, const std::vector<TokenFunctionPair> &token_function_pairs) {
        for (const auto &[token, write_func]: token_function_pairs) {
            s << token << '\n';  // write token, add newline so that the reader stops properly.
            if (!write_func(obj, s) || !s.good()) {
                ERL_WARN("Failed to write {}.", token);
                return false;
            }
            s << '\n';  // add a newline so that the reader stops properly.
        }
        return s.good();
    }

    template<typename T, typename TokenFunctionPair>
    bool
    ReadTokens(std::istream &s, T *obj, const std::vector<TokenFunctionPair> &token_function_pairs) {
        std::string token;
        std::size_t token_idx = 0;
        for (const auto &[expected_token, read_func]: token_function_pairs) {
            s >> token;                           // skip leading whitespaces before reading, and stop at the first whitespace (space, tab, newline)
            if (token.compare(0, 1, "#") == 0) {  // comment line, skip forward until the end of the line
                SkipLine(s);
                continue;
            }
            ++token_idx;
            // non-comment line
            if (token != expected_token) {
                ERL_WARN("Expected token {}, got {}.", expected_token, token);  // check token
                return false;
            }
            try {
                if (!read_func(obj, s)) {
                    ERL_WARN("Failed to read {}.", expected_token);
                    return false;
                }
            } catch (const std::exception &e) {
                ERL_WARN("Exception while reading {}: {}", expected_token, e.what());
                return false;
            }
            if (token_idx == token_function_pairs.size()) { return true; }  // last token, return true
        }
        ERL_WARN("Failed to read {}. Truncated file?", typeid(*obj).name());
        return false;  // should not reach here
    }
}  // namespace erl::common
