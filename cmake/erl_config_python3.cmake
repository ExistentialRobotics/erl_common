macro(erl_config_python3)
    erl_find_package(
            PACKAGE Python3
            REQUIRED COMPONENTS Interpreter Development
            COMMANDS APPLE "try `brew install python3`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install python3-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S python`")
    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.25)
        set_target_properties(Python3::Python PROPERTIES SYSTEM ON)
    endif ()
    message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
endmacro()
