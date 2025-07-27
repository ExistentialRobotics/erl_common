#!/usr/bin/bash

BASH_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ ! $(uname) == "Darwin" ]; then
    echo "This script is only for macOS"
    exit 1
fi

cd $BASH_DIR/../
mkdir -p build
cd build

if [ ! -d "yaml-cpp"]; then
    git clone https://github.com/jbeder/yaml-cpp.git
fi
cd yaml-cpp
if [ -d "build"]; then
    rm -rf build
fi
mkdir build
cd build

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_C_COMPILER=/usr/local/opt/llvm/bin/clang \
    -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++ \
    ..
sudo cmake --build . --target install -j$(nproc)
