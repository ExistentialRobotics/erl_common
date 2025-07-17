macro(erl_config_qt6)
    set(components ${ARGN})
    if (NOT components)
        message(FATAL_ERROR "No components specified for erl_config_qt6")
    endif ()
    erl_find_package(
            PACKAGE Qt6
            COMPONENTS ${components}
            REQUIRED)
    unset(components)
endmacro()
