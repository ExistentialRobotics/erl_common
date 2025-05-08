macro(erl_config_absl)
    erl_find_package(
            PACKAGE absl
            REQUIRED
            COMMANDS APPLE "try `brew install abseil`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install libabseil-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S abseil-cpp`")
    get_target_property(absl_flat_hash_map_location absl::flat_hash_map LOCATION)
    get_target_property(absl_flat_hash_set_location absl::flat_hash_set LOCATION)
    list(APPEND absl_LIBRARIES ${absl_flat_hash_map_location} ${absl_flat_hash_set_location})
endmacro()
