#include "erl_common/tensor.hpp"

namespace erl::common {

    template class Tensor<double, 2>;
    template class Tensor<float, 2>;
    template class Tensor<int, 2>;
    template class Tensor<uint8_t, 2>;
    template class Tensor<double, 3>;
    template class Tensor<float, 3>;
    template class Tensor<int, 3>;
    template class Tensor<uint8_t, 3>;
    template class Tensor<double, Eigen::Dynamic>;
    template class Tensor<float, Eigen::Dynamic>;
    template class Tensor<int, Eigen::Dynamic>;
    template class Tensor<uint8_t, Eigen::Dynamic>;

}  // namespace erl::common
