#pragma once

#include "erl_common/storage_order.hpp"

namespace erl::common {

    /**
     * Eigen provides only Tensor support of fixed-rank (number of dimensions) or fixed-size.
     * Tensor supports dynamic tensor shape like NumPy NDArray.
     *
     * @tparam T
     */
    template<typename T, int Rank, bool RowMajor = true>
    class Tensor {
    public:
        using IndexType = int;
        using DataBufferType = Eigen::VectorX<T>;
        using ShapeType = Eigen::Vector<IndexType, Rank>;

    protected:
        DataBufferType m_data_;
        ShapeType m_shape_;
        ShapeType m_strides_;

    public:
        Tensor() = default;

        explicit Tensor(ShapeType shape)
            : m_shape_(std::move(shape)) {
            CheckShape();
            if (const IndexType total_size = Size(); total_size > 0) { m_data_.resize(total_size); }
        }

        Tensor(ShapeType shape, const T fill_value)
            : m_shape_(std::move(shape)) {
            CheckShape();
            if (IndexType total_size = Size(); total_size > 0) {
                m_data_.setConstant(total_size, fill_value);
            }
        }

        Tensor(ShapeType shape, DataBufferType data)
            : m_shape_(std::move(shape)) {
            CheckShape();
            IndexType total_size = Size();
            ERL_ASSERTM(total_size == data.size(), "shape and data are not matched.");
            if (total_size > 0) { m_data_ = data; }
        }

        Tensor(ShapeType shape, const std::function<T()> &data_init_func)
            : m_shape_(std::move(shape)) {
            CheckShape();
            if (IndexType total_size = Size(); total_size > 0) {
                m_data_.resize(total_size);
                for (IndexType i = 0; i < total_size; ++i) { m_data_[i] = data_init_func(); }
            }
        }

        DataBufferType &
        Data() {
            return m_data_;
        }

        [[nodiscard]] const DataBufferType &
        Data() const {
            return m_data_;
        }

        [[nodiscard]] const T *
        GetDataPtr() const {
            return m_data_.data();
        }

        T *
        GetMutableDataPtr() {
            return m_data_.data();
        }

        [[nodiscard]] IndexType
        Dims() const {
            return m_shape_.size();
        }

        [[nodiscard]] ShapeType
        Shape() const {
            return m_shape_;
        }

        [[nodiscard]] IndexType
        Size() const {
            if (Dims()) { return m_shape_.prod(); }
            return 0;
        }

        [[nodiscard]] bool
        IsRowMajor() const {
            return RowMajor;
        }

        void
        Fill(const T value) {
            if (IndexType total_size = Size(); total_size > 0) {
                m_data_.setConstant(total_size, value);
            }
        }

        T &
        operator[](const ShapeType &coords) {
            IndexType index = CoordsToIndex<IndexType>(m_strides_, coords);
            return m_data_[index];
        }

        [[nodiscard]] const T &
        operator[](const ShapeType &coords) const {
            IndexType index = CoordsToIndex<IndexType>(m_strides_, coords);
            return m_data_[index];
        }

        T &
        operator[](IndexType index) {
            return m_data_[index];
        }

        [[nodiscard]] const T &
        operator[](IndexType index) const {
            return m_data_[index];
        }

        class Slice {
        public:
            using SliceShape = Eigen::VectorX<IndexType>;

        private:
            Tensor &m_tensor_;
            std::vector<std::vector<IndexType>> m_index_map_;
            SliceShape m_shape_;
            SliceShape m_strides_;

        public:
            Slice(Tensor &tensor, const std::vector<std::pair<IndexType, IndexType>> &slice_layout)
                : m_tensor_(tensor) {

                ERL_ASSERTM(m_tensor_.Size() > 0, "tensor should not be empty");

                ERL_ASSERTM(!slice_layout.empty(), "slice_layout should not be empty");

                const IndexType ndim = tensor.Dims();
                m_index_map_.resize(ndim);
                for (const auto &[dim, index]: slice_layout) {
                    ERL_ASSERTM(
                        0 <= dim && dim < ndim,
                        "{}-dim is out of range for {}-dim shape",
                        dim,
                        ndim);
                    ERL_ASSERTM(
                        0 <= index && index < tensor.m_shape_[dim],
                        "{}-dim index {} is out of range for size {}",
                        dim,
                        index,
                        tensor.m_shape_[dim]);
                    m_index_map_[dim].push_back(index);
                }
                m_shape_ = tensor.m_shape_;
                IndexType slice_dims = 0;
                for (IndexType i = 0; i < ndim; ++i) {
                    const auto &indices = m_index_map_[i];
                    if (indices.empty()) {
                        m_shape_[slice_dims++] = tensor.m_shape_[i];
                        continue;
                    }
                    if (indices.size() == 1) { continue; }
                    m_shape_[slice_dims++] = static_cast<IndexType>(indices.size());
                }
                m_shape_.conservativeResize(slice_dims);
                m_strides_ = RowMajor ? ComputeCStrides<IndexType>(m_shape_, 1)
                                      : ComputeFStrides<IndexType>(m_shape_, 1);
            }

            [[nodiscard]] IndexType
            Dims() const {
                return m_shape_.size();
            }

            [[nodiscard]] SliceShape
            Shape() const {
                return m_shape_;
            }

            [[nodiscard]] IndexType
            Size() const {
                return m_shape_.prod();
            }

            T &
            operator[](const SliceShape &coords) {
                const ShapeType org_coords = GetOrgCoords(coords);
                return m_tensor_[org_coords];
            }

            [[nodiscard]] const T &
            operator[](const SliceShape &coords) const {
                const ShapeType org_coords = GetOrgCoords(coords);
                return m_tensor_[org_coords];
            }

            T &
            operator[](const IndexType index) {
                const SliceShape org_coords = IndexToCoordsWithStrides(m_strides_, index, RowMajor);
                return operator[](org_coords);
            }

            [[nodiscard]] const T &
            operator[](const IndexType index) const {
                const SliceShape org_coords = IndexToCoordsWithStrides(m_strides_, index, RowMajor);
                return operator[](org_coords);
            }

        private:
            ShapeType
            GetOrgCoords(const SliceShape &coords) const {
                ShapeType org_coords = ShapeType::Zero(m_tensor_.Dims());
                IndexType coord_idx = 0;
                for (IndexType i = 0; i < m_tensor_.Dims(); ++i) {
                    const auto &indices = m_index_map_[i];
                    if (indices.empty()) {
                        org_coords[i] = coords[coord_idx++];
                        continue;
                    }
                    if (indices.size() == 1) {
                        org_coords[i] = indices[0];
                        continue;
                    }
                    ERL_ASSERTM(
                        0 <= coords[coord_idx] &&
                            coords[coord_idx] < static_cast<IndexType>(indices.size()),
                        "slice coord {} at {}-dim is out of range for size {}",
                        coords[coord_idx],
                        i,
                        indices.size());
                    org_coords[i] = indices[coords[coord_idx++]];
                }
                return org_coords;
            }
        };

        [[nodiscard]] Slice
        GetSlice(const std::vector<std::pair<IndexType, IndexType>> &slice_layout) {
            return Slice(*this, slice_layout);
        }

        void
        Print(std::ostream &os) const {
            os << "Tensor, shape: " << m_shape_.transpose() << ", data: array of "
               << typeid(T).name() << std::endl;
        }

        [[nodiscard]] bool
        Write(std::ostream &s) const {
            const IndexType dims = m_shape_.size();
            if (dims == 0) { return s.good(); }
            s.write(reinterpret_cast<const char *>(&dims), sizeof(IndexType));
            s.write(reinterpret_cast<const char *>(m_shape_.data()), sizeof(IndexType) * dims);
            const auto data_size = static_cast<std::streamsize>(m_data_.size() * sizeof(T));
            s.write(reinterpret_cast<const char *>(m_data_.data()), data_size);
            return s.good();
        }

        [[nodiscard]] bool
        Read(std::istream &s) {
            IndexType dims = 0;
            s.read(reinterpret_cast<char *>(&dims), sizeof(IndexType));
            if (dims <= 0) { return s.good(); }
            ERL_DEBUG_ASSERT(
                Rank == Eigen::Dynamic || dims == Rank,
                "The number of dimensions read from the stream ({}) does not match the template "
                "parameter Rank ({}).",
                dims,
                Rank);
            if constexpr (Rank == Eigen::Dynamic) { m_shape_.resize(dims); }
            s.read(
                reinterpret_cast<char *>(m_shape_.data()),
                static_cast<std::streamsize>(sizeof(IndexType) * dims));
            CheckShape();
            if (const IndexType total_size = Size(); total_size > 0) {
                m_data_.resize(total_size);
                const auto data_size = static_cast<std::streamsize>(total_size * sizeof(T));
                s.read(reinterpret_cast<char *>(m_data_.data()), data_size);
            }
            return s.good();
        }

    private:
        void
        CheckShape() {
            for (IndexType i = 0; i < m_shape_.size(); ++i) {
                ERL_ASSERTM(m_shape_[i] >= 0, "negative size {} at {}-dim", m_shape_[i], i);
            }
            m_strides_ = RowMajor ? ComputeCStrides<IndexType>(m_shape_, 1)
                                  : ComputeFStrides<IndexType>(m_shape_, 1);
        }
    };

    template<typename T, int Rank, bool RowMajor = true>
    std::ostream &
    operator<<(std::ostream &os, const Tensor<T, Rank, RowMajor> &tensor) {
        tensor.Print(os);
        return os;
    }

    template<typename T, bool RowMajor = true>
    using TensorXD = Tensor<T, Eigen::Dynamic, RowMajor>;

    extern template class Tensor<double, 2>;
    extern template class Tensor<float, 2>;
    extern template class Tensor<int, 2>;
    extern template class Tensor<uint8_t, 2>;
    extern template class Tensor<double, 3>;
    extern template class Tensor<float, 3>;
    extern template class Tensor<int, 3>;
    extern template class Tensor<uint8_t, 3>;
    extern template class Tensor<double, Eigen::Dynamic>;
    extern template class Tensor<float, Eigen::Dynamic>;
    extern template class Tensor<int, Eigen::Dynamic>;
    extern template class Tensor<uint8_t, Eigen::Dynamic>;

    using Tensor2Dd = Tensor<double, 2>;
    using Tensor2Df = Tensor<float, 2>;
    using Tensor2Di = Tensor<int, 2>;
    using Tensor2D8u = Tensor<uint8_t, 2>;
    using Tensor3Dd = Tensor<double, 3>;
    using Tensor3Df = Tensor<float, 3>;
    using Tensor3Di = Tensor<int, 3>;
    using Tensor3D8u = Tensor<uint8_t, 3>;
    using TensorXDd = TensorXD<double>;
    using TensorXDf = TensorXD<float>;
    using TensorXDi = TensorXD<int>;
    using TensorXD8u = TensorXD<uint8_t>;
}  // namespace erl::common
