# ##################################################################################################
# Add this cmake module path to the cmake module path
# ##################################################################################################
if (NOT DEFINED ERL_CMAKE_DIR)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} CACHE INTERNAL "CMake module path")
    set(ERL_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "ERL CMake directory")
endif ()
message(STATUS "ERL_CMAKE_DIR: ${ERL_CMAKE_DIR}")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
file(GLOB MODULE_CONFIG_FILES ${ERL_CMAKE_DIR}/erl_config_*.cmake)
foreach (file ${MODULE_CONFIG_FILES})
    include(${file})
endforeach ()

# ##################################################################################################
# erl_append_property
# ##################################################################################################
function(erl_append_property target_name property_name output_var)
    get_target_property(_property ${target_name} ${property_name})
    if (_property)
        list(APPEND ${output_var} ${_property})
        set(${output_var} ${${output_var}} PARENT_SCOPE)
    endif ()
endfunction()

# ##################################################################################################
# erl_collect_library_dependencies
# ##################################################################################################
function(erl_collect_library_dependencies target_name includes_out libs_out)

    set(queue ${target_name})
    set(visited "")
    set(_includes)
    set(_libs)

    while (queue)
        list(POP_FRONT queue current)

        list(FIND visited ${current} already_seen)
        if (already_seen EQUAL -1)
            list(APPEND visited ${current})  # mark as visited

            set(_local_libs "")
            get_target_property(_cfg ${current} IMPORTED_CONFIGURATIONS)
            if (_cfg)
                set(_cfg "_${_cfg}")
                # check libraries
                erl_append_property(${current} IMPORTED_LINK_DEPENDENT_LIBRARIES${_cfg} _local_libs)
                erl_append_property(${current} IMPORTED_LINK_INTERFACE_LIBRARIES${_cfg} _local_libs)
                erl_append_property(${current} IMPORTED_LOCATION${_cfg} _local_libs)
                erl_append_property(${current} INTERFACE_LINK_LIBRARIES${_cfg} _local_libs)
                erl_append_property(${current} LOCATION${_cfg} _local_libs)
            endif ()
            # check includes
            erl_append_property(${current} INCLUDE_DIRECTORIES _includes)
            erl_append_property(${current} INTERFACE_INCLUDE_DIRECTORIES _includes)
            erl_append_property(${current} INTERFACE_SYSTEM_INCLUDE_DIRECTORIES _includes)
            # check libraries
            erl_append_property(${current} IMPORTED_LINK_DEPENDENT_LIBRARIES _local_libs)
            erl_append_property(${current} IMPORTED_LINK_INTERFACE_LIBRARIES _local_libs)
            erl_append_property(${current} IMPORTED_LOCATION _local_libs)
            erl_append_property(${current} INTERFACE_LINK_LIBRARIES _local_libs)
            erl_append_property(${current} LINK_LIBRARIES _local_libs)
            erl_append_property(${current} LOCATION _local_libs)

            foreach (_lib IN LISTS _local_libs)
                if (TARGET ${_lib})
                    list(APPEND queue ${_lib})
                else ()
                    list(APPEND _libs ${_lib})
                endif ()
            endforeach ()
            list(REMOVE_DUPLICATES _includes)
            list(REMOVE_DUPLICATES _libs)
        endif ()
    endwhile ()

    # message(STATUS "All deps: ${visited}")
    # message(STATUS "Includes: ${_includes}")
    # message(STATUS "Libs: ${_libs}")

    set(${includes_out} ${_includes} PARENT_SCOPE)
    set(${libs_out} ${_libs} PARENT_SCOPE)
endfunction()

# ##################################################################################################
# erl_set_gtest_args
# ##################################################################################################
macro(erl_set_gtest_args)
    set(_gtest_name ${ARGV0})
    set(_gtest_args ${ARGN})
    list(POP_FRONT _gtest_args)  # get the remaining arguments
    set(${_gtest_name}_GTEST_ARGS ${_gtest_args}
            CACHE STRING "GTest arguments for ${_gtest_args}" FORCE)
    message(STATUS "GTest arguments for ${_gtest_name}: ${${_gtest_name}_GTEST_ARGS}")
endmacro()

# ##################################################################################################
# erl_set_gtest_extra_libraries
# ##################################################################################################
macro(erl_set_gtest_extra_libraries)
    set(_gtest_name ${ARGV0})
    set(_gtest_libraries ${ARGN})
    list(POP_FRONT _gtest_libraries)  # get the remaining arguments
    set(${_gtest_name}_EXTRA_LIBRARIES ${_gtest_libraries}
            CACHE STRING "GTest extra libraries for ${_gtest_libraries}" FORCE)
    message(STATUS "Extra libraries for GTest ${_gtest_name}: ${${_gtest_name}_EXTRA_LIBRARIES}")
endmacro()

# ##################################################################################################
# erl_set_gtest_working_directory
# ##################################################################################################
macro(erl_set_gtest_working_directory _gtest_name _gtest_working_directory)
    set(${_gtest_name}_GTEST_WORKING_DIRECTORY ${_gtest_working_directory}
            CACHE STRING "GTest working directory for ${_gtest_name}" FORCE)
endmacro()

# ##################################################################################################
# erl_add_test
# ##################################################################################################
macro(erl_add_tests)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs LIBRARIES EXCLUDE_FROM_ALL IGNORE_FILES)
    cmake_parse_arguments(${PROJECT_NAME}_TEST
            "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ERL_BUILD_TEST_${PROJECT_NAME})
        # add gtest
        file(GLOB GTEST_SOURCES ${${PROJECT_NAME}_TEST_DIR}/gtest/*.cpp)  # lexicographically sorted
        # remove files in the ignore list
        foreach (file IN LISTS ${PROJECT_NAME}_TEST_IGNORE_FILES)
            list(REMOVE_ITEM GTEST_SOURCES ${${PROJECT_NAME}_TEST_DIR}/gtest/${file})
        endforeach ()
        list(REVERSE GTEST_SOURCES)

        if (ROS_ACTIVATED AND ROS_VERSION STREQUAL "1" AND CATKIN_ENABLE_TESTING)
            foreach (file IN LISTS GTEST_SOURCES)
                get_filename_component(name ${file} NAME_WE)
                if (${name} IN_LIST ${PROJECT_NAME}_TEST_EXCLUDE_FROM_ALL)
                    message(STATUS "Excluding gtest ${name}")
                    add_executable(${name} ${file})
                    target_link_libraries(${name}
                            ${${PROJECT_NAME}_TEST_LIBRARIES} GTest::Main
                            ${${name}_${name}_LIBRARIES} ${${name}_EXTRA_LIBRARIES})
                    set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
                else ()
                    catkin_add_gtest(${name} ${file})
                    target_include_directories(${name} PRIVATE ${catkin_INCLUDE_DIRS})
                    target_link_libraries(${name}
                            ${catkin_LIBRARIES} ${${PROJECT_NAME}_TEST_LIBRARIES} ${${name}_EXTRA_LIBRARIES})
                    message(STATUS "Adding gtest ${name}")
                endif ()
            endforeach ()
        else ()
            foreach (file IN LISTS GTEST_SOURCES)
                get_filename_component(name ${file} NAME_WE)
                add_executable(${name} ${file})

                target_link_libraries(${name}
                        ${${PROJECT_NAME}_TEST_LIBRARIES} GTest::Main
                        ${${name}_${name}_LIBRARIES} ${${name}_EXTRA_LIBRARIES})

                if (${name} IN_LIST ${PROJECT_NAME}_TEST_EXCLUDE_FROM_ALL)
                    message(STATUS "Excluding gtest ${name}")
                    set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
                else ()
                    message(STATUS "Adding gtest ${name}")
                    list(APPEND _args ${name})
                    if (DEFINED ${name}_GTEST_ARGS)
                        list(APPEND _args "EXTRA_ARGS" ${${name}_GTEST_ARGS})
                    endif ()
                    if (DEFINED ${name}_GTEST_WORKING_DIRECTORY)
                        list(APPEND _args "WORKING_DIRECTORY" ${${name}_GTEST_WORKING_DIRECTORY})
                    endif ()
                    list(APPEND _args "DISCOVERY_TIMEOUT" 60)
                    gtest_discover_tests(${_args})
                    unset(_args)
                endif ()
            endforeach ()
        endif ()

        # TODO: add python tests
    endif ()
endmacro()

# ##################################################################################################
# Function erl_os_release_info - Determine and return OS name and version
# Borrowed from https://github.com/intel/compute-runtime/blob/master/os_release_info.cmake
#
# Args:
# 1.  the name of a variable to receive os_name
# 2.  the name of a variable to receive os_version
#
# Return values: (Quotation marks are always stripped).
# Upon failure, return values are null strings.
#
# Examples:
# os_name           os_version
# --------------    -------
# clear-linux-os    21180          (Changes twice daily)
# ubuntu            12.04  16.04  17.10  18.04
# fedora            27
# centos            6.9  7.4.1708
#
# Potential sources are tried (in order of preference) until a
# suitable one is found.

# Implementation documentation:
#
# The potential sources, in order, are as follows.
# - /etc/centos-release
# Centos 7 also has /etc/os-release.  File /etc/os-release is less
# precise about the Centos version (e.g., "7" instead of "7.4.1708").
# For that reason, this file is checked first.
# Examples:
# CentOS release 6.9 (Final)
# CentOS Linux release 7.4.1708 (Core)
# - /usr/lib/os-release
# Present for Clear Linux, modern Fedora, and Ubuntu since some time
# between 14.04 and 16.04.  The ID and VERSION_ID values are used.
# Examples:
# ID=clear-linux-os      VERSION_ID=21180
# ID=fedora              VERSION_ID=27
# ID=ubuntu              VERSION_ID="14.04"
# ID=ubuntu              VERSION_ID="16.04"
# ID="ubuntu"            VERSION_ID="17.10"
# - /etc/os-release - Same form as (sometimes a link to) /usr/lib/os-release
# ID="Ubuntu"            VERSION_ID="12.04"
# ID="Ubuntu"            VERSION_ID="14.04"
# with a symbolic link: /etc/os-release -> ../usr/lib/os-release
# ID="CentOS Linux"      VERSION_ID="7"    Also: ID_LIKE="rhel fedora"
# - /etc/lsb-release
# For Centos, not too meaningful.
# Other "OS"s are more reasonable:
# DISTRIB_ID=Ubuntu      DISTRIB_RELEASE=12.04
# DISTRIB_ID=Ubuntu      DISTRIB_RELEASE=14.04
# DISTRIB_ID=Ubuntu      DISTRIB_RELEASE=17.10
# ######################################################################################################################
function(erl_os_release_info _vn_id _vn_version_id _vn_codename)
    set(_var_id "")
    set(_var_version_id "")
    set(_var_codename "")

    if ("${_var_id}" STREQUAL "")
        set(file_path "/etc/centos-release")

        if (EXISTS "${file_path}")
            # Example: CentOS release 6.9 (Final)
            file(STRINGS "${file_path}" file_list LIMIT_COUNT 1)
            list(GET file_list 0 file_line)

            # Remove all parenthesized items.
            string(REGEX REPLACE "\\([^)]+\\)" "" file_line "${file_line}")

            # Extract start and end, discard optional "version" or "release"
            string(REGEX MATCH "^([A-Za-z0-9_]+)( +(version|release))? +(.*)$"
                    _dummy "${file_line}")

            # 1              2  3                    4
            set(_var_id "${CMAKE_MATCH_1}")
            set(_var_version_id "${CMAKE_MATCH_4}")
        endif ()
    endif ()

    if ("${_var_id}" STREQUAL "")
        if (EXISTS "/usr/lib/os-release")
            set(file_path "/usr/lib/os-release")
        elseif (EXISTS "/etc/os-release")
            set(file_path "/etc/os-release")
        else ()
            set(file_path "")
        endif ()

        if (NOT "${file_path}" STREQUAL "")
            file(STRINGS "${file_path}" data_list REGEX "^(ID|VERSION_ID|VERSION_CODENAME)=")

            # Look for lines like "ID="..." and VERSION_ID="..."
            foreach (_var ${data_list})
                if ("${_var}" MATCHES "^(ID)=(.*)$")
                    set(_var_id "${CMAKE_MATCH_2}")
                elseif ("${_var}" MATCHES "^(VERSION_ID)=(.*)$")
                    set(_var_version_id "${CMAKE_MATCH_2}")
                elseif ("${_var}" MATCHES "^(VERSION_CODENAME)=(.*)$")
                    set(_var_codename "${CMAKE_MATCH_2}")
                endif ()
            endforeach ()
        endif ()
    endif ()

    if ("${_var_id}" STREQUAL "")
        set(file_path "/etc/lsb-release")

        if (EXISTS "${file_path}")
            file(STRINGS "${file_path}" data_list
                    REGEX "^(DISTRIB_ID|DISTRIB_RELEASE|DISTRIB_CODENAME)=")

            # Look for lines like "DISTRIB_ID="..." and DISTRIB_RELEASE="..."
            foreach (_var ${data_list})
                if ("${_var}" MATCHES "^(DISTRIB_ID)=(.*)$")
                    set(_var_id "${CMAKE_MATCH_2}")
                elseif ("${_var}" MATCHES "^(DISTRIB_RELEASE)=(.*)$")
                    set(_var_version_id "${CMAKE_MATCH_2}")
                elseif ("${_var}" MATCHES "^(DISTRIB_CODENAME)=(.*)$")
                    set(_var_codename "${CMAKE_MATCH_2}")
                endif ()
            endforeach ()
        endif ()
    endif ()

    string(TOUPPER "${_var_id}" "_var_id")

    string(STRIP "${_var_id}" _var_id)
    string(STRIP "${_var_version_id}" _var_version_id)
    string(STRIP "${_var_codename}" _var_codename)

    # Remove any enclosing quotation marks
    string(REGEX REPLACE "^\"(.*)\"$" "\\1" _var_id "${_var_id}")
    string(REGEX REPLACE "^\"(.*)\"$" "\\1" _var_version_id "${_var_version_id}")
    string(REGEX REPLACE "^\"(.*)\"$" "\\1" _var_codename "${_var_codename}")

    if (NOT "${_vn_id}" STREQUAL "")
        set(${_vn_id} "${_var_id}" PARENT_SCOPE)
    endif ()

    if (NOT "${_vn_version_id}" STREQUAL "")
        set(${_vn_version_id} "${_var_version_id}" PARENT_SCOPE)
    endif ()

    if (NOT "${_vn_codename}" STREQUAL "")
        set(${_vn_codename} "${_var_codename}" PARENT_SCOPE)
    endif ()
endfunction()

# ##################################################################################################
# erl_parse_key_value_pairs
# ##################################################################################################
function(erl_parse_key_value_pairs _pairs prefix)
    # _pairs is read-only and string based, so we need to unpack it into a local variable.
    set(pairs ${${_pairs}})
    list(LENGTH pairs len)
    math(EXPR pair_counts "${len} / 2 - 1")
    math(EXPR left_over "${len} % 2")

    if (NOT "${left_over}" STREQUAL "0")
        message(FATAL_ERROR "pairs must be a list of pairs: <key> <value> <key> <value> ...")
    endif ()

    foreach (index RANGE ${pair_counts})
        math(EXPR key_index "${index} * 2")
        math(EXPR value_index "${key_index} + 1")
        list(GET pairs ${key_index} key)
        list(GET pairs ${value_index} value)
        set(${prefix}_${key} ${value} PARENT_SCOPE)
    endforeach ()
endfunction()

# ##################################################################################################
# erl_platform_based_message
# ##################################################################################################
function(erl_platform_based_message)
    set(options)
    set(oneValueArgs MSG_TYPE MSG_PREFIX)
    set(multiValueArgs MESSAGES)
    cmake_parse_arguments(ERL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ERL_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unparsed arguments: ${ERL_UNPARSED_ARGUMENTS}")
    endif ()

    erl_parse_key_value_pairs(ERL_MESSAGES ERL_MESSAGES)
    string(TOUPPER "${CMAKE_HOST_SYSTEM_NAME}" ERL_PLATFORM)

    if (ERL_PLATFORM STREQUAL "LINUX")
        erl_os_release_info(LINUX_DISTRO LINUX_VERSION LINUX_CODENAME)

        if (DEFINED ERL_MESSAGES_${LINUX_DISTRO}_LINUX)
            set(ERL_PLATFORM ${LINUX_DISTRO}_LINUX)
        endif ()
    endif ()

    if (NOT DEFINED ERL_MESSAGES_${ERL_PLATFORM} AND DEFINED ERL_MESSAGES_GENERAL)
        set(ERL_PLATFORM GENERAL)
    endif ()

    message(${ERL_MSG_TYPE} "${ERL_MSG_PREFIX}: ${ERL_MESSAGES_${ERL_PLATFORM}}")
endfunction()

# ##################################################################################################
# erl_suggest_cmd_for_assert
# ##################################################################################################
function(erl_suggest_cmd_for_assert)
    set(options)
    set(oneValueArgs ASSERT MSG MSG_TYPE)
    set(multiValueArgs COMMANDS)
    cmake_parse_arguments(ERL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ERL_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unparsed arguments: ${ERL_UNPARSED_ARGUMENTS}")
    endif ()

    if (NOT DEFINED ERL_ASSERT)
        message(FATAL_ERROR "ASSERT must be set")
    endif ()

    if (NOT ERL_MSG)
        message(FATAL_ERROR "MSG must be set")
    endif ()

    if (NOT ERL_MSG_TYPE)
        set(ERL_MSG_TYPE "FATAL_ERROR")
    endif ()

    if (NOT ERL_ASSERT)
        if (NOT DEFINED ERL_COMMANDS)
            message(${ERL_MSG_TYPE} "${ERL_MSG}: try ${ERL_GENERAL_CMD}")
        else ()
            erl_platform_based_message(
                    MSG_TYPE ${ERL_MSG_TYPE}
                    MSG_PREFIX ${ERL_MSG}
                    MESSAGES ${ERL_COMMANDS}
            )
        endif ()
    endif ()
endfunction()

# ##################################################################################################
# erl_find_package
# ##################################################################################################
macro(erl_find_package)
    set(options NO_RECORD QUIET REQUIRED PKGCONFIG)
    set(oneValueArgs PACKAGE)
    set(multiValueArgs COMMANDS)
    cmake_parse_arguments(ERL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT ERL_QUIET)
        if (NOT DEFINED ${ERL_PACKAGE}_VERBOSE_ONCE) # avoid printing multiple times
            set(${ERL_PACKAGE}_VERBOSE_ONCE ON
                    CACHE BOOL "Flag of whether print detailed logs for ${ERL_PACKAGE}" FORCE)
        else ()
            set(ERL_QUIET ON)
        endif ()

        message(STATUS "==========================================================================")
        if (ERL_PACKAGE STREQUAL "Python3")
            message(STATUS "To specify python interpreter, run `cmake -DPython3_ROOT_DIR=/path/to/python3_bin_folder ..`")
            message(STATUS "With CLion, Python_EXECUTABLE is set to the selected python interpreter")

            if (DEFINED Python_EXECUTABLE)
                get_filename_component(Python3_ROOT_DIR ${Python_EXECUTABLE} DIRECTORY)
            endif ()
        endif ()

        erl_platform_based_message(
                MSG_TYPE STATUS
                MSG_PREFIX "Finding package ${ERL_PACKAGE}, if not found"
                MESSAGES ${ERL_COMMANDS})
    endif ()

    unset(_args)
    if (ERL_REQUIRED)
        list(APPEND _args REQUIRED)
    endif ()
    if (ERL_QUIET)
        list(APPEND _args QUIET)
    endif ()
    list(APPEND _args ${ERL_UNPARSED_ARGUMENTS})

    if (ERL_PKGCONFIG)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(${ERL_PACKAGE} ${_args})
    else ()
        find_package(${ERL_PACKAGE} ${_args})
    endif ()

    if (${ERL_PACKAGE}_FOUND AND NOT ERL_QUIET)
        foreach (item IN ITEMS FOUND INCLUDE_DIR INCLUDE_DIRS LIBRARY LIBRARIES LIBS DEFINITIONS)
            if (DEFINED ${ERL_PACKAGE}_${item})
                message(STATUS "${ERL_PACKAGE}_${item}: ${${ERL_PACKAGE}_${item}}")
            endif ()
        endforeach ()
    endif ()

    if (ERL_NO_RECORD)
        if (NOT ERL_QUIET)
            message(STATUS "${ERL_PACKAGE} is not added to ${PROJECT_NAME}_DEPENDS")
        endif ()
    else ()
        list(APPEND ${PROJECT_NAME}_DEPENDS ${ERL_PACKAGE})
    endif ()

    unset(_args)
    unset(ERL_PKGCONFIG)
    unset(ERL_NO_RECORD)
    unset(ERL_QUIET)
    unset(ERL_REQUIRED)
    unset(ERL_PACKAGE)
    unset(ERL_COMMANDS)
    unset(ERL_UNPARSED_ARGUMENTS)
endmacro()

# ##################################################################################################
# erl_find_path
# ##################################################################################################
function(erl_find_path)
    set(options)
    set(oneValueArgs OUTPUT PACKAGE)
    set(multiValueArgs COMMANDS)
    cmake_parse_arguments(ERL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED ERL_OUTPUT)
        set(ERL_OUTPUT "FILE_FOUND")
    endif ()

    message(STATUS "==============================================================================")

    if (DEFINED ERL_PACKAGE)
        erl_platform_based_message(
                MSG_TYPE STATUS
                MSG_PREFIX "Checking file for ${ERL_PACKAGE}, if not found"
                MESSAGES ${ERL_COMMANDS})
    else ()
        erl_platform_based_message(
                MSG_TYPE STATUS
                MSG_PREFIX "Checking file, if not found"
                MESSAGES ${ERL_COMMANDS})
    endif ()

    find_path(
            ${ERL_OUTPUT}
            ${ERL_UNPARSED_ARGUMENTS}
    )
    get_filename_component(${ERL_OUTPUT} ${${ERL_OUTPUT}} REALPATH)
    message(STATUS "${ERL_OUTPUT}: ${${ERL_OUTPUT}}")
    set(${ERL_OUTPUT} ${${ERL_OUTPUT}} PARENT_SCOPE)
endfunction()

# ##################################################################################################
# erl_detect_ros
# ##################################################################################################
macro(erl_detect_ros)
    message(STATUS "==============================================================================")

    if (DEFINED ENV{ROS_VERSION})
        set(ROS_ACTIVATED ON)
        set(ROS_VERSION "$ENV{ROS_VERSION}")
        set(ROS_DISTRO "$ENV{ROS_DISTRO}")
    elseif (DEFINED CATKIN_DEVEL_PREFIX)
        message(STATUS "catkin devel prefix: ${CATKIN_DEVEL_PREFIX}")
        set(ROS_ACTIVATED ON)
        set(ROS_VERSION "1")
    endif ()

    if (ROS_VERSION STREQUAL "1")
        message(STATUS "ROS_VERSION: ${ROS_VERSION}")
        add_definitions(-DERL_ROS_VERSION_1)
    elseif (ROS_VERSION STREQUAL "2")
        message(STATUS "ROS_VERSION: ${ROS_VERSION}")
        add_definitions(-DERL_ROS_VERSION_2)
    endif ()
endmacro()

# ##################################################################################################
# erl_set_project_paths
# ##################################################################################################
macro(erl_set_project_paths)
    # set project paths
    set(${PROJECT_NAME}_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}
            CACHE PATH "Root directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include
            CACHE PATH "Include directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}   # set to project root due to catkin
            CACHE PATH "Source directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test
            CACHE PATH "Test directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake
            CACHE PATH "CMake directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_PYTHON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python
            CACHE PATH "Python directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_PYTHON_BINDING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python/binding
            CACHE PATH "Python binding directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scripts
            CACHE PATH "Script directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config
            CACHE PATH "Config directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_LAUNCH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/launch
            CACHE PATH "Launch directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_MSG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/msg
            CACHE PATH "Msg directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_SRV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/srv
            CACHE PATH "Srv directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_ACTION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/action
            CACHE PATH "Action directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_DOC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doc
            CACHE PATH "Doc directory of ${PROJECT_NAME}" FORCE)

    set(${PROJECT_NAME}_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}
            CACHE PATH "Build directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_BUILD_PYTHON_DIR ${CMAKE_CURRENT_BINARY_DIR}/python
            CACHE PATH "Build python directory of ${PROJECT_NAME}" FORCE)
    set(${PROJECT_NAME}_BUILD_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/test
            CACHE PATH "Build test directory of ${PROJECT_NAME}" FORCE)

    # set project devel & install paths
    if (ROS_VERSION STREQUAL "1")
        set(${PROJECT_NAME}_INSTALL_BINARY_DIR ${CATKIN_GLOBAL_BIN_DESTINATION} # `bin`
                CACHE PATH "Binary directory of ${PROJECT_NAME}" FORCE)
        set(${PROJECT_NAME}_INSTALL_ETC_DIR ${CATKIN_PACKAGE_ETC_DESTINATION}
                CACHE PATH "Etc directory of ${PROJECT_NAME}" FORCE)
        set(${PROJECT_NAME}_INSTALL_INCLUDE_DIR ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
                CACHE PATH "Include directory of ${PROJECT_NAME}" FORCE)
        set(${PROJECT_NAME}_INSTALL_LIBRARY_DIR ${CATKIN_PACKAGE_LIB_DESTINATION}
                CACHE PATH "Library directory of ${PROJECT_NAME}" FORCE)
        set(${PROJECT_NAME}_INSTALL_PYTHON_DIR ${CATKIN_PACKAGE_PYTHON_DESTINATION}
                CACHE PATH "Python directory of ${PROJECT_NAME}" FORCE)
        set(${PROJECT_NAME}_INSTALL_SHARE_DIR ${CATKIN_PACKAGE_SHARE_DESTINATION}
                CACHE PATH "Share directory of ${PROJECT_NAME}" FORCE)
        set(${PROJECT_NAME}_INSTALL_CMAKE_DIR ${CATKIN_PACKAGE_SHARE_DESTINATION}/cmake
                CACHE PATH "CMake directory of ${PROJECT_NAME}" FORCE)

        if (NOT DEFINED ERL_CATKIN_INSTALL_DIR)
            set(ERL_CATKIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
            get_filename_component(ERL_CATKIN_WORKSPACE_DIR ${ERL_CATKIN_INSTALL_DIR} DIRECTORY)
            set(ERL_CATKIN_INSTALL_LIB_DIR ${ERL_CATKIN_INSTALL_DIR}/lib)
            set(ERL_CATKIN_INSTALL_PYTHON_DIR
                    ${ERL_CATKIN_INSTALL_DIR}/${CATKIN_GLOBAL_PYTHON_DESTINATION})

            set(ERL_CATKIN_DEVEL_DIR ${ERL_CATKIN_WORKSPACE_DIR}/devel)
            set(ERL_CATKIN_DEVEL_LIB_DIR ${ERL_CATKIN_DEVEL_DIR}/lib)
            set(ERL_CATKIN_DEVEL_PYTHON_DIR
                    ${ERL_CATKIN_DEVEL_DIR}/${CATKIN_GLOBAL_PYTHON_DESTINATION})
        endif ()
    elseif (ROS_VERSION STREQUAL "2")
        message(FATAL_ERROR "ROS2 is not supported yet")
    else ()
        include(GNUInstallDirs)
        include(CMakePackageConfigHelpers)

        # /usr/local/bin
        set(${PROJECT_NAME}_INSTALL_BINARY_DIR ${CMAKE_INSTALL_BINDIR} # `bin`
                CACHE PATH "Path to ${PROJECT_NAME} binary directory during installation" FORCE)

        # /usr/local/etc/PROJECT_NAME
        set(${PROJECT_NAME}_INSTALL_ETC_DIR ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
                CACHE PATH "Path to ${PROJECT_NAME} etc directory during installation" FORCE)

        # /usr/local/include/PROJECT_NAME
        set(${PROJECT_NAME}_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}
                CACHE PATH "Path to ${PROJECT_NAME} include directory during installation" FORCE)

        # /usr/local/lib/PROJECT_NAME
        set(${PROJECT_NAME}_INSTALL_LIBRARY_DIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
                CACHE PATH "Path to ${PROJECT_NAME} library directory during installation" FORCE)

        # /usr/local/lib/pythonX.Y/dist-packages/PROJECT_NAME
        set(${PROJECT_NAME}_INSTALL_PYTHON_DIR ${Python3_SITELIB}/${PROJECT_NAME}
                CACHE PATH "Path to ${PROJECT_NAME} python directory during installation" FORCE)

        # /usr/local/share/PROJECT_NAME
        set(${PROJECT_NAME}_INSTALL_SHARE_DIR ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
                CACHE PATH "Path to ${PROJECT_NAME} share directory during installation" FORCE)

        # /usr/local/share/PROJECT_NAME/cmake
        set(${PROJECT_NAME}_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
                CACHE PATH "Path to ${PROJECT_NAME} cmake directory during installation" FORCE)
    endif ()
endmacro()

# ##################################################################################################
# erl_print_variables
# ##################################################################################################
function(erl_print_variables)
    get_cmake_property(VARIABLE_NAMES VARIABLES)
    list(SORT VARIABLE_NAMES)

    foreach (VARIABLE_NAME ${VARIABLE_NAMES})
        message(STATUS "${VARIABLE_NAME}=${${VARIABLE_NAME}}")
    endforeach ()
endfunction()

# ##################################################################################################
# erl_print_variable
# ##################################################################################################
function(erl_print_variable VARIABLE_NAME)
    message(STATUS "${VARIABLE_NAME}=${${VARIABLE_NAME}}")
endfunction()

# ##################################################################################################
# erl_setup_compiler
# ##################################################################################################
macro(erl_setup_compiler)
    option(ERL_IGNORE_CONDA_LIBRARIES "Ignore conda libraries" ON)
    option(ERL_PRINT_HEADER_DEPENDENCIES "Print header dependencies" OFF)

    if (NOT $ENV{CONDA_PREFIX} STREQUAL "" AND ERL_IGNORE_CONDA_LIBRARIES)
        list(APPEND CMAKE_IGNORE_PREFIX_PATH $ENV{CONDA_PREFIX}) # ignore conda libraries
    endif ()

    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif ()

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    if (NOT CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 17)
    endif ()

    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    if (ERL_PRINT_HEADER_DEPENDENCIES)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -H")
    endif ()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fopenmp -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--disable-new-dtags") # pass arguments to the linker
    # disable new DTAGS (DT_RUNPATH) since it is not supported in Ubuntu
    # old DTAGS (DT_RPATH) is used to specify paths for libraries that are
    # directly linked to the executable
    # new DTAGS (DT_RUNPATH) is used to specify paths for libraries that are
    # transitively linked to the executable
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color -fdiagnostics-show-template-tree")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -ftrack-macro-expansion=2")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -funroll-loops -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -funroll-loops -flto=auto")

    if (NOT CMAKE_OSX_DEPLOYMENT_TARGET)
        set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)
    endif ()

    find_program(CCACHE_FOUND ccache)

    if (CCACHE_FOUND)
        set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
    else ()
        message(STATUS "ccache is not found")
    endif ()

    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DNDEBUG)
    else ()
        set(CMAKE_VERBOSE_MAKEFILE ON)
    endif ()

    string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)
    add_definitions(-D${PROJECT_NAME_UPPER}_ROOT_DIR="${${PROJECT_NAME}_ROOT_DIR}")
endmacro()

# ##################################################################################################
# erl_enable_cuda
# ##################################################################################################
macro(erl_enable_cuda)
    set(CMAKE_CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER} CACHE STRING "Host compiler for CUDA" FORCE)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    set(CMAKE_CUDA_FLAGS_RELEASE "-O3")
    set(CMAKE_CUDA_FLAGS_DEBUG "-G")
endmacro()

# ##################################################################################################
# erl_setup_common_packages
# ##################################################################################################
macro(erl_setup_common_packages)
    # must be before erl_config_eigen
    # because some flags about Eigen3 are set.
    erl_config_lapack()

    erl_config_boost()  # Boost libraries
    erl_config_fmt()    # string formatting
    erl_config_openmp() # OpenMP support
    erl_config_eigen()  # Eigen3 library

    # file format libraries
    erl_config_json()
    erl_config_nanoflann()
    erl_config_yaml()

    # image processing, visualization libraries
    erl_config_opencv()
    erl_config_pangolin()
    erl_config_plplot()

    erl_config_tracy()   # profiling tool
endmacro()

# ######################################################################################################################
# erl_setup_python
# ######################################################################################################################
macro(erl_setup_python)
    option(ERL_BUILD_PYTHON "Build Python binding" ON)
    if (ERL_BUILD_PYTHON)
        erl_config_python3()
        erl_config_pybind11()
    endif ()
endmacro()

# ##################################################################################################
# erl_setup_test
# ##################################################################################################
macro(erl_setup_test)
    option(ERL_BUILD_TEST "Build executables for test" ON)

    if (ERL_BUILD_TEST)
        add_definitions(-DERL_BUILD_TEST)
        enable_testing()

        if (NOT ROS_ACTIVATED) # GTest is configured by ROS if ROS is activated
            # we need to use GTest::GTest and GTest::Main in other subprojects
            erl_find_package(
                    PACKAGE GTest
                    REQUIRED
                    COMMANDS UBUNTU_LINUX "try `sudo apt install libgtest-dev`"
                    COMMANDS ARCH_LINUX "try `sudo pacman -S gtest`")
            include(GoogleTest)
        endif ()
    endif ()

    if (NOT DEFINED ERL_BUILD_TEST_${PROJECT_NAME})
        set(ERL_BUILD_TEST_${PROJECT_NAME} ${ERL_BUILD_TEST})
    endif ()
endmacro()

# ##################################################################################################
# erl_setup_ros
# ##################################################################################################
macro(erl_setup_ros)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs MSG_DEPENDENCIES MSG_FILES SRV_FILES ACTION_FILES)
    cmake_parse_arguments(${PROJECT_NAME} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ROS_ACTIVATED)
        if (ROS_VERSION STREQUAL "1")
            if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/package.xml)
                message(FATAL_ERROR "No package.xml found in ${CMAKE_CURRENT_LIST_DIR}")
            endif ()

            erl_find_package(
                    PACKAGE catkin
                    NO_RECORD
                    REQUIRED COMPONENTS ${${PROJECT_NAME}_CATKIN_COMPONENTS}
                    COMMANDS UBUNTU_LINUX "try `sudo apt install ros-${ROS_DISTRO}-catkin python3-catkin-pkg python3-empy python3-nose python3-setuptools`"
                    COMMANDS ARCH_LINUX "try `paru -S python-catkin_tools ros-${ROS_DISTRO}-catkin`")

            if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/setup.py)
                catkin_python_setup()
                set(${PROJECT_NAME}_CATKIN_PYTHON_SETUP TRUE
                        CACHE BOOL "TRUE if catkin_python_setup() was called" FORCE)
            else ()
                set(${PROJECT_NAME}_CATKIN_PYTHON_SETUP FALSE
                        CACHE BOOL "TRUE if catkin_python_setup() was called" FORCE)
            endif ()

            if (${PROJECT_NAME}_MSG_FILES)
                add_message_files(FILES ${${PROJECT_NAME}_MSG_FILES})
            endif ()

            if (${PROJECT_NAME}_SRV_FILES)
                add_service_files(FILES ${${PROJECT_NAME}_SRV_FILES})
            endif ()

            if (${PROJECT_NAME}_ACTION_FILES)
                add_action_files(FILES ${${PROJECT_NAME}_ACTION_FILES})
            endif ()

            if (${PROJECT_NAME}_MSG_DEPENDENCIES)
                generate_messages(DEPENDENCIES ${${PROJECT_NAME}_MSG_DEPENDENCIES})
            endif ()
        else ()
            message(FATAL_ERROR "ROS2 is not supported yet")
        endif ()
    endif ()
endmacro()

# ##################################################################################################
# erl_catkin_package
# ##################################################################################################
macro(erl_catkin_package)
    if (ROS_ACTIVATED AND ROS_VERSION STREQUAL "1")
        catkin_package(${ARGV})

        # filter out Eigen3 installed at `/usr/include/eigen3` from catkin_INCLUDE_DIRS
        # if `/usr/local/include/eigen3` is also in catkin_INCLUDE_DIRS
        set(EIGEN3_AT_USR_INCLUDE_EIGEN3 FALSE)
        set(EIGEN3_AT_USR_LOCAL_INCLUDE_EIGEN3 FALSE)

        foreach (inc ${catkin_INCLUDE_DIRS})
            if (inc STREQUAL "/usr/include/eigen3")
                set(EIGEN3_AT_USR_INCLUDE_EIGEN3 TRUE)
            else ()
                list(APPEND filtered_catkin_INCLUDE_DIRS ${inc})
            endif ()

            if (inc STREQUAL "/usr/local/include/eigen3")
                set(EIGEN3_AT_USR_LOCAL_INCLUDE_EIGEN3 TRUE)
            endif ()
        endforeach ()

        if (NOT EIGEN3_AT_USR_LOCAL_INCLUDE_EIGEN3 AND EIGEN3_AT_USR_INCLUDE_EIGEN3)
            list(APPEND filtered_catkin_INCLUDE_DIRS "/usr/include/eigen3")
        endif ()

        set(catkin_INCLUDE_DIRS ${filtered_catkin_INCLUDE_DIRS})
        unset(filtered_catkin_INCLUDE_DIRS)

        # filter out MODULE_LIBRARY from catkin_LIBRARIES
        foreach (lib ${catkin_LIBRARIES})
            get_filename_component(lib_dir ${lib} DIRECTORY)
            list(APPEND catkin_LIBRARY_DIRS ${lib_dir})

            if (TARGET ${lib})
                get_target_property(lib_type ${lib} TYPE)

                if (NOT lib_type STREQUAL "MODULE_LIBRARY") # MODULE_LIBRARY, e.g. pybind11 lib, cannot be linked
                    list(APPEND filtered_catkin_LIBRARIES ${lib})
                endif ()
            else ()
                list(APPEND filtered_catkin_LIBRARIES ${lib})
            endif ()
        endforeach ()

        set(catkin_LIBRARIES ${filtered_catkin_LIBRARIES})
        unset(filtered_catkin_LIBRARIES)
    endif ()

    erl_set_project_paths()

    if (ROS_ACTIVATED AND ROS_VERSION STREQUAL "1")
        set(CATKIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
        get_filename_component(CATKIN_WORKSPACE_DIR ${CATKIN_INSTALL_DIR} DIRECTORY)
        set(CATKIN_DEVEL_DIR ${CATKIN_WORKSPACE_DIR}/devel)
        set(CATKIN_DEVEL_LIB_DIR ${CATKIN_DEVEL_DIR}/lib)
        set(CATKIN_INSTALL_LIB_DIR ${CATKIN_INSTALL_DIR}/lib)
        set(CATKIN_INSTALL_PYTHON_DIR ${CATKIN_INSTALL_DIR}/${CATKIN_GLOBAL_PYTHON_DESTINATION})
    endif ()
endmacro()

# ##################################################################################################
# erl_project_setup
# ##################################################################################################
macro(erl_project_setup _name)
    set(options ENABLE_CUDA)
    set(oneValueArgs)
    set(multiValueArgs ERL_PACKAGES CATKIN_COMPONENTS CATKIN_DEPENDS)
    cmake_parse_arguments(${_name} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (${_name}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Some arguments are not recognized by erl_project_setup")
    endif ()

    if (${_name}_ENABLE_CUDA)
        erl_enable_cuda()
    endif ()

    foreach (erl_package ${${_name}_ERL_PACKAGES})
        if (NOT TARGET ${erl_package})
            erl_find_package(
                    PACKAGE ${erl_package}
                    REQUIRED
                    COMMANDS GENERAL "build and install ${erl_package} at first")
        endif ()
    endforeach ()

    # check if is top-level
    if (CMAKE_VERSION VERSION_LESS 3.21)
        get_property(NOT_TOP_LEVEL DIRECTORY PROPERTY PARENT_DIRECTORY)

        if (NOT NOT_TOP_LEVEL)
            set(PROJECT_IS_TOP_LEVEL ON)
        else ()
            set(PROJECT_IS_TOP_LEVEL OFF)
        endif ()
    endif ()

    erl_set_project_paths()
    erl_setup_compiler()
    erl_detect_ros()

    if (ROS_ACTIVATED)
        if (ROS_VERSION STREQUAL "1")
            list(APPEND ${_name}_CATKIN_COMPONENTS ${${PROJECT_NAME}_ERL_PACKAGES})
            list(REMOVE_DUPLICATES ${_name}_CATKIN_COMPONENTS)
            list(APPEND ${_name}_CATKIN_DEPENDS ${${PROJECT_NAME}_ERL_PACKAGES})
            list(REMOVE_DUPLICATES ${_name}_CATKIN_DEPENDS)
        endif ()
    endif ()


    erl_setup_common_packages()
    if (NOT ERL_PROJECT_SETUP_DONE OR ROS_ACTIVATED)
        erl_setup_python()

        if (NOT ROS_ACTIVATED) # if ROS is activated, there is no PARENT_SCOPE when erl_project_setup is called
            if (PROJECT_IS_TOP_LEVEL)
                set(ERL_PROJECT_SETUP_DONE TRUE)
            else ()
                set(ERL_PROJECT_SETUP_DONE TRUE PARENT_SCOPE)
            endif ()
        endif ()
    endif ()

    erl_setup_test()
endmacro()

function(erl_add_pybind_module)
    # PYBIND_MODULE_NAME: module name
    # PYBIND_SRC_DIR: source directory of the pybind module
    # PYTHON_PKG_DIR: source directory of the python package, where the module will be copied to when ROS is activated
    # INCLUDE_DIRS: directories to include
    # LIBRARIES: libraries to link

    set(options)
    set(oneValueArgs PYBIND_MODULE_NAME PYBIND_SRC_DIR PYTHON_PKG_DIR)
    set(multiValueArgs INCLUDE_DIRS LIBRARIES)
    cmake_parse_arguments("arg" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED ERL_BUILD_PYTHON_${PROJECT_NAME})
        set(ERL_BUILD_PYTHON_${PROJECT_NAME} ${ERL_BUILD_PYTHON})
    endif ()

    if (NOT ERL_BUILD_PYTHON)
        set(ERL_BUILD_PYTHON_${PROJECT_NAME} OFF)
    endif ()

    if (NOT ERL_BUILD_PYTHON_${PROJECT_NAME})
        return()
    endif ()

    message(STATUS "Adding Python binding module ${arg_PYBIND_MODULE_NAME} for ${PROJECT_NAME}")
    erl_setup_python()

    file(GLOB_RECURSE SRC_FILES "${arg_PYBIND_SRC_DIR}/*.cpp")
    if (NOT SRC_FILES)
        message(FATAL_ERROR "No cpp file found in ${arg_PYBIND_SRC_DIR}")
    endif ()

    if (NOT DEFINED arg_PYBIND_MODULE_NAME)
        message(FATAL_ERROR "PYBIND_MODULE_NAME not set")
    endif ()

    # pybind runtime lib
    get_target_property(lib_type pybind11::module TYPE)
    if (NOT lib_type STREQUAL "INTERFACE_LIBRARY") # check if it is an interface library
        set_target_properties(pybind11::module PROPERTIES SYSTEM ON)
    endif ()
    pybind11_add_module(${arg_PYBIND_MODULE_NAME} ${SRC_FILES})

    # ref: https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling
    # Use separate rpaths during build and install phases
    # set(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # Don't use the install-rpath during the build phase
    # set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    # set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

    # Automatically add all linked folders that are NOT in the build directory to the rpath
    # (per library?)
    # set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    if (APPLE)
        set(CMAKE_MACOSX_RPATH ON)
        set(_rpath_portable_origin "@loader_path")
    else ()
        set(_rpath_portable_origin $ORIGIN)
    endif ()

    set(_rpath "${_rpath_portable_origin}:${_rpath_portable_origin}/lib/${PROJECT_NAME}")

    # how to check rpath of a library file:
    # objdump -x <library_name>.so | grep 'R.*PATH'
    # output: RUNPATH     $ORIGIN:$ORIGIN/lib/<project_name>
    # how to check shared library dependencies of a library file:
    # ldd <library_name>.so
    set_target_properties(${arg_PYBIND_MODULE_NAME} PROPERTIES
            SKIP_BUILD_RPATH FALSE # Use separate rpaths during build and install phases
            BUILD_WITH_INSTALL_RPATH FALSE # Don't use the install-rpath during the build phase
            INSTALL_RPATH "${_rpath}" # search in the same directory, or in lib/<project_name>
            INSTALL_RPATH_USE_LINK_PATH TRUE)
    target_compile_definitions(${arg_PYBIND_MODULE_NAME}
            PRIVATE PYBIND_MODULE_NAME=${arg_PYBIND_MODULE_NAME})
    target_include_directories(${arg_PYBIND_MODULE_NAME} SYSTEM PRIVATE ${Python3_INCLUDE_DIRS})
    if (DEFINED arg_INCLUDE_DIRS)
        target_include_directories(${arg_PYBIND_MODULE_NAME} PRIVATE ${arg_INCLUDE_DIRS})
    endif ()
    if (DEFINED arg_LIBRARIES)
        target_link_libraries(${arg_PYBIND_MODULE_NAME} PRIVATE ${arg_LIBRARIES})
    endif ()

    # put the library in the source python package directory, such that setup.py can find it
    # set_target_properties(${${PROJECT_NAME}_PYBIND_MODULE_NAME} PROPERTIES
    # LIBRARY_OUTPUT_DIRECTORY ${${PROJECT_NAME}_PYTHON_PKG_DIR})
    if (ROS_ACTIVATED AND ROS_VERSION STREQUAL "1")
        # copy file to a regular library name so that catkin does not throw an error, but this file may not
        # work with Python3 because the filename is lib<name>.so, which does not match the module name.
        set(LIB_NAME lib${arg_PYBIND_MODULE_NAME}.so)
        set(DEVEL_LIB_PATH ${ERL_CATKIN_DEVEL_LIB_DIR}/${LIB_NAME})
        set(INSTALL_LIB_PATH ${ERL_CATKIN_INSTALL_LIB_DIR}/${LIB_NAME})

        add_custom_command(TARGET ${arg_PYBIND_MODULE_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${arg_PYBIND_MODULE_NAME}> ${DEVEL_LIB_PATH}
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${arg_PYBIND_MODULE_NAME}> ${INSTALL_LIB_PATH}
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${arg_PYBIND_MODULE_NAME}> ${arg_PYTHON_PKG_DIR}) # copy to source directory for devel
    endif ()
endfunction()

# ##################################################################################################
# erl_add_python_package
# ##################################################################################################
function(erl_add_python_package)
    set(options)
    set(oneValueArgs PYTHON_PKG_DIR)
    set(multiValueArgs DEPENDS_PYTHON_PKGS)
    cmake_parse_arguments(${PROJECT_NAME} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED ERL_BUILD_PYTHON_${PROJECT_NAME})
        set(ERL_BUILD_PYTHON_${PROJECT_NAME} ${ERL_BUILD_PYTHON})
    endif ()

    if (NOT ERL_BUILD_PYTHON)
        set(ERL_BUILD_PYTHON_${PROJECT_NAME} OFF)
    endif ()

    if (NOT ERL_BUILD_PYTHON_${PROJECT_NAME})
        return()
    endif ()

    message(STATUS "Adding Python package for ${PROJECT_NAME}")
    erl_setup_python()

    # ${PROJECT_NAME}_PYTHON_DIR: <project_dir>/python
    # ${PROJECT_NAME}_PYTHON_PKG_DIR: <project_dir>/python/<py_package_name>
    # ${PROJECT_NAME}_PYTHON_BINDING_DIR: <project_dir>/python/binding
    # ${PROJECT_NAME}_BUILD_DIR: <project_build_dir>
    # ${PROJECT_NAME}_BUILD_PYTHON_DIR: <project_build_dir>/python
    # ${PROJECT_NAME}_BUILD_PYTHON_PKG_DIR: <project_build_dir>/python/<py_package_name>

    # get package name
    get_filename_component(${PROJECT_NAME}_PY_PACKAGE_NAME ${${PROJECT_NAME}_PYTHON_PKG_DIR} NAME)

    # ${PROJECT_NAME}_BUILD_PYTHON_PKG_DIR: <project_build_dir>/python/<py_package_name>
    set(${PROJECT_NAME}_BUILD_PYTHON_PKG_DIR ${${PROJECT_NAME}_BUILD_PYTHON_DIR}/${${PROJECT_NAME}_PY_PACKAGE_NAME})

    if (EXISTS ${${PROJECT_NAME}_ROOT_DIR}/setup.py)
        option(ERL_PYTHON_INSTALL_USER "Install Python package to user directory" OFF)
        set(erl_pip_install_args "--verbose")

        if (ERL_PYTHON_INSTALL_USER)
            set(erl_pip_install_args "${erl_pip_install_args} --user")
        endif ()

        add_custom_target(${PROJECT_NAME}_py_wheel
                COMMAND ${Python3_EXECUTABLE} setup.py bdist_wheel
                WORKING_DIRECTORY ${${PROJECT_NAME}_ROOT_DIR}
                COMMENT "Building Python wheel for ${PROJECT_NAME}")
        add_custom_target(${PROJECT_NAME}_py_develop
                COMMAND ${Python3_EXECUTABLE} -m pip install -e . ${erl_pip_install_args}
                WORKING_DIRECTORY ${${PROJECT_NAME}_ROOT_DIR}
                DEPENDS ${${PROJECT_NAME}_PYBIND_MODULE_NAME}
                COMMENT "Installing Python package ${PROJECT_NAME} in develop mode")
        add_custom_target(${PROJECT_NAME}_py_install
                COMMAND ${Python3_EXECUTABLE} -m pip install . ${erl_pip_install_args}
                WORKING_DIRECTORY ${${PROJECT_NAME}_ROOT_DIR}
                COMMENT "Installing Python package ${PROJECT_NAME} in install mode")
        get_filename_component(stubgen_path ${Python3_EXECUTABLE} DIRECTORY)
        set(stubgen_path ${stubgen_path}/stubgen)

        if (EXISTS ${stubgen_path})
            add_custom_target(${PROJECT_NAME}_py_stub
                    COMMAND ${stubgen_path} -o ${CMAKE_CURRENT_BINARY_DIR}/python/stubs -p ${${PROJECT_NAME}_PY_PACKAGE_NAME}.${${PROJECT_NAME}_PYBIND_MODULE_NAME} --verbose
                    DEPENDS ${PROJECT_NAME}_py_install
                    WORKING_DIRECTORY ${${PROJECT_NAME}_ROOT_DIR}
                    COMMENT "Generating stub files for Python package ${PROJECT_NAME}")
        endif ()

        unset(erl_pip_install_args)
    else ()
        message(WARNING "setup.py not found in ${${PROJECT_NAME}_ROOT_DIR}, rules for Python package ${PROJECT_NAME} will not be generated.")
    endif ()
endfunction()

# ##################################################################################################
# erl_install
# ##################################################################################################
macro(erl_install)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs EXECUTABLES LIBRARIES PYBIND_MODULES CATKIN_PYTHON_PROGRAMS OTHER_FILES)
    cmake_parse_arguments(${PROJECT_NAME}_INSTALL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Create a package to distribute the software
    # reference 1: https://dominikberner.ch/cmake-interface-lib/ (not complete)
    # reference 2: https://gitlab.com/libeigen/eigen/-/blob/master/CMakeLists.txt
    # include(GNUInstallDirs)  # called by erl_set_project_paths
    # include(CMakePackageConfigHelpers)  # called by erl_set_project_paths

    # Install the executable files
    if (${PROJECT_NAME}_INSTALL_EXECUTABLES)
        foreach (executable ${${PROJECT_NAME}_INSTALL_EXECUTABLES})
            message(STATUS "Generate install rule for executable ${executable}")
        endforeach ()

        # Export targets defined in this file and define the installation location
        # See http://docs.ros.org/melodic/api/catkin/html/howto/format1/building_executables.html
        install(TARGETS ${${PROJECT_NAME}_INSTALL_EXECUTABLES} # add the targets here
                RUNTIME DESTINATION ${${PROJECT_NAME}_INSTALL_BINARY_DIR})
    endif ()

    # Install the library files
    if (${PROJECT_NAME}_INSTALL_LIBRARIES)
        foreach (library ${${PROJECT_NAME}_INSTALL_LIBRARIES})
            message(STATUS "Generate install rule for library ${library}")
        endforeach ()

        # Export targets defined in this file and define the installation location
        install(TARGETS ${${PROJECT_NAME}_INSTALL_LIBRARIES} # add the targets here
                EXPORT ${PROJECT_NAME}_Targets
                ARCHIVE DESTINATION ${${PROJECT_NAME}_INSTALL_LIBRARY_DIR}
                LIBRARY DESTINATION ${${PROJECT_NAME}_INSTALL_LIBRARY_DIR}
                RUNTIME DESTINATION ${${PROJECT_NAME}_INSTALL_BINARY_DIR})
    endif ()

    # Install the header files
    if (EXISTS ${${PROJECT_NAME}_INCLUDE_DIR}/${PROJECT_NAME})
        install(DIRECTORY ${${PROJECT_NAME}_INCLUDE_DIR}/${PROJECT_NAME}
                DESTINATION ${${PROJECT_NAME}_INSTALL_INCLUDE_DIR}) # ${PROJECT_NAME} is added automatically
    endif ()

    # Install other files
    if (${PROJECT_NAME}_INSTALL_OTHER_FILES)
        install(FILES ${${PROJECT_NAME}_INSTALL_OTHER_FILES}
                DESTINATION ${${PROJECT_NAME}_INSTALL_SHARE_DIR})
    endif ()

    # Install cmake files, only for erl_common
    if (PROJECT_NAME STREQUAL "erl_common")
        file(GLOB_RECURSE CMAKE_FILES ${ERL_CMAKE_DIR}/*.cmake)
        install(FILES ${CMAKE_FILES}
                DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKE_DIR})
    endif ()

    # ROS Support
    if (ROS_ACTIVATED AND ROS_VERSION STREQUAL "1")
        # Install the pybind module
        if (${PROJECT_NAME}_INSTALL_PYBIND_MODULES)
            foreach (module ${${PROJECT_NAME}_INSTALL_PYBIND_MODULES})
                message(STATUS "Generate install rule for pybind module ${module}")
            endforeach ()

            install(TARGETS ${${PROJECT_NAME}_INSTALL_PYBIND_MODULES}
                    ARCHIVE DESTINATION ${${PROJECT_NAME}_INSTALL_PYTHON_DIR}
                    LIBRARY DESTINATION ${${PROJECT_NAME}_INSTALL_PYTHON_DIR}
                    RUNTIME DESTINATION ${${PROJECT_NAME}_INSTALL_PYTHON_DIR})
        endif ()

        # Config file generation is taken care of by catkin_package()

        # Mark executable scripts (Python etc.) for installation in contrast to setup.py
        if (DEFINED ${PROJECT_NAME}_INSTALL_CATKIN_PYTHON_PROGRAMS)
            catkin_install_python(PROGRAMS
                    ${${PROJECT_NAME}_INSTALL_CATKIN_PYTHON_PROGRAMS}
                    DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
        endif ()

        # Install the python package
        if (${${PROJECT_NAME}_CATKIN_PYTHON_SETUP} AND DEFINED ${PROJECT_NAME}_BUILD_PYTHON_PKG_DIR)
            # install files generated from files in python/${PROJECT_NAME}
            install(DIRECTORY ${${PROJECT_NAME}_BUILD_PYTHON_PKG_DIR}
                    DESTINATION ${CATKIN_GLOBAL_PYTHON_DESTINATION})
        endif ()

        # TODO: install other files, e.g. launch files, config files, etc.
    elseif (BUILD_FOR_ROS2)
        message(FATAL_ERROR "Not implemented yet")
    else ()
        # Install the pybind module if pip install is used
        if (${PROJECT_NAME}_INSTALL_PYBIND_MODULES AND DEFINED PIP_LIB_DIR)
            foreach (module ${${PROJECT_NAME}_INSTALL_PYBIND_MODULES})
                message(STATUS "Generate install rule for pybind module ${module}")
            endforeach ()

            install(TARGETS ${${PROJECT_NAME}_INSTALL_PYBIND_MODULES}
                    ARCHIVE DESTINATION ${PIP_LIB_DIR}
                    LIBRARY DESTINATION ${PIP_LIB_DIR}
                    RUNTIME DESTINATION ${PIP_LIB_DIR})
        endif ()

        foreach (TARGET IN LISTS ${PROJECT_NAME}_Targets)
            message(STATUS "Generate the rule to export ${TARGET} from ${PROJECT_NAME}")
        endforeach ()

        # Do version comparison
        write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
                VERSION ${PROJECT_VERSION}
                COMPATIBILITY SameMajorVersion)

        # Generate the configuration file which CMake uses for using an installed package
        configure_package_config_file(
                "${${PROJECT_NAME}_CMAKE_DIR}/${PROJECT_NAME}Config.cmake.in"
                "${${PROJECT_NAME}_BUILD_DIR}/${PROJECT_NAME}Config.cmake"
                INSTALL_DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKE_DIR})

        # Select targets and files to install
        # Generate and Install Targets.cmake file that defines the targets offered by this project
        # ${PROJECT_NAME} target will be located in the ${PROJECT_NAME} namespace.
        # Other CMake targets can refer to it using ${PROJECT_NAME}::${PROJECT_NAME}
        install(EXPORT ${PROJECT_NAME}_Targets
                FILE ${PROJECT_NAME}Targets.cmake
                DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKE_DIR})

        # reference: https://gitlab.com/libeigen/eigen/-/blob/master/CMakeLists.txt#L662
        # to make find_package consider the directory while searching for ${PROJECT_NAME}
        export(PACKAGE ${PROJECT_NAME})

        # Install the build configuration and information about version compatibility
        install(FILES "${${PROJECT_NAME}_BUILD_DIR}/${PROJECT_NAME}Config.cmake"
                "${${PROJECT_NAME}_BUILD_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
                DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKE_DIR})
    endif ()
endmacro()

# ##################################################################################################
# erl_mark_project_found
# ##################################################################################################
macro(erl_mark_project_found _name)
    set(${_name}_FOUND
            TRUE
            CACHE BOOL "TRUE if ${_name} and all required components found on the system" FORCE)
    message(STATUS "${_name} is found")
endmacro()
