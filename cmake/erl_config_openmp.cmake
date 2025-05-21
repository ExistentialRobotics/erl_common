macro(erl_config_openmp)
    erl_find_package(
            PACKAGE OpenMP
            REQUIRED
            COMMANDS APPLE "try `brew install libomp`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install libomp-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S openmp`")
    if (ROS1_ACTIVATED)
        set(OpenMP_INCLUDE_DIRS /usr/include)
        get_target_property(OpenMP_LIBRARIES OpenMP::OpenMP_CXX INTERFACE_LINK_LIBRARIES)
        message(STATUS "OpenMP_INCLUDE_DIRS: ${OpenMP_INCLUDE_DIRS}")
        message(STATUS "OpenMP_LIBRARIES: ${OpenMP_LIBRARIES}")
    endif ()
endmacro()
