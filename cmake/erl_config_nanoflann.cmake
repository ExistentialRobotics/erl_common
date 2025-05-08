macro(erl_config_nanoflann)
    erl_find_package(
            PACKAGE nanoflann
            REQUIRED
            COMMANDS UBUNTU_LINUX "try `sudo apt install libnanoflann-dev`"
            COMMANDS ARCH_LINUX "try `paru -S nanoflann`")
    if (ROS_ACTIVATED)
        if (ROS_VERSION STREQUAL "1")
            get_target_property(nanoflann_INCLUDE_DIRS nanoflann::nanoflann INTERFACE_INCLUDE_DIRECTORIES)
            message(STATUS "nanoflann_INCLUDE_DIRS: ${nanoflann_INCLUDE_DIRS}")
        endif ()
    endif ()
endmacro()
