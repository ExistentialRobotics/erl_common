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
    endif ()
endmacro()
