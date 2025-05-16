macro(erl_config_fmt)
    erl_find_package(
            PACKAGE fmt
            REQUIRED
            COMMANDS ARCH_LINUX "try `sudo pacman -S fmt`"
    )
    set_target_properties(fmt::fmt PROPERTIES SYSTEM ON)
    if (TARGET fmt::fmt-header-only)
        set_target_properties(fmt::fmt-header-only PROPERTIES SYSTEM ON)
    endif ()
    if (ROS_ACTIVATED)
        if (ROS_VERSION STREQUAL "1")
            get_target_property(_cfg fmt::fmt IMPORTED_CONFIGURATIONS)
            get_target_property(fmt_INCLUDE_DIRS fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
            get_target_property(fmt_LIBRARIES fmt::fmt IMPORTED_LOCATION_${_cfg})
            unset(_cfg)
            message(STATUS "fmt_INCLUDE_DIRS: ${fmt_INCLUDE_DIRS}")
            message(STATUS "fmt_LIBRARIES: ${fmt_LIBRARIES}")
        endif ()
    endif ()
endmacro()
