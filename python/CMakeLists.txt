erl_generate_python_package(
        PY_PACKAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}
        PYBIND_MODULE_NAME py${PROJECT_NAME}
        PY_MODULES common
)
