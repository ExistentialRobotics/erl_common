macro(erl_config_json)
    erl_find_package(
            PACKAGE nlohmann_json
            REQUIRED
            COMMANDS APPLE "try `brew install nlohmann-json`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install nlohmann-json3-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S nlohmann-json`")
    if (ROS1_ACTIVATED)
        get_target_property(nlohmann_json_INCLUDE_DIRS nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
        message(STATUS "nlohmann_json_INCLUDE_DIRS: ${nlohmann_json_INCLUDE_DIRS}")
    endif ()
endmacro()
