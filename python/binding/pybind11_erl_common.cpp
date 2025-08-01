#include "pybind11_erl_common.hpp"

#include "erl_common/logging.hpp"
#include "erl_common/random.hpp"
#include "erl_common/yaml.hpp"

using namespace erl::common;

void
BindYaml(py::module &m);

void
BindStorage(py::module &m);

void
BindLogging(const py::module &m);

PYBIND11_MODULE(PYBIND_MODULE_NAME, m) {
    m.doc() = "Python 3 Interface of erl_common";
    m.def("set_global_random_seed", &SetGlobalRandomSeed, py::arg("seed"));

    BindYaml(m);
    BindStorage(m);
    BindLogging(m);
}
