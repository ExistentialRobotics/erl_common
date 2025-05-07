#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

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
  pybind11-dev \
  libepoxy-dev \
  libgl1-mesa-dev \
  libwayland-dev \
  libxkbcommon-dev \
  wayland-protocols \
  libegl1-mesa-dev \
  libc++-dev \
  libglew-dev \
  ninja-build

VERSION_ID=$(source /etc/os-release; echo $VERSION_ID)
if [ "$VERSION_ID" == "24.04" ]; then
  sudo apt-get install libxrandr-dev libxinerama-dev libxcursor-dev libc++-14-dev libc++abi-14-dev xinput libxi-dev
fi

# Upgrade pip
python3 -m pip install --upgrade pip --user

# Install CMake
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
sudo apt-add-repository -y "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"
sudo apt update
sudo apt install -y kitware-archive-keyring
sudo apt install -y cmake

# Install Intel Base Toolkit
wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \ | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list
sudo apt update
sudo apt install -y intel-basekit-2024.1 liblapacke-dev liblapack-dev

mkdir -p build/3rdparty
cd build/3rdparty

# Install Eigen3
if dpkg --compare-versions ${VERSION_ID} lt 24.04; then   # older than 24.04
    # before 24.04, Eigen3 is older than 3.4.0
    if [ ! -d "eigen" ]; then
      git clone https://gitlab.com/libeigen/eigen.git
    fi
    cd eigen
    git checkout 3.4.0
    mkdir -p build
    cd build
    cmake ..
    sudo cmake --build . --target install -- -j $(nproc)
    cd ../..
fi

# Install pybind11
if [ ! -d "pybind11" ]; then
  git clone https://github.com/pybind/pybind11.git
fi
cd pybind11
git checkout v2.13.4
mkdir -p build
cd build
cmake ..
sudo cmake --build . --target install -- -j $(nproc)
cd ../..

# Install nlohmann/json
if [ ! -d "json" ]; then
  git clone https://github.com/nlohmann/json.git
fi
cd json
git checkout b230614
mkdir -p build
cd build
cmake ..
sudo cmake --build . --target install -- -j $(nproc)
cd ../..

# Install nanoflann
if [ ! -d "nanoflann" ]; then
  git clone https://github.com/jlblancoc/nanoflann
fi
cd nanoflann
git checkout 9a653cb243db6a09c94f833b28732b62f033e2b5  # v1.7.1
mkdir -p build
cd build
cmake ..
sudo cmake --build . --target install -- -j $(nproc)
cd ../..

# Install fmt
if [ ! -d "fmt" ]; then
  git clone https://github.com/fmtlib/fmt
fi
cd fmt
git checkout 11.0.2
mkdir -p build
cd build
cmake ..  -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
sudo cmake --build . --target install -- -j $(nproc)
cd ../..

# Install Pangolin
PANGOLIN_VERSION="0.9.3"
if [ ! -d "Pangolin-${PANGOLIN_VERSION}" ]; then
    git clone https://github.com/stevenlovegrove/Pangolin.git
fi
cd Pangolin
git checkout v${PANGOLIN_VERSION}
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j`nproc`
sudo make install
cd ../..

# Install Open3D for erl_geometry
if [ -d "${SCRIPT_DIR}/../../erl_geometry" ]; then  # If erl_geometry is used

    # Install absl
    ABSEIL_VERSION="20240722.1"
    if [ ! -d "abseil-cpp-${ABSEIL_VERSION}" ]; then
      wget https://github.com/abseil/abseil-cpp/releases/download/${ABSEIL_VERSION}/abseil-cpp-${ABSEIL_VERSION}.tar.gz
      tar -xf abseil-cpp-${ABSEIL_VERSION}.tar.gz
    fi
    cd abseil-cpp-${ABSEIL_VERSION}
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="${CXXFLAGS} -DNDEBUG" \
      -DCMAKE_CXX_STANDARD=17 -DBUILD_SHARED_LIBS=ON -DABSL_PROPAGATE_CXX_STD=ON
    make -j`nproc`
    sudo make install
    cd ../..

    if [ ! -d "qhull" ]; then
      git clone --recursive https://github.com/qhull/qhull.git
    fi
    cd qhull
    git checkout 2020.2
    mkdir -p my_build
    cd my_build
    cmake .. -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-fPIC -ffat-lto-objects" \
      -DCMAKE_C_FLAGS="-fPIC -ffat-lto-objects" \
      -DCMAKE_SKIP_RPATH=ON
    make -j`nproc`
    sudo make install
    cd ../..

    if [ ! -d "Open3D" ]; then
      git clone --recursive https://github.com/isl-org/Open3D.git
    fi
    cd Open3D
    git checkout 0f06a14
    echo y | util/install_deps_ubuntu.sh
    mkdir -p build
    cd build
    cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release \
        -DUSE_SYSTEM_EIGEN3=ON \
        -DUSE_SYSTEM_FMT=ON \
        -DUSE_SYSTEM_GOOGLETEST=ON \
        -DUSE_SYSTEM_JSONCPP=ON \
        -DUSE_SYSTEM_NANOFLANN=ON \
        -DUSE_SYSTEM_PYBIND11=ON \
        -DUSE_SYSTEM_QHULLCPP=ON \
        -DGLIBCXX_USE_CXX11_ABI=ON \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_UNIT_TESTS=OFF \
        -DBUILD_BENCHMARKS=OFF

    make -j`nproc`
    sudo make install
    # sudo make install-pip-package -j`nproc`  # run this command if you want to use Open3D in Python
    cd ../..

    sudo apt install -y libcgal-dev
fi

# Install libtorch for erl_neural_sddf
if [ -d "${SCRIPT_DIR}/../../erl_neural_sddf" ]; then # If erl_neural_sddf is used
    if [ ! -d "libtorch" ]; then
      wget https://download.pytorch.org/libtorch/cu124/libtorch-cxx11-abi-shared-with-deps-2.4.0%2Bcu124.zip
      sudo apt install -y unzip
      unzip libtorch-cxx11-abi-shared-with-deps-2.4.0+cu124.zip
    fi
    sudo cp -r libtorch /opt/libtorch-cuda

    tarfile="libcusparse_lt-linux-x86_64-0.6.2.3-archive.tar.xz"
    if [ ! -f "${tarfile}" ]; then
        wget "https://developer.download.nvidia.com/compute/cusparselt/redist/libcusparse_lt/linux-x86_64/${tarfile}"
        tar -xf ${tarfile}
    fi
    sudo cp -r ${tarfile%.tar.xz} /opt/cusparselt

    tarfile="libzip-1.10.1.tar.gz"
    if [ ! -f "${tarfile}" ]; then
        wget "https://libzip.org/download/${tarfile}"
        tar -xf ${tarfile}
    fi
    cd ${tarfile%.tar.gz}
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
    make -j`nproc`
    sudo make install
    cd ../..
fi
