#pragma once

#include "logging.hpp"

#include <Eigen/Dense>

#include <fstream>

// https://stackoverflow.com/questions/4433950/overriding-functions-from-dynamic-libraries
// https://danieldk.eu/Posts/2020-08-31-MKL-Zen.html
extern "C" {
// ReSharper disable once CppInconsistentNaming
[[maybe_unused]] int
mkl_serv_intel_cpu_true();
}

namespace Eigen {
    // MATRIX
    using MatrixXl = MatrixX<long>;
    using MatrixXb = MatrixX<bool>;
    using MatrixX8U = MatrixX<uint8_t>;
    using Matrix6Xd = Matrix<double, 6, Dynamic>;
    using Matrix2Xl = Matrix<long, 2, Dynamic>;
    using Matrix3Xl = Matrix<long, 3, Dynamic>;
    using Matrix23d = Matrix<double, 2, 3>;
    using Matrix24d = Matrix<double, 2, 4>;
    using Matrix34d = Matrix<double, 3, 4>;

    template<typename T, int Rows, int Cols>
    using RMatrix = Matrix<T, Rows, Cols, RowMajor>;

    using RMatrix23d = RMatrix<double, 2, 3>;
    using RMatrix2Xd = RMatrix<double, 2, Dynamic>;

    template<typename T>
    using Scalar = Matrix<T, 1, 1>;
    using Scalari = Scalar<int>;
    using Scalard = Scalar<double>;

    // VECTOR
    using Vector2l = Vector2<long>;
    using VectorXl = VectorX<long>;
    using VectorXb = VectorX<bool>;
    using VectorX8U = VectorX<uint8_t>;
}  // namespace Eigen

namespace erl::common {
    // https://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html
    enum class EigenTextFormat {
        kDefaultFmt = 0,
        kCommaInitFmt = 1,
        kCleanFmt = 2,
        kOctaveFmt = 3,
        kNumpyFmt = 4,
        kCsvFmt = 5,
    };

    Eigen::IOFormat
    GetEigenTextFormat(EigenTextFormat format);

    template<typename T>
    void
    SaveEigenMatrixToTextFile(
        const std::string& file_path,
        const Eigen::Ref<const Eigen::MatrixX<T>>& matrix,
        const EigenTextFormat format = EigenTextFormat::kDefaultFmt) {
        std::ofstream ofs(file_path);
        if (!ofs.is_open()) { throw std::runtime_error("Could not open file " + file_path); }
        ofs << matrix.format(GetEigenTextFormat(format));
        ofs.close();
    }

    template<typename T, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic, int RowMajor = Eigen::ColMajor>
    Eigen::Matrix<T, Rows, Cols, RowMajor>
    LoadEigenMatrixFromTextFile(const std::string& file_path, const EigenTextFormat format = EigenTextFormat::kDefaultFmt, const bool transpose = false) {
        std::vector<T> data;
        std::ifstream ifs(file_path);

        if (!ifs.is_open()) { throw std::runtime_error("Could not open file " + file_path); }

        std::string row_string;
        std::string entry_string;
        int cols = 0;
        int rows = 0;

        char delim = ',';
        switch (format) {
            case EigenTextFormat::kDefaultFmt:
                delim = ' ';
                break;
            case EigenTextFormat::kCommaInitFmt:
            case EigenTextFormat::kCleanFmt:
            case EigenTextFormat::kOctaveFmt:
            case EigenTextFormat::kNumpyFmt:
            case EigenTextFormat::kCsvFmt:
                delim = ',';
                break;
        }

        while (std::getline(ifs, row_string)) {
            std::stringstream row_stream(row_string);
            int row_cols = 0;
            while (std::getline(row_stream, entry_string, delim)) {
                if (entry_string.empty()) { continue; }
                if (entry_string == ";" || entry_string == "]" || entry_string == "[") { continue; }
                data.push_back(T(std::stod(entry_string)));
                if (rows == 0) {
                    cols++;
                } else {
                    row_cols++;
                }
            }
            if (rows == 0) {
                ERL_ASSERTM(
                    Cols == Eigen::Dynamic || cols == Cols,
                    "Number of columns in file does not match template parameter. Expected {}, got {}",
                    Cols,
                    cols);
            } else {
                ERL_ASSERTM(cols == row_cols, "Invalid matrix file: row {} has {} columns, expected {}", rows, row_cols, cols);
            }
            rows++;
        }
        ifs.close();

        ERL_ASSERTM(Rows == Eigen::Dynamic || rows == Rows, "Number of rows in file does not match template parameter. Expected {}, got {}", Rows, rows);

        if (RowMajor == Eigen::RowMajor) {
            Eigen::MatrixX<T> matrix(rows, cols);
            std::copy(data.begin(), data.end(), matrix.data());
            return matrix;
        }
        Eigen::MatrixX<T> matrix(cols, rows);
        std::copy(data.begin(), data.end(), matrix.data());
        if (transpose) {
            ERL_ASSERTM((Rows == Eigen::Dynamic && Cols == Eigen::Dynamic) || rows == cols, "transpose=true requires square or dynamic-size matrix.");
            return matrix;
        }
        return matrix.transpose();
    }

    template<typename T = double, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic>
    [[nodiscard]] bool
    SaveEigenMapToBinaryStream(std::ostream& s, const Eigen::Map<const Eigen::Matrix<T, Rows, Cols>>& matrix) {
        const long matrix_size = matrix.size();
        s.write(reinterpret_cast<const char*>(&matrix_size), sizeof(long));
        if (matrix_size == 0) {
            // ERL_WARN("Writing empty matrix to stream.");
            return s.good();
        }
        const long matrix_shape[2] = {matrix.rows(), matrix.cols()};
        s.write(reinterpret_cast<const char*>(matrix_shape), 2 * sizeof(long));
        s.write(reinterpret_cast<const char*>(matrix.data()), matrix.size() * sizeof(T));
        return s.good();
    }

    template<typename T = double, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic>
    [[nodiscard]] bool
    SaveEigenMatrixToBinaryStream(std::ostream& s, const Eigen::Matrix<T, Rows, Cols>& matrix) {
        return SaveEigenMapToBinaryStream<T, Rows, Cols>(s, Eigen::Map<const Eigen::Matrix<T, Rows, Cols>>(matrix.data(), matrix.rows(), matrix.cols()));
    }

    template<typename T = double, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic>
    [[nodiscard]] bool
    SaveEigenMatrixToBinaryFile(const std::string& file_path, const Eigen::Matrix<T, Rows, Cols>& matrix) {
        std::ofstream ofs(file_path, std::ios::binary);
        if (!ofs.is_open()) { throw std::runtime_error("Could not open file " + file_path); }
        const bool success = SaveEigenMatrixToBinaryStream(ofs, matrix);
        ofs.close();
        return success;
    }

    template<typename T, int Rows, int Cols>
    [[nodiscard]] bool
    SaveVectorOfEigenMatricesToBinaryStream(std::ostream& s, const std::vector<Eigen::Matrix<T, Rows, Cols>>& matrices) {
        const std::size_t num_matrices = matrices.size();
        s.write(reinterpret_cast<const char*>(&num_matrices), sizeof(std::size_t));
        if (Rows != Eigen::Dynamic && Cols != Eigen::Dynamic) {
            return SaveEigenMapToBinaryStream<T, Rows * Cols, Eigen::Dynamic>(
                s,
                Eigen::Map<const Eigen::Matrix<T, Rows * Cols, Eigen::Dynamic>>(matrices.data()->data(), Rows * Cols, static_cast<long>(num_matrices)));
        }
        for (const auto& matrix: matrices) {
            if (!SaveEigenMatrixToBinaryStream<T, Rows, Cols>(s, matrix)) { return false; }
        }
        return s.good();
    }

    template<typename T, int Rows1, int Cols1, int Rows2 = Eigen::Dynamic, int Cols2 = Eigen::Dynamic>
    [[nodiscard]] bool
    SaveEigenMatrixOfEigenMatricesToBinaryStream(std::ostream& s, const Eigen::Matrix<Eigen::Matrix<T, Rows1, Cols1>, Rows2, Cols2>& matrix_of_matrices) {
        const long rows = matrix_of_matrices.rows();
        const long cols = matrix_of_matrices.cols();
        s.write(reinterpret_cast<const char*>(&rows), sizeof(long));
        s.write(reinterpret_cast<const char*>(&cols), sizeof(long));
        if (rows == 0 || cols == 0) {
            ERL_WARN("Writing empty matrix to stream.");
            return s.good();
        }
        if (Rows1 != Eigen::Dynamic && Cols1 != Eigen::Dynamic) {
            // for performance and smaller file, storage for fixed size matrices is assumed to be contiguous
            return SaveEigenMapToBinaryStream<T, Rows1 * Cols1, Eigen::Dynamic>(
                s,
                Eigen::Map<const Eigen::Matrix<T, Rows1 * Cols1, Eigen::Dynamic>>(matrix_of_matrices.data()->data(), Rows1 * Cols1, rows * cols));
        }
        // Rows1 == Eigen::Dynamic or Cols1 == Eigen::Dynamic
        // warning: the storage of the matrix_of_matrices may not be contiguous
        for (long j = 0; j < cols; j++) {
            for (long i = 0; i < rows; i++) {
                if (!SaveEigenMatrixToBinaryStream<T, Rows1, Cols1>(s, matrix_of_matrices(i, j))) { return false; }
            }
        }
        return s.good();
    }

    template<typename T = double, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic>
    [[nodiscard]] bool
    LoadEigenMatrixFromBinaryStream(std::istream& s, Eigen::Matrix<T, Rows, Cols>& matrix) {
        long matrix_size = 0;
        s.read(reinterpret_cast<char*>(&matrix_size), sizeof(long));
        if (matrix_size == 0) {
            if constexpr (Rows == Eigen::Dynamic || Cols == Eigen::Dynamic) {
                // ERL_WARN("Reading empty matrix from stream.");
                return true;
            } else if (matrix_size != Rows * Cols) {
                ERL_WARN("Matrix size mismatch. Expected {}, got {}", Rows * Cols, matrix_size);
                return false;
            }
        }

        long matrix_shape[2];
        s.read(reinterpret_cast<char*>(matrix_shape), 2 * sizeof(long));
        if (Rows != Eigen::Dynamic && matrix_shape[0] != Rows) {
            ERL_WARN("Number of rows in file does not match template parameter. Expected {}, got {}", Rows, matrix_shape[0]);
            return false;
        }
        if (Cols != Eigen::Dynamic && matrix_shape[1] != Cols) {
            ERL_WARN("Number of columns in file does not match template parameter. Expected {}, got {}", Cols, matrix_shape[1]);
            return false;
        }
        if (matrix_size != matrix_shape[0] * matrix_shape[1]) {
            ERL_WARN("Matrix size mismatch. Expected {}, got {}", matrix_size, matrix_shape[0] * matrix_shape[1]);
            return false;
        }

        if constexpr (Rows == Eigen::Dynamic || Cols == Eigen::Dynamic) {
            if (matrix.rows() != matrix_shape[0] || matrix.cols() != matrix_shape[1]) { matrix.resize(matrix_shape[0], matrix_shape[1]); }
        }
        s.read(reinterpret_cast<char*>(matrix.data()), static_cast<long>(matrix_size * sizeof(T)));
        if (!s.good()) {
            ERL_WARN("Error reading matrix from stream.");
            return false;
        }
        return s.good();
    }

    template<typename T = double, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic>
    Eigen::Matrix<T, Rows, Cols>
    LoadEigenMatrixFromBinaryFile(const std::string& file_path) {
        std::ifstream ifs(file_path, std::ios::binary);
        if (!ifs.is_open()) { throw std::runtime_error("Could not open file " + file_path); }
        Eigen::Matrix<T, Rows, Cols> matrix;
        ERL_ASSERTM(LoadEigenMatrixFromBinaryStream(ifs, matrix), "Error reading matrix from file.");
        ifs.close();
        return matrix;
    }

    template<typename T = double, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic>
    [[nodiscard]] bool
    LoadEigenMapFromBinaryStream(std::istream& s, Eigen::Map<Eigen::Matrix<T, Rows, Cols>> matrix) {
        long matrix_size = 0;
        s.read(reinterpret_cast<char*>(&matrix_size), sizeof(long));
        if (matrix_size == 0) {
            if constexpr (Rows == Eigen::Dynamic || Cols == Eigen::Dynamic) { ERL_WARN("Reading empty matrix from stream."); }
            return false;
        }

        long matrix_shape[2];
        s.read(reinterpret_cast<char*>(matrix_shape), 2 * sizeof(long));
        if (Rows != Eigen::Dynamic && matrix_shape[0] != Rows) {
            ERL_WARN("Number of rows in file does not match template parameter. Expected {}, got {}", Rows, matrix_shape[0]);
            return false;
        }
        if (Cols != Eigen::Dynamic && matrix_shape[1] != Cols) {
            ERL_WARN("Number of columns in file does not match template parameter. Expected {}, got {}", Cols, matrix_shape[1]);
            return false;
        }
        if (matrix_size != matrix_shape[0] * matrix_shape[1]) {
            ERL_WARN("Matrix size mismatch. Expected {}, got {}", matrix_size, matrix_shape[0] * matrix_shape[1]);
            return false;
        }
        if (matrix.rows() != matrix_shape[0]) {
            ERL_WARN("Matrix rows mismatch. Expected {}, got {}", matrix_shape[0], matrix.rows());
            return false;
        }
        if (matrix.cols() != matrix_shape[1]) {
            ERL_WARN("Matrix cols mismatch. Expected {}, got {}", matrix_shape[1], matrix.cols());
            return false;
        }

        s.read(reinterpret_cast<char*>(matrix.data()), static_cast<long>(matrix_size * sizeof(T)));
        if (!s.good()) {
            ERL_WARN("Error reading matrix from stream.");
            return false;
        }
        return s.good();
    }

    template<typename T = double, int Rows, int Cols>
    [[nodiscard]] bool
    LoadVectorOfEigenMatricesFromBinaryStream(std::istream& s, std::vector<Eigen::Matrix<T, Rows, Cols>>& matrices) {
        std::size_t num_matrices = 0;
        s.read(reinterpret_cast<char*>(&num_matrices), sizeof(std::size_t));
        if (num_matrices == 0) {
            ERL_WARN("Reading empty matrix from stream.");
            return s.good();
        }
        matrices.resize(num_matrices);

        if (Rows != Eigen::Dynamic && Cols != Eigen::Dynamic) {
            return LoadEigenMapFromBinaryStream<T, Rows * Cols, Eigen::Dynamic>(
                s,
                Eigen::Map<Eigen::Matrix<T, Rows * Cols, Eigen::Dynamic>>(matrices.data()->data(), Rows * Cols, static_cast<long>(num_matrices)));
        }

        for (auto& matrix: matrices) {
            if (!LoadEigenMatrixFromBinaryStream<T, Rows, Cols>(s, matrix)) { return false; }
        }
        return s.good();
    }

    template<typename T, int Rows1, int Cols1, int Rows2 = Eigen::Dynamic, int Cols2 = Eigen::Dynamic>
    [[nodiscard]] bool
    LoadEigenMatrixOfEigenMatricesFromBinaryStream(std::istream& s, Eigen::Matrix<Eigen::Matrix<T, Rows1, Cols1>, Rows2, Cols2>& matrix_of_matrices) {
        long rows, cols;
        s.read(reinterpret_cast<char*>(&rows), sizeof(long));
        s.read(reinterpret_cast<char*>(&cols), sizeof(long));
        if (rows == 0 || cols == 0) {
            ERL_WARN("Reading empty matrix from stream.");
            return s.good();
        }
        if (Rows2 == Eigen::Dynamic || Cols2 == Eigen::Dynamic) { matrix_of_matrices.resize(rows, cols); }
        if (Rows1 != Eigen::Dynamic && Cols1 != Eigen::Dynamic) {
            // for performance and smaller file, storage for fixed size matrices is assumed to be contiguous
            return LoadEigenMapFromBinaryStream<T, Rows1 * Cols1, Eigen::Dynamic>(
                s,
                Eigen::Map<Eigen::Matrix<T, Rows1 * Cols1, Eigen::Dynamic>>(matrix_of_matrices.data()->data(), Rows1 * Cols1, rows * cols));
        }
        // Rows1 == Eigen::Dynamic or Cols1 == Eigen::Dynamic
        // warning: the storage of the matrix_of_matrices may not be contiguous
        for (long j = 0; j < cols; j++) {
            for (long i = 0; i < rows; i++) {
                if (!LoadEigenMatrixFromBinaryStream<T, Rows1, Cols1>(s, matrix_of_matrices(i, j))) { return false; }
            }
        }
        return s.good();
    }

    template<EigenTextFormat Format, typename Matrix>
    std::string
    EigenToString(const Matrix& matrix) {
        std::stringstream ss;
        ss << matrix.format(GetEigenTextFormat(Format));
        return ss.str();
    }

    template<typename Matrix>
    std::string
    EigenToDefaultFmtString(const Matrix& matrix) {
        std::stringstream ss;
        ss << matrix.format(GetEigenTextFormat(EigenTextFormat::kDefaultFmt));
        return ss.str();
    }

    template<typename Matrix>
    std::string
    EigenToCommaInitFmtString(const Matrix& matrix) {
        std::stringstream ss;
        ss << matrix.format(GetEigenTextFormat(EigenTextFormat::kCommaInitFmt));
        return ss.str();
    }

    template<typename Matrix>
    std::string
    EigenToCleanFmtString(const Matrix& matrix) {
        std::stringstream ss;
        ss << matrix.format(GetEigenTextFormat(EigenTextFormat::kCleanFmt));
        return ss.str();
    }

    template<typename Matrix>
    std::string
    EigenToOctaveFmtString(const Matrix& matrix) {
        std::stringstream ss;
        ss << matrix.format(GetEigenTextFormat(EigenTextFormat::kOctaveFmt));
        return ss.str();
    }

    template<typename Matrix>
    std::string
    EigenToNumPyFmtString(const Matrix& matrix) {
        std::stringstream ss;
        ss << matrix.format(GetEigenTextFormat(EigenTextFormat::kNumpyFmt));
        return ss.str();
    }

    template<typename Matrix>
    std::string
    EigenToCsvFmtString(const Matrix& matrix) {
        std::stringstream ss;
        ss << matrix.format(GetEigenTextFormat(EigenTextFormat::kCsvFmt));
        return ss.str();
    }
}  // namespace erl::common
