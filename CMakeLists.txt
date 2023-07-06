cmake_minimum_required(VERSION 3.20)

project(erl_common
        LANGUAGES CXX
        VERSION 0.1.0
        DESCRIPTION "erl_common is a C++ header-only module commonly used by other modules")

if (NOT COMMAND erl_project_setup)
    include(../erl_cmake/erl_project_setup.cmake)
    set(ERL_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/../erl_cmake)
    erl_project_setup()
endif ()

# resolve dependencies
erl_find_package(
        PACKAGE_NAME OpenMP
        MAC_CMD "run `brew install libomp`"
        UBUNTU_CMD "run `sudo apt install libomp-dev`"
        FIND_PACKAGE_ARGS REQUIRED)
erl_find_package(
        PACKAGE_NAME Boost
        MAC_CMD "run `brew install boost`"
        UBUNTU_CMD "run `sudo apt install libboost-all-dev`"
        FIND_PACKAGE_ARGS REQUIRED COMPONENTS program_options)

# There are some bugs in Eigen3.4.0 when EIGEN_USE_MKL_ALL is defined. We should use the latest version.
if (USE_INTEL_MKL)  # EIGEN_USE_MKL_ALL is defined in erl_cmake/erl_project_setup.cmake
    erl_find_package(
            PACKAGE_NAME Eigen3
            NO_CMD "visit https://gitlab.com/libeigen/eigen to install the required version"
            FIND_PACKAGE_ARGS 3.4.90 REQUIRED)
else ()
    erl_find_package(
            PACKAGE_NAME Eigen3
            NO_CMD "visit https://gitlab.com/libeigen/eigen to install the required version"
            FIND_PACKAGE_ARGS REQUIRED)
endif ()

erl_find_package(
        PACKAGE_NAME nlohmann_json
        MAC_CMD "run `brew install nlohmann-json`"
        UBUNTU_CMD "run `sudo apt install nlohmann-json3-dev`"
        FIND_PACKAGE_ARGS REQUIRED)

erl_find_package(
        PACKAGE_NAME OpenCV
        MAC_CMD "run scripts/install_opencv.bash"
        UBUNTU_CMD "run `sudo apt install libopencv-dev`"
        FIND_PACKAGE_ARGS REQUIRED COMPONENTS core imgproc highgui)

erl_find_package(
        PACKAGE_NAME YAML-CPP
        MAC_CMD "run scripts/install_yaml_cpp.bash"
        UBUNTU_CMD "run `sudo apt install libyaml-cpp-dev`"
        FIND_PACKAGE_ARGS REQUIRED)

erl_find_package(
        PACKAGE_NAME Matplot++
        NO_CMD "run scripts/install_matplotplusplus.bash"
        FIND_PACKAGE_ARGS REQUIRED)

#find_package(Matplot++ REQUIRED) # Matplot++::nodesoup, Matplot++::matplot, Matplot++::cimg

# define the libraries
# erl_common::erl_common for the most common headers
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_library(${PROJECT_NAME} ${SRC_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME}) # this allows namespace style, erl_common::erl_common
set(${PROJECT_NAME}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}) # CMAKE_INSTALL_INCLUDEDIR provided by GNUInstallDirs, `include` by default
set(PACKAGE_INSTALL_DIR ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PACKAGE_INSTALL_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>)

# https://eigen.tuxfamily.org/dox/TopicCMakeGuide.html
target_link_libraries(${PROJECT_NAME}
        PUBLIC ${OpenCV_LIBRARIES} ${YAML_CPP_LIBRARIES} Eigen3::Eigen
        PUBLIC yaml-cpp Matplot++::matplot nlohmann_json::nlohmann_json
        PUBLIC pthread OpenMP::OpenMP_CXX Boost::boost Boost::program_options
        PUBLIC ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
target_compile_options(${PROJECT_NAME} PUBLIC -fopenmp)

if (USE_LAPACK AND USE_INTEL_MKL)
    target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${MKL_H})
endif ()

list(APPEND TARGETS_TO_EXPORT ${PROJECT_NAME})

if (BUILD_PYTHON)
    # binding lib used to build pybind runtime lib
    file(GLOB_RECURSE SRC_FILES "binding/pybind11_*.cpp")
    add_library(${PROJECT_NAME}_pybind STATIC ${SRC_FILES})
    add_library(${PROJECT_NAME}::pybind ALIAS ${PROJECT_NAME}_pybind)
    target_include_directories(${PROJECT_NAME}_pybind SYSTEM
            PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_pybind
            PUBLIC ${PROJECT_NAME})
    list(APPEND TARGETS_TO_EXPORT ${PROJECT_NAME}_pybind)

    # pybind runtime lib
    pybind11_add_module(py${PROJECT_NAME} binding/py${PROJECT_NAME}.cpp)
    target_compile_definitions(py${PROJECT_NAME}
            PRIVATE PYBIND_MODULE_NAME=py${PROJECT_NAME})
    target_include_directories(py${PROJECT_NAME} SYSTEM
            PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(py${PROJECT_NAME}
            PRIVATE ${PROJECT_NAME}_pybind)
    set_target_properties(py${PROJECT_NAME} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python/${PROJECT_NAME})

    add_subdirectory(python)
endif ()

erl_install(TARGETS ${TARGETS_TO_EXPORT})

message(STATUS "${PROJECT_NAME} provides the following libraries:")
message(STATUS "${PROJECT_NAME}::${PROJECT_NAME}")

if (BUILD_PYTHON)
    message(STATUS "${PROJECT_NAME}::pybind")
endif ()

message("=================================================================")

# add test
if (BUILD_TEST)
    add_subdirectory(test)
endif ()

# export variables
set(${PROJECT_NAME}_FOUND
        TRUE
        CACHE INTERNAL "TRUE if @PROJECT_NAME@ and all required components found on the system" FORCE)
