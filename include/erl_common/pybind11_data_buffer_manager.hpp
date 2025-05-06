#pragma once

#include "data_buffer_manager.hpp"
#include "pybind11.hpp"

namespace erl::common {
    template<typename T, class Buffer>
    auto
    BindDataBufferManagerImpl(const py::module &m, const char *name) {
        using namespace erl::common;
        using Manager = DataBufferManager<T, Buffer>;
        return py::class_<Manager>(m, name)
            .def(py::init<>())
            .def("size", &Manager::Size)
            .def("reserve", &Manager::Reserve)
            .def(
                "add_entry",
                [](Manager &self, const T &entry) { return self.AddEntry(entry); },
                py::arg("entry"))
            .def("remove_entry", &Manager::RemoveEntry, py::arg("index"))
            .def(
                "__getitem__",
                py::overload_cast<std::size_t>(&Manager::operator[], py::const_),
                py::arg("index"))
            .def_property_readonly("entries", &Manager::GetEntries)
            .def("clear", &Manager::Clear);
    }
}  // namespace erl::common
