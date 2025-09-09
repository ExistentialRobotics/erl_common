#include "pybind11_erl_common.hpp"

void
BindStorage(py::module &m) {
    using namespace erl::common;

    m.def(
         "compute_c_strides",
         [](const std::vector<int> &shape, const int item_size) {
             return ComputeCStrides(shape, item_size);
         },
         py::arg("shape"),
         py::arg("item_size"))
        .def(
            "compute_c_strides",
            [](const Eigen::VectorX<int> &shape, const int item_size) {
                return ComputeCStrides(shape, item_size);
            },
            py::arg("shape"),
            py::arg("item_size"))
        .def(
            "compute_f_strides",
            [](const std::vector<int> &shape, const int item_size) {
                return ComputeFStrides(shape, item_size);
            },
            py::arg("shape"),
            py::arg("item_size"))
        .def(
            "compute_f_strides",
            [](const Eigen::VectorX<int> &shape, const int item_size) {
                return ComputeFStrides(shape, item_size);
            },
            py::arg("shape"),
            py::arg("item_size"));

    BindGridMapInfo<double, Eigen::Dynamic>(m, "GridMapInfoXDd");
    BindGridMapInfo<float, Eigen::Dynamic>(m, "GridMapInfoXDf");
    BindGridMapInfo<double, 2>(m, "GridMapInfo2Dd");
    BindGridMapInfo<float, 2>(m, "GridMapInfo2Df");
    BindGridMapInfo<double, 3>(m, "GridMapInfo3Dd");
    BindGridMapInfo<float, 3>(m, "GridMapInfo3Df");

    BindTensor<double, 2>(m, "Tensor2Dd");
    BindTensor<float, 2>(m, "Tensor2Df");
    BindTensor<double, 3>(m, "Tensor3Dd");
    BindTensor<float, 3>(m, "Tensor3Df");
    BindTensor<double, Eigen::Dynamic>(m, "TensorXDd");
    BindTensor<float, Eigen::Dynamic>(m, "TensorXDf");
    BindTensor<int, 2>(m, "Tensor2Di");
    BindTensor<int, 3>(m, "Tensor3Di");
    BindTensor<int, Eigen::Dynamic>(m, "TensorXDi");
    BindTensor<uint8_t, 2>(m, "Tensor2D8u");
    BindTensor<uint8_t, 3>(m, "Tensor3D8u");
    BindTensor<uint8_t, Eigen::Dynamic>(m, "TensorXD8u");

    BindGridMap<double, double, 2>(m, "GridMap2Dd");
    BindGridMap<float, float, 2>(m, "GridMap2Df");
    BindGridMap<int, float, 2>(m, "GridMap2Di");
    BindGridMap<uint8_t, float, 2>(m, "GridMap2D8u");
    BindGridMap<double, double, 3>(m, "GridMap3Dd");
    BindGridMap<float, float, 3>(m, "GridMap3Df");
    BindGridMap<int, float, 3>(m, "GridMap3Di");
    BindGridMap<uint8_t, float, 3>(m, "GridMap3D8u");
    BindGridMap<double, double, Eigen::Dynamic>(m, "GridMapXDd");
    BindGridMap<float, float, Eigen::Dynamic>(m, "GridMapXDf");
    BindGridMap<int, float, Eigen::Dynamic>(m, "GridMapXDi");
    BindGridMap<uint8_t, float, Eigen::Dynamic>(m, "GridMapXD8u");

    BindGridMapDrawer2D<double>(m, "GridMapDrawer2Dd");
    BindGridMapDrawer2D<float>(m, "GridMapDrawer2Df");
}
