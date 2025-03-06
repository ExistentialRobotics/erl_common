#include "pybind11_erl_common.hpp"

void
BindStorage(py::module &m) {
    using namespace erl::common;
    auto submodule = m.def_submodule("storage", "This module handles linear continuous data storage.");

    submodule
        .def(
            "compute_c_strides",
            [](const std::vector<int> &shape, const int item_size) { return ComputeCStrides(shape, item_size); },
            py::arg("shape"),
            py::arg("item_size"))
        .def(
            "compute_c_strides",
            [](const Eigen::Ref<const Eigen::VectorX<int>> &shape, const int item_size) { return ComputeCStrides(shape, item_size); },
            py::arg("shape"),
            py::arg("item_size"))
        .def(
            "compute_f_strides",
            [](const std::vector<int> &shape, const int item_size) { return ComputeFStrides(shape, item_size); },
            py::arg("shape"),
            py::arg("item_size"))
        .def(
            "compute_f_strides",
            [](const Eigen::Ref<const Eigen::VectorX<int>> &shape, const int item_size) { return ComputeFStrides(shape, item_size); },
            py::arg("shape"),
            py::arg("item_size"));

    BindGridMapInfo<double, Eigen::Dynamic>(submodule, "GridMapInfoXDd");
    BindGridMapInfo<float, Eigen::Dynamic>(submodule, "GridMapInfoXDf");
    BindGridMapInfo<double, 2>(submodule, "GridMapInfo2Dd");
    BindGridMapInfo<float, 2>(submodule, "GridMapInfo2Df");
    BindGridMapInfo<double, 3>(submodule, "GridMapInfo3Dd");
    BindGridMapInfo<float, 3>(submodule, "GridMapInfo3Df");

    BindTensor<double, 2>(submodule, "Tensor2Dd");
    BindTensor<float, 2>(submodule, "Tensor2Df");
    BindTensor<double, 3>(submodule, "Tensor3Dd");
    BindTensor<float, 3>(submodule, "Tensor3Df");
    BindTensor<double, Eigen::Dynamic>(submodule, "TensorXDd");
    BindTensor<float, Eigen::Dynamic>(submodule, "TensorXDf");
    BindTensor<int, 2>(submodule, "Tensor2Di");
    BindTensor<int, 3>(submodule, "Tensor3Di");
    BindTensor<int, Eigen::Dynamic>(submodule, "TensorXDi");
    BindTensor<uint8_t, 2>(submodule, "Tensor2D8u");
    BindTensor<uint8_t, 3>(submodule, "Tensor3D8u");
    BindTensor<uint8_t, Eigen::Dynamic>(submodule, "TensorXD8u");

    BindGridMap<double, double, 2>(submodule, "GridMap2Dd");
    BindGridMap<float, float, 2>(submodule, "GridMap2Df");
    BindGridMap<int, float, 2>(submodule, "GridMap2Di");
    BindGridMap<uint8_t, float, 2>(submodule, "GridMap2D8u");
    BindGridMap<double, double, 3>(submodule, "GridMap3Dd");
    BindGridMap<float, float, 3>(submodule, "GridMap3Df");
    BindGridMap<int, float, 3>(submodule, "GridMap3Di");
    BindGridMap<uint8_t, float, 3>(submodule, "GridMap3D8u");
    BindGridMap<double, double, Eigen::Dynamic>(submodule, "GridMapXDd");
    BindGridMap<float, float, Eigen::Dynamic>(submodule, "GridMapXDf");
    BindGridMap<int, float, Eigen::Dynamic>(submodule, "GridMapXDi");
    BindGridMap<uint8_t, float, Eigen::Dynamic>(submodule, "GridMapXD8u");

    BindGridMapDrawer2D<double>(submodule, "GridMapDrawer2Dd");
    BindGridMapDrawer2D<float>(submodule, "GridMapDrawer2Df");
}
