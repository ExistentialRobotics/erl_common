macro(erl_config_boost)
    erl_find_package(
            PACKAGE Boost
            REQUIRED CONFIG COMPONENTS program_options graph
            COMMANDS APPLE "try `brew install boost`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install libboost-all-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S boost`")
endmacro()
