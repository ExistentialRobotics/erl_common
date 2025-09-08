#include "erl_common/storage_order.hpp"
#include "erl_common/test_helper.hpp"

TEST(StorageOrderTest, CoordsToIndexTiming) {

    using namespace erl::common;

    Eigen::Vector<int, 5> shape;
    shape << 2, 3, 4, 5, 6;
    Eigen::Vector<int, 5> coords;
    coords << 1, 2, 2, 3, 3;

    int index_1, index_2;
    const auto strides = ComputeCStrides<int>(shape, 1);
    ReportTime<std::chrono::nanoseconds>("CoordsToIndex(shape, coords)", 100, false, [&] {
        index_1 = CoordsToIndex<int, 5>(shape, coords, true);
    });
    ReportTime<std::chrono::nanoseconds>("CoordsToIndex(strides, coords)", 100, false, [&] {
        index_2 = CoordsToIndex<int, 5>(strides, coords);
    });
    ASSERT_EQ(index_1, index_2);
}
