macro(erl_config_pcl)
    erl_find_package(
            PACKAGE PCL
            REQUIRED
            COMMANDS UBUNTU_LINUX "try `sudo apt install libpcl-dev`"
            COMMANDS ARCH_LINUX "try `paru -S pcl`")
endmacro()
