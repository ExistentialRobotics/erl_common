message(WARNING "erl_project_setup at: ${CMAKE_CURRENT_LIST_DIR}")

macro(erl_project_setup)
    message(WARNING "erl_project_setup at: ${CMAKE_CURRENT_LIST_DIR}")

    ###################################################################################################################
    # Load CMake functions and macros
    ###################################################################################################################
    if (NOT ERL_CMAKE_DIR)
        message(FATAL_ERROR "Please set ERL_CMAKE_DIR before calling erl_project_setup")
    endif ()

    if (NOT ERL_PROJECT_SETUP_DONE)
        set(ERL_PROJECT_SETUP_DONE ON CACHE INTERNAL "Whether erl_project_setup has been called" FORCE)
        set(ERL_CMAKE_DIR ${ERL_CMAKE_DIR}
                CACHE INTERNAL "The directory containing CMake modules for the project." FORCE)
    endif ()
    list(APPEND CMAKE_MODULE_PATH ${ERL_CMAKE_DIR})

#    include(erl_add_tests)
#    include(erl_before_find_package)
#    include(erl_check_file)
#    include(erl_dump_cmake_vars)
#    include(erl_find_package)
    include(erl_install)
#    include(erl_os_release_info)
#    include(erl_set_project_paths)
#    include(erl_suggest_cmd)
    include(FetchContent)

    ###################################################################################################################
    # set options
    ###################################################################################################################
    option(USE_LAPACK "Use LAPACK" ON)
    option(USE_INTEL_MKL "Use Intel MKL (Math Kernel Library)" ON)
    option(USE_AOCL "Use AMD Optimizing CPU Library" OFF)
    option(USE_SINGLE_THREADED_BLAS "Use single-threaded BLAS" ON)
    option(BUILD_TEST "Build executables for test" ON)
    option(BUILD_PYTHON "Build Python binding" ON)
    option(BUILD_FOR_ROS1 "Build for ROS1" OFF)
    option(BUILD_FOR_ROS2 "Build for ROS2" OFF)

    ###################################################################################################################
    # set build type, compiler flags, and ccache
    ###################################################################################################################
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release")
    endif ()
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -flto=auto")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mavx2 -mfma -march=core-avx2")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)
    find_program(CCACHE_FOUND ccache)
    if (CCACHE_FOUND)
        set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
    else ()
        message(STATUS "ccache is not found, execute `sudo apt install ccache` for faster compiling")
    endif ()
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DNDEBUG)
    endif ()

    ###################################################################################################################
    # ROS
    ###################################################################################################################
    if (DEFINED CATKIN_DEVEL_PREFIX)
        message(STATUS "CATKIN_DEVEL_PREFIX is defined, so we are building for ROS1")
        set(BUILD_FOR_ROS1 ON)
    endif ()
    if (BUILD_FOR_ROS1 AND BUILD_FOR_ROS2)
        message(FATAL_ERROR "BUILD_FOR_ROS1 and BUILD_FOR_ROS2 cannot be both ON")
    endif ()
    if (BUILD_FOR_ROS1)
        include(erl_catkin_setup)
    endif ()

    ###################################################################################################################
    # enable test or not
    ###################################################################################################################
    if (BUILD_TEST)
        add_definitions(-DBUILD_TEST)
        if (NOT BUILD_FOR_ROS1)  # GTest is configured by find_package(catkin)
            erl_find_package(
                    PACKAGE_NAME GTest
                    MAC_CMD "run scripts/install_google_test.bash"
                    UBUNTU_CMD "run `sudo apt install libgtest-dev`"
                    ARCH_CMD "run `sudo pacman -S gtest`"
                    FIND_PACKAGE_ARGS REQUIRED GLOBAL)
            enable_testing()
            include(GoogleTest)
        endif ()
    endif ()

    ###################################################################################################################
    # lapack
    ###################################################################################################################
    if (USE_INTEL_MKL AND USE_AOCL)
        message(FATAL_ERROR "USE_INTEL_MKL and USE_AOCL cannot be both ON")
    endif ()

    if (USE_INTEL_MKL OR USE_AOCL)
        set(USE_LAPACK ON)
    endif ()

    if (USE_LAPACK)
        erl_check_file(
                MAC_PATH "/usr/local/Cellar/lapack/3.11/include/lapacke.h"
                LINUX_PATH "/usr/include/lapacke.h"
                PACKAGE_NAME lapacke
                MAC_CMD "run `brew install lapack`"
                UBUNTU_CMD "run `sudo apt install liblapacke-dev`"
                ASSERT_FOUND)

        if (USE_INTEL_MKL)
            message(STATUS "Use Intel Math Kernel Library")
            add_definitions(-DEIGEN_USE_MKL_ALL)
            if (USE_SINGLE_THREADED_BLAS)
                # we use MKL inside our OpenMP for loop or threaded code, so we need sequential BLAS
                set(BLA_VENDOR Intel10_64lp_seq)
            else ()
                # MKL is used outside OpenMP for loop or threaded code, so we can use threaded BLAS
                set(BLA_VENDOR Intel10_64lp)
            endif ()
            if (NOT DEFINED $ENV{MKLROOT})
                erl_check_file(
                        MAC_PATH "/opt/intel/oneapi/mkl/latest/include/mkl.h"
                        LINUX_PATH "/opt/intel/oneapi/mkl/latest/include/mkl.h"
                        PACKAGE_NAME intel-oneapi-mkl-devel
                        NO_CMD "visit https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl-download.html"
                        ASSERT_FOUND)
                set(ENV{MKLROOT} /opt/intel/oneapi/mkl/latest)
                set(MKL_DIR /opt/intel/oneapi/mkl/latest/lib/cmake/mkl)
                message(STATUS "Set MKLROOT to $ENV{MKLROOT}")
            endif ()
            erl_find_package(   # We need to find MKL to get MKL_H
                    PACKAGE_NAME MKL
                    NO_CMD "visit https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl-download.html"
                    FIND_PACKAGE_ARGS CONFIG REQUIRED
                    NO_RECORD)  # MKL_LIBRARIES contains library names instead of full path, so we cannot use it
            erl_find_package(   # LAPACK will resolve the full paths of MKL libraries
                    PACKAGE_NAME LAPACK
                    MAC_CMD "run `brew install lapack`"
                    UBUNTU_CMD "run `sudo apt install liblapack-dev`"
                    ARCH_CMD "run `sudo pacman -S lapack`"
                    FIND_PACKAGE_ARGS REQUIRED)
        elseif (USE_AOCL)  # AMD Optimizing CPU Library, not supported by CMake yet
            message(STATUS "Use AMD Optimizing CPU Library")
            add_definitions(-DEIGEN_USE_BLAS)
            add_definitions(-DEIGEN_USE_LAPACKE)
            set(BLA_VENDOR AOCL)
            erl_check_file(
                    LINUX_PATH "/opt/AMD/aocl/*/lib/libblis.so"
                    PACKAGE_NAME AMD-AOCL
                    NO_CMD "visit https://www.amd.com/en/developer/aocl.html"
                    ASSERT_FOUND
            )
            file(GLOB AOCL_ROOT /opt/AMD/aocl/aocl*/README)
            get_filename_component(AOCL_ROOT ${AOCL_ROOT} DIRECTORY)
            if (USE_SINGLE_THREADED_BLAS)
                set(BLAS_LIBRARIES ${AOCL_ROOT}/lib/libblis.so ${AOCL_ROOT}/lib/libalm.so -lm)  # single-threaded BLAS
            else ()
                set(BLAS_LIBRARIES ${AOCL_ROOT}/lib/libblis-mt.so ${AOCL_ROOT}/lib/libalm.so -lm)  # multi-threaded BLAS
            endif ()
            set(LAPACK_LIBRARIES ${AOCL_ROOT}/lib/libflame.so)
        else ()
            message(STATUS "Use OpenBLAS")
            add_definitions(-DEIGEN_USE_BLAS)
            add_definitions(-DEIGEN_USE_LAPACKE)
            set(BLA_VENDOR OpenBLAS)
            if (USE_SINGLE_THREADED_BLAS)
                erl_check_file(
                        MAC_PATH "/opt/OpenBLAS/lib/libopenblas.so"
                        LINUX_PATH "/opt/OpenBLAS/lib/libopenblas.so"
                        PACKAGE_NAME openblas
                        NO_CMD "Run `bash scripts/install_openblas_seq.bash` for Ubuntu"
                        ASSERT_FOUND
                )
            endif ()
            erl_find_package(
                    PACKAGE_NAME LAPACK
                    MAC_CMD "run `brew install lapack`"
                    UBUNTU_CMD "run `sudo apt install liblapack-dev`"
                    ARCH_CMD "run `sudo pacman -S lapack`"
                    FIND_PACKAGE_ARGS REQUIRED
            )
        endif ()
        message(STATUS "BLAS_LIBRARIES: ${BLAS_LIBRARIES}")
        message(STATUS "LAPACK_LIBRARIES: ${LAPACK_LIBRARIES}")
    else ()
        set(BLAS_LIBRARIES "")
        set(LAPACK_LIBRARIES "")
    endif ()

    ###################################################################################################################
    # Find other packages
    ###################################################################################################################
    erl_find_package(
            PACKAGE_NAME OpenMP
            MAC_CMD "run `brew install libomp`"
            UBUNTU_CMD "run `sudo apt install libomp-dev`"
            ARCH_CMD "run `sudo pacman -S openmp`"
            FIND_PACKAGE_ARGS REQUIRED)
    if (BUILD_FOR_ROS1)
        set(OpenMP_INCLUDE_DIRS /usr/include)
        get_target_property(OpenMP_LIBRARIES OpenMP::OpenMP_CXX INTERFACE_LINK_LIBRARIES)
    endif ()

    erl_find_package(
            PACKAGE_NAME Boost
            MAC_CMD "run `brew install boost`"
            UBUNTU_CMD "run `sudo apt install libboost-all-dev`"
            ARCH_CMD "run `sudo pacman -S boost`"
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
    if (BUILD_FOR_ROS1)
        get_target_property(Eigen3_INCLUDE_DIRS Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
    endif ()
    set_target_properties(Eigen3::Eigen PROPERTIES SYSTEM ON)

    erl_find_package(
            PACKAGE_NAME nlohmann_json
            MAC_CMD "run `brew install nlohmann-json`"
            UBUNTU_CMD "run `sudo apt install nlohmann-json3-dev`"
            ARCH_CMD "run `sudo pacman -S nlohmann-json`"
            FIND_PACKAGE_ARGS REQUIRED)
    if (BUILD_FOR_ROS1)
        get_target_property(nlohmann_json_INCLUDE_DIRS nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
    endif ()

    erl_find_package(
            PACKAGE_NAME OpenCV
            MAC_CMD "run scripts/install_opencv.bash"
            UBUNTU_CMD "run `sudo apt install libopencv-dev`"
            ARCH_CMD "run `sudo pacman -S opencv`"
            FIND_PACKAGE_ARGS REQUIRED COMPONENTS core imgproc highgui)

    erl_find_package(
            PACKAGE_NAME PCL
            UBUNTU_CMD "run `sudo apt install libpcl-dev`"
            ARCH_CMD "run `sudo pacman -S pcl`"
            FIND_PACKAGE_ARGS REQUIRED)

    erl_find_package(
            PACKAGE_NAME nanoflann
            WIN_CMD "run `vcpkg install nanoflann`"
            MAC_CMD "run `brew install nanoflann`"
            UBUNTU_CMD "run `sudo apt install libnanoflann-dev`"
            ARCH_CMD "run `paru -S nanoflann`"
            FIND_PACKAGE_ARGS REQUIRED)
    if (BUILD_FOR_ROS1)
        get_target_property(nanoflann_INCLUDE_DIRS nanoflann::nanoflann INTERFACE_INCLUDE_DIRECTORIES)
    endif ()

    erl_find_package(
            PACKAGE_NAME yaml-cpp
            MAC_CMD "run scripts/install_yaml_cpp.bash"
            UBUNTU_CMD "run `sudo apt install libyaml-cpp-dev`"
            ARCH_CMD "run `sudo pacman -S yaml-cpp`"
            FIND_PACKAGE_ARGS REQUIRED)
    if (BUILD_FOR_ROS1)
        set(yaml-cpp_INCLUDE_DIRS ${YAML_CPP_INCLUDE_DIR} CACHE INTERNAL "yaml-cpp include directories" FORCE)
        set(yaml-cpp_LIBRARIES ${YAML_CPP_LIBRARIES} CACHE INTERNAL "yaml-cpp libraries" FORCE)
    endif ()

    erl_find_package(
            PACKAGE_NAME Matplot++
            NO_CMD "run scripts/install_matplotplusplus.bash"
            FIND_PACKAGE_ARGS REQUIRED) # Matplot++::nodesoup, Matplot++::matplot, Matplot++::cimg
    get_target_property(Matplot++_INCLUDE_DIRS Matplot++::matplot INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(Matplot++_LIBRARIES Matplot++::matplot LOCATION)

    ###################################################################################################################
    # enable support to build python
    ###################################################################################################################
    if (BUILD_PYTHON)
        include(erl_generate_python_package)
        message(STATUS "To specify python interpreter, run `cmake -DPython3_ROOT_DIR=/path/to/python3_bin_folder ..`")
        erl_find_package(
                PACKAGE_NAME Python3
                MAC_CMD "run `brew install python3`"
                UBUNTU_CMD "run `sudo apt install python3-dev`"
                ARCH_CMD "run `sudo pacman -S python`"
                FIND_PACKAGE_ARGS REQUIRED COMPONENTS Interpreter Development NO_POLICY_SCOPE)
        set_target_properties(Python3::Python PROPERTIES SYSTEM ON)
        erl_find_package(
                PACKAGE_NAME pybind11
                MAC_CMD "run `brew install pybind11`"
                UBUNTU_CMD "run `sudo apt install pybind11-dev`"
                ARCH_CMD "run `sudo pacman -S pybind11`"
                FIND_PACKAGE_ARGS REQUIRED)  # installed to /usr/local/include/pybind11 or /usr/include/pybind11
        foreach (item IN ITEMS python_link_helper python_headers headers module embed windows_extras thin_lto lto opt_size)
            set_target_properties(pybind11::${item} PROPERTIES SYSTEM ON)
        endforeach ()
    endif ()

endmacro()
