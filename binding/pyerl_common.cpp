#include "erl_common/pybind11_erl_common.hpp"

PYBIND11_MODULE(PYBIND_MODULE_NAME, m) {
    m.doc() = "Python 3 Interface of erl_common";
    BindCommon(m);
}
