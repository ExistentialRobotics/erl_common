macro(erl_config_open3d)
    erl_find_package(
            PACKAGE Open3D
            REQUIRED
            COMMANDS GENERAL "visit http://www.open3d.org/"
            COMMANDS ARCH_LINUX "try `paru -S open3d`"
    )
    if (ROS_ACTIVATED)
        if (ROS_VERSION STREQUAL "1")
            # Open3D_INCLUDE_DIRS is set by Open3DConfig.cmake
            get_target_property(Open3D_LIBRARIES Open3D::Open3D LOCATION)
            message(STATUS "Open3D_LIBRARIES: ${Open3D_LIBRARIES}")
        endif ()
    endif ()
endmacro()
