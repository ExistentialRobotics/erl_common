function(erl_check_file)
    set(options ASSERT_FOUND)
    set(oneValueArgs OUTPUT WIN_PATH MAC_PATH LINUX_PATH PACKAGE_NAME WIN_CMD MAC_CMD UBUNTU_CMD NO_CMD)
    set(multiValueArgs)
    cmake_parse_arguments(ERL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED ASSERT_FOUND)
        set(ERL_ASSERT_FOUND OFF)
    endif()

    if (NOT DEFINED ERL_OUTPUT)
        set(ERL_OUTPUT "FILE_FOUND")
    endif()

    if (WIN32)
        if (NOT ERL_WIN_PATH)
            message(FATAL_ERROR "erl_check_file: WIN_PATH not specified")
        endif ()
        file(GLOB ERL_WIN_PATH ${ERL_WIN_PATH})  # expand wildcards
        if (EXISTS ${ERL_WIN_PATH})
            set(${ERL_OUTPUT} ON)
        else ()
            set(${ERL_OUTPUT} OFF)
        endif ()
    elseif (APPLE)
        if (NOT ERL_MAC_PATH)
            message(FATAL_ERROR "erl_check_file: MAC_PATH not specified")
        endif ()
        file(GLOB ERL_MAC_PATH ${ERL_MAC_PATH})  # expand wildcards
        if (EXISTS ${ERL_MAC_PATH})
            set(${ERL_OUTPUT} ON)
        else ()
            set(${ERL_OUTPUT} OFF)
        endif ()
    else ()
        if (NOT ERL_LINUX_PATH)
            message(FATAL_ERROR "erl_check_file: LINUX_PATH not specified")
        endif ()
        file(GLOB ERL_LINUX_PATH ${ERL_LINUX_PATH})  # expand wildcards
        if (EXISTS ${ERL_LINUX_PATH})
            set(${ERL_OUTPUT} ON)
        else ()
            set(${ERL_OUTPUT} OFF)
        endif ()
    endif ()

    if (ERL_ASSERT_FOUND)
        set(MSG_TYPE FATAL_ERROR)
    else ()
        set(MSG_TYPE WARNING)
    endif ()

    if (NOT ${ERL_OUTPUT})
        erl_suggest_cmd(
                PACKAGE_NAME ${ERL_PACKAGE_NAME}
                WIN_CMD ${ERL_WIN_CMD}
                MAC_CMD ${ERL_MAC_CMD}
                UBUNTU_CMD ${ERL_UBUNTU_CMD}
                NO_CMD ${ERL_NO_CMD}
                MSG_TYPE ${MSG_TYPE}
        )
    endif ()

    set(${ERL_OUTPUT} ${${ERL_OUTPUT}} PARENT_SCOPE)

endfunction()
