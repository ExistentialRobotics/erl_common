#include "erl_common/csv.hpp"
#include "erl_common/logging.hpp"

#include <gtest/gtest.h>

using namespace erl::common;

TEST(CsvIoTest, ReadWrite) {

    const std::vector<std::vector<double>> csv_gt{{1., 2., 3., 4., 5.}, {3., 4., 5.}};

    SaveCsvFile("example.csv", csv_gt);

    const auto csv_rows = LoadCsvFile("example.csv");
    for (int i = 0; i < static_cast<int>(csv_rows.size()); i++) {
        for (int j = 0; j < static_cast<int>(csv_rows[i].size()); j++) {
            double ans = std::stod(csv_rows[i][j]);
            ASSERT_EQ(ans, csv_gt[i][j]);
        }
    }

    const auto csv_data = LoadAndCastCsvFile<double>("example.csv", [](const std::string &string) {
        return std::stod(string);
    });
    for (int i = 0; i < static_cast<int>(csv_data.size()); i++) {
        for (int j = 0; j < static_cast<int>(csv_data[i].size()); j++) {
            ASSERT_EQ(csv_data[i][j], csv_gt[i][j]);
        }
    }
}
