macro(erl_config_fmt)
    erl_find_package(
            PACKAGE fmt
            REQUIRED
            COMMANDS ARCH_LINUX "try `sudo pacman -S fmt`"
    )
    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.25)
        set_target_properties(fmt::fmt PROPERTIES SYSTEM ON)
        set_target_properties(fmt::fmt-header-only PROPERTIES SYSTEM ON)
    endif ()
    get_target_property(fmt_INCLUDE_DIRS fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(fmt_LIBRARIES fmt::fmt IMPORTED_LOCATION_RELEASE)
endmacro()
