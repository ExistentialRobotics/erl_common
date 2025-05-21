macro(erl_config_qhull)
    erl_find_package(
            PACKAGE Qhull
            REQUIRED
            COMMANDS UBUNTU_LINUX "try `sudo apt-get install libqhull-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S qhull`")
    if (ROS1_ACTIVATED)
        get_target_property(Qhull_INCLUDE_DIRS Qhull::qhullcpp INTERFACE_INCLUDE_DIRECTORIES)
        get_target_property(qhull_r_location Qhull::qhull_r LOCATION)
        get_target_property(qhullcpp_location Qhull::qhullcpp LOCATION)
        list(APPEND Qhull_LIBRARIES ${qhull_r_location} ${qhullcpp_location})
    endif ()
endmacro()
