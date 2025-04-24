ARG BASE_IMAGE=ubuntu:20.04
FROM ${BASE_IMAGE}

ARG TZ=America/Los_Angeles
ARG LOCALE=en_US.UTF-8

ENV TZ=${TZ}
ENV LANG=${LOCALE}
ENV LANGUAGE=${LOCALE}
ENV LC_ALL=${LOCALE}

RUN DEBIAN_FRONTEND=noninteractive apt update && apt upgrade -y
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
    intel-mkl-full \
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

WORKDIR /root
RUN git clone https://github.com/jlblancoc/nanoflann && \
    cd nanoflann && git checkout v1.7.1 && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make install && \
    cd /root && rm -rf nanoflann

RUN DEBIAN_FRONTEND=noninteractive apt install -y locales && locale-gen en_US.UTF-8
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# clean
RUN apt-get clean  && rm -rf /var/lib/apt/lists/*
