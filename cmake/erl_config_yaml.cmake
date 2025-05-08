macro(erl_config_yaml)
    erl_find_package(
            PACKAGE yaml-cpp
            REQUIRED
            COMMANDS UBUNTU_LINUX "try `sudo apt install libyaml-cpp-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S yaml-cpp`")
    if (ROS_ACTIVATED)
        if (ROS_VERSION STREQUAL "1")
            if (YAML_CPP_INCLUDE_DIR OR YAML_CPP_LIBRARIES)
                set(yaml-cpp_INCLUDE_DIRS ${YAML_CPP_INCLUDE_DIR}
                        CACHE PATH "yaml-cpp include directories" FORCE)
                set(yaml-cpp_LIBRARIES ${YAML_CPP_LIBRARIES}
                        CACHE STRING "yaml-cpp libraries" FORCE)
            else () # noconfig version of yaml-cpp: yaml-cpp is installed without setting CMAKE_BUILD_TYPE
                get_target_property(yaml-cpp_LIBRARIES yaml-cpp IMPORTED_LOCATION_NOCONFIG)
                get_filename_component(yaml-cpp_INCLUDE_DIRS ${yaml-cpp_LIBRARIES} DIRECTORY)
                get_filename_component(yaml-cpp_INCLUDE_DIRS ${yaml-cpp_INCLUDE_DIRS}/../include ABSOLUTE)
                set(yaml-cpp_INCLUDE_DIRS ${yaml-cpp_INCLUDE_DIRS}
                        CACHE PATH "yaml-cpp include directories" FORCE)
                set(yaml-cpp_LIBRARIES ${yaml-cpp_LIBRARIES}
                        CACHE STRING "yaml-cpp libraries" FORCE)
            endif ()
            message(STATUS "yaml-cpp_INCLUDE_DIRS: ${yaml-cpp_INCLUDE_DIRS}")
            message(STATUS "yaml-cpp_LIBRARIES: ${yaml-cpp_LIBRARIES}")
        endif ()
    endif ()
endmacro()
