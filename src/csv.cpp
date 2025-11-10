#include "erl_common/csv.hpp"

#include "erl_common/logging.hpp"

namespace erl::common {

    std::vector<std::vector<std::string>>
    LoadCsvFile(const char *path, char delimiter) {
        std::ifstream ifs;
        ifs.open(path);
        ERL_ASSERTM(ifs.is_open(), "Fail to open file {}", path);

        std::vector<std::vector<std::string>> rows;
        std::string line;
        std::string cell;

        while (std::getline(ifs, line)) {
            std::stringstream ss(line);
            std::vector<std::string> row;

            while (std::getline(ss, cell, delimiter)) {
                const auto type_of_whitespaces = "\t\n\r ";
                auto first = cell.find_first_not_of(type_of_whitespaces);
                auto last = cell.find_last_not_of(type_of_whitespaces);
                cell = cell.substr(first, last - first + 1);
                row.push_back(cell);
            }

            rows.push_back(row);
        }

        ifs.close();
        return rows;
    }

    void
    SimpleCsv::Load(const char *path, char delimiter) {
        const std::vector<std::vector<std::string>> rows = LoadCsvFile(path, delimiter);

        header = rows[0];
        data.clear();
        for (const std::string &column_name: header) {
            ERL_ASSERTM(
                data.find(column_name) == data.end(),
                "Duplicate column name: {}",
                column_name);
            data[column_name] = std::vector<std::string>();
        }
        for (std::size_t i = 1; i < rows.size(); ++i) {
            std::size_t j = 0;
            const std::vector<std::string> &row = rows[i];
            for (const std::string &column_name: header) {
                if (row.size() <= j) {
                    ERL_WARN(
                        "Row size is smaller than header size. Column name: {}, row size: {}, "
                        "header size: {}",
                        column_name,
                        row.size(),
                        header.size());
                    data[column_name].emplace_back("");
                } else {
                    data[column_name].push_back(row[j]);
                }
                ++j;
            }
        }
    }

    void
    SimpleCsv::Save(const char *path, char delimiter) {
        std::vector<std::vector<std::string>> rows;
        rows.push_back(header);
        if (data.empty()) {
            SaveCsvFile(path, rows, delimiter);
            return;
        }
        for (std::size_t i = 0; i < data.begin()->second.size(); ++i) {
            std::vector<std::string> row;
            for (const std::string &column_name: header) { row.push_back(data[column_name][i]); }
            rows.push_back(row);
        }
        SaveCsvFile(path, rows, delimiter);
    }
}  // namespace erl::common
