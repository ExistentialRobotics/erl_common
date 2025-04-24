#include "erl_common/eigen.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <random>

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

    std::cout << matrix.format(GetEigenTextFormat(erl::common::EigenTextFormat::kNumpyFmt)) << std::endl;
    std::cout << erl::common::EigenToString<erl::common::EigenTextFormat::kNumpyFmt>(matrix) << std::endl;
}

TEST(EigenTest, SaveAndLoadText) {

    using namespace erl::common;
    Eigen::Matrix4d matrix = Eigen::Matrix4d::Random();
    SaveEigenMatrixToTextFile<double>("matrix.txt", matrix);

    Eigen::Matrix4d matrix_load = LoadEigenMatrixFromTextFile<double, 4, 4>("matrix.txt");

    // std::cout << "matrix: " << std::endl << matrix << std::endl;
    // std::cout << "matrix_load: " << std::endl << matrix_load << std::endl;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) { ASSERT_NEAR(matrix(i, j), matrix_load(i, j), 1e-6); }
    }
}

TEST(EigenTest, SaveAndLoadTextWithInf) {

    using namespace erl::common;
    Eigen::Matrix4d matrix = Eigen::Matrix4d::Random();
    matrix(0, 0) = std::numeric_limits<double>::infinity();
    SaveEigenMatrixToTextFile<double>("matrix.txt", matrix);

    Eigen::Matrix4d matrix_load = LoadEigenMatrixFromTextFile<double, 4, 4>("matrix.txt");

    // std::cout << "matrix: " << std::endl << matrix << std::endl;
    // std::cout << "matrix_load: " << std::endl << matrix_load << std::endl;

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
    ASSERT_TRUE(SaveEigenMatrixToBinaryFile<double>("matrix.bin", matrix));

    Eigen::Matrix4d matrix_load = LoadEigenMatrixFromBinaryFile<double, 4, 4>("matrix.bin");

    // std::cout << "matrix: " << std::endl << matrix << std::endl;
    // std::cout << "matrix_load: " << std::endl << matrix_load << std::endl;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) { ASSERT_DOUBLE_EQ(matrix(i, j), matrix_load(i, j)); }
    }
}

TEST(EigenTest, SaveAndLoadBinaryWithInf) {

    using namespace erl::common;
    Eigen::Matrix4d matrix = Eigen::Matrix4d::Random();
    matrix(0, 0) = std::numeric_limits<double>::infinity();
    ASSERT_TRUE(SaveEigenMatrixToBinaryFile<double>("matrix.bin", matrix));

    Eigen::Matrix4d matrix_load = LoadEigenMatrixFromBinaryFile<double, 4, 4>("matrix.bin");

    // std::cout << "matrix: " << std::endl << matrix << std::endl;
    // std::cout << "matrix_load: " << std::endl << matrix_load << std::endl;

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

TEST(EigenTest, SaveAndLoadEigenMap) {
    using namespace erl::common;
    Eigen::MatrixXd matrix = Eigen::MatrixXd::Random(10, 5);
    Eigen::Map<const Eigen::MatrixXd> matrix_map(matrix.data(), matrix.rows(), matrix.cols());
    std::ofstream ofs("matrix.bin", std::ios::binary);
    ASSERT_TRUE(SaveEigenMapToBinaryStream(ofs, matrix_map));
    ofs.close();

    Eigen::MatrixXd matrix_load(matrix.rows(), matrix.cols());
    std::ifstream ifs("matrix.bin", std::ios::binary);
    ASSERT_TRUE(LoadEigenMapFromBinaryStream(ifs, Eigen::Map<Eigen::MatrixXd>(matrix_load.data(), matrix.rows(), matrix.cols())));
    ifs.close();

    EXPECT_TRUE(matrix.cwiseEqual(matrix_load).all());
}

TEST(EigenTest, SaveAndLoadVectorOfFixedSizedMatrices) {
    using namespace erl::common;
    std::vector<Eigen::Matrix4d> matrices(10);
    for (auto& matrix: matrices) { matrix = Eigen::Matrix4d::Random(); }

    std::ofstream ofs("matrices.bin", std::ios::binary);
    ASSERT_TRUE(SaveVectorOfEigenMatricesToBinaryStream(ofs, matrices));
    ofs.close();

    std::vector<Eigen::Matrix4d> matrices_load;
    std::ifstream ifs("matrices.bin", std::ios::binary);
    ASSERT_TRUE(LoadVectorOfEigenMatricesFromBinaryStream(ifs, matrices_load));
    ifs.close();

    for (std::size_t i = 0; i < matrices.size(); ++i) { EXPECT_TRUE(matrices[i].cwiseEqual(matrices_load[i]).all()); }
}

TEST(EigenTest, SaveAndLoadVectorOfDynamicSizedMatrices) {
    using namespace erl::common;
    std::uniform_int_distribution dist(1, 10);
    std::mt19937 gen(0);
    std::vector<Eigen::MatrixXd> matrices(10);
    for (auto& matrix: matrices) { matrix = Eigen::MatrixXd::Random(dist(gen), dist(gen)); }

    std::ofstream ofs("matrices.bin", std::ios::binary);
    ASSERT_TRUE(SaveVectorOfEigenMatricesToBinaryStream(ofs, matrices));
    ofs.close();

    std::vector<Eigen::MatrixXd> matrices_load;
    std::ifstream ifs("matrices.bin", std::ios::binary);
    ASSERT_TRUE(LoadVectorOfEigenMatricesFromBinaryStream(ifs, matrices_load));
    ifs.close();

    for (std::size_t i = 0; i < matrices.size(); ++i) { EXPECT_TRUE(matrices[i].cwiseEqual(matrices_load[i]).all()); }
}

TEST(EigenTest, SaveAndLoadEigenMatrixOfFixedSizedEigenMatrices) {
    using namespace erl::common;
    Eigen::MatrixX<Eigen::Matrix4d> matrices(10, 5);
    for (int i = 0; i < matrices.rows(); ++i) {
        for (int j = 0; j < matrices.cols(); ++j) { matrices(i, j) = Eigen::Matrix4d::Random(); }
    }

    std::ofstream ofs("matrices.bin", std::ios::binary);
    ASSERT_TRUE(SaveEigenMatrixOfEigenMatricesToBinaryStream(ofs, matrices));
    ofs.close();

    Eigen::MatrixX<Eigen::Matrix4d> matrices_load;
    std::ifstream ifs("matrices.bin", std::ios::binary);
    ASSERT_TRUE(LoadEigenMatrixOfEigenMatricesFromBinaryStream(ifs, matrices_load));
    ifs.close();

    for (int i = 0; i < matrices.rows(); ++i) {
        for (int j = 0; j < matrices.cols(); ++j) { EXPECT_TRUE(matrices(i, j).cwiseEqual(matrices_load(i, j)).all()); }
    }
}

#include <eigen3/unsupported/Eigen/CXX11/Tensor>

TEST(EigenTest, Tensor) {
    Eigen::Tensor<double, 3> tensor(3, 4, 5);  //
    tensor(0, 0, 0);
}

TEST(EigenTest, Solve) {
    Eigen::Matrix3d A = Eigen::Matrix3d::Random();
    A = (A + A.transpose()).eval();  // make it symmetric
    A = A * A.transpose();           // make it positive definite
    Eigen::Matrix3d B = Eigen::Matrix3d::Random();
    Eigen::Matrix3d L = A.llt().matrixL();
    Eigen::Matrix3d X = L.triangularView<Eigen::Lower>().solve(B);
    L.transpose().triangularView<Eigen::Upper>().solveInPlace(X);
    std::cout << "A = " << std::endl << A << std::endl;
    std::cout << "L * L.T = " << std::endl << L * L.transpose() << std::endl;
    std::cout << "B = " << std::endl << B << std::endl;
    std::cout << "A * X.col(i) = " << std::endl;
    std::cout << (A * X.col(0)).eval().transpose() << std::endl;
    std::cout << (A * X.col(1)).eval().transpose() << std::endl;
    std::cout << (A * X.col(2)).eval().transpose() << std::endl;
}
