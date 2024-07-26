#include "pybind11_erl_common.hpp"

#include "erl_common/logging.hpp"

void
BindLogging(const py::module &m) {
    using namespace erl::common;

    py::class_<Logging> logging(m, "Logging");
    py::enum_<Logging::Level>(logging, "Level")
        .value("INFO", Logging::Level::INFO)
        .value("DEBUG", Logging::Level::DEBUG)
        .value("WARN", Logging::Level::WARN)
        .value("ERROR", Logging::Level::ERROR)
        .value("SILIENT", Logging::Level::SILIENT)
        .export_values();
    logging.def_property_static("level", &Logging::GetLevel, &Logging::SetLevel)
        .def_static("get_date_str", &Logging::GetDateStr)
        .def_static("get_time_str", &Logging::GetTimeStr)
        .def_static("get_date_time_str", &Logging::GetDateTimeStr)
        .def_static("get_time_stamp", &Logging::GetTimeStamp);
}
