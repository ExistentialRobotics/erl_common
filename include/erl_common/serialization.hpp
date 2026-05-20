#pragma once

#include "logging.hpp"
#include "string_utils.hpp"

#include <filesystem>
#include <functional>
#include <istream>
#include <vector>

namespace erl::common::serialization {
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
            do {
                s >> token;
                if (token.compare(0, 1, "#") == 0) {
                    SkipLine(s);  // comment line, skip forward until the end of the line
                    continue;
                }
                break;
            } while (true);
            ++token_idx;
            // non-comment line
            if (token != expected_token) {
                ERL_WARN("Expected token {}, got {}.", expected_token, token);  // check token
                return false;
            }
            try {
                SkipLine(s);
                if (!read_func(obj, s)) {
                    ERL_WARN("Failed to read {}.", expected_token);
                    return false;
                }
                SkipLine(s);
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

    template<typename T, typename = void>
    struct Writer {
        static bool
        Run(const T *entry, std::ostream &stream) {
            stream.write(reinterpret_cast<const char *>(entry), sizeof(T));
            return stream.good();
        }
    };

    template<typename T>
    struct Writer<T, std::void_t<decltype(std::declval<T>().Write())>> {
        static bool
        Run(const T *entry, std::ostream &stream) {
            return entry->Write(stream);
        }
    };

    template<typename T, typename = void>
    struct Reader {
        static bool
        Run(T *entry, std::istream &stream) {
            stream.read(reinterpret_cast<char *>(entry), sizeof(T));
            return stream.good();
        }
    };

    template<typename T>
    struct Reader<T, std::void_t<decltype(std::declval<T>().Read())>> {
        static bool
        Run(T *entry, std::istream &stream) {
            return entry->Read(stream);
        }
    };

    /**
     * Template specialization for serialization.
     * @tparam T Object type.
     * @note It is better to pass the object as a pointer in case of polymorphism.
     */
    template<typename T>
    struct Serialization {

        // ---- filename overloads: open the file, then delegate to the stream overloads ----

        [[nodiscard]] static bool
        Write(const std::string &filename, const std::shared_ptr<T> &data) {
            return Write(filename, data.get());
        }

        [[nodiscard]] static bool
        Write(const std::string &filename, const std::shared_ptr<const T> &data) {
            if (data == nullptr) {
                ERL_WARN("Data is nullptr.");
                return false;
            }
            return Write(filename, data.get());
        }

        [[nodiscard]] static bool
        Write(const std::string &filename, T *data) {
            return Write(filename, static_cast<const T *>(data));
        }

        [[nodiscard]] static bool
        Write(const std::string &filename, const T *data) {
            ERL_INFO("Writing {} to {}.", type_name(*data), filename);
            std::ofstream ofs;
            if (!OpenForWrite(filename, ofs)) { return false; }
            return Write(ofs, data);
        }

        [[nodiscard]] static bool
        Read(const std::string &filename, const std::shared_ptr<T> &data) {
            if (data == nullptr) {
                ERL_WARN("Data is nullptr.");
                return false;
            }
            return Read(filename, data.get());
        }

        [[nodiscard]] static bool
        Read(const std::string &filename, T *data) {
            ERL_INFO("Reading {} from {}.", type_name(*data), filename);
            std::ifstream ifs;
            if (!OpenForRead(filename, ifs)) { return false; }
            return Read(ifs, data);
        }

        template<typename Func>
        [[nodiscard]] static bool
        Write(const std::string &filename, Func func) {
            ERL_INFO("Writing {} to {}.", type_name<T>(), filename);
            std::ofstream ofs;
            if (!OpenForWrite(filename, ofs)) { return false; }
            return Write(ofs, std::move(func));
        }

        template<typename Func>
        [[nodiscard]] static bool
        Read(const std::string &filename, Func func) {
            ERL_INFO("Reading {} from {}.", type_name<T>(), filename);
            std::ifstream ifs;
            if (!OpenForRead(filename, ifs)) { return false; }
            return Read(ifs, std::move(func));
        }

        // ---- stream overloads: own the header / body / footer protocol ----

        [[nodiscard]] static bool
        Write(std::ostream &ofs, const std::shared_ptr<T> &data) {
            return Write(ofs, data.get());
        }

        [[nodiscard]] static bool
        Write(std::ostream &ofs, const std::shared_ptr<const T> &data) {
            if (data == nullptr) {
                ERL_WARN("Data is nullptr.");
                return false;
            }
            return Write(ofs, data.get());
        }

        [[nodiscard]] static bool
        Write(std::ostream &ofs, T *data) {
            return Write(ofs, static_cast<const T *>(data));
        }

        [[nodiscard]] static bool
        Write(std::ostream &ofs, const T *data) {
            const std::string type_str = type_name(*data);
            WriteHeader(ofs, type_str);
            const bool success = data->Write(ofs);
            WriteFooter(ofs, type_str);
            return success;
        }

        [[nodiscard]] static bool
        Read(std::istream &ifs, const std::shared_ptr<T> &data) {
            if (data == nullptr) {
                ERL_WARN("Data is nullptr.");
                return false;
            }
            return Read(ifs, data.get());
        }

        [[nodiscard]] static bool
        Read(std::istream &ifs, T *data) {
            const std::string type_str = type_name(*data);
            if (!ReadHeader(ifs, type_str)) { return false; }
            if (!data->Read(ifs)) {
                ERL_WARN("Failed to read {} from stream.", type_str);
                return false;
            }
            return ReadFooter(ifs, type_str);
        }

        template<typename Func>
        [[nodiscard]] static bool
        Write(std::ostream &ofs, Func func) {
            const std::string type_str = type_name<T>();
            WriteHeader(ofs, type_str);
            const bool success = func(ofs);
            WriteFooter(ofs, type_str);
            return success;
        }

        template<typename Func>
        [[nodiscard]] static bool
        Read(std::istream &ifs, Func func) {
            const std::string type_str = type_name<T>();
            if (!ReadHeader(ifs, type_str)) { return false; }
            if (!func(ifs)) {
                ERL_WARN("Failed to read {} from stream.", type_str);
                return false;
            }
            return ReadFooter(ifs, type_str);
        }

    private:
        static bool
        OpenForWrite(const std::string &filename, std::ofstream &ofs) {
            const std::filesystem::path folder = std::filesystem::absolute(filename).parent_path();
            std::filesystem::create_directories(folder);
            ofs.open(filename, std::ios_base::out | std::ios_base::binary);
            if (!ofs.is_open()) {
                ERL_WARN("Failed to open file {} for writing.", filename);
                return false;
            }
            return true;
        }

        static bool
        OpenForRead(const std::string &filename, std::ifstream &ifs) {
            ifs.open(filename, std::ios_base::in | std::ios_base::binary);
            if (!ifs.is_open()) {
                ERL_WARN("Failed to open file {} for reading.", filename);
                return false;
            }
            return true;
        }

        static void
        WriteHeader(std::ostream &os, const std::string &type_str) {
            os << "# " << type_str
               << "\n# (feel free to add / change comments, but leave the first line as it is!)\n";
        }

        static void
        WriteFooter(std::ostream &os, const std::string &type_str) {
            os << "end_of_" << type_str << '\n';
        }

        static bool
        ReadHeader(std::istream &is, const std::string &type_str) {
            std::string line;
            std::getline(is, line);
            if (const std::string file_header = "# " + type_str; line != file_header) {
                ERL_WARN("Header does not start with \"{}\"", file_header);
                return false;
            }
            return true;
        }

        static bool
        ReadFooter(std::istream &is, const std::string &type_str) {
            std::string line;
            std::getline(is, line);
            if (const std::string end_token = "end_of_" + type_str; line != end_token) {
                ERL_WARN("Last line does not end with \"end_of_{}\" but \"{}\"", type_str, line);
                return false;
            }
            return true;
        }
    };

    template<typename T>
    using TokenWriteFunctionPair =
        std::pair<const char *, std::function<bool(const T *, std::ostream &)>>;

    template<typename T>
    using TokenWriteFunctionPairs = std::vector<TokenWriteFunctionPair<T>>;

    template<typename T>
    using TokenReadFunctionPair = std::pair<const char *, std::function<bool(T *, std::istream &)>>;

    template<typename T>
    using TokenReadFunctionPairs = std::vector<TokenReadFunctionPair<T>>;
}  // namespace erl::common::serialization
