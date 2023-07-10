#include <gtest/gtest.h>

#include "erl_common/assert.hpp"
#include "erl_common/binary_file.hpp"
using namespace erl::common;

TEST(BinaryFileIoTest, ReadWrite) {

    std::vector<double> data_gt{1., 2., 3.};
    const char *path = "example.bin";

    SaveBinaryFile(path, data_gt.data(), std::streamsize(data_gt.size()));

    auto data_loaded = LoadBinaryFile<double>(path);

    ASSERT_EQ(data_gt.size(), data_loaded.size());

    for (std::size_t i = 0; i < data_loaded.size(); ++i) { ASSERT_EQ(data_gt[i], data_loaded[i]); }
}
