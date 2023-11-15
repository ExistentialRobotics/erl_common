#pragma once

#include <random>
#include <vector>
#include <numeric>
#include <algorithm>
#include <Eigen/Dense>

namespace erl::common {

    extern std::mt19937 g_random_engine;

    template<typename T>
    std::vector<T>
    GenerateShuffledIndices(std::size_t size) {
        std::vector<T> indices(size);
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), g_random_engine);
        return indices;
    }

    inline Eigen::VectorXd
    GenerateGaussianNoise(long size, double mean, double scale) {
        Eigen::VectorXd noise(size);
        std::normal_distribution<double> distribution(mean, scale);
        for (long i = 0; i < size; ++i) { noise[i] = distribution(common::g_random_engine); }
        return noise;
    }

    inline Eigen::MatrixXd
    GenerateGaussianNoise(long rows, long cols, double mean, double scale) {
        Eigen::MatrixXd noise(rows, cols);
        std::normal_distribution<double> distribution(mean, scale);
        for (long i = 0; i < rows; ++i) {
            for (long j = 0; j < cols; ++j) { noise(i, j) = distribution(common::g_random_engine); }
        }
        return noise;
    }
}  // namespace erl::common
