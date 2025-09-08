#!/usr/bin/bash

sudo apt install -y \
    sudo \
    apt-utils \
    software-properties-common \
    lsb-release \
    ca-certificates \
    wget \
    curl \
    python3 \
    python3-pip \
    python3-dev \
    zsh \
    git \
    build-essential \
    cmake \
    intel-mkl-full \
    liblapack-dev \
    liblapacke-dev \
    libomp-dev \
    libeigen3-dev \
    libfmt-dev \
    libboost-all-dev \
    nlohmann-json3-dev \
    libopencv-dev \
    libplplot-dev \
    libplplot-java \
    libplplot-data \
    libplplotada4 \
    plplot-tcl-dev \
    plplot-driver-cairo \
    python3-plplot \
    octave-plplot \
    libcairomm-1.0-dev \
    libyaml-cpp-dev \
    pybind11-dev \
    libgtest-dev

# patch
sudo touch /usr/lib/python3/dist-packages/_Pltk_init.so
sudo touch /usr/lib/python3/dist-packages/_plplotc.so
sudo touch /usr/lib/python3/dist-packages/plplot_pyqt5.so
sudo touch /usr/bin/pltek
sudo sed 's/..\/..\/..\/..\/..\/include/..\/..\/..\/..\/include/g' \
        /usr/lib/x86_64-linux-gnu/cmake/yaml-cpp/yaml-cpp-config.cmake

sudo echo "export GOMP_SPINCOUNT=1" >> /etc/profile  # for OpenMP
