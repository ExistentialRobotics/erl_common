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
    logging.def_property_static("level", &Logging::GetLevel, &Logging::SetLevel)
        .def_static("get_date_str", &Logging::GetDateStr)
        .def_static("get_time_str", &Logging::GetTimeStr)
        .def_static("get_date_time_str", &Logging::GetDateTimeStr)
        .def_static("get_time_stamp", &Logging::GetTimeStamp);
}
