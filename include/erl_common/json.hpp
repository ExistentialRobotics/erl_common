#pragma once

#include "eigen.hpp"

#include <nlohmann/json.hpp>

namespace nlohmann {

    template<
        typename T,
        int Rows = Eigen::Dynamic,
        int Cols = Eigen::Dynamic,
        int Order = Eigen::ColMajor>
    struct ConvertEigenMatrix {
        static void
        to_json(json& array, const Eigen::Matrix<T, Rows, Cols, Order>& mat) {
            array = json::array();
            const int rows = Rows == Eigen::Dynamic ? mat.rows() : Rows;
            const int cols = Cols == Eigen::Dynamic ? mat.cols() : Cols;

            if (Order == Eigen::RowMajor) {
                for (int i = 0; i < rows; ++i) {
                    auto row_array = json::array();
                    for (int j = 0; j < cols; ++j) { row_array.push_back(mat(i, j)); }
                    array.push_back(row_array);
                }
            } else {
                for (int j = 0; j < cols; ++j) {
                    auto col_array = json::array();
                    for (int i = 0; i < rows; ++i) { col_array.push_back(mat(i, j)); }
                    array.push_back(col_array);
                }
            }
        }

        static void
        from_json(const json& array, Eigen::Matrix<T, Rows, Cols, Order>& mat) {
            if (!array.is_array()) {
                throw std::invalid_argument("The json type must be an array.");
            }
            if (array.empty()) { throw std::invalid_argument("The json array must not be empty."); }
            if (!array[0].is_array()) {
                throw std::invalid_argument("The json array must be a 2D array.");
            }

            if (Order == Eigen::RowMajor) {
                int rows = Rows == Eigen::Dynamic ? array.size() : Rows;
                int cols = Cols == Eigen::Dynamic ? array[0].size() : Cols;
                mat.resize(rows, cols);

                for (int i = 0; i < rows; ++i) {
                    auto& row_array = array[i];
                    for (int j = 0; j < cols; ++j) { row_array.at(j).get_to(mat(i, j)); }
                }
            } else {
                int rows = Rows == Eigen::Dynamic ? array[0].size() : Rows;
                int cols = Cols == Eigen::Dynamic ? array.size() : Cols;
                mat.resize(rows, cols);

                for (int j = 0; j < cols; ++j) {
                    auto& col_array = array[j];
                    for (int i = 0; i < rows; ++i) { col_array.at(i).get_to(mat(i, j)); }
                }
            }
        }
    };

    template<>
    struct adl_serializer<Eigen::Matrix2i> : ConvertEigenMatrix<int, 2, 2> {};

    template<>
    struct adl_serializer<Eigen::Matrix2f> : ConvertEigenMatrix<float, 2, 2> {};

    template<>
    struct adl_serializer<Eigen::Matrix2d> : ConvertEigenMatrix<double, 2, 2> {};

    template<>
    struct adl_serializer<Eigen::Matrix2Xi> : ConvertEigenMatrix<int, 2> {};

    template<>
    struct adl_serializer<Eigen::Matrix2Xf> : ConvertEigenMatrix<float, 2> {};

    template<>
    struct adl_serializer<Eigen::Matrix2Xd> : ConvertEigenMatrix<double, 2> {};

    template<>
    struct adl_serializer<Eigen::MatrixX2i> : ConvertEigenMatrix<int, Eigen::Dynamic, 2> {};

    template<>
    struct adl_serializer<Eigen::MatrixX2f> : ConvertEigenMatrix<float, Eigen::Dynamic, 2> {};

    template<>
    struct adl_serializer<Eigen::MatrixX2d> : ConvertEigenMatrix<double, Eigen::Dynamic, 2> {};

    template<>
    struct adl_serializer<Eigen::Matrix3i> : ConvertEigenMatrix<int, 3, 3> {};

    template<>
    struct adl_serializer<Eigen::Matrix3f> : ConvertEigenMatrix<float, 3, 3> {};

    template<>
    struct adl_serializer<Eigen::Matrix3d> : ConvertEigenMatrix<double, 3, 3> {};

    template<>
    struct adl_serializer<Eigen::Matrix3Xi> : ConvertEigenMatrix<int, 3> {};

    template<>
    struct adl_serializer<Eigen::Matrix3Xf> : ConvertEigenMatrix<float, 3> {};

    template<>
    struct adl_serializer<Eigen::Matrix3Xd> : ConvertEigenMatrix<double, 3> {};

    template<>
    struct adl_serializer<Eigen::MatrixX3i> : ConvertEigenMatrix<int, Eigen::Dynamic, 3> {};

    template<>
    struct adl_serializer<Eigen::MatrixX3f> : ConvertEigenMatrix<float, Eigen::Dynamic, 3> {};

    template<>
    struct adl_serializer<Eigen::MatrixX3d> : ConvertEigenMatrix<double, Eigen::Dynamic, 3> {};

    template<>
    struct adl_serializer<Eigen::Matrix4i> : ConvertEigenMatrix<int, 4, 4> {};

    template<>
    struct adl_serializer<Eigen::Matrix4f> : ConvertEigenMatrix<float, 4, 4> {};

    template<>
    struct adl_serializer<Eigen::Matrix4d> : ConvertEigenMatrix<double, 4, 4> {};

    template<>
    struct adl_serializer<Eigen::Matrix4Xi> : ConvertEigenMatrix<int, 4> {};

    template<>
    struct adl_serializer<Eigen::Matrix4Xf> : ConvertEigenMatrix<float, 4> {};

    template<>
    struct adl_serializer<Eigen::Matrix4Xd> : ConvertEigenMatrix<double, 4> {};

    template<>
    struct adl_serializer<Eigen::MatrixX4i> : ConvertEigenMatrix<int, Eigen::Dynamic, 4> {};

    template<>
    struct adl_serializer<Eigen::MatrixX4f> : ConvertEigenMatrix<float, Eigen::Dynamic, 4> {};

    template<>
    struct adl_serializer<Eigen::MatrixX4d> : ConvertEigenMatrix<double, Eigen::Dynamic, 4> {};

    template<>
    struct adl_serializer<Eigen::MatrixXi> : ConvertEigenMatrix<int> {};

    template<>
    struct adl_serializer<Eigen::MatrixXf> : ConvertEigenMatrix<float> {};

    template<>
    struct adl_serializer<Eigen::MatrixXd> : ConvertEigenMatrix<double> {};

    template<typename T, int Size = Eigen::Dynamic>
    struct ConvertEigenVector {
        static void
        to_json(json& array, const Eigen::Matrix<T, Size, 1>& vec) {
            array = json::array();
            for (int i = 0; i < Size; ++i) { array.push_back(vec(i)); }
        }

        static void
        from_json(const json& array, Eigen::Matrix<T, Size, 1>& vec) {
            if (!array.is_array()) {
                throw std::invalid_argument("The json type must be an array.");
            }

            unsigned int size = Size;
            if (Size == Eigen::Dynamic) {
                size = array.size();
                vec.resize(size);
            } else if (array.size() != Size) {
                throw std::invalid_argument("The json array size must be " + std::to_string(Size));
            }

            for (unsigned int i = 0; i < size; ++i) { array.at(i).get_to(vec[i]); }
        }
    };

    template<>
    struct adl_serializer<Eigen::Vector2i> : ConvertEigenVector<int, 2> {};

    template<>
    struct adl_serializer<Eigen::Vector2f> : ConvertEigenVector<float, 2> {};

    template<>
    struct adl_serializer<Eigen::Vector2d> : ConvertEigenVector<double, 2> {};

    template<>
    struct adl_serializer<Eigen::Vector3i> : ConvertEigenVector<int, 3> {};

    template<>
    struct adl_serializer<Eigen::Vector3f> : ConvertEigenVector<float, 3> {};

    template<>
    struct adl_serializer<Eigen::Vector3d> : ConvertEigenVector<double, 3> {};

    template<>
    struct adl_serializer<Eigen::Vector4i> : ConvertEigenVector<int, 4> {};

    template<>
    struct adl_serializer<Eigen::Vector4f> : ConvertEigenVector<float, 4> {};

    template<>
    struct adl_serializer<Eigen::Vector4d> : ConvertEigenVector<double, 4> {};

    template<>
    struct adl_serializer<Eigen::VectorXi> : ConvertEigenVector<int> {};

    template<>
    struct adl_serializer<Eigen::VectorXf> : ConvertEigenVector<float> {};

    template<>
    struct adl_serializer<Eigen::VectorXd> : ConvertEigenVector<double> {};
}  // namespace nlohmann
