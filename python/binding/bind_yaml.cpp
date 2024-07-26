#include "pybind11_erl_common.hpp"

#include "erl_common/yaml.hpp"

void
BindYaml(py::module &m) {
    using namespace erl::common;
    const py::module &submodule = m.def_submodule("yaml", "Interface for YAML");
    py::class_<YamlableBase, std::shared_ptr<YamlableBase>>(submodule, "YamlableBase")
        .def("as_yaml_string", &YamlableBase::AsYamlString)
        .def("as_yaml_file", &YamlableBase::AsYamlFile, py::arg("yaml_file"))
        .def("from_yaml_string", &YamlableBase::FromYamlString, py::arg("yaml_str"))
        .def("from_yaml_file", &YamlableBase::FromYamlFile, py::arg("yaml_file"))
        .def("__str__", [](const YamlableBase &self) -> std::string {
            std::stringstream ss;
            ss << self;
            return ss.str();
        });
}
