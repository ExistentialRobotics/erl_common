#include <gtest/gtest.h>

#include "erl_common/grid_map_info.hpp"

TEST(GridMapInfoTest, Generate2DCellCoordinatesWithCStride) {

    using namespace erl::common;

    Eigen::Vector2i map_shape;
    map_shape << 3, 5;

    Eigen::Vector2d min = Eigen::Vector2d::Zero();
    Eigen::Vector2d max = Eigen::Vector2d::Ones();

    GridMapInfo<2> grid_map_info(map_shape, min, max);

    Eigen::Matrix<int, 2, 15> expect;
    // clang-format off
    expect << 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
              0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4;
    // clang-format on

    Eigen::Matrix<int, 2, 15> actual = grid_map_info.GenerateGridCoordinates(true);

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 15; ++j) { EXPECT_EQ(expect(i, j), actual(i, j)); }
    }
}

TEST(GridMapInfoTest, Generate2DCellCoordinatesWithFStride) {

    using namespace erl::common;

    Eigen::Vector2i map_shape;
    map_shape << 3, 5;

    Eigen::Vector2d min = Eigen::Vector2d::Zero();
    Eigen::Vector2d max = Eigen::Vector2d::Ones();

    GridMapInfo<2> grid_map_info(map_shape, min, max);

    Eigen::Matrix<int, 2, 15> expect;
    // clang-format off
    expect << 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2,
              0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4;
    // clang-format on

    Eigen::Matrix<int, 2, 15> actual = grid_map_info.GenerateGridCoordinates(false);

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 15; ++j) { EXPECT_EQ(expect(i, j), actual(i, j)); }
    }
}

TEST(GridMapInfoTest, Generate3DCellCoordinatesWithCStride) {

    using namespace erl::common;

    Eigen::Vector3i map_shape;
    map_shape << 3, 5, 7;

    Eigen::Vector3d min = Eigen::Vector3d::Zero();
    Eigen::Vector3d max = Eigen::Vector3d::Ones();

    GridMapInfo<3> grid_map_info(map_shape, min, max);

    Eigen::Matrix<int, 3, 3 * 5 * 7> actual = grid_map_info.GenerateGridCoordinates(true);

    int cnt = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 5; ++j) {
            for (int k = 0; k < 7; ++k) {
                Eigen::Vector3i expect;
                expect << i, j, k;
                for (int d = 0; d < 3; ++d) { EXPECT_EQ(expect[d], actual(d, cnt)); }
                cnt++;
            }
        }
    }
}

TEST(GridMapInfoTest, Generate3DCellCoordinatesWithFStride) {

    using namespace erl::common;

    Eigen::Vector3i map_shape;
    map_shape << 3, 5, 7;

    Eigen::Vector3d min = Eigen::Vector3d::Zero();
    Eigen::Vector3d max = Eigen::Vector3d::Ones();

    GridMapInfo<3> grid_map_info(map_shape, min, max);

    Eigen::Matrix<int, 3, 3 * 5 * 7> actual = grid_map_info.GenerateGridCoordinates(false);

    int cnt = 0;
    for (int k = 0; k < 7; ++k) {
        for (int j = 0; j < 5; ++j) {
            for (int i = 0; i < 3; ++i) {
                Eigen::Vector3i expect;
                expect << i, j, k;
                for (int d = 0; d < 3; ++d) { EXPECT_EQ(expect[d], actual(d, cnt)); }
                cnt++;
            }
        }
    }
}

TEST(GridMapInfoTest, Generate2DMeterCoordinatesWithCStride) {

    using namespace erl::common;

    Eigen::Vector2i map_shape;
    map_shape << 3, 5;

    Eigen::Vector2d min = Eigen::Vector2d::Zero();
    Eigen::Vector2d max = Eigen::Vector2d::Ones();

    GridMapInfo<2> grid_map_info(map_shape, min, max);
    Eigen::Matrix<double, 2, 15> actual = grid_map_info.GenerateMeterCoordinates(true);

    int cnt = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 5; ++j) {
            Eigen::Vector2d expect = grid_map_info.GridToMeterForPoints(Eigen::Vector2i{i, j});
            EXPECT_NEAR(expect[0], actual(0, cnt), 1.e-10);
            EXPECT_NEAR(expect[1], actual(1, cnt), 1.e-10);
            cnt++;
        }
    }
}

TEST(GridMapInfoTest, Generate2DMeterCoordinatesWithFStride) {

    using namespace erl::common;

    Eigen::Vector2i map_shape;
    map_shape << 3, 5;

    Eigen::Vector2d min = Eigen::Vector2d::Zero();
    Eigen::Vector2d max = Eigen::Vector2d::Ones();

    GridMapInfo<2> grid_map_info(map_shape, min, max);
    Eigen::Matrix<double, 2, 15> actual = grid_map_info.GenerateMeterCoordinates(false);

    int cnt = 0;
    for (int j = 0; j < 5; ++j) {
        for (int i = 0; i < 3; ++i) {
            Eigen::Vector2d expect = grid_map_info.GridToMeterForPoints(Eigen::Vector2i{i, j});
            EXPECT_NEAR(expect[0], actual(0, cnt), 1.e-10);
            EXPECT_NEAR(expect[1], actual(1, cnt), 1.e-10);
            cnt++;
        }
    }
}

TEST(GridMapInfoTest, Generate3DMeterCoordinatesWithCStride) {

    using namespace erl::common;

    Eigen::Vector3i map_shape;
    map_shape << 3, 5, 7;

    Eigen::Vector3d min = Eigen::Vector3d::Zero();
    Eigen::Vector3d max = Eigen::Vector3d::Ones();

    GridMapInfo<3> grid_map_info(map_shape, min, max);
    Eigen::Matrix<double, 3, 3 * 5 * 7> actual = grid_map_info.GenerateMeterCoordinates(true);

    int cnt = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 5; ++j) {
            for (int k = 0; k < 7; ++k) {
                Eigen::Vector3d expect = grid_map_info.GridToMeterForPoints(Eigen::Vector3i{i, j, k});
                EXPECT_NEAR(expect[0], actual(0, cnt), 1.e-10);
                EXPECT_NEAR(expect[1], actual(1, cnt), 1.e-10);
                EXPECT_NEAR(expect[2], actual(2, cnt), 1.e-10);
                cnt++;
            }
        }
    }
}

TEST(GridMapInfoTest, Generate3DMeterCoordinatesWithFStride) {

    using namespace erl::common;

    Eigen::Vector3i map_shape;
    map_shape << 3, 5, 7;

    Eigen::Vector3d min = Eigen::Vector3d::Zero();
    Eigen::Vector3d max = Eigen::Vector3d::Ones();

    GridMapInfo<3> grid_map_info(map_shape, min, max);
    Eigen::Matrix<double, 3, 3 * 5 * 7> actual = grid_map_info.GenerateMeterCoordinates(false);

    int cnt = 0;
    for (int k = 0; k < 7; ++k) {
        for (int j = 0; j < 5; ++j) {
            for (int i = 0; i < 3; ++i) {
                Eigen::Vector3d expect = grid_map_info.GridToMeterForPoints(Eigen::Vector3i{i, j, k});
                EXPECT_NEAR(expect[0], actual(0, cnt), 1.e-10);
                EXPECT_NEAR(expect[1], actual(1, cnt), 1.e-10);
                EXPECT_NEAR(expect[2], actual(2, cnt), 1.e-10);
                cnt++;
            }
        }
    }
}
