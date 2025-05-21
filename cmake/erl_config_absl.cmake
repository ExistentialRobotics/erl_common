macro(erl_config_absl)
    erl_find_package(
            PACKAGE absl
            REQUIRED
            COMMANDS APPLE "try `brew install abseil`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install libabseil-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S abseil-cpp`")
    if (ROS1_ACTIVATED)
        erl_collect_library_dependencies(absl::flat_hash_map absl_INCLUDE_DIRS absl_LIBRARIES)
        erl_collect_library_dependencies(absl::flat_hash_set includes libs)
        list(APPEND absl_INCLUDE_DIRS ${includes})
        list(APPEND absl_LIBRARIES ${libs})
        message(STATUS "absl_INCLUDE_DIRS: ${absl_INCLUDE_DIRS}")
        message(STATUS "absl_LIBRARIES: ${absl_LIBRARIES}")
        unset(includes)
        unset(libs)
    endif ()
endmacro()
