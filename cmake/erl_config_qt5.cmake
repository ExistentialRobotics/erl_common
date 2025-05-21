macro(erl_config_qt5)
    set(components ${ARGN})
    if (NOT components)
        message(FATAL_ERROR "No components specified for erl_config_qt5")
    endif ()
    erl_find_package(
            PACKAGE Qt5
            COMPONENTS ${components}
            REQUIRED)

    if (ROS1_ACTIVATED)
        # set(CMAKE_AUTOMOC ON)
        # add_definitions(-DQT_NO_KEYWORDS)
        # use set_target_properties(my_target PROPERTIES AUTOMOC ON) instead
        foreach (component IN LISTS components)
            erl_collect_library_dependencies(Qt5::${component} includes libs)
            list(APPEND Qt5_INCLUDE_DIRS ${includes})
            list(APPEND Qt5_LIBRARIES ${libs})
        endforeach ()
        list(REMOVE_DUPLICATES Qt5_INCLUDE_DIRS)
        list(REMOVE_DUPLICATES Qt5_LIBRARIES)
        message(STATUS "Qt5_INCLUDE_DIRS: ${Qt5_INCLUDE_DIRS}")
        message(STATUS "Qt5_LIBRARIES: ${Qt5_LIBRARIES}")
        unset(includes)
        unset(libs)
    endif ()
    unset(components)
endmacro()
