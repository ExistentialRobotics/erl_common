macro(erl_install)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs LIBRARIES EXECUTABLES CATKIN_PYTHON_PROGRAMS OTHER_FILES)
    cmake_parse_arguments(ERL_INSTALL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    unset(options)
    unset(oneValueArgs)
    unset(multiValueArgs)

    # Create a package to distribute the software
    # reference 1: https://dominikberner.ch/cmake-interface-lib/ (not complete)
    # reference 2: https://gitlab.com/libeigen/eigen/-/blob/master/CMakeLists.txt
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    foreach (TARGET IN LISTS TARGETS_TO_EXPORT)
        message(STATUS "Exporting ${TARGET} from ${PROJECT_NAME}")
    endforeach ()

    # ROS Support
    if (BUILD_FOR_ROS1)
        # Mark executable scripts (Python etc.) for installation in contrast to setup.py
        if (DEFINED ERL_INSTALL_CATKIN_PYTHON_PROGRAMS)
            catkin_install_python(PROGRAMS
                    ${ERL_INSTALL_CATKIN_PYTHON_PROGRAMS}
                    DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
        endif ()

        if (DEFINED ERL_INSTALL_EXECUTABLES)
            # Mark executables for installation
            # See http://docs.ros.org/melodic/api/catkin/html/howto/format1/building_executables.html
            install(TARGETS ${ERL_INSTALL_EXECUTABLES}
                    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
        endif ()

        if (DEFINED ERL_INSTALL_LIBRARIES)
            # Mark libraries for installation
            # See http://docs.ros.org/melodic/api/catkin/html/howto/format1/building_libraries.html
            install(TARGETS ${ERL_INSTALL_LIBRARIES}
                    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
                    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
                    RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
        endif ()

        # Install the header files
        # install to ${CATKIN_PACKAGE_INCLUDE_DESTINATION}=${CATKIN_GLOBAL_INCLUDE_DESTINATION}/${PROJECT_NAME}
        set(SRC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME})
        if (EXISTS ${SRC_INCLUDE_DIR})
            install(DIRECTORY ${SRC_INCLUDE_DIR}
                    DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION})  # ${PROJECT_NAME} is added automatically
        endif ()
        unset(SRC_INCLUDE_DIR)

        # Install other files
        if (DEFINED ERL_INSTALL_OTHER_FILES)
            install(FILES ${ERL_INSTALL_OTHER_FILES}
                    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
        endif ()

        # Config file generation is taken care of by catkin_package()

        if (${${PROJECT_NAME}_CATKIN_PYTHON_SETUP} AND DEFINED ${PROJECT_NAME}_PY_PACKAGE_DIR)
            # install files generated from files in python/${PROJECT_NAME}
            install(DIRECTORY ${${PROJECT_NAME}_PY_PACKAGE_DIR}
                    DESTINATION ${CATKIN_GLOBAL_PYTHON_DESTINATION})
        endif ()

        # Install cmake files, only for erl_common
        if (PROJECT_NAME STREQUAL "erl_common")
            file(GLOB CMAKE_FILES ${ERL_CMAKE_DIR}/*.cmake)
            install(FILES ${CMAKE_FILES}
                    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/cmake/erl_cmake)
        endif ()
    elseif (BUILD_FOR_ROS2)
        message(FATAL_ERROR "Not implemented yet")
    else ()
        if (DEFINED ERL_INSTALL_EXECUTABLES)
            # Export targets defined in this file and define the installation location
            install(TARGETS ${ERL_INSTALL_EXECUTABLES}  # add the targets here
                    RUNTIME DESTINATION ${ERL_${PROJECT_NAME}_INSTALL_BIN_DIR})
        endif ()

        if (DEFINED ERL_INSTALL_LIBRARIES)
            # Export targets defined in this file and define the installation location
            install(TARGETS ${ERL_INSTALL_LIBRARIES}  # add the targets here
                    EXPORT ${PROJECT_NAME}_Targets
                    ARCHIVE DESTINATION ${ERL_${PROJECT_NAME}_INSTALL_LIB_DIR}
                    LIBRARY DESTINATION ${ERL_${PROJECT_NAME}_INSTALL_LIB_DIR}
                    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # provided by GNUInstallDirs, `bin` by default
        endif ()

        # Install the header files
        # install to ${INSTALL_INCLUDE_DIR}=${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        set(SRC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME})
        if (EXISTS ${SRC_INCLUDE_DIR})
            install(DIRECTORY ${SRC_INCLUDE_DIR}
                    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})  # ${PROJECT_NAME} is added automatically
        endif ()
        unset(SRC_INCLUDE_DIR)

        # Install other files
        if (DEFINED ERL_INSTALL_OTHER_FILES)
            install(FILES ${ERL_INSTALL_OTHER_FILES}
                    DESTINATION ${ERL_${PROJECT_NAME}_INSTALL_SHARE_DIR})
        endif ()

        # Do version comparison
        write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
                VERSION ${PROJECT_VERSION}
                COMPATIBILITY SameMajorVersion)
        # Generate the configuration file which CMake uses for using an installed package
        configure_package_config_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                INSTALL_DESTINATION ${ERL_${PROJECT_NAME}_INSTALL_CMAKE_DIR})

        # Select targets and files to install
        # Generate and Install Targets.cmake file that defines the targets offered by this project
        # ${PROJECT_NAME} target will be located in the ${PROJECT_NAME} namespace.
        # Other CMake targets can refer to it using ${PROJECT_NAME}::${PROJECT_NAME}
        install(EXPORT ${PROJECT_NAME}_Targets
                FILE ${PROJECT_NAME}Targets.cmake
                NAMESPACE ${PROJECT_NAME}::
                DESTINATION ${ERL_${PROJECT_NAME}_INSTALL_CMAKE_DIR})

        # reference: https://gitlab.com/libeigen/eigen/-/blob/master/CMakeLists.txt#L662
        # to make find_package consider the directory while searching for ${PROJECT_NAME}
        export(PACKAGE ${PROJECT_NAME})
        # Install the build configuration and information about version compatibility
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                      "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
                DESTINATION ${ERL_${PROJECT_NAME}_INSTALL_CMAKE_DIR})

        # Install cmake files, only for erl_common
        if (PROJECT_NAME STREQUAL "erl_common")
            file(GLOB CMAKE_FILES ${ERL_CMAKE_DIR}/*.cmake)
            install(FILES ${CMAKE_FILES}
                    DESTINATION ${ERL_${PROJECT_NAME}_INSTALL_CMAKE_DIR}/erl_cmake)
        endif ()

        # # Install .py and .pyi files
        # if (BUILD_PYTHON AND DEFINED ${PROJECT_NAME}_PY_PACKAGE_DIR)
        #     install(DIRECTORY ${${PROJECT_NAME}_PY_PACKAGE_DIR}
        #             DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/python)
        #     install(DIRECTORY ${${PROJECT_NAME}_PY_MODULE_DIRS}
        #             DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/python)
        # endif ()
    endif ()

    unset(ERL_INSTALL_LIBRARIES)
    unset(ERL_INSTALL_EXECUTABLES)
    unset(ERL_INSTALL_PYTHON_PROGRAMS)
    unset(ERL_INSTALL_OTHER_FILES)
endmacro()
