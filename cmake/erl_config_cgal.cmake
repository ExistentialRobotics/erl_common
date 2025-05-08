macro(erl_config_cgal)
    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
    erl_find_package(
            PACKAGE CGAL
            COMPONENTS Core
            COMMANDS UBUNTU_LINUX "try `sudo apt-get install libcgal-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S cgal`"
    )
endmacro()
