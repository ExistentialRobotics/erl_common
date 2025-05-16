# plplot dependencies: cairomm-1.0
macro(erl_config_plplot)
    option(ERL_USE_PLPLOT "Use PLPlot for visualization" ON)
    if (ERL_USE_PLPLOT)
        erl_find_package(
                PACKAGE cairomm-1.0
                PKGCONFIG REQUIRED cairomm-1.0
        )
        erl_find_package(
                PACKAGE plplot
                REQUIRED
                COMMANDS ARCH_LINUX "try `paru -S plplot`"
                COMMANDS UBUNTU_LINUX "try `sudo apt install libplplot-dev`"
        )
        if (plplot_FOUND)
            message(STATUS "PLPlot is FOUND. Will build with PLPlot")
        else ()
            message(WARNING "PLPlot is NOT FOUND. Will build without PLPlot")
        endif ()
        if (ROS_ACTIVATED)
            if (ROS_VERSION STREQUAL "1")
                erl_collect_library_dependencies(PLPLOT::plplot plplot_INCLUDE_DIRS plplot_LIBRARIES)
                erl_collect_library_dependencies(PLPLOT::plplotcxx includes libs)
                list(APPEND plplot_INCLUDE_DIRS ${includes})
                list(APPEND plplot_LIBRARIES ${libs})
                if (NOT plplot_INCLUDE_DIRS)
                    set(plplot_INCLUDE_DIRS /usr/include)  # default to /usr/include
                endif ()
                message(STATUS "plplot_INCLUDE_DIRS: ${plplot_INCLUDE_DIRS}")
                message(STATUS "plplot_LIBRARIES: ${plplot_LIBRARIES}")
                unset(includes)
                unset(libs)
                # get_target_property(_cfg PLPLOT::plplot IMPORTED_CONFIGURATIONS)
                # get_target_property(plplot_LIBRARIES PLPLOT::plplot IMPORTED_LOCATION_${_cfg})
                # get_target_property(plplotcxx_LIBRARIES PLPLOT::plplotcxx IMPORTED_LOCATION_${_cfg})
                # list(APPEND plplot_LIBRARIES ${plplotcxx_LIBRARIES})
                # message(STATUS "plplot_LIBRARIES: ${plplot_LIBRARIES}")
                # unset(_cfg)
            endif ()
        endif ()
    endif ()
endmacro()
