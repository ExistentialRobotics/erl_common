#! /usr/bin/bash

docker build --rm -t erl/ros-noetic:cpu-common . --build-arg BASE_IMAGE=erl/common:20.04 $@
