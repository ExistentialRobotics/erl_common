#pragma once

#include "logging.hpp"

#include <filesystem>
#include <istream>
#include <vector>

namespace erl::common {
    /**
     * Read a line until a delimiter is found.
     * @param stream The input stream to skip the line from.
     * @param delimiter The delimiter to stop at. The default is "\n".
     */
    inline void
    SkipLine(std::istream &stream, const char delimiter = '\n') {
        char c;
        do { c = static_cast<char>(stream.get()); } while (stream.good() && c != delimiter);
    }

    /**
     * Write tokens to an output stream.
     * @tparam T Object type
     * @tparam TokenFunctionPair Pair of token and function to write.
     * @param s The output stream to write to.
     * @param obj The object to write.
     * @param token_function_pairs The pairs of tokens and function to execute.
     * @return
     */
    template<typename T, typename TokenFunctionPair>
    bool
    WriteTokens(
        std::ostream &s,
        const T *obj,
        const std::vector<TokenFunctionPair> &token_function_pairs) {
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

    /**
     * Read tokens from an input stream.
     * @tparam T Object type.
     * @tparam TokenFunctionPair Pair of token and function to read.
     * @param s The input stream to read from.
     * @param obj The object to store the read data.
     * @param token_function_pairs The pairs of tokens and function to execute.
     * @return
     */
    template<typename T, typename TokenFunctionPair>
    bool
    ReadTokens(
        std::istream &s,
        T *obj,
        const std::vector<TokenFunctionPair> &token_function_pairs) {
        std::string token;
        std::size_t token_idx = 0;
        for (const auto &[expected_token, read_func]: token_function_pairs) {
            // skip leading whitespaces before reading and stop at the first whitespace
            // (space, tab, newline)
            s >> token;
            if (token.compare(0, 1, "#") == 0) {
                // comment line, skip forward until the end of the line
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
            // last token, return true
            if (token_idx == token_function_pairs.size()) { return true; }
        }
        ERL_WARN("Failed to read {}. Truncated file?", typeid(*obj).name());
        return false;  // should not reach here
    }

    /**
     * Template specialization for serialization.
     * @tparam T Object type.
     */
    template<typename T>
    struct Serialization {
        [[nodiscard]] static bool
        Write(const std::string &filename, const T &data) {
            std::string type_str = type_name(data);
            ERL_INFO("Writing {} to {}.", type_str, filename);
            std::filesystem::create_directories(std::filesystem::path(filename).parent_path());
            std::ofstream ofs(filename, std::ios_base::out | std::ios_base::binary);
            if (!ofs.is_open()) {
                ERL_WARN("Failed to open file {} for writing.", filename);
                return false;
            }
            ofs << "# " << type_str
                << "\n# (feel free to add / change comments, but leave the first line as it is!)\n";
            const bool success = data.Write(ofs);
            ofs << "end_of_" << type_str << "\n";  // write end token
            ofs.close();
            return success;
        }

        [[nodiscard]] static bool
        Read(const std::string &filename, T &data) {
            std::string type_str = type_name(data);
            ERL_INFO("Reading {} from {}.", type_str, filename);
            std::ifstream ifs(filename, std::ios_base::in | std::ios_base::binary);
            if (!ifs.is_open()) {
                ERL_WARN("Failed to open file {} for reading.", filename);
                return false;
            }
            std::string line;
            std::getline(ifs, line);
            if (std::string file_header = fmt::format("# {}", type_str);
                line != file_header) {  // check if the first line is valid
                ERL_WARN("Header does not start with \"{}\"", file_header);
                return false;
            }
            bool success = data.Read(ifs);
            if (!success) {
                ERL_WARN("Failed to read {} from file.", type_str);
                return false;
            }
            std::getline(ifs, line);
            if (line != "end_of_" + type_str) {  // check if the last line is valid
                ERL_WARN("Last line does not end with \"end_of_{}\"", type_str);
                return false;
            }
            return success;
        }
    };
}  // namespace erl::common
