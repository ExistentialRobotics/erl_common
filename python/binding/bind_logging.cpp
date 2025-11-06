#include "pybind11_erl_common.hpp"

#include "erl_common/logging.hpp"

void
BindLogging(const py::module &m) {
    using namespace erl::common;

    py::enum_<LoggingLevel>(m, "Level")
        .value("kInfo", LoggingLevel::kInfo)
        .value("kDebug", LoggingLevel::kDebug)
        .value("kWarn", LoggingLevel::kWarn)
        .value("kError", LoggingLevel::kError)
        .value("kSilent", LoggingLevel::kSilent)
        .export_values();
    py::class_<Logging> logging(m, "Logging");
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
