#pragma once

#include "grid_map_info.hpp"
#include "serialization.hpp"
#include "template_helper.hpp"
#include "tensor.hpp"

#include <memory>
#include <mutex>
#include <shared_mutex>

namespace erl::common {

    template<typename MapDtype, typename InfoDtype, int Dim, bool RowMajor = true>
    struct GridMap {  // for 2D, row-axis is x, column-axis is y
        static_assert(std::is_same_v<InfoDtype, double> || std::is_same_v<InfoDtype, float>);
        using Info = GridMapInfo<InfoDtype, Dim>;

        std::shared_ptr<Info> info;
        Tensor<MapDtype, Dim, RowMajor> data;

        explicit GridMap(std::shared_ptr<Info> grid_map_info)
            : info(NotNull(std::move(grid_map_info), true, "grid_map_info is nullptr.")),
              data(info->Shape()) {}

        GridMap(std::shared_ptr<Info> grid_map_info, MapDtype value)
            : info(NotNull(std::move(grid_map_info), true, "grid_map_info is nullptr.")),
              data(info->Shape(), value) {}

        GridMap(std::shared_ptr<Info> grid_map_info, Tensor<MapDtype, Dim, RowMajor> data)
            : info(NotNull(std::move(grid_map_info), true, "grid_map_info is nullptr.")),
              data(std::move(data)) {
            ERL_ASSERTM(data.Shape() == info->Shape(), "shape of data and info are not matched.");
        }

        GridMap(std::shared_ptr<Info> grid_map_info, Eigen::VectorX<MapDtype> data)
            : info(NotNull(std::move(grid_map_info), true, "grid_map_info is nullptr.")),
              data(info->Shape(), data) {}

        GridMap(
            std::shared_ptr<Info> grid_map_info,
            const std::function<MapDtype()> &data_init_func)
            : info(NotNull(std::move(grid_map_info), true, "grid_map_info is nullptr.")),
              data(info->Shape(), data_init_func) {}

        [[nodiscard]] bool
        Write(std::ostream &s) const {
            static const common::TokenWriteFunctionPairs<GridMap> token_function_pairs = {
                {
                    "info",
                    [](const GridMap *self, std::ostream &stream) {
                        return self->info->Write(stream);
                    },
                },
                {
                    "data",
                    [](const GridMap *self, std::ostream &stream) {
                        return self->data.Write(stream);
                    },
                },
            };
            return WriteTokens(s, this, token_function_pairs);
        }

        [[nodiscard]] bool
        Read(std::istream &s) {
            static const common::TokenReadFunctionPairs<GridMap> token_function_pairs = {
                {
                    "info",
                    [](GridMap *self, std::istream &stream) {
                        if (self->info == nullptr) { self->info = std::make_shared<Info>(); }
                        return self->info->Read(stream);
                    },
                },
                {
                    "data",
                    [](GridMap *self, std::istream &stream) { return self->data.Read(stream); },
                },
            };
            return ReadTokens(s, this, token_function_pairs);
        }
    };

    template<typename MapDtype, typename InfoDtype, int Dim, bool RowMajor>
    std::ostream &
    operator<<(std::ostream &os, const GridMap<MapDtype, InfoDtype, Dim, RowMajor> &grid_map) {
        grid_map.data.Print(os);
        return os;
    }

    template<typename MapDtype, typename InfoDtype, bool RowMajor = true>
    using GridMapX = GridMap<MapDtype, InfoDtype, Eigen::Dynamic, RowMajor>;

    template<typename Dtype, typename InfoDtype = Dtype>
    class IncrementalGridMap2D {
    public:
        using Info = GridMapInfo<InfoDtype, 2>;
        using MetricCoords = Eigen::Vector2<InfoDtype>;

    private:
        std::shared_ptr<Info> m_grid_map_info_;
        Eigen::MatrixX<Dtype> m_data_;
        std::function<Dtype()> m_data_init_func_;
        mutable std::shared_mutex
            m_mutex_;  // mutable for const methods, m_mutex_ is for thread-safe of m_data_

        enum ExtendCode {
            kToTopLeft = 0b1001,
            kToTopCentral = 0b1000,
            kToTopRight = 0b1010,
            kToCentralLeft = 0b0001,
            kToCentralRight = 0b0010,
            kToBottomLeft = 0b0101,
            kToBottomCentral = 0b0100,
            kToBottomRight = 0b0110,
            kNoExtend = 0b0000,
        };

    public:
        IncrementalGridMap2D() = delete;

        explicit IncrementalGridMap2D(
            std::shared_ptr<Info> grid_map_info,
            const std::function<Dtype()> &data_init_func = {})
            : m_grid_map_info_(std::move(grid_map_info)),
              m_data_(m_grid_map_info_->Shape(0), m_grid_map_info_->Shape(1)),
              m_data_init_func_(data_init_func) {
            ERL_ASSERTM(
                m_data_.cols() > 0 && m_data_.rows() > 0,
                "The shape of the grid map must be positive.");
        }

        [[nodiscard]] std::shared_ptr<Info>
        GetGridMapInfo() const {
            return m_grid_map_info_;
        }

        [[nodiscard]] MetricCoords
        GetCanonicalMetricCoords(const Eigen::Ref<const MetricCoords> &metric_coords) const {
            return {
                m_grid_map_info_->GridToMeterAtDim(
                    m_grid_map_info_->MeterToGridAtDim(metric_coords[0], 0),
                    0),
                m_grid_map_info_->GridToMeterAtDim(
                    m_grid_map_info_->MeterToGridAtDim(metric_coords[1], 1),
                    1)};
        }

        [[nodiscard]] Eigen::MatrixX8U
        AsImage(
            const std::shared_ptr<Info> &grid_map_info,
            const std::function<uint8_t(const Dtype &)> &cast_func) const {
            Eigen::MatrixX8U image;
            const long n_rows = m_data_.rows();
            const long n_cols = m_data_.cols();

            if (grid_map_info == nullptr) {
                image.resize(m_data_.rows(), m_data_.cols());
                for (int i = 0; i < n_rows; ++i) {
                    for (int j = 0; j < n_cols; ++j) {
                        auto &data = m_data_(i, j);
                        image(i, j) = cast_func(data);
                    }
                }
            } else {
                image.setConstant(grid_map_info->Shape(0), grid_map_info->Shape(1), 0);
                for (int i = 0; i < n_rows; ++i) {
                    InfoDtype x = m_grid_map_info_->GridToMeterAtDim(i, 0);
                    const int ii = grid_map_info->MeterToGridAtDim(x, 0);
                    for (int j = 0; j < n_cols; ++j) {
                        auto &data = m_data_(i, j);
                        InfoDtype y = m_grid_map_info_->GridToMeterAtDim(j, 1);
                        if (!grid_map_info->InMap(MetricCoords(x, y))) { continue; }
                        const int jj = grid_map_info->MeterToGridAtDim(y, 1);
                        image(ii, jj) = cast_func(data);
                    }
                }
            }

            return image;
        }

        /**
         * @brief Get the data at the grid coordinates. If the point is not in the grid map, return
         * 0 if T is a smart pointer, otherwise throw std::out_of_range.
         * @param x_grid Grid x coordinate
         * @param y_grid Grid y coordinate
         * @return
         */
        Dtype
        operator()(const int x_grid, const int y_grid) const {
            if (x_grid < 0 || y_grid < 0 || x_grid >= m_grid_map_info_->Shape(0) ||
                y_grid >= m_grid_map_info_->Shape(1)) {
                if (!IsSmartPtr<Dtype>::value) {
                    throw std::out_of_range("The grid coordinates are out of range.");
                }
                return 0;
            }
            return m_data_(x_grid, y_grid);
        }

        /**
         * @brief Get the data at the grid coordinates. If the point is not in the grid map, return
         * nullptr if T is a smart pointer or throw std::out_of_range otherwise. The data is
         * unmodifiable.
         * @param grid_coords Grid coordinates
         * @return
         */
        Dtype
        operator[](const Eigen::Ref<const Eigen::Vector2i> &grid_coords) const {
            return operator()(grid_coords[0], grid_coords[1]);
        }

        Dtype
        operator()(const InfoDtype x, const InfoDtype y) const {
            return operator()(
                m_grid_map_info_->MeterToGridAtDim(x, 0),
                m_grid_map_info_->MeterToGridAtDim(y, 1));
        }

        /**
         * @brief Get the data at the metric coordinates. If the point is not in the grid map,
         * return nullptr if T is a smart pointer or throw std::out_of_range otherwise. The data is
         * unmodifiable.
         * @param metric_coords Metric coordinates
         * @return
         */
        Dtype
        operator[](const Eigen::Ref<const MetricCoords> &metric_coords) const {
            return operator()(metric_coords[0], metric_coords[1]);
        }

        Dtype &
        GetMutableData(const int x_grid, const int y_grid) {
            ERL_DEBUG_ASSERT(
                x_grid >= 0 && y_grid >= 0 && x_grid < m_grid_map_info_->Shape(0) &&
                    y_grid < m_grid_map_info_->Shape(1),
                "The grid coordinates are out of the grid map, auto extend is not working "
                "properly.");
            auto &data = m_data_(x_grid, y_grid);
            if (IsSmartPtr<Dtype>::value && m_data_init_func_ != nullptr && data == 0) {
                data = m_data_init_func_();
            }
            return data;
        }

        Dtype &
        GetMutableData(const Eigen::Ref<const Eigen::Vector2i> &grid_coords) {
            return GetMutableData(grid_coords[0], grid_coords[1]);
        }

        Dtype &
        GetMutableData(const InfoDtype x, const InfoDtype y) {
            ERL_DEBUG_ASSERT(!omp_in_parallel(), "The grid map is not thread safe.");
            int x_grid = m_grid_map_info_->MeterToGridAtDim(x, 0);
            int y_grid = m_grid_map_info_->MeterToGridAtDim(y, 1);
            while (x_grid < 0 || y_grid < 0 || x_grid >= m_grid_map_info_->Shape(0) ||
                   y_grid >= m_grid_map_info_->Shape(1)) {
                Extend(GetExtendCode(x_grid, y_grid));
                x_grid = m_grid_map_info_->MeterToGridAtDim(x, 0);
                y_grid = m_grid_map_info_->MeterToGridAtDim(y, 1);
            }
            return GetMutableData(x_grid, y_grid);
        }

        Dtype &
        GetMutableData(const Eigen::Ref<const MetricCoords> &metric_coords) {
            return GetMutableData(metric_coords[0], metric_coords[1]);
        }

        Dtype &
        GetMutableDataThreadSafe(const InfoDtype x, const InfoDtype y) {
            std::lock_guard<std::shared_mutex> lock(m_mutex_);
            return GetMutableData(x, y);
        }

        Dtype &
        GetMutableDataThreadSafe(const Eigen::Ref<const MetricCoords> &metric_coords) {
            return GetMutableDataThreadSafe(metric_coords[0], metric_coords[1]);
        }

        Eigen::Ref<Eigen::MatrixX<Dtype>>
        GetBlock(int x_grid, int y_grid, int height, int width) {
            ERL_DEBUG_ASSERT(
                x_grid >= 0 && y_grid >= 0 && (x_grid + height <= m_grid_map_info_->Shape(0)) &&
                    (y_grid + width <= m_grid_map_info_->Shape(1)),
                "The grid coordinates (%d, %d) are out of the grid map or the block size (%d, %d) "
                "is too large.",
                x_grid,
                y_grid,
                height,
                width);
            return m_data_.block(x_grid, y_grid, height, width);
        }

        Eigen::Ref<Eigen::MatrixX<Dtype>>
        GetBlock(
            const InfoDtype x_min,
            const InfoDtype y_min,
            const InfoDtype x_max,
            const InfoDtype y_max,
            const bool safe_crop = true) {
            int x_min_grid = m_grid_map_info_->MeterToGridAtDim(x_min, 0);
            int y_min_grid = m_grid_map_info_->MeterToGridAtDim(y_min, 1);
            int x_max_grid = m_grid_map_info_->MeterToGridAtDim(x_max, 0);
            int y_max_grid = m_grid_map_info_->MeterToGridAtDim(y_max, 1);
            if (safe_crop) {
                x_min_grid = std::max(x_min_grid, 0);
                y_min_grid = std::max(y_min_grid, 0);
                x_max_grid = std::min(x_max_grid, m_grid_map_info_->Shape(0) - 1);
                y_max_grid = std::min(y_max_grid, m_grid_map_info_->Shape(1) - 1);
            }
            return GetBlock(
                x_min_grid,
                y_min_grid,
                x_max_grid - x_min_grid + 1,
                y_max_grid - y_min_grid + 1);
        }

        Eigen::Ref<Eigen::MatrixX<Dtype>>
        GetBlock(
            const Eigen::Ref<const MetricCoords> &metric_min,
            const Eigen::Ref<const MetricCoords> &metric_max,
            const bool &safe_crop = true) {
            return GetBlock(metric_min[0], metric_min[1], metric_max[0], metric_max[1], safe_crop);
        }

        void
        CollectNonZeroData(
            const InfoDtype x_min,
            const InfoDtype y_min,
            const InfoDtype x_max,
            const InfoDtype y_max,
            std::vector<Dtype> &data) {
            int x_min_grid = m_grid_map_info_->MeterToGridAtDim(x_min, 0);
            int y_min_grid = m_grid_map_info_->MeterToGridAtDim(y_min, 1);
            int x_max_grid = m_grid_map_info_->MeterToGridAtDim(x_max, 0);
            int y_max_grid = m_grid_map_info_->MeterToGridAtDim(y_max, 1);
            x_min_grid = std::max(x_min_grid, 0);
            y_min_grid = std::max(y_min_grid, 0);
            x_max_grid = std::min(x_max_grid, m_grid_map_info_->Shape(0) - 1);
            y_max_grid = std::min(y_max_grid, m_grid_map_info_->Shape(1) - 1);
            for (int i = x_min_grid; i <= x_max_grid; ++i) {
                for (int j = y_min_grid; j <= y_max_grid; ++j) {
                    auto &element = m_data_(i, j);
                    if (element != 0) { data.push_back(element); }
                }
            }
        }

        void
        CollectNonZeroData(
            const Eigen::Ref<const MetricCoords> &metric_min,
            const Eigen::Ref<const MetricCoords> &metric_max,
            std::vector<Dtype> &data) {
            CollectNonZeroData(metric_min[0], metric_min[1], metric_max[0], metric_max[1], data);
        }

    private:
        /**
         * @brief Get the extending code for a grid point not in the grid map.
         * @param x Grid x coordinate
         * @param y Grid y coordinate
         * @return
         * @refitem https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
         */
        ExtendCode
        GetExtendCode(const int x, const int y) const {

            int code = 0;

            if (x < 0) {
                code |= 0b1000;
            } else if (x >= m_grid_map_info_->Shape(0)) {
                code |= 0b0100;
            }

            if (y < 0) {
                code |= 0b0001;
            } else if (y >= m_grid_map_info_->Shape(1)) {
                code |= 0b0010;
            }

            return static_cast<ExtendCode>(code);
        }

        void
        Extend(ExtendCode code) {
            if (code == kNoExtend) { return; }

            const InfoDtype x_min = m_grid_map_info_->Min(0);
            const InfoDtype y_min = m_grid_map_info_->Min(1);
            const InfoDtype x_max = m_grid_map_info_->Max(0);
            const InfoDtype y_max = m_grid_map_info_->Max(1);
            const InfoDtype x_range = x_max - x_min;
            const InfoDtype y_range = y_max - y_min;
            const InfoDtype x_res = m_grid_map_info_->Resolution(0);
            const InfoDtype y_res = m_grid_map_info_->Resolution(1);
            InfoDtype new_x_min = x_min, new_y_min = y_min, new_x_max = x_max, new_y_max = y_max;
            switch (code) {
                case kToCentralLeft:
                case kToTopLeft:
                    new_x_min = x_min - x_range - x_res;
                    new_y_min = y_min - y_range - y_res;
                    new_x_max = x_max;
                    new_y_max = y_max;
                    break;
                case kToTopCentral:
                case kToTopRight:
                    new_x_min = x_min - x_range - x_res;
                    new_y_min = y_min;
                    new_x_max = x_max;
                    new_y_max = y_max + y_range + y_res;
                    break;
                case kToCentralRight:
                case kToBottomRight:
                    new_x_min = x_min;
                    new_y_min = y_min;
                    new_x_max = x_max + x_range + x_res;
                    new_y_max = y_max + y_range + y_res;
                    break;
                case kToBottomCentral:
                case kToBottomLeft:
                    new_x_min = x_min;
                    new_y_min = y_min - y_range - y_res;
                    new_x_max = x_max + x_range + x_res;
                    new_y_max = y_max;
                    break;
                case kNoExtend:
                    return;
            }
            long n_rows = m_data_.rows();
            long n_cols = m_data_.cols();
            auto new_grid_map_info = std::make_shared<Info>(
                Eigen::Vector2i(n_rows * 2 + 1, n_cols * 2 + 1),
                MetricCoords(new_x_min, new_y_min),
                MetricCoords(new_x_max, new_y_max));
            ERL_ASSERTM(
                std::abs(new_grid_map_info->Resolution(0) - x_res) < 1.e-10,
                "x resolution is not equal.");
            ERL_ASSERTM(
                std::abs(new_grid_map_info->Resolution(1) - y_res) < 1.e-10,
                "y resolution is not equal.");

            Eigen::MatrixX<Dtype> new_data(
                new_grid_map_info->Shape(0),
                new_grid_map_info->Shape(1));
            // copy the old data matrix to the new data matrix
            Eigen::Vector2i loc =
                new_grid_map_info->MeterToGridForPoints(MetricCoords(x_min, y_min));
            new_data.block(loc[0], loc[1], n_rows, n_cols) = m_data_;

            // swap the results
            m_grid_map_info_ = std::move(new_grid_map_info);
            m_data_.swap(new_data);
        }
    };

}  // namespace erl::common
