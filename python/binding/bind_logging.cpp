#include "pybind11_erl_common.hpp"

#include "erl_common/logging.hpp"

void
BindLogging(const py::module &m) {
    using namespace erl::common;

    py::class_<Logging> logging(m, "Logging");
    py::enum_<Logging::Level>(logging, "Level")
        .value("kInfo", Logging::Level::kInfo)
        .value("kDebug", Logging::Level::kDebug)
        .value("kWarn", Logging::Level::kWarn)
        .value("kError", Logging::Level::kError)
        .value("kSilent", Logging::Level::kSilent)
        .export_values();
    logging.def_static("get_level", &Logging::GetLevel)
        .def_static("set_level", &Logging::SetLevel)
        .def_static("get_date_str", &Logging::GetDateStr)
        .def_static("get_time_str", &Logging::GetTimeStr)
        .def_static("get_date_time_str", &Logging::GetDateTimeStr)
        .def_static("get_time_stamp", &Logging::GetTimeStamp)
        .def_static("info", &Logging::Info<std::string>, py::arg("msg"))
        .def_static("debug", &Logging::Debug<std::string>, py::arg("msg"))
        .def_static("warn", &Logging::Warn<std::string>, py::arg("msg"))
        .def_static("error", &Logging::Error<std::string>, py::arg("msg"))
        .def_static("fatal", &Logging::Fatal<std::string>, py::arg("msg"))
        .def_static("success", &Logging::Success<std::string>, py::arg("msg"))
        .def_static("failure", &Logging::Failure<std::string>, py::arg("msg"));
}
