# This file provides functions and macros for setting up ROS1 catkin packages.

macro(erl_catkin_setup)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs COMPONENTS)
    cmake_parse_arguments(ERL_CATKIN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    unset(options)
    unset(oneValueArgs)
    unset(multiValueArgs)

    if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/package.xml)
        message(FATAL_ERROR "No package.xml found in ${CMAKE_CURRENT_LIST_DIR}")
    endif ()

    erl_find_package(
            PACKAGE_NAME catkin
            UBUNTU_CMD "run `sudo apt-get install cmake python3-catkin-pkg python3-empy python3-nose python3-setuptools`"
            ARCH_CMD "run `paru -S python-catkin_tools ros-noetic-catkin`"
            NO_CMD "visit http://wiki.ros.org/catkin"
            FIND_PACKAGE_ARGS COMPONENTS ${ERL_CATKIN_COMPONENTS}
            NO_RECORD)
    if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/setup.py)
        catkin_python_setup()
        set(${PROJECT_NAME}_CATKIN_PYTHON_SETUP TRUE CACHE BOOL "TRUE if catkin_python_setup() was called" FORCE)
    endif ()

    unset(ERL_CATKIN_COMPONENTS)
endmacro()
