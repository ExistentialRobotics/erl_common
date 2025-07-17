macro(erl_config_qt)
    set(components ${ARGN})
    if (NOT components)
        message(FATAL_ERROR "No components specified for erl_config_qt")
    endif ()
    erl_find_package(
            PACKAGE QT
            NAMES Qt6 Qt5
            COMPONENTS ${components}
            REQUIRED)

    # get the major version
    string(REGEX MATCH "^[0-9]+" QT_VERSION_MAJOR "${QT_VERSION}")
    if (NOT QT_LIBRARIES)
        foreach (component IN LISTS components)
            list(APPEND QT_LIBRARIES Qt${QT_VERSION_MAJOR}::${component})
        endforeach ()
    endif ()

    message(STATUS "Qt found: ${QT_VERSION} at ${QT_DIR}")
    message(STATUS "QT_INCLUDE_DIRS: ${QT_INCLUDE_DIRS}")
    message(STATUS "QT_LIBRARIES: ${QT_LIBRARIES}")
    unset(components)

endmacro()
