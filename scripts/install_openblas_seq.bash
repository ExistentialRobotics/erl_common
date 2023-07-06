#! /usr/bin/bash

# https://github.com/xianyi/OpenBLAS/blob/develop/USAGE.md
# https://github.com/xianyi/OpenBLAS/wiki/Installation-Guide#linux

BASH_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd $BASH_DIR/../
mkdir -p build
cd build
if [ -d "OpenBLAS"]; then
    rm -rf OpenBLAS
fi
git clone https://github.com/xianyi/OpenBLAS.git
cd OpenBLAS
make USE_THREAD=0 USE_OPENMP=0 USE_LOCKING=1 FC=gfortran -j$(nproc)  # single-threaded but used in multi-threaded programs
sudo make install
sudo ldconfig
sudo update-alternatives --install /usr/lib/x86_64-linux-gnu/libopenblas.so libopenblas.so-x86_64-linux-gnu /opt/OpenBLAS/lib/libopenblas.so 110
sudo update-alternatives --install /usr/lib/x86_64-linux-gnu/libblas.so.3 libblas.so.3-x86_64-linux-gnu /opt/OpenBLAS/lib/libopenblas.so 110
sudo update-alternatives --install /usr/lib/x86_64-linux-gnu/libblas.so libblas.so-x86_64-linux-gnu /opt/OpenBLAS/lib/libopenblas.so 110
sudo update-alternatives --install /usr/lib/x86_64-linux-gnu/liblapack.so liblapack.so-x86_64-linux-gnu /opt/OpenBLAS/lib/libopenblas.so 110
sudo update-alternatives --install /usr/lib/x86_64-linux-gnu/liblapack.so.3 liblapack.so.3-x86_64-linux-gnu /opt/OpenBLAS/lib/libopenblas.so 110
