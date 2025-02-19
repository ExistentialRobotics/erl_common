#include "erl_common/data_buffer_manager.hpp"
#include "erl_common/test_helper.hpp"

TEST(DataBufferManager, Basic) {
    using namespace erl::common;
    DataBufferManager<int> manager;
}
