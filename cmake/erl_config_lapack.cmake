macro(erl_config_lapack)
    option(ERL_USE_LAPACK "Use LAPACK" ON)
    option(ERL_USE_LAPACK_STRICT "Use robust LAPACK algorithms only" OFF)
    option(ERL_USE_INTEL_MKL "Use Intel MKL (Math Kernel Library)" ON)
    option(ERL_USE_AOCL "Use AMD Optimizing CPU Library" OFF)
    option(ERL_USE_SINGLE_THREADED_BLAS "Use single-threaded BLAS" ON)

    if (ERL_USE_INTEL_MKL AND ERL_USE_AOCL)
        message(FATAL_ERROR "ERL_USE_INTEL_MKL and ERL_USE_AOCL cannot be both ON")
    endif ()

    if (ERL_USE_INTEL_MKL OR ERL_USE_AOCL)
        set(ERL_USE_LAPACK ON)
    endif ()

    if (ERL_USE_LAPACK)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(ERL_USE_LAPACK_STRICT ON)
        endif ()

        erl_find_path(
                OUTPUT LAPACKE_INCLUDE_DIR
                PACKAGE LAPACKE
                REQUIRED
                NAMES lapacke.h
                PATHS /usr/include /usr/local/include /usr/local/Cellar/lapack/*/include
                COMMANDS UBUNTU_LINUX "try `sudo apt install liblapacke-dev`"
                COMMANDS ARCH_LINUX "try `sudo pacman -S lapacke`")

        if (ERL_USE_INTEL_MKL)
            # enable vectorization of Eigen, borrow from
            # https://github.com/dev-cafe/cmake-cookbook/tree/v1.0/chapter-02/recipe-06
            include(CheckCXXCompilerFlag)
            # check -march=native -xHost -mavx -mavx2 -mfma -mfma4
            unset(_CXX_FLAGS)
            foreach (_flag IN ITEMS "-march=native" "-xHost" "-mavx" "-mavx2" "-mfma" "-mfma4")
                string(REPLACE "=" "_" _flag_works ${_flag})
                string(REPLACE "-" "_" _flag_works ${_flag_works})
                check_cxx_compiler_flag("${_flag}" ${_flag_works})

                if (${_flag_work})
                    message(STATUS "Use ${_flag} for Release build")
                    set(_CXX_FLAGS "${_CXX_FLAGS} ${_flag}")
                endif ()
            endforeach ()

            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${_CXX_FLAGS}")
            unset(_CXX_FLAGS)

            message(STATUS "Use Intel Math Kernel Library")

            if (ERL_USE_LAPACK_STRICT)
                # We don't turn on some unstable MKL routines:
                # https://eigen.tuxfamily.org/dox/TopicUsingIntelMKL.html
                add_definitions(-DEIGEN_USE_BLAS)
                add_definitions(-DEIGEN_USE_LAPACKE_STRICT)
                add_definitions(-DEIGEN_USE_MKL_VML)
            else ()
                add_definitions(-DEIGEN_USE_MKL_ALL)
            endif ()

            if (ERL_USE_SINGLE_THREADED_BLAS)
                # we use MKL inside our OpenMP for loop or threaded code, so we need sequential BLAS
                set(BLA_VENDOR Intel10_64lp_seq)
                set(MKL_THREADING "sequential")
            else ()
                # MKL is used outside OpenMP for loop or threaded code, so we can use threaded BLAS
                set(BLA_VENDOR Intel10_64lp)
                set(MKL_THREADING "intel_thread")
            endif ()

            if (NOT TARGET MKL::mkl_core)  # suppose MKL is not configured yet.
                if (NOT DEFINED MKL_ROOT)
                    unset(MKL_INCLUDE_DIRS CACHE)
                    erl_find_path(
                            OUTPUT MKL_INCLUDE_DIRS
                            PACKAGE MKL
                            mkl.h
                            PATHS /usr/include /usr/include/mkl /usr/local/include
                            /usr/local/include/mkl /opt/intel/oneapi/mkl/*/include
                            REQUIRED
                            COMMANDS ARCH_LINUX "try `sudo pacman -S intel-oneapi-basekit`"
                            COMMANDS GENERAL "visit https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html")
                    get_filename_component(MKL_ROOT ${MKL_INCLUDE_DIRS} DIRECTORY)
                    get_filename_component(MKL_ROOT ${MKL_ROOT} REALPATH)
                endif ()
                message(STATUS "MKL_ROOT is set to ${MKL_ROOT}")

                if (NOT DEFINED IOMP_ROOT)
                    erl_find_path(
                            OUTPUT IOMP_ROOT
                            libiomp5.so
                            REQUIRED
                            PATHS /usr/lib /usr/lib/x86_64-linux-gnu
                            /usr/local/lib /opt/intel/oneapi/compiler/*/lib)
                    get_filename_component(IOMP_ROOT ${IOMP_ROOT} DIRECTORY)
                    get_filename_component(IOMP_ROOT ${IOMP_ROOT} REALPATH)
                endif ()
                message(STATUS "IOMP_ROOT is set to ${IOMP_ROOT}")

                set(MKL_DIR ${MKL_ROOT}/lib/cmake/mkl)
                if (NOT EXISTS ${MKL_DIR}/MKLConfig.cmake)
                    set(MKL_DIR ${ERL_CMAKE_DIR}/mkl)
                    message(STATUS "MKLConfig.cmake is not found, using local MKLConfig.cmake")
                endif ()
                set(MKL_ARCH "intel64")
                set(MKL_LINK "dynamic")
                # 32-bit integer indexing, for 64-bit integer indexing, use "intel_ilp64"
                set(MKL_INTERFACE "lp64")
                message(STATUS "MKL_DIR is set to ${MKL_DIR}")
                erl_find_package( # We need to find MKL to get MKL_H
                        PACKAGE MKL
                        REQUIRED
                        COMMANDS ARCH_LINUX "try `sudo pacman -S intel-oneapi-basekit`"
                        COMMANDS GENERAL "visit https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html")
                # some packages depending on MKL may use MKL_LIBRARIES, which is set by MKLConfig.cmake
                # MKL_LIBRARIES is a list of library names, not full paths,
                # which will not work correctly without MKL_LIBRARY_DIR
                if (NOT DEFINED MKL_LIBRARY_DIR)
                    get_filename_component(mkl_dir ${MKL_INCLUDE_DIRS} DIRECTORY)
                    set(MKL_LIBRARY_DIR ${mkl_dir}/lib CACHE PATH "Path to MKL library directory" FORCE)
                    unset(mkl_dir)  # remove temporary variable
                endif ()
                # MKL_LIBRARIES contains library names instead of full path.
                # change MKL_LIBRARIES to full paths
                set(_mkl_libs)
                foreach (lib IN LISTS MKL_LIBRARIES)
                    set(lib "lib${lib}${CMAKE_SHARED_LIBRARY_SUFFIX}")
                    list(APPEND _mkl_libs ${MKL_LIBRARY_DIR}/${lib})
                endforeach ()
                set(MKL_LIBRARIES ${_mkl_libs})
                unset(_mkl_libs)
            endif ()

            list(APPEND BLAS_mkl_MKLROOT ${MKL_ROOT})
            list(APPEND BLAS_mkl_MKLROOT ${IOMP_ROOT})
            erl_find_package( # LAPACK will resolve the full paths of MKL libraries
                    PACKAGE LAPACK
                    REQUIRED
                    COMMANDS APPLE "try `brew install lapack`"
                    COMMANDS UBUNTU_LINUX "try `sudo apt install liblapack-dev`"
                    COMMANDS ARCH_LINUX "try `sudo pacman -S lapack`")

            # set MKL_INCLUDE_DIRS to please catkin
            if (MKL_H)
                set(MKL_INCLUDE_DIRS ${MKL_H} CACHE PATH "Path to MKL include directory" FORCE)
            elseif (MKL_INCLUDE)
                set(MKL_INCLUDE_DIRS ${MKL_INCLUDE} CACHE PATH "Path to MKL include directory" FORCE)
            endif ()
            message(STATUS "MKL_INCLUDE_DIRS: ${MKL_INCLUDE_DIRS}")
            message(STATUS "MKL_LIBRARY_DIR: ${MKL_LIBRARY_DIR}")
            message(STATUS "MKL_LIBRARIES: ${MKL_LIBRARIES}")

            # Update: Since Intel-MKL 2024.02, don't set or use MKL_LIBRARIES since it is used by
            # MKLConfig.cmake internally. Instead, we should use MKL::MKL and MKL::<library_name>.
            # MKL::MKL is a target that holds only linker arguments and linked to all
            # MKL::<library_name> targets. MKL::<library_name> is a target that holds the full path
            # of the library. e.g.
            # target_link_libraries(<my_linkable_target>
            #       PUBLIC MKL::mkl_core MKL::mkl_sequential MKL::mkl_intel_lp64
            #       PUBLIC MKL::mkl_scalapack_lp64 MKL::mkl_cdft_core MKL::mkl_blacs_intelmpi_lp64
            # )
            # target_link_options(<my_linkable_target> PUBLIC MKL::MKL)

            # we should unset MKL_DIR and MKLROOT in case some other packages have their own
            # FindMKL scripts.
            unset(MKL_DIR)
            unset(MKL_ROOT)
            unset(IOMP_ROOT)
        elseif (ERL_USE_AOCL)
            message(STATUS "Use AMD Optimizing CPU Library")
            add_definitions(-DEIGEN_USE_BLAS)
            add_definitions(-DEIGEN_ERL_USE_LAPACKE)
            set(BLA_VENDOR AOCL)
            erl_find_path(
                    OUTPUT AOCL_LIB_DIR
                    PACKAGE AMD-AOCL
                    REQUIRED
                    NAMES libblis.so
                    PATHS /opt/AMD/aocl/*/lib
                    COMMANDS GENERAL "visit https://www.amd.com/en/developer/aocl.html"
                    COMMANDS ARCH_LINUX "try `paru -Ss blas-aocl-gcc`")
            get_filename_component(AOCL_ROOT ${AOCL_LIB_DIR} DIRECTORY)

            if (ERL_USE_SINGLE_THREADED_BLAS)
                # single-threaded BLAS
                set(BLAS_LIBRARIES ${AOCL_ROOT}/lib/libblis.so ${AOCL_ROOT}/lib/libalm.so -lm)
            else ()
                # multi-threaded BLAS
                set(BLAS_LIBRARIES ${AOCL_ROOT}/lib/libblis-mt.so ${AOCL_ROOT}/lib/libalm.so -lm)
            endif ()

            set(LAPACK_LIBRARIES ${AOCL_ROOT}/lib/libflame.so)
        else ()
            message(STATUS "Use OpenBLAS")
            add_definitions(-DEIGEN_USE_BLAS)
            add_definitions(-DEIGEN_ERL_USE_LAPACKE)
            set(BLA_VENDOR OpenBLAS)

            if (ERL_USE_SINGLE_THREADED_BLAS)
                erl_find_path(
                        OUTPUT BLAS_LIB_DIR
                        PACKAGE OpenBLAS
                        REQUIRED
                        NAMES libopenblas.so
                        PATHS /usr/lib/x86_64-linux-gnu /opt/OpenBLAS/lib
                        COMMANDS APPLE "try `brew install openblas`"
                        COMMANDS UBUNTU_LINUX "try `bash scripts/install_openblas_seq.bash`")
            endif ()

            erl_find_package(
                    PACKAGE LAPACK
                    REQUIRED
                    COMMANDS APPLE "try `brew install lapack`"
                    COMMANDS UBUNTU_LINUX "try `sudo apt install liblapack-dev`"
                    COMMANDS ARCH_LINUX "try `sudo pacman -S lapack`")
        endif ()

        message(STATUS "BLAS_LIBRARIES: ${BLAS_LIBRARIES}")
        message(STATUS "LAPACK_LIBRARIES: ${LAPACK_LIBRARIES}")
    else ()
        unset(BLAS_LIBRARIES)
        unset(LAPACK_LIBRARIES)
    endif ()
endmacro()
