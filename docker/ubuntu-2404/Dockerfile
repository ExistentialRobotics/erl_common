ARG BASE_IMAGE=ubuntu:24.04
FROM ${BASE_IMAGE}

ARG TZ=America/Los_Angeles
ARG LOCALE=en_US.UTF-8
ARG APT_MIRROR=""

ENV TZ=${TZ}
ENV LANG=${LOCALE}
ENV LANGUAGE=${LOCALE}
ENV LC_ALL=${LOCALE}

# make sure it is 24.04
RUN . /etc/os-release && if [ "$VERSION_ID" != "24.04" ]; then echo "This Dockerfile is for Ubuntu 24.04!" && exit 1; fi

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
    libplplotada4.2 \
    libplplotada-dev \
    plplot-tcl-dev \
    plplot-driver-cairo \
    python3-plplot \
    python3-plplot-qt \
    octave-plplot \
    libcairomm-1.0-dev \
    libyaml-cpp-dev \
    pybind11-dev \
    libgtest-dev \
    libabsl-dev

RUN PYTHON_VERSION=$(python3 -c "import sys;print(sys.version_info.major,sys.version_info.minor,sep='')") && \
    ln -s /usr/lib/python3/dist-packages/_Pltk_init.cpython-${PYTHON_VERSION}-x86_64-linux-gnu.so /usr/lib/python3/dist-packages/_Pltk_init.so && \
    ln -s /usr/lib/python3/dist-packages/_plplotc.cpython-${PYTHON_VERSION}-x86_64-linux-gnu.so /usr/lib/python3/dist-packages/_plplotc.so && \
    ln -s /usr/lib/python3/dist-packages/plplot_pyqt5.cpython-${PYTHON_VERSION}-x86_64-linux-gnu.so /usr/lib/python3/dist-packages/plplot_pyqt5.so
RUN ln -s /usr/lib/x86_64-linux-gnu/libplplotada.a /usr/lib/x86_64-linux-gnu/libplplotada_static.a
RUN touch /usr/bin/pltek

RUN userdel ubuntu

ENV GOMP_SPINCOUNT=1

# clean
RUN apt-get clean  && rm -rf /var/lib/apt/lists/*
RUN if [ -n "$APT_MIRROR" ]; then \
    mv /etc/apt/sources.list.backup /etc/apt/sources.list; \
    fi
