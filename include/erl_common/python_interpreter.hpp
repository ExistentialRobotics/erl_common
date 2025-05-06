#pragma once

#include "erl_common/pybind11.hpp"

#include <pybind11/embed.h>

#include <memory>

namespace erl::common {

    // Python interpreter, shared among all instances that use Python runtime
    inline static std::shared_ptr<py::scoped_interpreter> g_py_interpreter_guard_ = nullptr;

    /**
     * Any instance that uses Python runtime should call this function to initialize Python
     * interpreter and keep the guard to prevent the interpreter from being finalized before the
     * instance is destructed.
     * @return Python interpreter guard
     */
    inline std::shared_ptr<py::scoped_interpreter>
    InitPythonInterpreter() {
        if (!g_py_interpreter_guard_) {
            g_py_interpreter_guard_ = std::make_shared<py::scoped_interpreter>();
        }
        return g_py_interpreter_guard_;
    }

}  // namespace erl::common
