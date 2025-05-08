macro(erl_config_opencv)
    erl_find_package(
            PACKAGE OpenCV
            REQUIRED COMPONENTS core imgproc highgui
            COMMANDS APPLE "run scripts/install_opencv.bash"
            COMMANDS UBUNTU_LINUX "try `sudo apt install libopencv-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S opencv`")
endmacro()
