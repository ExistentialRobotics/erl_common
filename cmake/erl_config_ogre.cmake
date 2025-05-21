macro(erl_config_ogre)
    erl_find_package(
            PACKAGE OGRE
            PKGCONFIG REQUIRED OGRE
            COMMANDS ARCH_LINUX "try `paru -S ogre-1.9`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install libogre-1.9-dev`"
    )
endmacro()
