# erl_common

[![Tags](https://img.shields.io/github/v/tag/ExistentialRobotics/erl_gp_sdf_ros?label=version)](https://github.com/ExistentialRobotics/erl_gp_sdf_ros/tags)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![ROS1](https://img.shields.io/badge/ROS1-noetic-blue)](http://wiki.ros.org/)
[![ROS2](https://img.shields.io/badge/ROS2-humble-blue)](https://docs.ros.org/)
[![ROS2](https://img.shields.io/badge/ROS2-jazzy-blue)](https://docs.ros.org/)

This CMake project provides `erl_common` which is a collection of commonly used C++ libraries such
as

- Eigen: linear algebra
- LAPACK: linear algebra
- Intel MKL: linear algebra
- Boost: filesystem, program_options, system, thread, etc.
- OpenCV: image processing
- fmt: Python f-string style formatting
- absl: Google's common libraries (abseil-cpp)
- Pangolin: 2d curve plotting and 3d trajectory visualization (deprecated)
- Plplot: 2d and 3d plotting library
- OpenMP: multi-threading
- Python3: for Python binding or embedding Python interpreter in C++
- pybind11: for Python binding or embedding Python interpreter in C++
- GTest: Google Test
- nlohmann_json: JSON parser
- YAML-CPP: YAML parser

and some utility functions and data structures like

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
- Plplot extension
- grid map and visualization
- general serialization and deserialization interface

## Getting Started

### Create Workspace

```bash
cd <your_workspace>
mkdir -p src
vcs import --input https://raw.githubusercontent.com/ExistentialRobotics/erl_common/refs/head/main/erl_common.repos src
```

### Install Dependencies

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

[Scripts for installing dependencies on Ubuntu 20.04](scripts/setup_ubuntu_20.04.bash)

[Scripts for installing dependencies on Ubuntu 22.04 / 24.04](scripts/setup_ubuntu_22.04_24.04.bash)

[Scripts for installing dependencies on Arch Linux](scripts/setup_archlinux.bash)

We also provide [docker files](docker) for building a docker image with all dependencies installed. For example, you can run
```bash
cd <your_workspace>/src/erl_common/docker/ubuntu-20.04
./build.bash
```
to build a docker image with all dependencies installed on Ubuntu 20.04.

This library also depends on [erl_cmake_tools](https://github.com/ExistentialRobotics/erl_cmake_tools).

### Build from Source

#### As a standard CMake package

```bash
cd <your_workspace>
touch CMakeLists.txt
```

Add the following lines to your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.24)
project(<your_project_name>)
add_subdirectory(src/erl_cmake_tools)
add_subdirectory(src/erl_common)
```

Then run the following commands:

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j`nproc`
```

#### As a ROS package

```bash
cd <your_workspace>
source /opt/ros/<distro>/setup.bash
# for ROS1
catkin build erl_common
source devel/setup.bash
# for ROS2
colcon build --packages-up-to erl_common
source install/setup.bash
```

See also 🚪[erl_common_ros](https://github.com/ExistentialRobotics/erl_common_ros) for additional ROS tools.

#### As a Python package

- Make sure you have installed all dependencies.
- Make sure you have the correct Python environment activated, `pipenv` is recommended.

```bash
cd <your_workspace>
for package in erl_cmake_tools erl_common; do
    cd src/$package
    pip install . --verbose
    cd ../..
done
```
