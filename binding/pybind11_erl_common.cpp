#include "erl_common/pybind11_erl_common.hpp"
#include "erl_common/grid_map_drawer_2d.hpp"
#include "erl_common/grid_map_info.hpp"
#include "erl_common/string_utils.hpp"
#include "erl_common/tensor.hpp"
#include "erl_common/yaml.hpp"

using namespace erl::common;

static void
BindYaml(py::module &m) {
    auto submodule = m.def_submodule("yaml", "Interface for YAML");
    py::class_<YamlableBase, std::shared_ptr<YamlableBase>>(submodule, ERL_AS_STRING(YamlableBase))
        .def("as_yaml_string", &YamlableBase::AsYamlString)
        .def("as_yaml_file", &YamlableBase::AsYamlFile, py::arg("yaml_file"))
        .def("from_yaml_string", &YamlableBase::FromYamlString, py::arg("yaml_str"))
        .def("from_yaml_file", &YamlableBase::FromYamlFile, py::arg("yaml_file"));
}

static void
BindStorage(py::module &m) {
    auto submodule = m.def_submodule("storage", "This module handles linear continuous data storage.");

    submodule.def("compute_c_strides", py::overload_cast<const std::vector<int> &, int>(&ComputeCStrides<int>), py::arg("shape"), py::arg("item_size"))
        .def(
            "compute_c_strides",
            py::overload_cast<const Eigen::Ref<const Eigen::VectorX<int>> &, int>(&ComputeCStrides<int>),
            py::arg("shape"),
            py::arg("item_size"))
        .def("compute_f_strides", py::overload_cast<const std::vector<int> &, int>(&ComputeFStrides<int>), py::arg("shape"), py::arg("item_size"))
        .def(
            "compute_f_strides",
            py::overload_cast<const Eigen::Ref<const Eigen::VectorX<int>> &, int>(&ComputeFStrides<int>),
            py::arg("shape"),
            py::arg("item_size"));

    BindGridMapInfo<Eigen::Dynamic>(submodule, "GridMapInfoXd")
        .def("grid_to_pixel_for_points", &GridMapInfoXd::GridToPixelForPoints<Eigen::Dynamic>, py::arg("grid_points"))
        .def("pixel_to_grid_for_points", &GridMapInfoXd::PixelToGridForPoints<Eigen::Dynamic>, py::arg("pixel_points"))
        .def("meter_to_pixel_for_points", &GridMapInfoXd::MeterToPixelForPoints<Eigen::Dynamic>, py::arg("meter_points"))
        .def("pixel_to_meter_for_points", &GridMapInfoXd::PixelToMeterForPoints<Eigen::Dynamic>, py::arg("pixel_points"))
        .def("grid_to_pixel_for_vectors", &GridMapInfoXd::GridToPixelForVectors<Eigen::Dynamic>, py::arg("grid_vectors"))
        .def("pixel_to_grid_for_vectors", &GridMapInfoXd::PixelToGridForVectors<Eigen::Dynamic>, py::arg("pixel_vectors"))
        .def("meter_to_pixel_for_vectors", &GridMapInfoXd::MeterToPixelForVectors<Eigen::Dynamic>, py::arg("meter_vectors"))
        .def("pixel_to_meter_for_vectors", &GridMapInfoXd::PixelToMeterForVectors<Eigen::Dynamic>, py::arg("pixel_vectors"))
        .def("pixel_to_index", &GridMapInfoXd::PixelToIndex<Eigen::Dynamic>, py::arg("pixel"), py::arg("c_stride"))
        .def("index_to_pixel", &GridMapInfoXd::IndexToPixel<Eigen::Dynamic>, py::arg("index"), py::arg("c_stride"))
        .def(
            "get_metric_coordinates_of_filled_metric_polygon",
            &GridMapInfoXd::GetMetricCoordinatesOfFilledMetricPolygon<Eigen::Dynamic>,
            py::arg("polygon_metric_vertices"))
        .def(
            "get_grid_coordinates_of_filled_metric_polygon",
            &GridMapInfoXd::GetGridCoordinatesOfFilledMetricPolygon<Eigen::Dynamic>,
            py::arg("polygon_metric_vertices"))
        .def(
            "get_pixel_coordinates_of_filled_metric_polygon",
            &GridMapInfoXd::GetPixelCoordinatesOfFilledMetricPolygon<Eigen::Dynamic>,
            py::arg("polygon_metric_vertices"));
    BindGridMapInfo<2>(submodule, "GridMapInfo2D")
        .def("grid_to_pixel_for_points", &GridMapInfo2D::GridToPixelForPoints<2>, py::arg("grid_points"))
        .def("pixel_to_grid_for_points", &GridMapInfo2D::PixelToGridForPoints<2>, py::arg("pixel_points"))
        .def("meter_to_pixel_for_points", &GridMapInfo2D::MeterToPixelForPoints<2>, py::arg("meter_points"))
        .def("pixel_to_meter_for_points", &GridMapInfo2D::PixelToMeterForPoints<2>, py::arg("pixel_points"))
        .def("grid_to_pixel_for_vectors", &GridMapInfo2D::GridToPixelForVectors<2>, py::arg("grid_vectors"))
        .def("pixel_to_grid_for_vectors", &GridMapInfo2D::PixelToGridForVectors<2>, py::arg("pixel_vectors"))
        .def("meter_to_pixel_for_vectors", &GridMapInfo2D::MeterToPixelForVectors<2>, py::arg("meter_vectors"))
        .def("pixel_to_meter_for_vectors", &GridMapInfo2D::PixelToMeterForVectors<2>, py::arg("pixel_vectors"))
        .def("pixel_to_index", &GridMapInfo2D::PixelToIndex<2>, py::arg("pixel"), py::arg("c_stride"))
        .def("index_to_pixel", &GridMapInfo2D::IndexToPixel<2>, py::arg("index"), py::arg("c_stride"))
        .def(
            "get_metric_coordinates_of_filled_metric_polygon",
            &GridMapInfo2D::GetMetricCoordinatesOfFilledMetricPolygon<2>,
            py::arg("polygon_metric_vertices"))
        .def("get_grid_coordinates_of_filled_metric_polygon", &GridMapInfo2D::GetGridCoordinatesOfFilledMetricPolygon<2>, py::arg("polygon_metric_vertices"))
        .def("get_pixel_coordinates_of_filled_metric_polygon", &GridMapInfo2D::GetPixelCoordinatesOfFilledMetricPolygon<2>, py::arg("polygon_metric_vertices"));

    BindGridMapInfo<3>(submodule, ERL_AS_STRING(GridMapInfo3D));

    BindTensor<double, 2>(submodule, ERL_AS_STRING(TensorDouble2D));
    BindTensor<double, 3>(submodule, ERL_AS_STRING(TensorDouble3D));
    BindTensor<double, Eigen::Dynamic>(submodule, ERL_AS_STRING(TensorDoubleXd));
    BindTensor<int, 2>(submodule, ERL_AS_STRING(TensorInt2D));
    BindTensor<int, 3>(submodule, ERL_AS_STRING(TensorInt3D));
    BindTensor<int, Eigen::Dynamic>(submodule, ERL_AS_STRING(TensorIntXd));
    BindTensor<uint8_t, 2>(submodule, ERL_AS_STRING(TensorUnsigned2D));
    BindTensor<uint8_t, 3>(submodule, ERL_AS_STRING(TensorUnsigned3D));
    BindTensor<uint8_t, Eigen::Dynamic>(submodule, ERL_AS_STRING(TensorUnsignedXd));

    BindGridMap<double, 2>(submodule, ERL_AS_STRING(GridMapDouble2D));
    BindGridMap<double, 3>(submodule, ERL_AS_STRING(GridMapDouble3D));
    BindGridMap<double, Eigen::Dynamic>(submodule, ERL_AS_STRING(GridMapDoubleXd));
    BindGridMap<int, 2>(submodule, ERL_AS_STRING(GridMapInt2D));
    BindGridMap<int, 3>(submodule, ERL_AS_STRING(GridMapInt3D));
    BindGridMap<int, Eigen::Dynamic>(submodule, ERL_AS_STRING(GridMapIntXd));
    BindGridMap<uint8_t, 2>(submodule, ERL_AS_STRING(GridMapUnsigned2D));
    BindGridMap<uint8_t, 3>(submodule, ERL_AS_STRING(GridMapUnsigned3D));
    BindGridMap<uint8_t, Eigen::Dynamic>(submodule, ERL_AS_STRING(GridMapUnsignedXd));

    py::class_<GridMapDrawer2D>(submodule, ERL_AS_STRING(GridMapDrawer2D))
        .def(py::init<const std::shared_ptr<GridMapInfo2D> &>(), py::arg("grid_map_info"))
        .def_readwrite("grid_map_info", &GridMapDrawer2D::grid_map_info)
        .def_property(
            "image",
            [](const GridMapDrawer2D &self) { return self.image; },
            [](GridMapDrawer2D &self, const cv::Mat &image) { image.copyTo(self.image); })
        .def("reset_image", &GridMapDrawer2D::ResetImage)
        .def(
            "draw_segments_inplace",
            &GridMapDrawer2D::DrawSegmentsInplace,
            py::arg("mat"),
            py::arg("color"),
            py::arg("thickness"),
            py::arg("starts"),
            py::arg("ends"))
        .def("draw_segments", &GridMapDrawer2D::DrawSegments, py::arg("mat"), py::arg("color"), py::arg("thickness"), py::arg("starts"), py::arg("ends"))
        .def("draw_rays_inplace", &GridMapDrawer2D::DrawRaysInplace, py::arg("mat"), py::arg("color"), py::arg("thickness"), py::arg("starts"), py::arg("ends"))
        .def("draw_rays", &GridMapDrawer2D::DrawRays, py::arg("mat"), py::arg("color"), py::arg("thickness"), py::arg("starts"), py::arg("ends"))
        .def(
            "draw_polyline_inplace",
            &GridMapDrawer2D::DrawPolylineInplace,
            py::arg("mat"),
            py::arg("color"),
            py::arg("thickness"),
            py::arg("closed"),
            py::arg("points"))
        .def("draw_polyline", &GridMapDrawer2D::DrawPolyline, py::arg("mat"), py::arg("color"), py::arg("thickness"), py::arg("closed"), py::arg("points"))
        .def("draw_contour_inplace", &GridMapDrawer2D::DrawContourInplace, py::arg("mat"), py::arg("color"), py::arg("thickness"), py::arg("contour"))
        .def("draw_contour", &GridMapDrawer2D::DrawContour, py::arg("mat"), py::arg("color"), py::arg("thickness"), py::arg("contour"))
        .def("show_image", &GridMapDrawer2D::ShowImage, py::arg("title"));
}

void
BindCommon(py::module &m) {
    auto submodule = m.def_submodule("common", "Interface of erl_common");

    BindYaml(submodule);
    BindStorage(submodule);
}
