#pragma once

#include "erl_common/pybind11.hpp"
#include "erl_common/grid_map.hpp"

namespace py = pybind11;

namespace erl::common {
    template<int Dim>
    auto
    BindGridMapInfo(py::module &m, const std::string &name) {
        using Self = GridMapInfo<Dim>;
        return py::class_<Self, std::shared_ptr<Self>>(m, name.c_str())
            .def(
                py::init<const Eigen::Vector<double, Dim> &, const Eigen::Vector<double, Dim> &, const Eigen::Vector<double, Dim> &, Eigen::Vector<int, Dim>>(),
                py::arg("min"),
                py::arg("max"),
                py::arg("resolution"),
                py::arg("padding"))
            .def(
                py::init<Eigen::Vector<int, Dim>, const Eigen::Vector<double, Dim> &, const Eigen::Vector<double, Dim> &>(),
                py::arg("map_shape"),
                py::arg("min"),
                py::arg("max"))
            .def(
                "extend",
                py::overload_cast<int, double, double, int>(&Self::Extend, py::const_),
                py::arg("size"),
                py::arg("min"),
                py::arg("max"),
                py::arg("dim"))
            .def(
                "extend",
                py::overload_cast<double, double, double, int, int>(&Self::Extend, py::const_),
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
            .def_property_readonly("resolution", py::overload_cast<>(&Self::Resolution, py::const_))
            .def("resolution_at", py::overload_cast<int>(&Self::Resolution, py::const_), py::arg("dim"))
            .def_property_readonly("center", &Self::Center)
            .def_property_readonly("center_grid", &Self::CenterGrid)
            .def("get_dim_lin_space", &Self::GetDimLinSpace, py::arg("dim"))
            .def("grid_to_meter_for_value", &Self::GridToMeterForValue, py::arg("grid_value"), py::arg("dim"))
            .def("grid_to_meter_for_values", &Self::GridToMeterForValues, py::arg("grid_values"), py::arg("dim"))
            .def("meter_to_grid_for_value", &Self::MeterToGridForValue, py::arg("meter_value"), py::arg("dim"))
            .def("meter_to_grid_for_values", &Self::MeterToGridForValues, py::arg("meter_values"), py::arg("dim"))
            .def("grid_to_meter_for_points", &Self::GridToMeterForPoints, py::arg("grid_points"))
            .def("meter_to_grid_for_points", &Self::MeterToGridForPoints, py::arg("meter_points"))
            .def("grid_to_meter_for_vectors", &Self::GridToMeterForVectors, py::arg("grid_vectors"))
            .def("meter_to_grid_for_vectors", &Self::MeterToGridForVectors, py::arg("meter_vectors"))
            .def("in_map", &Self::InMap, py::arg("meter_point"))
            .def("in_grids", &Self::InGrids, py::arg("grid_point"))
            .def("grid_to_index", &Self::GridToIndex, py::arg("grid"), py::arg("c_stride"))
            .def("index_to_grid", &Self::IndexToGrid, py::arg("index"), py::arg("c_stride"))
            .def("generate_grid_coordinates", &Self::GenerateGridCoordinates, py::arg("c_stride"))
            .def("generate_meter_coordinates", &Self::GenerateMeterCoordinates, py::arg("c_stride"))
            .def("ray_casting", &Self::RayCasting, py::arg("start"), py::arg("end"));
    }

    // T must be POD or arithmetic type, which is required by pybind11's NumPy interface
    template<typename T, int Rank>
    typename std::enable_if<py::supported_by_numpy<T>::value, py::class_<Tensor<T, Rank>>>::type
    BindTensor(py::module &m, const std::string &name) {
        using Self = Tensor<T, Rank>;
        return py::class_<Self>(m, name.c_str(), py::buffer_protocol())
            .def(py::init([&](const py::array_t<T, py::array::c_style> &b) {
                py::buffer_info info = b.request();
                std::string buffer_format = info.format;
                std::string expected_format = py::format_descriptor<T>::format();
                if (buffer_format != expected_format) { throw std::runtime_error("Incompatible format: " + buffer_format + " vs " + expected_format); }
                if (Rank != -1) { ERL_ASSERTM(Rank == info.ndim, "Incompatible ndim for %s: %ld", name.c_str(), info.ndim); }
                Eigen::VectorXi tensor_shape(info.ndim);
                for (int i = 0; i < Rank; ++i) { tensor_shape[i] = int(info.shape[i]); }
                Eigen::VectorX<T> data(info.size);
                auto ptr = static_cast<T *>(info.ptr);
                std::copy(ptr, ptr + info.size, data.begin());
                Self tensor(tensor_shape, data);
                return tensor;
            }))
            .def_buffer([](Self &tensor) -> py::buffer_info {
                auto shape = tensor.Shape();
                std::vector<py::ssize_t> array_shape(shape.size());
                std::copy(shape.begin(), shape.end(), array_shape.begin());
                std::vector<py::ssize_t> strides = ComputeCStrides<py::ssize_t>(array_shape, sizeof(T));
                return {tensor.GetMutableDataPtr(), sizeof(T), py::format_descriptor<T>::format(), tensor.Dims(), array_shape, strides};
            })
            .def(py::init<Eigen::VectorXi, const T &>(), py::arg("shape"), py::arg("constant"))
            .def(py::init<Eigen::VectorXi, Eigen::VectorX<T>>(), py::arg("shape"), py::arg("data"))
            .def(py::init<Eigen::VectorXi, const std::function<T(void)> &>(), py::arg("shape"), py::arg("data_init_func"))
            .def_property_readonly("dims", &Self::Dims)
            .def_property_readonly("shape", &Self::Shape)
            .def_property_readonly("size", &Self::Size)
            .def_property_readonly("is_row_major", &Self::IsRowMajor)
            .def("fill", &Self::Fill, py::arg("value"))
            .def(
                "__setitem__",
                [](Self &tensor, const Eigen::Ref<const Eigen::Vector<int, Rank>> &coords, T value) { tensor[coords] = value; },
                py::arg("coords"),
                py::arg("value"))
            .def("__getitem__", py::overload_cast<const Eigen::Ref<const Eigen::Vector<int, Rank>> &>(&Self::operator[], py::const_), py::arg("coords"))
            .def(
                "__setitem__",
                [](Self &tensor, int index, T value) { tensor[index] = value; },
                py::arg("index"),
                py::arg("value"))
            .def("__getitem__", py::overload_cast<int>(&Self::operator[], py::const_), py::arg("index"))
            .def("get_slice", &Self::GetSlice, py::arg("dims_to_remove"), py::arg("dim_indices_at_removed"))
            .def("__str__", [](Self &tensor) -> std::string {
                std::stringstream ss;
                tensor.Print(ss);
                return ss.str();
            });
    }

    template<typename T, int Rank>
    typename std::enable_if<!py::supported_by_numpy<T>::value, py::class_<Tensor<T, Rank>>>::type
    BindTensor(py::module &m, const std::string &name) {
        using Self = Tensor<T, Rank>;
        return py::class_<Self>(m, name.c_str())
            .def(py::init<Eigen::VectorXi, const T &>(), py::arg("shape"), py::arg("constant"))
            .def(
                py::init<>([](Eigen::VectorXi shape, const std::vector<T> &data) {
                    Eigen::Map<const Eigen::VectorX<T>> data_map(data.data(), data.size());
                    return Self(shape, Eigen::VectorX<T>(data_map));
                }),
                py::arg("shape"),
                py::arg("data"))
            .def(py::init<Eigen::VectorXi, const std::function<T(void)> &>(), py::arg("shape"), py::arg("data_init_func"))
            .def_property_readonly("dims", &Self::Dims)
            .def_property_readonly("shape", &Self::Shape)
            .def_property_readonly("size", &Self::Size)
            .def_property_readonly("is_row_major", &Self::IsRowMajor)
            .def("fill", &Self::Fill, py::arg("value"))
            .def(
                "__setitem__",
                [](Self &tensor, const Eigen::Ref<const Eigen::Vector<int, Rank>> &coords, T value) { tensor[coords] = value; },
                py::arg("coords"),
                py::arg("value"))
            .def("__getitem__", py::overload_cast<const Eigen::Ref<const Eigen::Vector<int, Rank>> &>(&Self::operator[], py::const_), py::arg("coords"))
            .def(
                "__setitem__",
                [](Self &tensor, int index, T value) { tensor[index] = value; },
                py::arg("index"),
                py::arg("value"))
            .def("__getitem__", py::overload_cast<int>(&Self::operator[], py::const_), py::arg("index"))
            .def("get_slice", &Self::GetSlice, py::arg("dims_to_remove"), py::arg("dim_indices_at_removed"))
            .def("__str__", [](Self &tensor) -> std::string {
                std::stringstream ss;
                tensor.Print(ss);
                return ss.str();
            });
    }

    template<typename T, int Dim>  // T is supported by NumPy interface provided by pybind11
    typename std::enable_if<py::supported_by_numpy<T>::value, py::class_<GridMap<T, Dim>, std::shared_ptr<GridMap<T, Dim>>>>::type
    BindGridMap(py::module &m, const std::string &name) {
        using Self = GridMap<T, Dim>;
        auto py_class =
            py::class_<Self, std::shared_ptr<Self>>(m, name.c_str())
                .def(py::init<std::shared_ptr<GridMapInfo<Dim>>>(), py::arg("grid_map_info"))
                .def(py::init<std::shared_ptr<GridMapInfo<Dim>>, T>(), py::arg("grid_map_info"), py::arg("value"))
                .def(py::init<std::shared_ptr<GridMapInfo<Dim>>, Tensor<T, Dim>>(), py::arg("grid_map_info"), py::arg("data"))
                .def(py::init<std::shared_ptr<GridMapInfo<Dim>>, const std::function<T(void)> &>(), py::arg("grid_map_info"), py::arg("data_init_func"))
                .def_readwrite("data", &Self::data)
                .def_readwrite("info", &Self::info);
        py_class.def(py::init<std::shared_ptr<GridMapInfo<Dim>>, Eigen::VectorX<T>>(), py::arg("grid_map_info"), py::arg("data"));
        return py_class;
    }

    template<typename T, int Dim>
    typename std::enable_if<!py::supported_by_numpy<T>::value, py::class_<GridMap<T, Dim>, std::shared_ptr<GridMap<T, Dim>>>>::type
    BindGridMap(py::module &m, const std::string &name) {
        using Self = GridMap<T, Dim>;
        auto py_class =
            py::class_<Self, std::shared_ptr<Self>>(m, name.c_str())
                .def(py::init<std::shared_ptr<GridMapInfo<Dim>>>(), py::arg("grid_map_info"))
                .def(py::init<std::shared_ptr<GridMapInfo<Dim>>, T>(), py::arg("grid_map_info"), py::arg("value"))
                .def(py::init<std::shared_ptr<GridMapInfo<Dim>>, Tensor<T, Dim>>(), py::arg("grid_map_info"), py::arg("data"))
                .def(py::init<std::shared_ptr<GridMapInfo<Dim>>, const std::function<T(void)> &>(), py::arg("grid_map_info"), py::arg("data_init_func"))
                .def_readwrite("data", &Self::data)
                .def_readwrite("info", &Self::info);
        py_class.def(
            py::init<>([](std::shared_ptr<GridMapInfo<Dim>> grid_map_info, const std::vector<T> &data) {
                Eigen::Map<const Eigen::VectorX<T>> data_map(data.data(), data.size());
                return std::make_shared<Self>(grid_map_info, Eigen::VectorX<T>(data_map));
            }),
            py::arg("grid_map_info"),
            py::arg("data"));
        return py_class;
    }

    template<typename T>
    typename std::enable_if<py::supported_by_numpy<T>::value, py::class_<IncrementalGridMap2D<T>>>::type
    BindIncrementalGridMap2D(py::module &m, const std::string &name) {
        using Self = IncrementalGridMap2D<T>;
        return py::class_<Self, std::shared_ptr<Self>>(m, name.c_str())
            .def(
                py::init<>([](std::shared_ptr<GridMapInfo2D> grid_map_info, const std::function<T(void)> &data_init_func) {
                    return std::make_shared<Self>(grid_map_info, data_init_func);
                }),
                py::arg("grid_map_info"),
                py::arg("data_init_func") = py::none())
            .def_property_readonly("grid_map_info", &Self::GetGridMapInfo)
            .def("get_canonical_metric_coords", &Self::GetCanonicalMetricCoords, py::arg("metric_coords"))
            .def(
                "as_image",
                py::overload_cast<const std::shared_ptr<GridMapInfo2D> &, const std::function<uint8_t(const T &)> &>(&Self::AsImage, py::const_),
                py::arg("grid_map_info"),
                py::arg("cast_func").none(false))
            .def("__call__", py::overload_cast<int, int>(&Self::operator(), py::const_), py::arg("x_grid"), py::arg("y_grid"))
            .def("__call__", py::overload_cast<double, double>(&Self::operator(), py::const_), py::arg("x"), py::arg("y"))
            .def("__getitem__", py::overload_cast<const Eigen::Ref<const Eigen::Vector2i> &>(&Self::operator[], py::const_), py::arg("grid_coords"))
            .def("__getitem__", py::overload_cast<const Eigen::Ref<const Eigen::Vector2d> &>(&Self::operator[], py::const_), py::arg("metric_coords"))
            .def(
                "__setitem__",
                [](Self &self, const Eigen::Ref<const Eigen::Vector2i> &grid_coords, const T &data) { self.GetMutableData(grid_coords) = data; },
                py::arg("grid_coords"),
                py::arg("data_container"))
            .def(
                "__setitem__",
                [](Self &self, const Eigen::Ref<const Eigen::Vector2d> &metric_coords, const T &data) { self.GetMutableData(metric_coords) = data; },
                py::arg("metric_coords"),
                py::arg("data_container"))
            .def("get_block", py::overload_cast<int, int, int, int>(&Self::GetBlock), py::arg("x_grid"), py::arg("y_grid"), py::arg("height"), py::arg("width"))
            .def(
                "get_block",
                py::overload_cast<double, double, double, double, bool>(&Self::GetBlock),
                py::arg("x_min"),
                py::arg("y_min"),
                py::arg("x_max"),
                py::arg("y_max"),
                py::arg("safe_crop") = true)
            .def(
                "get_block",
                py::overload_cast<const Eigen::Ref<const Eigen::Vector2d> &, const Eigen::Ref<const Eigen::Vector2d> &, bool>(&Self::GetBlock),
                py::arg("metric_min"),
                py::arg("metric_max"),
                py::arg("safe_crop") = true)
            .def(
                "collect_non_zero_data",
                [](Self &self, double x_min, double y_min, double x_max, double y_max) {
                    std::vector<T> data;
                    self.CollectNonZeroData(x_min, y_min, x_max, y_max, data);
                    return data;
                },
                py::arg("x_min"),
                py::arg("y_min"),
                py::arg("x_max"),
                py::arg("y_max"))
            .def(
                "collect_non_zero_data",
                [](Self &self, const Eigen::Ref<const Eigen::Vector2d> &metric_min, const Eigen::Ref<const Eigen::Vector2d> &metric_max) {
                    std::vector<T> data;
                    self.CollectNonZeroData(metric_min, metric_max, data);
                    return data;
                },
                py::arg("metric_min"),
                py::arg("metric_max"));
    }

    template<typename T>
    typename std::enable_if<!py::supported_by_numpy<T>::value, py::class_<IncrementalGridMap2D<T>>>::type
    BindIncrementalGridMap2D(py::module &m, const std::string &name) {
        using Self = IncrementalGridMap2D<T>;
        return py::class_<Self, std::shared_ptr<Self>>(m, name.c_str())
            .def(
                py::init<>([](std::shared_ptr<GridMapInfo2D> grid_map_info, const std::function<T(void)> &data_init_func) {
                    return std::make_shared<Self>(grid_map_info, data_init_func);
                }),
                py::arg("grid_map_info"),
                py::arg("data_init_func") = py::none())
            .def_property_readonly("grid_map_info", &Self::GetGridMapInfo)
            .def("get_canonical_metric_coords", &Self::GetCanonicalMetricCoords, py::arg("metric_coords"))
            .def(
                "as_image",
                py::overload_cast<const std::shared_ptr<GridMapInfo2D> &, const std::function<uint8_t(const T &)> &>(&Self::AsImage, py::const_),
                py::arg("grid_map_info"),
                py::arg("cast_func").none(false))
            .def("__call__", py::overload_cast<int, int>(&Self::operator(), py::const_), py::arg("x_grid"), py::arg("y_grid"))
            .def("__call__", py::overload_cast<double, double>(&Self::operator(), py::const_), py::arg("x"), py::arg("y"))
            .def("__getitem__", py::overload_cast<const Eigen::Ref<const Eigen::Vector2i> &>(&Self::operator[], py::const_), py::arg("grid_coords"))
            .def("__getitem__", py::overload_cast<const Eigen::Ref<const Eigen::Vector2d> &>(&Self::operator[], py::const_), py::arg("metric_coords"))
            .def(
                "__setitem__",
                [](Self &self, const Eigen::Ref<const Eigen::Vector2i> &grid_coords, const T &data) { self.GetMutableData(grid_coords) = data; },
                py::arg("grid_coords"),
                py::arg("data_container"))
            .def(
                "__setitem__",
                [](Self &self, const Eigen::Ref<const Eigen::Vector2d> &metric_coords, const T &data) { self.GetMutableData(metric_coords) = data; },
                py::arg("metric_coords"),
                py::arg("data_container"))
            .def(
                "get_block",
                [](Self &self, int x_grid, int y_grid, int height, int width) {
                    auto block = self.GetBlock(x_grid, y_grid, height, width);
                    std::vector<std::vector<T>> data(height);
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
                [](Self &self, double x_min, double y_min, double x_max, double y_max, bool safe_crop) {
                    auto block = self.GetBlock(x_min, y_min, x_max, y_max, safe_crop);
                    long height = block.rows();
                    long width = block.cols();
                    std::vector<std::vector<T>> data(height);
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
                [](Self &self, const Eigen::Ref<const Eigen::Vector2d> &metric_min, const Eigen::Ref<const Eigen::Vector2d> &metric_max, bool safe_crop) {
                    auto block = self.GetBlock(metric_min, metric_max, safe_crop);
                    long height = block.rows();
                    long width = block.cols();
                    std::vector<std::vector<T>> data(height);
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
                [](Self &self, double x_min, double y_min, double x_max, double y_max) {
                    std::vector<T> data;
                    self.CollectNonZeroData(x_min, y_min, x_max, y_max, data);
                    return data;
                },
                py::arg("x_min"),
                py::arg("y_min"),
                py::arg("x_max"),
                py::arg("y_max"))
            .def(
                "collect_non_zero_data",
                [](Self &self, const Eigen::Ref<const Eigen::Vector2d> &metric_min, const Eigen::Ref<const Eigen::Vector2d> &metric_max) {
                    std::vector<T> data;
                    self.CollectNonZeroData(metric_min, metric_max, data);
                    return data;
                },
                py::arg("metric_min"),
                py::arg("metric_max"));
    }
}  // namespace erl::common
