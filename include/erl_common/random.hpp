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
}  // namespace erl::common
