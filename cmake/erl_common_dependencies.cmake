if (NOT COMMAND erl_project_setup)
    find_package(erl_cmake_tools REQUIRED)
endif ()

# must be before erl_config_eigen because some flags about Eigen3 are set.
erl_config_lapack()

erl_config_boost() # Boost libraries
erl_config_fmt() # string formatting
erl_config_openmp() # OpenMP support
erl_config_eigen() # Eigen3 library
erl_config_absl()  # Abseil library

# file libraries
erl_config_json()
erl_config_yaml()
erl_config_libzip()

# image processing, visualization libraries
erl_config_opencv()
erl_config_pangolin()
erl_config_plplot()

erl_config_tracy() # profiling tool
