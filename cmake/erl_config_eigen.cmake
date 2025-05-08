macro(erl_config_eigen)
    erl_find_package(
            PACKAGE Eigen3
            REQUIRED CONFIG  # in case some other packages define FindEigen3.cmake
            COMMANDS APPLE "try `brew install eigen`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install libeigen3-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S eigen`")
    set(EIGEN3_VERSION_STRING ${Eigen3_VERSION} CACHE STRING "Eigen3 version" FORCE)
    if (EIGEN3_VERSION_STRING VERSION_LESS "3.4.0")
        message(WARNING "Eigen3 version is older than 3.4.0")
    endif ()
    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.25)
        set_target_properties(Eigen3::Eigen PROPERTIES SYSTEM ON)
    endif ()

    if (ROS_ACTIVATED)
        if (ROS_VERSION STREQUAL "1")
            get_target_property(Eigen3_INCLUDE_DIRS Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
            message(STATUS "Eigen3_INCLUDE_DIRS: ${Eigen3_INCLUDE_DIRS}")
        endif ()
    endif ()
endmacro()
