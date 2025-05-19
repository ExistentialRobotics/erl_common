`erl_common`
============

This CMake project provides `erl_common` which is a collection of commonly used C++ libraries such
as

- Eigen: linear algebra
- LAPACK: linear algebra
- Intel MKL: linear algebra
- Boost: filesystem, program_options, system, thread, etc.
- OpenCV: image processing
- fmt: Python f-string style formatting
- absl: Google's common libraries (abseil-cpp)
- Pangolin: 2d curve plotting and 3d trajectory visualization
- OpenMP: multi-threading
- Python3: Python binding or embedding Python interpreter in C++
- pybind11: Python binding or embedding Python interpreter in C++
- GTest: Google Test
- nlohmann_json: JSON parser
- YAML-CPP: YAML parser

and some utility functions and data structures like

- **adaptive catkin build system**: support both catkin and native CMake build
- color print / logging
- assertion, timing, check results
- binary IO, csv IO
- C++ template helper
- C++ tqdm implementation
- support for embedding Python interpreter in C++
- random number generator
- string utilities
- storage order converter
- Eigen3 extension
- YAML-CPP extension
- OpenCV extension
- Pangolin extension
- grid map and visualization

# Install Dependencies

- CMake >= 3.24
- OpenMP
- Boost
- Eigen3
- nlohmann_json
- OpenCV
- nanoflann
- yaml-cpp
- Intel MKL
- LAPACK
- LAPACKE
- fmt
- absl
- Pangolin
- Python3
- pybind11
- GTest

[Scripts for installing dependencies on Ubuntu 22.04](scripts/setup_ubuntu_22.04.bash)

[Scripts for installing dependencies on Arch Linux](scripts/setup_archlinux.bash)

# Getting Started

## Use as a standard CMake package

```bash
cd <your_workspace>/src
git clone https://github.com/ExistentialRobotics/erl_common.git
touch CMakeLists.txt
```

Add the following lines to your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.24)
project(<your_project_name>)
add_subdirectory(erl_common)
```

## Use as a catkin package

```bash
cd <your_workspace>/src
git clone https://github.com/ExistentialRobotics/erl_common.git
cd ..
catkin build
```

## Install as a Python package

- Make sure you have installed all dependencies.
- Make sure you have the correct Python environment activated, `pipenv` is recommended.

```bash
cd erl_common
pip install . --verbose
```

# CMake helper functions and macros

`erl_common` also provides some helper functions and macros to make other packages support both
catkin and native CMake
build easily. See [cmake/README.md](cmake/README.md) for more details.

# Functionalities

- [Azimuth Elevation](include/erl_common/angle_utils.hpp)

# Docker

```shell
cd docker
./build.bash
# if you get errors about apt 404, try to use different mirror source, e.g.
# ./build.bash --build-arg APT_MIRROR=http://mirror.arizona.edu/ubuntu/
```

# Troubleshooting

## The program is slower in docker

If you are using docker on Windows or Mac, the program may be slower than running on native Linux.
This is because docker uses a virtual machine to run Linux, which adds some overhead. You can try
to use WSL2 on Windows or run Linux natively on your Mac to improve the performance.
With Linux docker, native performance should be achieved. If it does not, please run the program
with `OMP_DISPLAY_ENV=verbose` and check if the OpenMP environment variables are set correctly.
Here are the expected OMP environment variables:

```shell
OPENMP DISPLAY ENVIRONMENT BEGIN
  _OPENMP = '201511'
  [host] OMP_DYNAMIC = 'FALSE'
  [host] OMP_NESTED = 'FALSE'
  [host] OMP_NUM_THREADS = '32'
  [host] OMP_SCHEDULE = 'DYNAMIC'
  [host] OMP_PROC_BIND = 'FALSE'
  [host] OMP_PLACES = ''
  [host] OMP_STACKSIZE = '0'
  [host] OMP_WAIT_POLICY = 'PASSIVE'
  [host] OMP_THREAD_LIMIT = '4294967295'
  [host] OMP_MAX_ACTIVE_LEVELS = '1'
  [host] OMP_NUM_TEAMS = '0'
  [host] OMP_TEAMS_THREAD_LIMIT = '0'
  [all] OMP_CANCELLATION = 'FALSE'
  [all] OMP_DEFAULT_DEVICE = '0'
  [all] OMP_MAX_TASK_PRIORITY = '0'
  [all] OMP_DISPLAY_AFFINITY = 'FALSE'
  [host] OMP_AFFINITY_FORMAT = 'level %L thread %i affinity %A'
  [host] OMP_ALLOCATOR = 'omp_default_mem_alloc'
  [all] OMP_TARGET_OFFLOAD = 'DEFAULT'
  [host] GOMP_CPU_AFFINITY = ''
  [host] GOMP_STACKSIZE = '0'
  [host] GOMP_SPINCOUNT = '1'
OPENMP DISPLAY ENVIRONMENT END
```

Pay attention to `GOMP_SPINCOUNT`, which is default to `300000` in Ubuntu 20.04 and slows down the
program significantly. You can set it to `1` in your shell configuration file, or run the program
with `GOMP_SPINCOUNT=1` to improve the performance. e.g.,

```shell
GOMP_SPINCOUNT=1 ./your_program
```
