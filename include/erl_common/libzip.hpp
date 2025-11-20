#pragma once

#ifdef ERL_USE_LIBZIP

    #include "logging.hpp"

    #include <zip.h>

    #include <filesystem>

namespace erl::common {
    class LibZip {

        int m_error_ = 0;
        zip_t *m_zip_ = nullptr;
        std::string m_zip_filepath_;

    public:
        explicit LibZip(const std::string &zip_filepath)
            : m_zip_(zip_open(zip_filepath.c_str(), ZIP_CREATE | ZIP_EXCL, &m_error_)),
              m_zip_filepath_(zip_filepath) {
            if (m_zip_ == nullptr) {
                zip_error_t zip_error;
                zip_error_init_with_code(&zip_error, m_error_);
                const std::string error_msg = fmt::format(
                    "Failed to create zip file {}: {}",
                    zip_filepath,
                    zip_error_strerror(&zip_error));
                zip_error_fini(&zip_error);
                ERL_FATAL(error_msg);  // throw error
            }
        }

        ~LibZip() {
            if (m_zip_ != nullptr) {
                if (zip_close(m_zip_) < 0) {
                    ERL_ERROR(
                        "Failed to close zip file {}: {}",
                        m_zip_filepath_,
                        zip_strerror(m_zip_));
                }
                m_zip_ = nullptr;
            }
        }

        [[nodiscard]] bool
        AddFile(const std::string &filepath, const std::string &filename_in_zip) const {
            zip_source_t *source = zip_source_file(m_zip_, filepath.c_str(), 0, 0);
            if (source == nullptr) {
                ERL_ERROR("Failed to add file {} as {} in zip", filepath, filename_in_zip);
                return false;
            }
            if (zip_file_add(
                    m_zip_,
                    filename_in_zip.c_str(),
                    source,
                    ZIP_FL_ENC_UTF_8 | ZIP_FL_OVERWRITE) < 0) {
                ERL_ERROR(
                    "Failed to add file {} as {} in zip: {}",
                    filepath,
                    filename_in_zip,
                    zip_strerror(m_zip_));
                zip_source_free(source);
                return false;
            }
            return true;
        }

        [[nodiscard]] bool
        AddDirectory(const std::string &dirpath, const std::string &dirname_in_zip)
            const {  // NOLINT(*-no-recursion)
            ERL_ASSERTM(std::filesystem::is_directory(dirpath), "Not a directory: {}", dirpath);
            auto directory_itr = std::filesystem::directory_iterator(dirpath);
            return std::all_of(
                std::filesystem::begin(directory_itr),
                std::filesystem::end(directory_itr),
                [this, &dirname_in_zip](const auto &entry) {  // NOLINT(*-no-recursion)
                    const std::string entry_path = entry.path().string();
                    const std::string entry_name = entry.path().filename().string();
                    const std::string next_dirname_in_zip =
                        dirname_in_zip + "/" +
                        entry_name;  // NOLINT(*-inefficient-string-concatenation)
                    if (entry.is_directory()) {
                        return AddDirectory(entry_path, next_dirname_in_zip);
                    }
                    if (entry.is_regular_file()) {
                        return AddFile(entry_path, next_dirname_in_zip);
                    }
                    ERL_ERROR("Unsupported file {}.", entry_path);
                    return false;
                });
        }

        [[nodiscard]] bool
        AddItems(
            const std::vector<std::pair<std::string, std::string>> &item_and_name_in_zip) const {
            return std::all_of(
                item_and_name_in_zip.begin(),
                item_and_name_in_zip.end(),
                [this](const auto &pair) {
                    if (std::filesystem::is_directory(pair.first)) {
                        return AddDirectory(pair.first, pair.second);
                    }
                    if (std::filesystem::is_regular_file(pair.first)) {
                        return AddFile(pair.first, pair.second);
                    }
                    ERL_ERROR("Unsupported file {}.", pair.first);
                    return false;
                });
        }
    };
}  // namespace erl::common

#endif
