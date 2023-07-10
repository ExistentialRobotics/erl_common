macro(erl_find_package)
    set(options NO_RECORD)
    set(oneValueArgs PACKAGE_NAME WIN_CMD MAC_CMD UBUNTU_CMD ARCH_CMD NO_CMD)
    set(multiValueArgs FIND_PACKAGE_ARGS)

    cmake_parse_arguments(ERL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    erl_before_find_package(
            PACKAGE_NAME ${ERL_PACKAGE_NAME}
            WIN_CMD ${ERL_WIN_CMD}
            MAC_CMD ${ERL_MAC_CMD}
            UBUNTU_CMD ${ERL_UBUNTU_CMD}
            ARCH_CMD ${ERL_ARCH_CMD}
            NO_CMD ${ERL_NO_CMD})
    find_package(${ERL_PACKAGE_NAME} ${ERL_FIND_PACKAGE_ARGS})
    foreach (item IN ITEMS FOUND INCLUDE_DIR INCLUDE_DIRS LIBRARY LIBRARIES LIBS DEFINITIONS)
        if (DEFINED ${ERL_PACKAGE_NAME}_${item})
            message(STATUS "${ERL_PACKAGE_NAME}_${item}: ${${ERL_PACKAGE_NAME}_${item}}")
        endif ()
    endforeach ()
    if (ERL_NO_RECORD)
        message(STATUS "${ERL_PACKAGE_NAME} is not added to ERL_${PROJECT_NAME}_DEPENDS")
    else ()
        list(APPEND ERL_${PROJECT_NAME}_DEPENDS ${ERL_PACKAGE_NAME})
    endif ()

    unset(ERL_NO_RECORD)
    unset(ERL_PACKAGE_NAME)
    unset(ERL_WIN_CMD)
    unset(ERL_MAC_CMD)
    unset(ERL_UBUNTU_CMD)
    unset(ERL_ARCH_CMD)
    unset(ERL_NO_CMD)
    unset(ERL_FIND_PACKAGE_ARGS)
endmacro()
