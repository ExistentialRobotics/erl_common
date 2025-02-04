# pangolin dependencies: OpenGL, EGL, epoxy
erl_find_package(
        PACKAGE OpenGL
        REQUIRED COMPONENTS OpenGL EGL
        COMMANDS UBUNTU_LINUX "try `sudo apt install libglvnd-dev`"
        COMMANDS ARCH_LINUX "try `sudo pacman -S libglvnd`")
erl_find_package(
        PACKAGE epoxy
        REQUIRED
        COMMANDS UBUNTU_LINUX "try `sudo apt install libepoxy-dev`"
        COMMANDS ARCH_LINUX "try `sudo pacman -S libepoxy`")
erl_find_package(
        PACKAGE Pangolin
        REQUIRED
        COMMANDS UBUNTU_LINUX "try install from https://github.com/stevenlovegrove/Pangolin.git"
        COMMANDS ARCH_LINUX "try `paru -S pangolin-git`")

# remove pango_python as it is not needed and causes error when loading other python modules
list(REMOVE_ITEM Pangolin_LIBRARIES "pango_python")
list(REMOVE_ITEM Pangolin_LIBRARY "pango_python")
