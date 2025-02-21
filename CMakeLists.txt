cmake_minimum_required(VERSION 3.16)

project(erl_common
        LANGUAGES CXX
        VERSION 0.1.0
        DESCRIPTION "erl_common is a C++ module commonly used by other modules")
message(STATUS "Configuring ${PROJECT_NAME} ${PROJECT_VERSION}")

if (NOT COMMAND erl_project_setup)
    include(cmake/erl_common_tools.cmake)
endif ()
erl_project_setup(
        ${PROJECT_NAME}
        CATKIN_COMPONENTS roscpp
)
erl_setup_ros()
erl_catkin_package(
        INCLUDE_DIRS include
        LIBRARIES ${PROJECT_NAME} py${PROJECT_NAME}
        CATKIN_DEPENDS roscpp
        DEPENDS ${${PROJECT_NAME}_DEPENDS}  # non-catkin dependencies are collected via erl_find_package
        CFG_EXTRAS erl_common_tools.cmake)
if (ERL_USE_TRACY)
    set(BUILD_SHARED_LIBS ON)
    file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/_deps/imgui-src)  # delete folder
    file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/_deps/imgui-subbuild)  # delete folder
    add_subdirectory(deps/tracy SYSTEM)
    set(LEGACY ON)  # use X11 for Tracy
    add_subdirectory(deps/tracy/profiler SYSTEM)
    target_compile_options(TracyClient PRIVATE -w)  # disable warnings for Tracy
    target_compile_options(TracyServer PRIVATE -w)  # disable warnings for Tracy
    target_compile_options(tracy-profiler PRIVATE -w)  # disable warnings for Tracy
    erl_find_package(
            PACKAGE TBB
            REQUIRED
            COMMANDS UBUNTU_LINUX "try `sudo apt install libtbb-dev`"
            COMMANDS ARCH_LINUX "try `sudo pacman -S onetbb`")
    target_link_libraries(tracy-profiler PRIVATE TBB::tbb)
    link_libraries(Tracy::TracyClient)  # link Tracy to all targets
endif ()

#######################################################################################################################
# BUILD
#######################################################################################################################

########################
# define the libraries #
########################
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}) # this allows namespace style, erl_common::erl_common
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${${PROJECT_NAME}_INSTALL_INCLUDE_DIR}>)
target_include_directories(${PROJECT_NAME} SYSTEM
        PUBLIC ${MKL_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS})
# https://eigen.tuxfamily.org/dox/TopicCMakeGuide.html
target_link_libraries(${PROJECT_NAME}
        PUBLIC fmt::fmt
        PUBLIC ${OpenCV_LIBRARIES}
        PUBLIC ${Pangolin_LIBRARIES}
        PUBLIC yaml-cpp nlohmann_json::nlohmann_json
        PUBLIC Boost::boost Boost::program_options Boost::graph
        PUBLIC pthread OpenMP::OpenMP_CXX
        PUBLIC Eigen3::Eigen ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES}
        PUBLIC ${catkin_LIBRARIES})

###################
# Python bindings #
###################
erl_add_pybind_module(
        PYBIND_MODULE_NAME py${PROJECT_NAME}
        PYBIND_SRC_DIR ${${PROJECT_NAME}_PYTHON_BINDING_DIR}
        PYTHON_PKG_DIR ${${PROJECT_NAME}_PYTHON_DIR}/${PROJECT_NAME}
        LIBRARIES ${PROJECT_NAME}
)
erl_add_python_package(
        PYTHON_PKG_DIR ${${PROJECT_NAME}_PYTHON_DIR}/${PROJECT_NAME}
)

#######################################################################################################################
# INSTALL
#######################################################################################################################
erl_install(
        LIBRARIES ${PROJECT_NAME}
        PYBIND_MODULES py${PROJECT_NAME})

#######################################################################################################################
# TEST
#######################################################################################################################
erl_add_tests(
        LIBRARIES ${PROJECT_NAME}
)

#######################################################################################################################
# done
#######################################################################################################################
erl_mark_project_found(${PROJECT_NAME})
