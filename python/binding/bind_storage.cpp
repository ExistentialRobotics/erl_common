#include "pybind11_erl_common.hpp"

#include "erl_common/grid_map_drawer_2d.hpp"
#include "erl_common/grid_map_info.hpp"

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

    BindGridMapInfo<double, Eigen::Dynamic>(submodule, "GridMapInfoXd")
        .def("grid_to_pixel_for_points", &GridMapInfoXDd::GridToPixelForPoints<Eigen::Dynamic>, py::arg("grid_points"))
        .def("pixel_to_grid_for_points", &GridMapInfoXDd::PixelToGridForPoints<Eigen::Dynamic>, py::arg("pixel_points"))
        .def("meter_to_pixel_for_points", &GridMapInfoXDd::MeterToPixelForPoints<Eigen::Dynamic>, py::arg("meter_points"))
        .def("pixel_to_meter_for_points", &GridMapInfoXDd::PixelToMeterForPoints<Eigen::Dynamic>, py::arg("pixel_points"))
        .def("grid_to_pixel_for_vectors", &GridMapInfoXDd::GridToPixelForVectors<Eigen::Dynamic>, py::arg("grid_vectors"))
        .def("pixel_to_grid_for_vectors", &GridMapInfoXDd::PixelToGridForVectors<Eigen::Dynamic>, py::arg("pixel_vectors"))
        .def("meter_to_pixel_for_vectors", &GridMapInfoXDd::MeterToPixelForVectors<Eigen::Dynamic>, py::arg("meter_vectors"))
        .def("pixel_to_meter_for_vectors", &GridMapInfoXDd::PixelToMeterForVectors<Eigen::Dynamic>, py::arg("pixel_vectors"))
        .def("pixel_to_index", &GridMapInfoXDd::PixelToIndex<Eigen::Dynamic>, py::arg("pixel"), py::arg("c_stride"))
        .def("index_to_pixel", &GridMapInfoXDd::IndexToPixel<Eigen::Dynamic>, py::arg("index"), py::arg("c_stride"))
        .def(
            "get_metric_coordinates_of_filled_metric_polygon",
            &GridMapInfoXDd::GetMetricCoordinatesOfFilledMetricPolygon<Eigen::Dynamic>,
            py::arg("polygon_metric_vertices"))
        .def(
            "get_grid_coordinates_of_filled_metric_polygon",
            &GridMapInfoXDd::GetGridCoordinatesOfFilledMetricPolygon<Eigen::Dynamic>,
            py::arg("polygon_metric_vertices"))
        .def(
            "get_pixel_coordinates_of_filled_metric_polygon",
            &GridMapInfoXDd::GetPixelCoordinatesOfFilledMetricPolygon<Eigen::Dynamic>,
            py::arg("polygon_metric_vertices"));
    BindGridMapInfo<double, 2>(submodule, "GridMapInfo2D")
        .def("grid_to_pixel_for_points", &GridMapInfo2Dd::GridToPixelForPoints<2>, py::arg("grid_points"))
        .def("pixel_to_grid_for_points", &GridMapInfo2Dd::PixelToGridForPoints<2>, py::arg("pixel_points"))
        .def("meter_to_pixel_for_points", &GridMapInfo2Dd::MeterToPixelForPoints<2>, py::arg("meter_points"))
        .def("pixel_to_meter_for_points", &GridMapInfo2Dd::PixelToMeterForPoints<2>, py::arg("pixel_points"))
        .def("grid_to_pixel_for_vectors", &GridMapInfo2Dd::GridToPixelForVectors<2>, py::arg("grid_vectors"))
        .def("pixel_to_grid_for_vectors", &GridMapInfo2Dd::PixelToGridForVectors<2>, py::arg("pixel_vectors"))
        .def("meter_to_pixel_for_vectors", &GridMapInfo2Dd::MeterToPixelForVectors<2>, py::arg("meter_vectors"))
        .def("pixel_to_meter_for_vectors", &GridMapInfo2Dd::PixelToMeterForVectors<2>, py::arg("pixel_vectors"))
        .def("pixel_to_index", &GridMapInfo2Dd::PixelToIndex<2>, py::arg("pixel"), py::arg("c_stride"))
        .def("index_to_pixel", &GridMapInfo2Dd::IndexToPixel<2>, py::arg("index"), py::arg("c_stride"))
        .def(
            "get_metric_coordinates_of_filled_metric_polygon",
            &GridMapInfo2Dd::GetMetricCoordinatesOfFilledMetricPolygon<2>,
            py::arg("polygon_metric_vertices"))
        .def("get_grid_coordinates_of_filled_metric_polygon", &GridMapInfo2Dd::GetGridCoordinatesOfFilledMetricPolygon<2>, py::arg("polygon_metric_vertices"))
        .def("get_pixel_coordinates_of_filled_metric_polygon", &GridMapInfo2Dd::GetPixelCoordinatesOfFilledMetricPolygon<2>, py::arg("polygon_metric_vertices"));

    BindGridMapInfo<double, 3>(submodule, "GridMapInfo3D");

    BindTensor<double, 2>(submodule, "TensorDouble2D");
    BindTensor<double, 3>(submodule, "TensorDouble3D");
    BindTensor<double, Eigen::Dynamic>(submodule, "TensorDoubleXd");
    BindTensor<int, 2>(submodule, "TensorInt2D");
    BindTensor<int, 3>(submodule, "TensorInt3D");
    BindTensor<int, Eigen::Dynamic>(submodule, "TensorIntXd");
    BindTensor<uint8_t, 2>(submodule, "TensorUnsigned2D");
    BindTensor<uint8_t, 3>(submodule, "TensorUnsigned3D");
    BindTensor<uint8_t, Eigen::Dynamic>(submodule, "TensorUnsignedXd");

    BindGridMap<double, 2>(submodule, "GridMapDouble2D");
    BindGridMap<double, 3>(submodule, "GridMapDouble3D");
    BindGridMap<double, Eigen::Dynamic>(submodule, "GridMapDoubleXd");
    BindGridMap<int, 2>(submodule, "GridMapInt2D");
    BindGridMap<int, 3>(submodule, "GridMapInt3D");
    BindGridMap<int, Eigen::Dynamic>(submodule, "GridMapIntXd");
    BindGridMap<uint8_t, 2>(submodule, "GridMapUnsigned2D");
    BindGridMap<uint8_t, 3>(submodule, "GridMapUnsigned3D");
    BindGridMap<uint8_t, Eigen::Dynamic>(submodule, "GridMapUnsignedXd");

    BindGridMapDrawer2D<double>(submodule, "GridMapDrawer2D");
}
