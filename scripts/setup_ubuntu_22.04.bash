#!/usr/bin/env bash

# Install via APT
sudo apt install -y \
  git \
  wget \
  curl \
  gcc \
  g++ \
  build-essential \
  ccache \
  python3 \
  python3-pip \
  python3-setuptools \
  software-properties-common \
  lsb-release \
  libgtest-dev \
  liblapacke-dev \
  liblapack-dev \
  libomp-dev \
  libboost-all-dev \
  nlohmann-json3-dev \
  libopencv-dev \
  libpcl-dev \
  libyaml-cpp-dev \
  libnanoflann-dev \
  pybind11-dev

# Upgrade pip
python3 -m pip install --upgrade pip --user

# Install CMake
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"
sudo apt update
sudo apt install kitware-archive-keyring
sudo apt install cmake

# Install Intel Base Toolkit
wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \ | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | tee /etc/apt/sources.list.d/oneAPI.list
sudo apt update
sudo apt install intel-basekit liblapacke-dev liblapack-dev

# Install Eigen3
git clone https://gitlab.com/libeigen/eigen.git
cd eigen
git checkout 3abe12472ece589e223e5ee50a1da908c97dceb1
mkdir build
cd build
cmake ..
cmake --build . --target install -- -j $(nproc)

# Install pybind11
git clone https://github.com/pybind/pybind11.git
cd pybind11
git checkout 3cc7e42
mkdir build
cd build
cmake ..
cmake --build . --target install -- -j $(nproc)

# Install nlohmann/json
git clone https://github.com/nlohmann/json.git
cd json
git checkout b230614
mkdir build
cd build
cmake ..
cmake --build . --target install -- -j $(nproc)
