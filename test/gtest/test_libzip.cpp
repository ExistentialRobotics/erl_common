#ifdef ERL_USE_LIBZIP
    #include "erl_common/libzip.hpp"
    #include "erl_common/test_helper.hpp"

TEST(LibZip, AddDirectory) {
    GTEST_PREPARE_OUTPUT_DIR();
    using namespace erl::common;
    EXPECT_TRUE(
        LibZip(test_output_dir / "add_directory.zip")
            .AddDirectory(gtest_src_dir.parent_path().parent_path().string(), "erl_common"));
}

TEST(LibZip, AddItems) {
    GTEST_PREPARE_OUTPUT_DIR();
    using namespace erl::common;
    const std::filesystem::path module_root = gtest_src_dir.parent_path().parent_path();
    EXPECT_TRUE(LibZip(test_output_dir / "add_items.zip")
                    .AddItems({
                        {module_root / "include", "erl_common/include"},
                        {module_root / "src", "erl_common/src"},
                        {module_root / "python", "erl_common/python"},
                        {module_root / "CMakeLists.txt", "erl_common/CMakeLists.txt"},
                    }));
}
#endif
