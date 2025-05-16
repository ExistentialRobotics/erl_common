macro(erl_config_yaml)
    erl_find_package(
            PACKAGE yaml-cpp
            REQUIRED
            COMMANDS UBUNTU_LINUX "try `sudo apt install libyaml-cpp-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S yaml-cpp`")
    message(STATUS "yaml-cpp VERSION: ${yaml-cpp_VERSION}")
    # get version, major, minor, patch
    string(REGEX MATCHALL "[0-9]+" yaml-cpp_VERSION_LIST ${yaml-cpp_VERSION})
    list(GET yaml-cpp_VERSION_LIST 0 yaml-cpp_VERSION_MAJOR)
    list(GET yaml-cpp_VERSION_LIST 1 yaml-cpp_VERSION_MINOR)
    list(GET yaml-cpp_VERSION_LIST 2 yaml-cpp_VERSION_PATCH)
    message(STATUS "yaml-cpp VERSION_MAJOR: ${yaml-cpp_VERSION_MAJOR}")
    message(STATUS "yaml-cpp VERSION_MINOR: ${yaml-cpp_VERSION_MINOR}")
    message(STATUS "yaml-cpp VERSION_PATCH: ${yaml-cpp_VERSION_PATCH}")
    add_definitions("-DYAML_CPP_VERSION_MAJOR=${yaml-cpp_VERSION_MAJOR}")
    add_definitions("-DYAML_CPP_VERSION_MINOR=${yaml-cpp_VERSION_MINOR}")
    add_definitions("-DYAML_CPP_VERSION_PATCH=${yaml-cpp_VERSION_PATCH}")
    if (ROS_ACTIVATED)
        if (ROS_VERSION STREQUAL "1")
            erl_collect_library_dependencies(yaml-cpp yaml-cpp_INCLUDE_DIRS yaml-cpp_LIBRARIES)
            message(STATUS "yaml-cpp_INCLUDE_DIRS: ${yaml-cpp_INCLUDE_DIRS}")
            message(STATUS "yaml-cpp_LIBRARIES: ${yaml-cpp_LIBRARIES}")
        endif ()
    endif ()
endmacro()
