cmake_minimum_required(VERSION 3.24)

project(erl_common
        LANGUAGES CXX
        VERSION 0.1.0
        DESCRIPTION "erl_common is a C++ module commonly used by other modules")
message(STATUS "Configuring ${PROJECT_NAME} ${PROJECT_VERSION}")

if (NOT COMMAND erl_project_setup)
    include(cmake/erl_common_tools.cmake)
endif ()
erl_project_setup(${PROJECT_NAME})
erl_setup_ros(CATKIN_COMPONENTS roscpp)
erl_catkin_package(
        INCLUDE_DIRS include
        LIBRARIES ${PROJECT_NAME} py${PROJECT_NAME}
        CATKIN_DEPENDS roscpp
        DEPENDS ${${PROJECT_NAME}_DEPENDS}  # non-catkin dependencies are collected via erl_find_package
        CFG_EXTRAS erl_common_tools.cmake)

#######################################################################################################################
# BUILD
#######################################################################################################################

########################
# define the libraries #
########################
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_library(${PROJECT_NAME} ${SRC_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}) # this allows namespace style, erl_common::erl_common
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${${PROJECT_NAME}_INSTALL_INCLUDE_DIR}>)
target_include_directories(${PROJECT_NAME} SYSTEM
        PUBLIC ${MKL_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS})
# https://eigen.tuxfamily.org/dox/TopicCMakeGuide.html
target_link_libraries(${PROJECT_NAME}
        PUBLIC ${OpenCV_LIBRARIES} libdeflate::libdeflate_shared
        PUBLIC Eigen3::Eigen yaml-cpp nlohmann_json::nlohmann_json Boost::program_options Boost::graph
        PUBLIC pthread OpenMP::OpenMP_CXX Boost::boost
        PUBLIC ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES}
        PUBLIC ${catkin_LIBRARIES})

###################
# Python bindings #
###################
erl_add_python_package(
        PYTHON_PKG_DIR ${${PROJECT_NAME}_PYTHON_DIR}/${PROJECT_NAME}
        PYBIND_MODULE_NAME py${PROJECT_NAME})

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
