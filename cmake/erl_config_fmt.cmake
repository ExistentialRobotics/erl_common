macro(erl_config_fmt)
    set(fmt_VERBOSE_ONCE FALSE CACHE BOOL "Print fmt configuration once" FORCE)
    erl_find_package(
            PACKAGE fmt
            REQUIRED
            COMMANDS ARCH_LINUX "try `sudo pacman -S fmt`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install libfmt-dev`"
    )
    set_target_properties(fmt::fmt PROPERTIES SYSTEM ON)
    if (TARGET fmt::fmt-header-only)
        set_target_properties(fmt::fmt-header-only PROPERTIES SYSTEM ON)
    endif ()
    if (ROS1_ACTIVATED)
        get_target_property(_cfg fmt::fmt IMPORTED_CONFIGURATIONS)
        get_target_property(fmt_INCLUDE_DIRS fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
        get_target_property(fmt_LIBRARIES fmt::fmt IMPORTED_LOCATION_${_cfg})
        unset(_cfg)
        message(STATUS "fmt_INCLUDE_DIRS: ${fmt_INCLUDE_DIRS}")
        message(STATUS "fmt_LIBRARIES: ${fmt_LIBRARIES}")

    endif ()
endmacro()
