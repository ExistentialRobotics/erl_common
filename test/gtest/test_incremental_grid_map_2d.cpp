#include "erl_common/grid_map.hpp"
#include "erl_common/test_helper.hpp"

TEST(IncrementalGridMap2DTest, DataAccess) {
    using namespace erl::common;

    const auto grid_map_info = std::make_shared<GridMapInfo2Dd>(
        Eigen::Vector2d(0, 0),
        Eigen::Vector2d(1.0, 1.0),
        Eigen::Vector2d(0.1, 0.1),
        Eigen::Vector2i(0, 0));
    IncrementalGridMap2D<std::shared_ptr<int>, double> grid_map(grid_map_info);

    const int n_rows = grid_map_info->Shape(0);
    const int n_cols = grid_map_info->Shape(1);
    for (int i = 0; i < n_rows; ++i) {
        for (int j = 0; j < n_cols; ++j) {
            ASSERT_EQ(grid_map(i, j), nullptr);  // check the default value
        }
    }

    for (int i = 0; i < n_rows; ++i) {
        for (int j = 0; j < n_cols; ++j) {  // test assignment
            grid_map.GetMutableData(i, j) = std::make_shared<int>(i * n_rows + j);
        }
    }

    for (int i = 0; i < n_rows; ++i) {
        for (int j = 0; j < n_cols; ++j) {
            ASSERT_EQ(*grid_map(i, j), i * n_rows + j);
        }  // check the value
    }
}

TEST(IncrementalGridMap2DTest, ExtendToTopLeft) {
    using namespace erl::common;

    auto grid_map_info = std::make_shared<GridMapInfo2Dd>(
        Eigen::Vector2d(0, 0),
        Eigen::Vector2d(1.0, 1.0),
        Eigen::Vector2d(0.1, 0.1),
        Eigen::Vector2i(0, 0));
    IncrementalGridMap2D<std::shared_ptr<int>, double> grid_map(grid_map_info);
    ASSERT_EQ(grid_map_info->Shape(0), 11);
    ASSERT_EQ(grid_map_info->Shape(1), 11);
    double x_res = grid_map_info->Resolution(0);
    double y_res = grid_map_info->Resolution(1);

    double x = -1.0;
    double y = -1.0;

    auto &data = grid_map.GetMutableData(x, y);
    ASSERT_EQ(data, nullptr);
    data = std::make_shared<int>(1);

    auto new_grid_map_info = grid_map.GetGridMapInfo();
    ASSERT_EQ(new_grid_map_info->Shape(0), 23);
    ASSERT_EQ(new_grid_map_info->Shape(1), 23);
    ASSERT_EQ(new_grid_map_info->Min(0), -1 - x_res);
    ASSERT_EQ(new_grid_map_info->Min(1), -1 - y_res);

    int x_grid = new_grid_map_info->MeterToGridForValue(x, 0);
    int y_grid = new_grid_map_info->MeterToGridForValue(y, 1);
    ASSERT_EQ(*grid_map.GetMutableData(x_grid, y_grid), 1);
}

TEST(IncrementalGridMap2DTest, ExtendToCentralLeft) {
    using namespace erl::common;

    auto grid_map_info = std::make_shared<GridMapInfo2Dd>(
        Eigen::Vector2d(0, 0),
        Eigen::Vector2d(1.0, 1.0),
        Eigen::Vector2d(0.1, 0.1),
        Eigen::Vector2i(0, 0));
    IncrementalGridMap2D<std::shared_ptr<int>, double> grid_map(grid_map_info);
    ASSERT_EQ(grid_map_info->Shape(0), 11);
    ASSERT_EQ(grid_map_info->Shape(1), 11);
    double x_res = grid_map_info->Resolution(0);
    double y_res = grid_map_info->Resolution(1);

    double x = 0.5;
    double y = -1.0;

    auto &data = grid_map.GetMutableData(x, y);
    ASSERT_EQ(data, nullptr);
    data = std::make_shared<int>(1);

    auto new_grid_map_info = grid_map.GetGridMapInfo();
    ASSERT_EQ(new_grid_map_info->Shape(0), 23);
    ASSERT_EQ(new_grid_map_info->Shape(1), 23);
    ASSERT_EQ(new_grid_map_info->Min(0), -1 - x_res);
    ASSERT_EQ(new_grid_map_info->Min(1), -1 - y_res);

    int x_grid = new_grid_map_info->MeterToGridForValue(x, 0);
    int y_grid = new_grid_map_info->MeterToGridForValue(y, 1);
    ASSERT_EQ(*grid_map.GetMutableData(x_grid, y_grid), 1);
}

TEST(IncrementalGridMap2DTest, ExtendToTopCentral) {
    using namespace erl::common;

    auto grid_map_info = std::make_shared<GridMapInfo2Dd>(
        Eigen::Vector2d(0, 0),
        Eigen::Vector2d(1.0, 1.0),
        Eigen::Vector2d(0.1, 0.1),
        Eigen::Vector2i(0, 0));
    IncrementalGridMap2D<std::shared_ptr<int>, double> grid_map(grid_map_info);
    ASSERT_EQ(grid_map_info->Shape(0), 11);
    ASSERT_EQ(grid_map_info->Shape(1), 11);
    double x_res = grid_map_info->Resolution(0);
    double y_res = grid_map_info->Resolution(1);

    double x = -1.0;
    double y = 0.5;

    auto &data = grid_map.GetMutableData(x, y);
    ASSERT_EQ(data, nullptr);
    data = std::make_shared<int>(1);

    auto new_grid_map_info = grid_map.GetGridMapInfo();
    ASSERT_EQ(new_grid_map_info->Shape(0), 23);
    ASSERT_EQ(new_grid_map_info->Shape(1), 23);
    ASSERT_EQ(new_grid_map_info->Min(0), -1 - x_res);
    ASSERT_EQ(new_grid_map_info->Max(1), 2 + y_res);

    int x_grid = new_grid_map_info->MeterToGridForValue(x, 0);
    int y_grid = new_grid_map_info->MeterToGridForValue(y, 1);
    ASSERT_EQ(*grid_map.GetMutableData(x_grid, y_grid), 1);
}

TEST(IncrementalGridMap2DTest, ExtendToTopRight) {
    using namespace erl::common;

    auto grid_map_info = std::make_shared<GridMapInfo2Dd>(
        Eigen::Vector2d(0, 0),
        Eigen::Vector2d(1.0, 1.0),
        Eigen::Vector2d(0.1, 0.1),
        Eigen::Vector2i(0, 0));
    IncrementalGridMap2D<std::shared_ptr<int>, double> grid_map(grid_map_info);
    ASSERT_EQ(grid_map_info->Shape(0), 11);
    ASSERT_EQ(grid_map_info->Shape(1), 11);
    double x_res = grid_map_info->Resolution(0);
    double y_res = grid_map_info->Resolution(1);

    double x = -1.0;
    double y = 2.0;

    auto &data = grid_map.GetMutableData(x, y);
    ASSERT_EQ(data, nullptr);
    data = std::make_shared<int>(1);

    auto new_grid_map_info = grid_map.GetGridMapInfo();
    ASSERT_EQ(new_grid_map_info->Shape(0), 23);
    ASSERT_EQ(new_grid_map_info->Shape(1), 23);
    ASSERT_EQ(new_grid_map_info->Min(0), -1 - x_res);
    ASSERT_EQ(new_grid_map_info->Max(1), 2 + y_res);

    int x_grid = new_grid_map_info->MeterToGridForValue(x, 0);
    int y_grid = new_grid_map_info->MeterToGridForValue(y, 1);
    ASSERT_EQ(*grid_map.GetMutableData(x_grid, y_grid), 1);
}

TEST(IncrementalGridMap2DTest, ExtendToCentalRight) {
    using namespace erl::common;

    auto grid_map_info = std::make_shared<GridMapInfo2Dd>(
        Eigen::Vector2d(0, 0),
        Eigen::Vector2d(1.0, 1.0),
        Eigen::Vector2d(0.1, 0.1),
        Eigen::Vector2i(0, 0));
    IncrementalGridMap2D<std::shared_ptr<int>, double> grid_map(grid_map_info);
    ASSERT_EQ(grid_map_info->Shape(0), 11);
    ASSERT_EQ(grid_map_info->Shape(1), 11);
    double x_res = grid_map_info->Resolution(0);
    double y_res = grid_map_info->Resolution(1);

    double x = 0.5;
    double y = 2.0;

    auto &data = grid_map.GetMutableData(x, y);
    ASSERT_EQ(data, nullptr);
    data = std::make_shared<int>(1);

    auto new_grid_map_info = grid_map.GetGridMapInfo();
    ASSERT_EQ(new_grid_map_info->Shape(0), 23);
    ASSERT_EQ(new_grid_map_info->Shape(1), 23);
    ASSERT_EQ(new_grid_map_info->Max(0), 2 + x_res);
    ASSERT_EQ(new_grid_map_info->Max(1), 2 + y_res);

    int x_grid = new_grid_map_info->MeterToGridForValue(x, 0);
    int y_grid = new_grid_map_info->MeterToGridForValue(y, 1);
    ASSERT_EQ(*grid_map.GetMutableData(x_grid, y_grid), 1);
}

TEST(IncrementalGridMap2DTest, ExtendToBottomRight) {
    using namespace erl::common;

    auto grid_map_info = std::make_shared<GridMapInfo2Dd>(
        Eigen::Vector2d(0, 0),
        Eigen::Vector2d(1.0, 1.0),
        Eigen::Vector2d(0.1, 0.1),
        Eigen::Vector2i(0, 0));
    IncrementalGridMap2D<std::shared_ptr<int>, double> grid_map(grid_map_info);
    ASSERT_EQ(grid_map_info->Shape(0), 11);
    ASSERT_EQ(grid_map_info->Shape(1), 11);
    double x_res = grid_map_info->Resolution(0);
    double y_res = grid_map_info->Resolution(1);

    double x = 2.0;
    double y = 2.0;

    auto &data = grid_map.GetMutableData(x, y);
    ASSERT_EQ(data, nullptr);
    data = std::make_shared<int>(1);

    auto new_grid_map_info = grid_map.GetGridMapInfo();
    ASSERT_EQ(new_grid_map_info->Shape(0), 23);
    ASSERT_EQ(new_grid_map_info->Shape(1), 23);
    ASSERT_EQ(new_grid_map_info->Max(0), 2 + x_res);
    ASSERT_EQ(new_grid_map_info->Max(1), 2 + y_res);

    int x_grid = new_grid_map_info->MeterToGridForValue(x, 0);
    int y_grid = new_grid_map_info->MeterToGridForValue(y, 1);
    ASSERT_EQ(*grid_map.GetMutableData(x_grid, y_grid), 1);
}

TEST(IncrementalGridMap2DTest, ExtendToBottomCentral) {
    using namespace erl::common;

    auto grid_map_info = std::make_shared<GridMapInfo2Dd>(
        Eigen::Vector2d(0, 0),
        Eigen::Vector2d(1.0, 1.0),
        Eigen::Vector2d(0.1, 0.1),
        Eigen::Vector2i(0, 0));
    IncrementalGridMap2D<std::shared_ptr<int>, double> grid_map(grid_map_info);
    ASSERT_EQ(grid_map_info->Shape(0), 11);
    ASSERT_EQ(grid_map_info->Shape(1), 11);
    double x_res = grid_map_info->Resolution(0);
    double y_res = grid_map_info->Resolution(1);

    double x = 2.0;
    double y = 0.5;

    auto &data = grid_map.GetMutableData(x, y);
    ASSERT_EQ(data, nullptr);
    data = std::make_shared<int>(1);

    auto new_grid_map_info = grid_map.GetGridMapInfo();
    ASSERT_EQ(new_grid_map_info->Shape(0), 23);
    ASSERT_EQ(new_grid_map_info->Shape(1), 23);
    ASSERT_EQ(new_grid_map_info->Max(0), 2 + x_res);
    ASSERT_EQ(new_grid_map_info->Min(1), -1 - y_res);

    int x_grid = new_grid_map_info->MeterToGridForValue(x, 0);
    int y_grid = new_grid_map_info->MeterToGridForValue(y, 1);
    ASSERT_EQ(*grid_map.GetMutableData(x_grid, y_grid), 1);
}

TEST(IncrementalGridMap2DTest, ExtendToBottomLeft) {
    using namespace erl::common;

    auto grid_map_info = std::make_shared<GridMapInfo2Dd>(
        Eigen::Vector2d(0, 0),
        Eigen::Vector2d(1.0, 1.0),
        Eigen::Vector2d(0.1, 0.1),
        Eigen::Vector2i(0, 0));
    IncrementalGridMap2D<std::shared_ptr<int>, double> grid_map(grid_map_info);
    ASSERT_EQ(grid_map_info->Shape(0), 11);
    ASSERT_EQ(grid_map_info->Shape(1), 11);
    double x_res = grid_map_info->Resolution(0);
    double y_res = grid_map_info->Resolution(1);

    double x = 2;
    double y = -1;

    auto &data = grid_map.GetMutableData(x, y);
    ASSERT_EQ(data, nullptr);
    data = std::make_shared<int>(1);

    auto new_grid_map_info = grid_map.GetGridMapInfo();
    ASSERT_EQ(new_grid_map_info->Shape(0), 23);
    ASSERT_EQ(new_grid_map_info->Shape(1), 23);
    ASSERT_EQ(new_grid_map_info->Max(0), 2 + x_res);
    ASSERT_EQ(new_grid_map_info->Min(1), -1 - y_res);

    int x_grid = new_grid_map_info->MeterToGridForValue(x, 0);
    int y_grid = new_grid_map_info->MeterToGridForValue(y, 1);
    ASSERT_EQ(*grid_map.GetMutableData(x_grid, y_grid), 1);
}
