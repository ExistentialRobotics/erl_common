#!/usr/bin/bash

BASH_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ ! $(uname) == "Darwin" ]; then
    echo "This script is only for macOS"
    exit 1
fi

# Install dependencies
brew install cmake pkg-config wget jpeg libpng libtiff openexr tbb hdf5 vtk

cd $BASH_DIR/../
mkdir -p build
cd build

# overwrite the eigen installed by brew
if [ ! -d "eigen"]; then
    git clone https://gitlab.com/libeigen/eigen.git
fi
cd eigen
if [ -d "build"]; then
    rm -rf build
fi
mkdir build
cd build
cmake ..
sudo cmake --build . --target install -j$(nproc)
cd ../..

if [ ! -d "opencv"]; then
    git clone https://github.com/opencv/opencv.git
fi
if [ ! -d "opencv_contrib"]; then
    git clone https://github.com/opencv/opencv_contrib.git
fi

if [ -d "build_opencv"]; then
    rm -rf build_opencv
fi
mkdir build_opencv
cd build_opencv
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_EXAMPLES=ON \
    -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
    -DCMAKE_CXX_STANDARD=14 \
    -DBUILD_PNG=ON \
    -DBUILD_ZLIB=OFF \
    -DBUILD_TIFF=ON \
    -DOPENCV_GENERATE_PKGCONFIG=YES \
    ../opencv
sudo cmake --build . --target install -j$(nproc)
