#pragma once

#include "erl_common/grid_map.hpp"
#include "erl_common/grid_map_drawer_2d.hpp"
#include "erl_common/pybind11.hpp"
#include "erl_common/pybind11_yaml.hpp"

namespace py = pybind11;

namespace erl::common {

    template<typename Dtype, int Dim>
    std::enable_if_t<
        Dim == 2 || Dim == Eigen::Dynamic,
        py::class_<GridMapInfo<Dtype, Dim>, std::shared_ptr<GridMapInfo<Dtype, Dim>>>>
    BindGridMapInfoExtra(
        py::class_<GridMapInfo<Dtype, Dim>, std::shared_ptr<GridMapInfo<Dtype, Dim>>> cls) {
        using Info = GridMapInfo<Dtype, Dim>;

        cls.def(
               "grid_to_pixel_for_points",
               &Info::template GridToPixelForPoints<Dim>,
               py::arg("grid_points"))
            .def(
                "pixel_to_grid_for_points",
                &Info::template PixelToGridForPoints<Dim>,
                py::arg("pixel_points"))
            .def(
                "meter_to_pixel_for_points",
                &Info::template MeterToPixelForPoints<Dim>,
                py::arg("meter_points"))
            .def(
                "pixel_to_meter_for_points",
                &Info::template PixelToMeterForPoints<Dim>,
                py::arg("pixel_points"))
            .def(
                "grid_to_pixel_for_vectors",
                &Info::template GridToPixelForVectors<Dim>,
                py::arg("grid_vectors"))
            .def(
                "pixel_to_grid_for_vectors",
                &Info::template PixelToGridForVectors<Dim>,
                py::arg("pixel_vectors"))
            .def(
                "meter_to_pixel_for_vectors",
                &Info::template MeterToPixelForVectors<Dim>,
                py::arg("meter_vectors"))
            .def(
                "pixel_to_meter_for_vectors",
                &Info::template PixelToMeterForVectors<Dim>,
                py::arg("pixel_vectors"))
            .def(
                "pixel_to_index",
                &Info::template PixelToIndex<Dim>,
                py::arg("pixel"),
                py::arg("c_stride"))
            .def(
                "index_to_pixel",
                &Info::template IndexToPixel<Dim>,
                py::arg("index"),
                py::arg("c_stride"))
            .def(
                "get_metric_coordinates_of_filled_metric_polygon",
                &Info::template GetMetricCoordinatesOfFilledMetricPolygon<Dim>,
                py::arg("polygon_metric_vertices"))
            .def(
                "get_grid_coordinates_of_filled_metric_polygon",
                &Info::template GetGridCoordinatesOfFilledMetricPolygon<Dim>,
                py::arg("polygon_metric_vertices"))
            .def(
                "get_pixel_coordinates_of_filled_metric_polygon",
                &Info::template GetPixelCoordinatesOfFilledMetricPolygon<Dim>,
                py::arg("polygon_metric_vertices"));
        return cls;
    }

    template<typename Dtype, int Dim>
    std::enable_if_t<
        Dim != 2 && Dim != Eigen::Dynamic,
        py::class_<GridMapInfo<Dtype, Dim>, std::shared_ptr<GridMapInfo<Dtype, Dim>>>>
    BindGridMapInfoExtra(
        py::class_<GridMapInfo<Dtype, Dim>, std::shared_ptr<GridMapInfo<Dtype, Dim>>> cls) {
        return cls;
    }

    template<typename Dtype, int Dim>
    auto
    BindGridMapInfo(py::module &m, const char *name) {
        using Self = GridMapInfo<Dtype, Dim>;
        using VectorD = const Eigen::Vector<Dtype, Dim>;

        return BindGridMapInfoExtra<Dtype, Dim>(
            py::class_<Self, std::shared_ptr<Self>>(m, name)
                .def(
                    py::init<
                        const VectorD &,
                        const VectorD &,
                        const VectorD &,
                        Eigen::Vector<int, Dim>>(),
                    py::arg("min"),
                    py::arg("max"),
                    py::arg("resolution"),
                    py::arg("padding"))
                .def(
                    py::init<Eigen::Vector<int, Dim>, VectorD &, VectorD &>(),
                    py::arg("map_shape"),
                    py::arg("min"),
                    py::arg("max"))
                .def(
                    "extend",
                    py::overload_cast<int, Dtype, Dtype, int>(&Self::Extend, py::const_),
                    py::arg("size"),
                    py::arg("min"),
                    py::arg("max"),
                    py::arg("dim"))
                .def(
                    "extend",
                    py::overload_cast<Dtype, Dtype, Dtype, int, int>(&Self::Extend, py::const_),
                    py::arg("min"),
                    py::arg("max"),
                    py::arg("resolution"),
                    py::arg("padding"),
                    py::arg("dim"))
                .def("squeeze", &Self::Squeeze, py::arg("dim"))
                .def_property_readonly("dims", &Self::Dims)
                .def_property_readonly("shape", py::overload_cast<>(&Self::Shape, py::const_))
                .def("shape_at", py::overload_cast<int>(&Self::Shape, py::const_), py::arg("dim"))
                .def_property_readonly("size", &Self::Size)
                .def_property_readonly("height", &Self::Height)
                .def_property_readonly("width", &Self::Width)
                .def_property_readonly("length", &Self::Length)
                .def_property_readonly("min", py::overload_cast<>(&Self::Min, py::const_))
                .def("min_at", py::overload_cast<int>(&Self::Min, py::const_), py::arg("dim"))
                .def_property_readonly("max", py::overload_cast<>(&Self::Max, py::const_))
                .def("max_at", py::overload_cast<int>(&Self::Max, py::const_), py::arg("dim"))
                .def_property_readonly(
                    "resolution",
                    py::overload_cast<>(&Self::Resolution, py::const_))
                .def(
                    "resolution_at",
                    py::overload_cast<int>(&Self::Resolution, py::const_),
                    py::arg("dim"))
                .def_property_readonly("center", &Self::Center)
                .def_property_readonly("center_grid", &Self::CenterGrid)
                .def("get_dim_lin_space", &Self::GetDimLinSpace, py::arg("dim"))
                .def(
                    "grid_to_meter_at_dim",
                    py::overload_cast<int, int>(&Self::GridToMeterAtDim, py::const_),
                    py::arg("grid_value"),
                    py::arg("dim"))
                .def(
                    "grid_to_meter_at_dim",
                    py::overload_cast<const Eigen::Ref<const Eigen::VectorXi> &, int>(
                        &Self::GridToMeterAtDim,
                        py::const_),
                    py::arg("grid_values"),
                    py::arg("dim"))
                .def(
                    "meter_to_grid_at_dim",
                    py::overload_cast<Dtype, int>(&Self::MeterToGridAtDim, py::const_),
                    py::arg("meter_value"),
                    py::arg("dim"))
                .def(
                    "meter_to_grid_at_dim",
                    py::overload_cast<const Eigen::Ref<const Eigen::VectorX<Dtype>> &, int>(
                        &Self::MeterToGridAtDim,
                        py::const_),
                    py::arg("meter_values"),
                    py::arg("dim"))
                .def(
                    "grid_to_meter_for_points",
                    &Self::GridToMeterForPoints,
                    py::arg("grid_points"))
                .def(
                    "meter_to_grid_for_points",
                    &Self::MeterToGridForPoints,
                    py::arg("meter_points"))
                .def(
                    "grid_to_meter_for_vectors",
                    &Self::GridToMeterForVectors,
                    py::arg("grid_vectors"))
                .def(
                    "meter_to_grid_for_vectors",
                    &Self::MeterToGridForVectors,
                    py::arg("meter_vectors"))
                .def("in_map", &Self::InMap, py::arg("meter_point"))
                .def("in_grids", &Self::InGrids, py::arg("grid_point"))
                .def("grid_to_index", &Self::GridToIndex, py::arg("grid"), py::arg("c_stride"))
                .def("index_to_grid", &Self::IndexToGrid, py::arg("index"), py::arg("c_stride"))
                .def(
                    "generate_grid_coordinates",
                    &Self::GenerateGridCoordinates,
                    py::arg("c_stride"))
                .def(
                    "generate_meter_coordinates",
                    &Self::GenerateMeterCoordinates,
                    py::arg("c_stride"))
                .def("ray_casting", &Self::RayCasting, py::arg("start"), py::arg("end")));
    }

    // Dtype must be POD or arithmetic type, which is required by pybind11's NumPy interface
    template<typename Dtype, int Rank>
    std::enable_if_t<py::SupportedByNumpy<Dtype>::value, py::class_<Tensor<Dtype, Rank>>>
    BindTensor(py::module &m, const char *name) {
        using Self = Tensor<Dtype, Rank>;
        using IndexType = typename Self::IndexType;
        using ShapeType = typename Self::ShapeType;
        using Slice = typename Self::Slice;
        using SliceShape = typename Slice::SliceShape;
        py::class_<Self> cls(m, name, py::buffer_protocol());
        py::class_<Slice>(cls, "Slice")
            .def_property_readonly("dims", &Slice::Dims)
            .def_property_readonly("shape", &Slice::Shape)
            .def_property_readonly("size", &Slice::Size)
            .def(
                "__setitem__",
                [](Slice &self, const SliceShape &coords, Dtype value) { self[coords] = value; },
                py::arg("coords"),
                py::arg("value"))
            .def(
                "__getitem__",
                py::overload_cast<const SliceShape &>(&Slice::operator[], py::const_),
                py::arg("coords"))
            .def(
                "__setitem__",
                [](Slice &self, IndexType index, Dtype value) { self[index] = value; },
                py::arg("index"),
                py::arg("value"))
            .def(
                "__getitem__",
                py::overload_cast<IndexType>(&Slice::operator[], py::const_),
                py::arg("index"));

        cls.def(py::init([&](const py::array_t<Dtype, py::array::c_style> &b) {
               py::buffer_info info = b.request();
               std::string buffer_format = info.format;
               std::string expected_format = py::format_descriptor<Dtype>::format();
               if (buffer_format != expected_format) {
                   throw std::runtime_error(
                       "Incompatible format: " + buffer_format + " vs " + expected_format);
               }
               if (Rank != -1) {
                   ERL_ASSERTM(Rank == info.ndim, "Incompatible ndim for %s: {}", name, info.ndim);
               }
               ShapeType tensor_shape(info.ndim);
               for (int i = 0; i < Rank; ++i) {
                   tensor_shape[i] = static_cast<IndexType>(info.shape[i]);
               }
               Eigen::VectorX<Dtype> data(info.size);
               auto ptr = static_cast<Dtype *>(info.ptr);
               std::copy(ptr, ptr + info.size, data.data());
               Self tensor(tensor_shape, data);
               return tensor;
           }))
            .def_buffer([](Self &tensor) -> py::buffer_info {
                auto shape = tensor.Shape();
                std::vector<py::ssize_t> array_shape(shape.size());
                std::copy(shape.data(), shape.data() + shape.size(), array_shape.data());
                std::vector<py::ssize_t> strides =
                    ComputeCStrides<py::ssize_t>(array_shape, sizeof(Dtype));
                return {
                    tensor.GetMutableDataPtr(),
                    sizeof(Dtype),
                    py::format_descriptor<Dtype>::format(),
                    tensor.Dims(),
                    array_shape,
                    strides};
            })
            .def(py::init<ShapeType, const Dtype &>(), py::arg("shape"), py::arg("constant"))
            .def(py::init<ShapeType, Eigen::VectorX<Dtype>>(), py::arg("shape"), py::arg("data"))
            .def(
                py::init<ShapeType, const std::function<Dtype()> &>(),
                py::arg("shape"),
                py::arg("data_init_func"))
            .def_property_readonly("dims", &Self::Dims)
            .def_property_readonly("shape", &Self::Shape)
            .def_property_readonly("size", &Self::Size)
            .def_property_readonly("is_row_major", &Self::IsRowMajor)
            .def("fill", &Self::Fill, py::arg("value"))
            .def(
                "__setitem__",
                [](Self &self, const ShapeType &coords, Dtype value) { self[coords] = value; },
                py::arg("coords"),
                py::arg("value"))
            .def(
                "__getitem__",
                py::overload_cast<const ShapeType &>(&Self::operator[], py::const_),
                py::arg("coords"))
            .def(
                "__setitem__",
                [](Self &self, IndexType index, Dtype value) { self[index] = value; },
                py::arg("index"),
                py::arg("value"))
            .def(
                "__getitem__",
                py::overload_cast<IndexType>(&Self::operator[], py::const_),
                py::arg("index"))
            .def("get_slice", &Self::GetSlice, py::arg("slice_layout"))
            .def("__str__", [](Self &self) -> std::string {
                std::stringstream ss;
                self.Print(ss);
                return ss.str();
            });
        return cls;
    }

    template<typename Dtype, int Rank>
    std::enable_if_t<!py::SupportedByNumpy<Dtype>::value, py::class_<Tensor<Dtype, Rank>>>
    BindTensor(py::module &m, const char *name) {
        using Self = Tensor<Dtype, Rank>;
        using IndexType = typename Self::IndexType;
        using ShapeType = typename Self::ShapeType;
        using Slice = typename Self::Slice;
        using SliceShape = typename Slice::SliceShape;
        py::class_<Self> cls(m, name);
        py::class_<Slice>(cls, "Slice")
            .def_property_readonly("dims", &Slice::Dims)
            .def_property_readonly("shape", &Slice::Shape)
            .def_property_readonly("size", &Slice::Size)
            .def(
                "__setitem__",
                [](Slice &self, const SliceShape &coords, Dtype value) { self[coords] = value; },
                py::arg("coords"),
                py::arg("value"))
            .def(
                "__getitem__",
                py::overload_cast<const SliceShape &>(&Slice::operator[], py::const_),
                py::arg("coords"))
            .def(
                "__setitem__",
                [](Slice &self, IndexType index, Dtype value) { self[index] = value; },
                py::arg("index"),
                py::arg("value"))
            .def(
                "__getitem__",
                py::overload_cast<IndexType>(&Slice::operator[], py::const_),
                py::arg("index"));
        cls.def(py::init<ShapeType, const Dtype &>(), py::arg("shape"), py::arg("constant"))
            .def(
                py::init<>([](ShapeType shape, const std::vector<Dtype> &data) {
                    Eigen::Map<const Eigen::VectorX<Dtype>> data_map(data.data(), data.size());
                    return Self(shape, Eigen::VectorX<Dtype>(data_map));
                }),
                py::arg("shape"),
                py::arg("data"))
            .def(
                py::init<ShapeType, const std::function<Dtype()> &>(),
                py::arg("shape"),
                py::arg("data_init_func"))
            .def_property_readonly("dims", &Self::Dims)
            .def_property_readonly("shape", &Self::Shape)
            .def_property_readonly("size", &Self::Size)
            .def_property_readonly("is_row_major", &Self::IsRowMajor)
            .def("fill", &Self::Fill, py::arg("value"))
            .def(
                "__setitem__",
                [](Self &self, const ShapeType &coords, Dtype value) { self[coords] = value; },
                py::arg("coords"),
                py::arg("value"))
            .def(
                "__getitem__",
                py::overload_cast<const ShapeType &>(&Self::operator[], py::const_),
                py::arg("coords"))
            .def(
                "__setitem__",
                [](Self &tensor, IndexType index, Dtype value) { tensor[index] = value; },
                py::arg("index"),
                py::arg("value"))
            .def(
                "__getitem__",
                py::overload_cast<IndexType>(&Self::operator[], py::const_),
                py::arg("index"))
            .def("get_slice", &Self::GetSlice, py::arg("slice_layout"))
            .def("__str__", [](Self &self) -> std::string {
                std::stringstream ss;
                self.Print(ss);
                return ss.str();
            });
        return cls;
    }

    template<
        typename MapDtype,
        typename InfoDtype,
        int Dim>  // T is supported by NumPy interface provided by pybind11
    std::enable_if_t<
        py::SupportedByNumpy<MapDtype>::value,
        py::class_<
            GridMap<MapDtype, InfoDtype, Dim>,
            std::shared_ptr<GridMap<MapDtype, InfoDtype, Dim>>>>
    BindGridMap(py::module &m, const char *name) {
        using Self = GridMap<MapDtype, InfoDtype, Dim>;
        using Info = GridMapInfo<InfoDtype, Dim>;
        auto py_class =
            py::class_<Self, std::shared_ptr<Self>>(m, name)
                .def(py::init<std::shared_ptr<Info>>(), py::arg("grid_map_info"))
                .def(
                    py::init<std::shared_ptr<Info>, MapDtype>(),
                    py::arg("grid_map_info"),
                    py::arg("value"))
                .def(
                    py::init<std::shared_ptr<Info>, Tensor<MapDtype, Dim>>(),
                    py::arg("grid_map_info"),
                    py::arg("data"))
                .def(
                    py::init<std::shared_ptr<Info>, const std::function<MapDtype()> &>(),
                    py::arg("grid_map_info"),
                    py::arg("data_init_func"))
                .def_readwrite("data", &Self::data)
                .def_readwrite("info", &Self::info);
        py_class.def(
            py::init<std::shared_ptr<Info>, Eigen::VectorX<MapDtype>>(),
            py::arg("grid_map_info"),
            py::arg("data"));
        return py_class;
    }

    template<typename MapDtype, typename InfoDtype, int Dim>
    std::enable_if_t<
        !py::SupportedByNumpy<MapDtype>::value,
        py::class_<
            GridMap<MapDtype, InfoDtype, Dim>,
            std::shared_ptr<GridMap<MapDtype, InfoDtype, Dim>>>>
    BindGridMap(py::module &m, const char *name) {
        using Self = GridMap<MapDtype, InfoDtype, Dim>;
        using Info = GridMapInfo<InfoDtype, Dim>;
        auto py_class =
            py::class_<Self, std::shared_ptr<Self>>(m, name)
                .def(py::init<std::shared_ptr<Info>>(), py::arg("grid_map_info"))
                .def(
                    py::init<std::shared_ptr<Info>, MapDtype>(),
                    py::arg("grid_map_info"),
                    py::arg("value"))
                .def(
                    py::init<std::shared_ptr<Info>, Tensor<MapDtype, Dim>>(),
                    py::arg("grid_map_info"),
                    py::arg("data"))
                .def(
                    py::init<std::shared_ptr<Info>, const std::function<MapDtype()> &>(),
                    py::arg("grid_map_info"),
                    py::arg("data_init_func"))
                .def_readwrite("data", &Self::data)
                .def_readwrite("info", &Self::info);
        py_class.def(
            py::init<>([](std::shared_ptr<Info> grid_map_info, const std::vector<MapDtype> &data) {
                Eigen::Map<const Eigen::VectorX<MapDtype>> data_map(data.data(), data.size());
                return std::make_shared<Self>(grid_map_info, Eigen::VectorX<MapDtype>(data_map));
            }),
            py::arg("grid_map_info"),
            py::arg("data"));
        return py_class;
    }

    template<typename Dtype>
    std::enable_if_t<py::SupportedByNumpy<Dtype>::value, py::class_<IncrementalGridMap2D<Dtype>>>
    BindIncrementalGridMap2D(py::module &m, const char *name) {
        using Self = IncrementalGridMap2D<Dtype>;
        using Info = GridMapInfo2D<Dtype>;
        using Vector2 = Eigen::Vector2<Dtype>;

        return py::class_<Self, std::shared_ptr<Self>>(m, name)
            .def(
                py::init<>([](std::shared_ptr<Info> grid_map_info,
                              const std::function<Dtype()> &data_init_func) {
                    return std::make_shared<Self>(grid_map_info, data_init_func);
                }),
                py::arg("grid_map_info"),
                py::arg("data_init_func") = py::none())
            .def_property_readonly("grid_map_info", &Self::GetGridMapInfo)
            .def(
                "get_canonical_metric_coords",
                &Self::GetCanonicalMetricCoords,
                py::arg("metric_coords"))
            .def(
                "as_image",
                py::overload_cast<
                    const std::shared_ptr<Info> &,
                    const std::function<uint8_t(const Dtype &)> &>(&Self::AsImage, py::const_),
                py::arg("grid_map_info"),
                py::arg("cast_func").none(false))
            .def(
                "__call__",
                py::overload_cast<int, int>(&Self::operator(), py::const_),
                py::arg("x_grid"),
                py::arg("y_grid"))
            .def(
                "__call__",
                py::overload_cast<Dtype, Dtype>(&Self::operator(), py::const_),
                py::arg("x"),
                py::arg("y"))
            .def(
                "__getitem__",
                py::overload_cast<const Eigen::Ref<const Eigen::Vector2i> &>(
                    &Self::operator[],
                    py::const_),
                py::arg("grid_coords"))
            .def(
                "__getitem__",
                py::overload_cast<const Eigen::Ref<const Vector2> &>(&Self::operator[], py::const_),
                py::arg("metric_coords"))
            .def(
                "__setitem__",
                [](Self &self,
                   const Eigen::Ref<const Eigen::Vector2i> &grid_coords,
                   const Dtype data) { self.GetMutableData(grid_coords) = data; },
                py::arg("grid_coords"),
                py::arg("data_container"))
            .def(
                "__setitem__",
                [](Self &self, const Eigen::Ref<const Vector2> &metric_coords, const Dtype data) {
                    self.GetMutableData(metric_coords) = data;
                },
                py::arg("metric_coords"),
                py::arg("data_container"))
            .def(
                "get_block",
                py::overload_cast<int, int, int, int>(&Self::GetBlock),
                py::arg("x_grid"),
                py::arg("y_grid"),
                py::arg("height"),
                py::arg("width"))
            .def(
                "get_block",
                py::overload_cast<Dtype, Dtype, Dtype, Dtype, bool>(&Self::GetBlock),
                py::arg("x_min"),
                py::arg("y_min"),
                py::arg("x_max"),
                py::arg("y_max"),
                py::arg("safe_crop") = true)
            .def(
                "get_block",
                py::overload_cast<
                    const Eigen::Ref<const Vector2> &,
                    const Eigen::Ref<const Vector2> &,
                    bool>(&Self::GetBlock),
                py::arg("metric_min"),
                py::arg("metric_max"),
                py::arg("safe_crop") = true)
            .def(
                "collect_non_zero_data",
                [](Self &self, Dtype x_min, Dtype y_min, Dtype x_max, Dtype y_max) {
                    std::vector<Dtype> data;
                    self.CollectNonZeroData(x_min, y_min, x_max, y_max, data);
                    return data;
                },
                py::arg("x_min"),
                py::arg("y_min"),
                py::arg("x_max"),
                py::arg("y_max"))
            .def(
                "collect_non_zero_data",
                [](Self &self,
                   const Eigen::Ref<const Vector2> &metric_min,
                   const Eigen::Ref<const Vector2> &metric_max) {
                    std::vector<Dtype> data;
                    self.CollectNonZeroData(metric_min, metric_max, data);
                    return data;
                },
                py::arg("metric_min"),
                py::arg("metric_max"));
    }

    template<typename Dtype>
    std::enable_if_t<!py::SupportedByNumpy<Dtype>::value, py::class_<IncrementalGridMap2D<Dtype>>>
    BindIncrementalGridMap2D(py::module &m, const char *name) {
        using Self = IncrementalGridMap2D<Dtype>;
        using Info = GridMapInfo2D<Dtype>;
        using Vector2 = Eigen::Vector2<Dtype>;

        return py::class_<Self, std::shared_ptr<Self>>(m, name)
            .def(
                py::init<>([](std::shared_ptr<Info> grid_map_info,
                              const std::function<Dtype()> &data_init_func) {
                    return std::make_shared<Self>(grid_map_info, data_init_func);
                }),
                py::arg("grid_map_info"),
                py::arg("data_init_func") = py::none())
            .def_property_readonly("grid_map_info", &Self::GetGridMapInfo)
            .def(
                "get_canonical_metric_coords",
                &Self::GetCanonicalMetricCoords,
                py::arg("metric_coords"))
            .def(
                "as_image",
                py::overload_cast<
                    const std::shared_ptr<Info> &,
                    const std::function<uint8_t(const Dtype &)> &>(&Self::AsImage, py::const_),
                py::arg("grid_map_info"),
                py::arg("cast_func").none(false))
            .def(
                "__call__",
                py::overload_cast<int, int>(&Self::operator(), py::const_),
                py::arg("x_grid"),
                py::arg("y_grid"))
            .def(
                "__call__",
                py::overload_cast<Dtype, Dtype>(&Self::operator(), py::const_),
                py::arg("x"),
                py::arg("y"))
            .def(
                "__getitem__",
                py::overload_cast<const Eigen::Ref<const Eigen::Vector2i> &>(
                    &Self::operator[],
                    py::const_),
                py::arg("grid_coords"))
            .def(
                "__getitem__",
                py::overload_cast<const Eigen::Ref<const Vector2> &>(&Self::operator[], py::const_),
                py::arg("metric_coords"))
            .def(
                "__setitem__",
                [](Self &self,
                   const Eigen::Ref<const Eigen::Vector2i> &grid_coords,
                   const Dtype data) { self.GetMutableData(grid_coords) = data; },
                py::arg("grid_coords"),
                py::arg("data_container"))
            .def(
                "__setitem__",
                [](Self &self, const Eigen::Ref<const Vector2> &metric_coords, const Dtype data) {
                    self.GetMutableData(metric_coords) = data;
                },
                py::arg("metric_coords"),
                py::arg("data_container"))
            .def(
                "get_block",
                [](Self &self, int x_grid, int y_grid, int height, int width) {
                    auto block = self.GetBlock(x_grid, y_grid, height, width);
                    std::vector<std::vector<Dtype>> data(height);
                    for (int i = 0; i < height; ++i) {
                        data[i].resize(width);
                        for (int j = 0; j < width; ++j) { data[i][j] = block(i, j); }
                    }
                    return data;
                },
                py::arg("x_grid"),
                py::arg("y_grid"),
                py::arg("height"),
                py::arg("width"))
            .def(
                "get_block",
                [](Self &self, Dtype x_min, Dtype y_min, Dtype x_max, Dtype y_max, bool safe_crop) {
                    auto block = self.GetBlock(x_min, y_min, x_max, y_max, safe_crop);
                    long height = block.rows();
                    long width = block.cols();
                    std::vector<std::vector<Dtype>> data(height);
                    for (int i = 0; i < height; ++i) {
                        data[i].resize(width);
                        for (int j = 0; j < width; ++j) { data[i][j] = block(i, j); }
                    }
                    return data;
                },
                py::arg("x_min"),
                py::arg("y_min"),
                py::arg("x_max"),
                py::arg("y_max"),
                py::arg("safe_crop") = true)
            .def(
                "get_block",
                [](Self &self,
                   const Eigen::Ref<const Vector2> &metric_min,
                   const Eigen::Ref<const Vector2> &metric_max,
                   bool safe_crop) {
                    auto block = self.GetBlock(metric_min, metric_max, safe_crop);
                    long height = block.rows();
                    long width = block.cols();
                    std::vector<std::vector<Dtype>> data(height);
                    for (int i = 0; i < height; ++i) {
                        data[i].resize(width);
                        for (int j = 0; j < width; ++j) { data[i][j] = block(i, j); }
                    }
                    return data;
                },
                py::arg("metric_min"),
                py::arg("metric_max"),
                py::arg("safe_crop") = true)
            .def(
                "collect_non_zero_data",
                [](Self &self, Dtype x_min, Dtype y_min, Dtype x_max, Dtype y_max) {
                    std::vector<Dtype> data;
                    self.CollectNonZeroData(x_min, y_min, x_max, y_max, data);
                    return data;
                },
                py::arg("x_min"),
                py::arg("y_min"),
                py::arg("x_max"),
                py::arg("y_max"))
            .def(
                "collect_non_zero_data",
                [](Self &self,
                   const Eigen::Ref<const Vector2> &metric_min,
                   const Eigen::Ref<const Vector2> &metric_max) {
                    std::vector<Dtype> data;
                    self.CollectNonZeroData(metric_min, metric_max, data);
                    return data;
                },
                py::arg("metric_min"),
                py::arg("metric_max"));
    }

    template<typename Dtype>
    void
    BindGridMapDrawer2D(const py::module &m, const char *name) {
        using T = GridMapDrawer2D<Dtype>;
        using Info = GridMapInfo2D<Dtype>;

        py::class_<T>(m, name)
            .def(py::init<const std::shared_ptr<Info> &>(), py::arg("grid_map_info"))
            .def_readwrite("grid_map_info", &T::grid_map_info)
            // .def_property(
            //     "image",
            //     [](const T &self) { return self.image; },
            //     [](T &self, const cv::Mat &image) { image.copyTo(self.image); })
            .def("reset_image", &T::ResetImage)
            .def(
                "draw_segments_inplace",
                &T::DrawSegmentsInplace,
                py::arg("mat"),
                py::arg("color"),
                py::arg("thickness"),
                py::arg("starts"),
                py::arg("ends"))
            .def(
                "draw_segments",
                &T::DrawSegments,
                py::arg("mat"),
                py::arg("color"),
                py::arg("thickness"),
                py::arg("starts"),
                py::arg("ends"))
            .def(
                "draw_rays_inplace",
                &T::DrawRaysInplace,
                py::arg("mat"),
                py::arg("color"),
                py::arg("thickness"),
                py::arg("starts"),
                py::arg("ends"))
            .def(
                "draw_rays",
                &T::DrawRays,
                py::arg("mat"),
                py::arg("color"),
                py::arg("thickness"),
                py::arg("starts"),
                py::arg("ends"))
            .def(
                "draw_polyline_inplace",
                &T::DrawPolylineInplace,
                py::arg("mat"),
                py::arg("color"),
                py::arg("thickness"),
                py::arg("closed"),
                py::arg("points"))
            .def(
                "draw_polyline",
                &T::DrawPolyline,
                py::arg("mat"),
                py::arg("color"),
                py::arg("thickness"),
                py::arg("closed"),
                py::arg("points"))
            .def(
                "draw_contour_inplace",
                &T::DrawContourInplace,
                py::arg("mat"),
                py::arg("color"),
                py::arg("thickness"),
                py::arg("contour"))
            .def(
                "draw_contour",
                &T::DrawContour,
                py::arg("mat"),
                py::arg("color"),
                py::arg("thickness"),
                py::arg("contour"))
            .def("show_image", &T::ShowImage, py::arg("title"));
    }
}  // namespace erl::common
