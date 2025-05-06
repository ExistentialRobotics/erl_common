#pragma once

#include "erl_common/storage_order.hpp"

#include <numeric>
#include <unordered_set>

namespace erl::common {

    /**
     * Eigen provides only Tensor support of fixed-rank (number of dimensions) or fixed-size.
     * Tensor supports dynamic tensor shape like NumPy NDArray.
     *
     * @tparam T
     */
    template<typename T, int Rank, bool RowMajor = true>
    class Tensor {

    protected:
        Eigen::VectorX<T> m_data_;
        Eigen::Vector<int, Rank> m_shape_;

    public:
        Tensor() = default;

        explicit Tensor(Eigen::VectorXi shape)
            : m_shape_(std::move(shape)) {
            CheckShape();
            int total_size = Size();
            if (total_size > 0) { m_data_.resize(total_size); }
        }

        Tensor(Eigen::VectorXi shape, const T fill_value)
            : m_shape_(std::move(shape)) {
            CheckShape();
            if (int total_size = Size(); total_size > 0) {
                m_data_.setConstant(total_size, fill_value);
            }
        }

        Tensor(Eigen::VectorXi shape, Eigen::VectorX<T> data)
            : m_shape_(std::move(shape)) {
            CheckShape();
            int total_size = Size();
            ERL_ASSERTM(total_size == data.size(), "shape and data are not matched.");
            if (total_size > 0) { m_data_ = data; }
        }

        Tensor(Eigen::VectorXi shape, const std::function<T(void)> &data_init_func)
            : m_shape_(std::move(shape)) {
            CheckShape();
            int total_size = Size();
            if (total_size > 0) {
                m_data_.resize(total_size);
                for (int i = 0; i < total_size; ++i) { m_data_[i] = data_init_func(); }
            }
        }

        Eigen::VectorX<T> &
        Data() {
            return m_data_;
        }

        const Eigen::VectorX<T> &
        Data() const {
            return m_data_;
        }

        const T *
        GetDataPtr() const {
            return m_data_.data();
        }

        T *
        GetMutableDataPtr() {
            return m_data_.data();
        }

        [[nodiscard]] int
        Dims() const {
            return m_shape_.size();
        }

        [[nodiscard]] inline Eigen::VectorXi
        Shape() const {
            return m_shape_;
        }

        [[nodiscard]] inline int
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
            if (int total_size = Size(); total_size > 0) { m_data_.setConstant(total_size, value); }
        }

        T &
        operator[](const Eigen::Ref<const Eigen::Vector<int, Rank>> &coords) {
            int index = CoordsToIndex<Rank>(m_shape_, coords, RowMajor);
            return m_data_[index];
        }

        const T &
        operator[](const Eigen::Ref<const Eigen::Vector<int, Rank>> &coords) const {
            int index = CoordsToIndex<Rank>(m_shape_, coords, RowMajor);
            return m_data_[index];
        }

        T &
        operator[](int index) {
            return m_data_[index];
        }

        const T &
        operator[](int index) const {
            return m_data_[index];
        }

        Tensor<T, Eigen::Dynamic>
        GetSlice(
            const std::vector<int> &dims_to_remove,
            const std::vector<int> &dim_indices_at_removed) const {
            ERL_ASSERTM(
                dims_to_remove.size() == dim_indices_at_removed.size(),
                "dims_to_remove and dim_indices_at_removed should be of the same size");
            ERL_ASSERTM(!dims_to_remove.empty(), "dims_to_remove should not be empty");
            ERL_ASSERTM(
                !dim_indices_at_removed.empty(),
                "dim_indices_at_removed should not be empty");
            ERL_ASSERTM(
                std::unordered_set(dims_to_remove.begin(), dims_to_remove.end()).size() ==
                    dims_to_remove.size(),
                "there are duplicate dims in dims_to_remove");

            const auto ndim = Dims();
            // Remove unwanted dimensions
            std::vector<int> dims_to_keep(ndim);
            std::iota(dims_to_keep.begin(), dims_to_keep.end(), 0);

            // descending order ensures that indices remain valid when removing
            std::vector<int> sorted_indices(dims_to_remove.size());
            std::iota(sorted_indices.begin(), sorted_indices.end(), 0);
            std::sort(sorted_indices.begin(), sorted_indices.end(), std::greater());
            Eigen::VectorXi coords = Eigen::VectorXi::Zero(ndim);
            for (auto &i: sorted_indices) {
                ERL_ASSERTM(
                    0 <= dims_to_remove[i] && dims_to_remove[i] < ndim,
                    "%d-dim is out of range for %d-dim shape",
                    dims_to_remove[i],
                    ndim);
                dims_to_keep.erase(dims_to_keep.begin() + dims_to_remove[i]);
                coords[dims_to_remove[i]] = dim_indices_at_removed[i];
            }

            // generate new tensor
            Eigen::VectorXi slice_shape(dims_to_keep.size());
            for (std::size_t i = 0; i < dims_to_keep.size(); ++i) {
                slice_shape[static_cast<long>(i)] = m_shape_[dims_to_keep[i]];
            }
            Tensor<T, Eigen::Dynamic> slice(slice_shape);
            for (int i = 0; i < slice.Size(); ++i) {
                auto slice_coords = IndexToCoords<Eigen::Dynamic>(slice_shape, i, RowMajor);
                for (int j = 0; j < static_cast<int>(dims_to_keep.size()); ++j) {
                    coords[dims_to_keep[j]] = slice_coords[j];
                }
                slice[i] = (*this)[coords];
            }

            return slice;
        }

        void
        Print(std::ostream &os) const {
            os << "Tensor, shape: " << m_shape_.transpose() << ", data: array of "
               << typeid(T).name() << std::endl;
        }

    private:
        void
        CheckShape() {
            for (int i = 0; i < m_shape_.size(); ++i) {
                ERL_ASSERTM(m_shape_[i] >= 0, "negative size %d at %d-dim", m_shape_[i], i);
            }
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
