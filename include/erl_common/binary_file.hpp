#pragma once
#include "logging.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace erl::common {
    template<typename T>
    std::vector<T>
    LoadBinaryFile(const char *path) {
        std::ifstream ifs;
        ifs.open(path, std::ios::binary);
        ERL_ASSERTM(ifs.is_open(), "{} does not exist in {}.", path, std::filesystem::current_path());
        auto data = std::vector<char>(std::istreambuf_iterator<char>(ifs), {});
        ifs.close();

        T *begin = reinterpret_cast<T *>(data.data());
        T *end = begin + data.size() / sizeof(T);
        std::vector<T> reinterpreted_data(begin, end);

        return reinterpreted_data;
    }

    template<typename T>
    bool
    SaveBinaryFile(const char *path, const T *data, const std::streamsize n) {
        std::ofstream ofs;
        ofs.open(path, std::ios::out | std::ios::binary);
        if (ofs.is_open()) {
            ofs.write(reinterpret_cast<const char *>(data), n * sizeof(T));
            ofs.close();
            return true;
        }
        return false;
    }

}  // namespace erl::common
