#!/usr/bin/bash

BASH_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ ! $(uname) == "Linux" && ! $(uname) == "Darwin" ]; then
    echo "This script is only for macOS and Linux"
    exit 1
fi

cd $BASH_DIR/../
mkdir -p build
cd build

# Install dependencies
if [ $(uname) == "Darwin" ]; then
    brew install cmake pkg-config wget jpeg libpng libtiff openexr tbb hdf5
    if [ ! -d "fftw-3.3.10.tar.gz"]; then
        wget http://www.fftw.org/fftw-3.3.10.tar.gz
    fi
    if [ -d "fftw-3.3.10"]; then
        rm -rf fftw-3.3.10
    fi
    tar -xvzf fftw-3.3.10.tar.gz
    cd fftw-3.3.10
    ./configure
    make
    sudo make install
    cd ..
else
    sudo apt update
    sudo apt install -y cimg-dev libfftw3-dev gnuplot gnuplot-qt libjpeg-dev libtiff-dev zlib1g-dev libglfw3-dev
fi

if [ ! -d "matplotplusplus"]; then
    git clone https://github.com/alandefreitas/matplotplusplus.git
fi
cd matplotplusplus
if [ -d "build"]; then
    rm -rf build
fi
mkdir build
cd build
# IF using LLVM
# cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
if [ $(uname) == "Darwin" ]; then
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17 \
        -DMATPLOTPP_BUILD_EXAMPLES=OFF \
        -DCMAKE_C_COMPILER=/usr/local/opt/llvm/bin/clang \
        -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++ \
        ..
else
    cmake ..
fi
sudo cmake --build . --target install -j$(nproc)
