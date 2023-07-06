`erl_common`
============

This CMake project provides `erl_common` which is a collection of commonly used C++ libraries such as 
- Eigen: linear algebra
- LAPACK: linear algebra
- Intel MKL: linear algebra
- Boost: filesystem, program_options, system, thread, etc.
- OpenCV: image processing
- OpenMP: multi-threading
- pybind11: Python binding
- nlohmann_json: JSON parser
- YAML-CPP: YAML parser
- Matplot++: C++ version of matplotlib

and some utility functions and data structures like 
- color print / logging
- assertion, timing, check results 
- binary IO, csv IO
- C++ template helper
- random number generator
- string utilities
- storage order converter
- Eigen3 extension
- YAML-CPP extension
- OpenCV extension
- grid map and visualization

# Dependencies
- CMake >= 3.20
- [erl_cmake](https://github.com/ExistentialRobotics/erl_cmake)

# Getting Started
This project should be put together with `erl_cmake` in the same parent directory. e.g. 
```bash
cd <your_workspace>
git clone https://github.com/ExistentialRobotics/erl_cmake.git
git clone https://github.com/ExistentialRobotics/erl_common.git
cd erl_common
mkdir build && cd build
cmake ..
```
