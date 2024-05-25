`erl_common`
============

This CMake project provides `erl_common` which is a collection of commonly used C++ libraries such as 
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
`erl_common` also provides some helper functions and macros to make other packages support both catkin and native CMake 
build easily. See [cmake/README.md](cmake/README.md) for more details.
