#include <gtest/gtest.h>

#include <iostream>

#include "erl_common/eigen.hpp"

TEST(EigenTest, MatrixCreation) {

    Eigen::Matrix3d matrix;

    // clang-format off
    matrix << 1, 2, 3,
              4, 5, 6,
              7, 8, 9;
    // clang-format on

    std::cout << "number of bytes per element: " << sizeof(double) << std::endl;
    std::cout << matrix << std::endl;
}

TEST(EigenTest, FormatToString) {
    Eigen::Matrix3d matrix;

    // clang-format off
    matrix << 1, 2, 3,
              4, 5, 6,
              7, 8, 9;
    // clang-format on

    std::cout << matrix.format(erl::common::GetEigenTextFormat(erl::common::EigenTextFormat::kNumpyFmt)) << std::endl;
    std::cout << erl::common::EigenToString<erl::common::EigenTextFormat::kNumpyFmt>(matrix) << std::endl;
}

TEST(EigenTest, SaveAndLoadText) {

    using namespace erl::common;
    Eigen::Matrix4d matrix = Eigen::Matrix4d::Random();
    SaveEigenMatrixToTextFile<double>("matrix.txt", matrix);

    Eigen::Matrix4d matrix_load = LoadEigenMatrixFromTextFile<double, 4, 4>("matrix.txt");

    std::cout << "matrix: " << std::endl << matrix << std::endl;
    std::cout << "matrix_load: " << std::endl << matrix_load << std::endl;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            ASSERT_NEAR(matrix(i, j), matrix_load(i, j), 1e-6);
        }
    }
}

TEST(EigenTest, SaveAndLoadTextWithInf) {

    using namespace erl::common;
    Eigen::Matrix4d matrix = Eigen::Matrix4d::Random();
    matrix(0, 0) = std::numeric_limits<double>::infinity();
    SaveEigenMatrixToTextFile<double>("matrix.txt", matrix);

    Eigen::Matrix4d matrix_load = LoadEigenMatrixFromTextFile<double, 4, 4>("matrix.txt");

    std::cout << "matrix: " << std::endl << matrix << std::endl;
    std::cout << "matrix_load: " << std::endl << matrix_load << std::endl;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (std::isfinite(matrix(i, j))) {
                ASSERT_NEAR(matrix(i, j), matrix_load(i, j), 1e-6);
            } else {
                ASSERT_TRUE(std::isinf(matrix_load(i, j)));
            }
        }
    }
}

TEST(EigenTest, SaveAndLoadBinary) {

    using namespace erl::common;
    Eigen::Matrix4d matrix = Eigen::Matrix4d::Random();
    SaveEigenMatrixToBinaryFile<double>("matrix.bin", matrix);

    Eigen::Matrix4d matrix_load = LoadEigenMatrixFromBinaryFile<double, 4, 4>("matrix.bin");

    std::cout << "matrix: " << std::endl << matrix << std::endl;
    std::cout << "matrix_load: " << std::endl << matrix_load << std::endl;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            ASSERT_DOUBLE_EQ(matrix(i, j), matrix_load(i, j));
        }
    }
}

TEST(EigenTest, SaveAndLoadBinaryWithInf) {

    using namespace erl::common;
    Eigen::Matrix4d matrix = Eigen::Matrix4d::Random();
    matrix(0, 0) = std::numeric_limits<double>::infinity();
    SaveEigenMatrixToBinaryFile<double>("matrix.bin", matrix);

    Eigen::Matrix4d matrix_load = LoadEigenMatrixFromBinaryFile<double, 4, 4>("matrix.bin");

    std::cout << "matrix: " << std::endl << matrix << std::endl;
    std::cout << "matrix_load: " << std::endl << matrix_load << std::endl;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (std::isfinite(matrix(i, j))) {
                ASSERT_DOUBLE_EQ(matrix(i, j), matrix_load(i, j));
            } else {
                ASSERT_TRUE(std::isinf(matrix_load(i, j)));
            }
        }
    }
}
