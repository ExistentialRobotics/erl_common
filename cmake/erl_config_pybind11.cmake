macro(erl_config_pybind11)
    erl_find_package(
            PACKAGE pybind11
            REQUIRED
            COMMANDS APPLE "try `brew install pybind11`"
            COMMANDS UBUNTU_LINUX "try `sudo apt install pybind11-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S pybind11`")

    foreach (item IN ITEMS python_link_helper python_headers headers module embed windows_extras thin_lto lto opt_size)
        if (TARGET pybind11::${item})
            get_target_property(item_type pybind11::${item} TYPE)
            if (NOT item_type STREQUAL "INTERFACE_LIBRARY")  # check if it is an interface library
                set_target_properties(pybind11::${item} PROPERTIES SYSTEM ON)
            endif ()
        endif ()
    endforeach ()
endmacro()
