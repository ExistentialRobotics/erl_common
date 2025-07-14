ARG BASE_IMAGE=ubuntu:22.04
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
    libplplotada4.1 \
    plplot-tcl-dev \
    plplot-driver-cairo \
    python3-plplot \
    libcairomm-1.0-dev \
    libyaml-cpp-dev \
    pybind11-dev \
    libgtest-dev

RUN touch /usr/lib/python3/dist-packages/_Pltk_init.so
RUN touch /usr/lib/python3/dist-packages/_plplotc.so
RUN touch /usr/lib/python3/dist-packages/plplot_pyqt5.so
RUN touch /usr/bin/pltek

ENV GOMP_SPINCOUNT=1

# clean
RUN apt-get clean  && rm -rf /var/lib/apt/lists/*
RUN if [ -n "$APT_MIRROR" ]; then \
    mv /etc/apt/sources.list.backup /etc/apt/sources.list; \
    fi
