#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Install via APT
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
    python3-vedo \
    python3-tqdm \
    zsh \
    vim \
    git \
    build-essential \
    cmake \
    ccache \
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
    libplplotada4.1 \
    plplot-tcl-dev \
    plplot-driver-cairo \
    python3-plplot \
    libcairomm-1.0-dev \
    libyaml-cpp-dev \
    pybind11-dev \
    libgtest-dev

# patch
sudo touch /usr/lib/python3/dist-packages/_Pltk_init.so
sudo touch /usr/lib/python3/dist-packages/_plplotc.so
sudo touch /usr/lib/python3/dist-packages/plplot_pyqt5.so
sudo touch /usr/bin/pltek
