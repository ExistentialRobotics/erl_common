cmake_minimum_required(VERSION 3.16)

project(erl_common #
        LANGUAGES CXX #
        VERSION 0.1.0 #
        DESCRIPTION "erl_common is a C++ module commonly used by other modules")
message(STATUS "Configuring ${PROJECT_NAME} ${PROJECT_VERSION}")

if (NOT COMMAND erl_project_setup)
    find_package(erl_cmake_tools REQUIRED)
endif ()
erl_project_setup()
include(cmake/erl_common_dependencies.cmake)
erl_setup_ros(
    CATKIN_COMPONENTS roscpp #
    CATKIN_DEPENDS roscpp #
    LIBRARIES ${PROJECT_NAME} #
    CFG_EXTRAS erl_common_dependencies.cmake #
    ROS2_COMPONENTS rclcpp)

# ######################################################################################################################
# BUILD
# ######################################################################################################################

# ######################################################################################################################
# define the libraries
# ######################################################################################################################
file(GLOB SRC_FILES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}) # this allows namespace style erl_common::erl_common
erl_collect_targets(LIBRARIES ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIR}>
                                                  $<INSTALL_INTERFACE:${${PROJECT_NAME}_INSTALL_INCLUDE_DIR}>)
erl_target_dependencies(
    ${PROJECT_NAME} PUBLIC #
    fmt::fmt yaml-cpp nlohmann_json::nlohmann_json Boost::boost Boost::program_options Boost::graph pthread
    OpenMP::OpenMP_CXX Eigen3::Eigen ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${OpenCV_LIBRARIES})
if (ERL_USE_INTEL_MKL)
    target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${MKL_INCLUDE_DIRS})
endif ()
if (Pangolin_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${Pangolin_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DERL_USE_PANGOLIN)
endif ()
if (plplot_FOUND)
    target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${cairomm-1.0_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC PLPLOT::plplot PLPLOT::plplotcxx ${cairomm-1.0_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DERL_USE_PLPLOT)
endif ()

# ######################################################################################################################
# Python bindings #
# ######################################################################################################################
erl_add_pybind_module(
    PYBIND_MODULE_NAME py${PROJECT_NAME} #
    PYBIND_SRC_DIR ${${PROJECT_NAME}_PYTHON_BINDING_DIR} #
    LIBRARIES ${PROJECT_NAME})
erl_add_python_package()

# ######################################################################################################################
# INSTALL
# ######################################################################################################################
erl_install(
    EXECUTABLES ${${PROJECT_NAME}_COLLECTED_EXECUTABLES} #
    LIBRARIES ${${PROJECT_NAME}_COLLECTED_LIBRARIES} #
    PYBIND_MODULES py${PROJECT_NAME})

# ######################################################################################################################
# TEST
# ######################################################################################################################
erl_add_tests(LIBRARIES ${PROJECT_NAME})

# ######################################################################################################################
# done
# ######################################################################################################################
erl_mark_project_found()
