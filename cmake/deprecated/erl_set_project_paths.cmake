macro(erl_set_project_paths)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    set(ERL_${PROJECT_NAME}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include
            CACHE PATH "Path to ${PROJECT_NAME} include directory" FORCE)
    set(ERL_${PROJECT_NAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
            CACHE PATH "Path to ${PROJECT_NAME} source directory" FORCE)
    set(ERL_${PROJECT_NAME}_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test
            CACHE PATH "Path to ${PROJECT_NAME} test directory" FORCE)
    set(ERL_${PROJECT_NAME}_TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/data
            CACHE PATH "Path to ${PROJECT_NAME} test data directory" FORCE)

    # /usr/local/include/PROJECT_NAME
    set(ERL_${PROJECT_NAME}_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
            CACHE PATH "Path to ${PROJECT_NAME} include directory during installation" FORCE)
    # /usr/local/lib/PROJECT_NAME
    set(ERL_${PROJECT_NAME}_INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
            CACHE PATH "Path to ${PROJECT_NAME} library directory during installation" FORCE)
    # /usr/local/bin
    set(ERL_${PROJECT_NAME}_INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR}
            CACHE PATH "Path to ${PROJECT_NAME} binary directory during installation" FORCE)
    # /usr/local/etc/PROJECT_NAME
    set(ERL_${PROJECT_NAME}_INSTALL_ETC_DIR ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
            CACHE PATH "Path to ${PROJECT_NAME} etc directory during installation" FORCE)
    # /usr/local/share/PROJECT_NAME
    set(ERL_${PROJECT_NAME}_INSTALL_SHARE_DIR ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
            CACHE PATH "Path to ${PROJECT_NAME} share directory during installation" FORCE)
    # /usr/local/share/doc/PROJECT_NAME
    set(ERL_${PROJECT_NAME}_INSTALL_DOC_DIR ${CMAKE_INSTALL_DOCDIR}/${PROJECT_NAME}
            CACHE PATH "Path to ${PROJECT_NAME} doc directory during installation" FORCE)
    # /usr/local/share/PROJECT_NAME/cmake
    set(ERL_${PROJECT_NAME}_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
            CACHE PATH "Path to ${PROJECT_NAME} cmake directory during installation" FORCE)

#    message(WARNING "CMAKE_INSTALL_DATAROOTDIR: ${CMAKE_INSTALL_DATAROOTDIR}")
#    message(WARNING "CATKIN_DEVEL_PREFIX: ${CATKIN_DEVEL_PREFIX}")
#    message(WARNING "CATKIN_GLOBAL_PYTHON_DESTINATION: ${CATKIN_GLOBAL_PYTHON_DESTINATION}")
#    message(WARNING "CATKIN_GLOBAL_SHARE_DESTINATION: ${CATKIN_GLOBAL_SHARE_DESTINATION}")
#    message(WARNING "CATKIN_PACKAGE_SHARE_DESTINATION: ${CATKIN_PACKAGE_SHARE_DESTINATION}")
endmacro()
