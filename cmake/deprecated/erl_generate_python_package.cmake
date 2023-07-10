macro(erl_generate_python_package)
    set(options)
    set(oneValueArgs PY_PACKAGE_DIR PYBIND_MODULE_NAME)
    set(multiValueArgs PY_MODULES DEPENDS_PY_PACKAGES)
    cmake_parse_arguments(ERL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # get package name
    get_filename_component(ERL_PY_PACKAGE_NAME ${ERL_PY_PACKAGE_DIR} NAME)

    if (NOT DEFINED ERL_PYBIND_MODULE_NAME)
        set(ERL_PYBIND_MODULE_NAME py${PROJECT_NAME})
        message(STATUS "PYBIND_MODULE_NAME not set, using default value ${ERL_PYBIND_MODULE_NAME}")
    endif ()

    set(ERL_PY_PACKAGE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${ERL_PY_PACKAGE_NAME})

    # add a binding library for this package
    file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/binding/pybind11_*.cpp")
    add_library(${PROJECT_NAME}_pybind STATIC ${SRC_FILES})
    add_library(${PROJECT_NAME}::pybind ALIAS ${PROJECT_NAME}_pybind)
    target_include_directories(${PROJECT_NAME}_pybind SYSTEM
            PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_pybind
            PUBLIC ${PROJECT_NAME})
    list(APPEND TARGETS_TO_EXPORT ${PROJECT_NAME}_pybind)

    # pybind runtime lib
    pybind11_add_module(${ERL_PYBIND_MODULE_NAME} ${CMAKE_CURRENT_LIST_DIR}/binding/${ERL_PYBIND_MODULE_NAME}.cpp)
    target_compile_definitions(py${PROJECT_NAME}
            PRIVATE PYBIND_MODULE_NAME=py${PROJECT_NAME})
    target_include_directories(py${PROJECT_NAME} SYSTEM
            PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(py${PROJECT_NAME}
            PRIVATE ${PROJECT_NAME} ${PROJECT_NAME}_pybind)
    foreach (package ${ERL_DEPENDS_PY_PACKAGES})
        target_link_libraries(py${PROJECT_NAME}
                PRIVATE ${package}::pybind)
    endforeach ()
    set_target_properties(py${PROJECT_NAME} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${ERL_PY_PACKAGE_BINARY_DIR})

    # generate a custom target to clean files of this package
    add_custom_target(${PROJECT_NAME}_py_clean_files
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${ERL_PY_PACKAGE_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${ERL_PY_PACKAGE_BINARY_DIR}.egg-info
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/build
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/dist
            COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/requirements.txt
            COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.in
            COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
            COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/setup.py
            COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/entry_points.txt)

    # store module directories of this package
    set(${PROJECT_NAME}_PY_MODULE_DIRS "")
    foreach (module ${ERL_PY_MODULES})
        list(APPEND ${PROJECT_NAME}_PY_MODULE_DIRS ${ERL_PY_PACKAGE_DIR}/${module})
    endforeach ()
    set(${PROJECT_NAME}_PY_MODULE_DIRS ${${PROJECT_NAME}_PY_MODULE_DIRS}
            CACHE INTERNAL "${PROJECT_NAME}: Python module directories" FORCE)

    # Collect files to copy directly, generate custom commands to copy them.
    # We don't do the copying immediately, only when the target is built.
    # This makes the cmake configuration faster and makes sure that the target is rebuilt when the files change.

    # file(COPY ${$ERL_PY_PACKAGE_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    # glob files in top-level directory only, other files will be processed together with ERL_DEPENDS_PY_PACKAGES
    file(GLOB files LIST_DIRECTORIES false RELATIVE ${ERL_PY_PACKAGE_DIR} ${ERL_PY_PACKAGE_DIR}/*)
    foreach (file ${files})  # usually only __init__.py and __init__.pyi
        if (${file} MATCHES ".*\\.in$")
            continue()
        endif ()

        if (EXISTS ${file}.in)
            message(WARNING "File ${file} is ignored because it has a .in file")
            continue()
        endif ()

        set(src_file ${ERL_PY_PACKAGE_DIR}/${file})
        set(dst_file ${ERL_PY_PACKAGE_BINARY_DIR}/${file})
        list(APPEND ERL_PY_DEPENDS_FILES ${dst_file})
        add_custom_command(OUTPUT ${dst_file}
                COMMAND ${CMAKE_COMMAND} -E copy ${src_file} ${dst_file}
                DEPENDS ${src_file}
                COMMENT "Copying ${src_file} to ${dst_file}")
    endforeach ()

    # file(COPY ${${package}_PY_MODULE_DIRS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
    list(APPEND ERL_DEPENDS_PY_PACKAGES ${PROJECT_NAME})
    foreach (package ${ERL_DEPENDS_PY_PACKAGES})
        foreach (module_dir ${${package}_PY_MODULE_DIRS})
            get_filename_component(module_name ${module_dir} NAME)
            file(GLOB_RECURSE files LIST_DIRECTORIES false RELATIVE ${module_dir} ${module_dir}/*)
            foreach (file ${files})
                if (${file} MATCHES ".*\\.in$")
                    continue()
                endif ()

                if (EXISTS ${file}.in)
                    message(WARNING "File ${file} is ignored because it has a .in file")
                    continue()
                endif ()

                set(src_file ${module_dir}/${file})
                set(dst_file ${ERL_PY_PACKAGE_BINARY_DIR}/${module_name}/${file})
                list(APPEND ERL_PY_DEPENDS_FILES ${dst_file})
                add_custom_command(OUTPUT ${dst_file}
                        COMMAND ${CMAKE_COMMAND} -E copy ${src_file} ${dst_file}
                        DEPENDS ${src_file}
                        COMMENT "Copying ${src_file} to ${dst_file}")
            endforeach ()
        endforeach ()
    endforeach ()

    # Collect files to generate by configure_file, generate custom commands to generate them.
    file(GLOB files LIST_DIRECTORIES false RELATIVE ${ERL_PY_PACKAGE_DIR} ${ERL_PY_PACKAGE_DIR}/*.in)
    foreach (file ${files})
        set(src_file ${ERL_PY_PACKAGE_DIR}/${file})
        string(REGEX REPLACE "\\.in$" "" dst_file ${file})
        set(dst_file ${ERL_PY_PACKAGE_BINARY_DIR}/${dst_file})
        list(APPEND ERL_PY_DEPENDS_FILES ${dst_file})
        add_custom_command(OUTPUT ${dst_file}
                COMMAND ${CMAKE_COMMAND} -DPY_PACKAGE_NAME=${ERL_PY_PACKAGE_NAME} -DPYBIND_MODULE_NAME=${ERL_PYBIND_MODULE_NAME} -DERL_CONFIG_FILE_INPUT=${src_file} -DERL_CONFIG_FILE_OUTPUT=${dst_file} -P ${ERL_CMAKE_DIR}/erl_configure_file.cmake
                DEPENDS ${src_file} ${ERL_CMAKE_DIR}/erl_configure_file.cmake
                COMMENT "Generating ${dst_file}")
    endforeach ()

    foreach (package ${ERL_DEPENDS_PY_PACKAGES})
        foreach (module_dir ${${package}_PY_MODULE_DIRS})
            get_filename_component(module_name ${module_dir} NAME)
            file(GLOB_RECURSE files LIST_DIRECTORIES false RELATIVE ${module_dir} ${module_dir}/*.in)
            foreach (file ${files})
                set(src_file ${module_dir}/${file})
                string(REGEX REPLACE "\\.in$" "" dst_file ${file})
                set(dst_file ${ERL_PY_PACKAGE_BINARY_DIR}/${module_name}/${dst_file})
                list(APPEND ERL_PY_DEPENDS_FILES ${dst_file})
                add_custom_command(OUTPUT ${dst_file}
                        COMMAND ${CMAKE_COMMAND} -DPY_PACKAGE_NAME=${ERL_PY_PACKAGE_NAME} -DPYBIND_MODULE_NAME=${ERL_PYBIND_MODULE_NAME} -DERL_CONFIG_FILE_INPUT=${src_file} -DERL_CONFIG_FILE_OUTPUT=${dst_file} -P ${ERL_CMAKE_DIR}/erl_configure_file.cmake
                        DEPENDS ${src_file} ${ERL_CMAKE_DIR}/erl_configure_file.cmake
                        COMMENT "Generating ${dst_file}")
            endforeach ()
        endforeach ()
    endforeach ()

    foreach (file setup.py entry_points.txt)
        set(src_file ${CMAKE_CURRENT_LIST_DIR}/${file}.in)  # check .in file at first
        if (EXISTS ${src_file})
            set(dst_file ${CMAKE_CURRENT_BINARY_DIR}/${file})
            list(APPEND ERL_PY_DEPENDS_FILES ${dst_file})
            add_custom_command(OUTPUT ${dst_file}
                    COMMAND ${CMAKE_COMMAND} -DPY_PACKAGE_NAME=${ERL_PY_PACKAGE_NAME} -DPYBIND_MODULE_NAME=${ERL_PYBIND_MODULE_NAME} -DERL_CONFIG_FILE_INPUT=${src_file} -DERL_CONFIG_FILE_OUTPUT=${dst_file} -P ${ERL_CMAKE_DIR}/erl_configure_file.cmake
                    DEPENDS ${src_file} ${ERL_CMAKE_DIR}/erl_configure_file.cmake
                    COMMENT "Generating ${dst_file}")
        else ()  # no .in file, check original file
            set(src_file ${CMAKE_CURRENT_LIST_DIR}/${file})
            if (EXISTS ${src_file})
                set(dst_file ${CMAKE_CURRENT_BINARY_DIR}/${file})
                list(APPEND ERL_PY_DEPENDS_FILES ${dst_file})
                add_custom_command(OUTPUT ${dst_file}
                        COMMAND ${CMAKE_COMMAND} -E copy ${src_file} ${dst_file}
                        DEPENDS ${src_file}
                        COMMENT "Copying ${src_file} to ${dst_file}")
            endif ()
        endif ()
    endforeach ()

    # copy other package files
    foreach (file requirements.txt MANIFEST.in pyproject.toml)
        if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/${file})
            set(src_file ${CMAKE_CURRENT_LIST_DIR}/${file})
            set(dst_file ${CMAKE_CURRENT_BINARY_DIR}/${file})
            list(APPEND ERL_PY_DEPENDS_FILES ${dst_file})
            add_custom_command(OUTPUT ${dst_file}
                    COMMAND ${CMAKE_COMMAND} -E copy ${src_file} ${dst_file}
                    DEPENDS ${src_file}
                    COMMENT "Copying ${src_file} to ${dst_file}")
        endif ()
    endforeach ()

    # check pip version
    execute_process(COMMAND ${Python3_EXECUTABLE} -m pip --version
            OUTPUT_VARIABLE PIP_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCH "pip ([0-9]+)\\.([0-9]+)\\.([0-9]+)" PIP_VERSION ${PIP_VERSION})
    set(PIP_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(PIP_VERSION_MINOR ${CMAKE_MATCH_2})
    set(PIP_VERSION_PATCH ${CMAKE_MATCH_3})
    if (PIP_VERSION_MAJOR LESS 23)
        message(FATAL_ERROR "pip version must be greater than or equal to 23.0.0, please run 'pip install --upgrade pip' to upgrade pip")
    endif ()

    add_custom_target(${PROJECT_NAME}_py_generate
            DEPENDS py${PROJECT_NAME} ${ERL_PY_DEPENDS_FILES}
            COMMENT "Generate python package of ${PROJECT_NAME}")

    add_custom_target(${PROJECT_NAME}_py_wheel
            COMMAND ${Python3_EXECUTABLE} setup.py bdist_wheel
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${PROJECT_NAME}_py_generate
            COMMENT "Build python wheel package of ${PROJECT_NAME}")

    # https://packaging.python.org/en/latest/discussions/pip-vs-easy-install/
    add_custom_target(${PROJECT_NAME}_py_develop
            COMMAND ${Python3_EXECUTABLE} -m pip install -e . --user
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${PROJECT_NAME}_py_generate
            COMMENT "Install python package of ${PROJECT_NAME} in develop mode")

    add_custom_target(${PROJECT_NAME}_py_install
            COMMAND ${Python3_EXECUTABLE} -m pip install . --user --break-system-packages
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${PROJECT_NAME}_py_generate
            COMMENT "Install python package of ${PROJECT_NAME}")

    set(${PROJECT_NAME}_PY_PACKAGE_DIR ${ERL_PY_PACKAGE_BINARY_DIR}
            CACHE PATH "Python package directory of ${PROJECT_NAME} during build" FORCE)
endmacro()
