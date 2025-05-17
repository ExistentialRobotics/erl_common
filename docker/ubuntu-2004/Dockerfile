ARG BASE_IMAGE=ubuntu:20.04
FROM ${BASE_IMAGE}

ARG TZ=America/Los_Angeles
ARG LOCALE=en_US.UTF-8
ARG APT_MIRROR=""

ENV TZ=${TZ}
ENV LANG=${LOCALE}
ENV LANGUAGE=${LOCALE}
ENV LC_ALL=${LOCALE}

RUN cp /etc/apt/sources.list /etc/apt/sources.list.backup &&\
    if [ -n "$APT_MIRROR" ]; then \
        sed -i "s|http://archive.ubuntu.com/ubuntu/|$APT_MIRROR|g" /etc/apt/sources.list; \
    fi
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && \
    DEBIAN_FRONTEND=noninteractive apt update && apt install -y locales && locale-gen en_US.UTF-8
RUN DEBIAN_FRONTEND=noninteractive apt install -y \
    apt-utils \
    software-properties-common \
    lsb-release \
    ca-certificates \
    wget
RUN wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \
    | gpg --dearmor | tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
RUN echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | tee /etc/apt/sources.list.d/oneAPI.list
RUN DEBIAN_FRONTEND=noninteractive apt update && apt install -y intel-basekit-2024.1  # use oneAPI instead of intel-mkl-full from ubuntu-apt

RUN DEBIAN_FRONTEND=noninteractive apt install -y \
    sudo \
    python3 \
    python3-pip \
    python3-dev \
    zsh \
    git \
    wget \
    build-essential \
    cmake \
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

RUN touch /usr/lib/python3/dist-packages/_Pltk_init.so
RUN touch /usr/lib/python3/dist-packages/_plplotc.so
RUN touch /usr/lib/python3/dist-packages/plplot_pyqt5.so
RUN touch /usr/bin/pltek
RUN sed 's/..\/..\/..\/..\/..\/include/..\/..\/..\/..\/include/g' \
    /usr/lib/x86_64-linux-gnu/cmake/yaml-cpp/yaml-cpp-config.cmake

WORKDIR /root
RUN git clone https://github.com/jlblancoc/nanoflann && \
    cd nanoflann && git checkout v1.7.1 && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make install && \
    cd /root && rm -rf nanoflann

# clean
RUN apt-get clean  && rm -rf /var/lib/apt/lists/*
RUN if [ -n "$APT_MIRROR" ]; then \
        mv /etc/apt/sources.list.backup /etc/apt/sources.list; \
    fi
