ARG BASE_IMAGE=ubuntu:20.04
FROM ${BASE_IMAGE}

RUN DEBIAN_FRONTEND=noninteractive apt update -y
RUN DEBIAN_FRONTEND=noninteractive apt install -y ubuntu-desktop-minimal
RUN DEBIAN_FRONTEND=noninteractive apt install -y \
    software-properties-common lsb-release vim locales apt-utils ca-certificates curl iproute2

RUN sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
RUN curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | DEBIAN_FRONTEND=noninteractive apt-key add -
RUN DEBIAN_FRONTEND=noninteractive apt update -y
RUN DEBIAN_FRONTEND=noninteractive apt install -y \
    ros-noetic-desktop-full \
    python3-rosdep \
    python3-rosinstall \
    python3-rosinstall-generator \
    python3-wstool \
    python3-catkin-tools \
    python3-pip \
    python3-numpy \
    python3-toml \
    python3-scipy \
    python3-tqdm
RUN rosdep init && rosdep fix-permissions
RUN apt-get clean  && rm -rf /var/lib/apt/lists/*
