#include "erl_common/data_buffer_manager.hpp"
#include "erl_common/test_helper.hpp"

TEST(DataBufferManager, Basic) {
    using namespace erl::common;
    using Manager = DataBufferManager<int>;
    Manager manager;

    constexpr int num_entries = 10;
    std::vector<std::size_t> indices;
    indices.reserve(num_entries);
    for (int i = 0; i < num_entries; ++i) { indices.push_back(manager.AddEntry(i)); }
    EXPECT_EQ(manager.Size(), num_entries);
    for (int i = 0; i < num_entries; ++i) { EXPECT_EQ(manager[i], i); }

    ASSERT_TRUE(Serialization<Manager>::Write("data_buffer_manager.bin", &manager));
    Manager manager_read;
    ASSERT_TRUE(Serialization<Manager>::Read("data_buffer_manager.bin", &manager_read));
    EXPECT_EQ(manager, manager_read);
}
