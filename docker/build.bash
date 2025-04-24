#!/usr/bin/env bash
set -e
SCRIPT_DIR=$(cd $(dirname $0); pwd)

cd $SCRIPT_DIR/ubuntu-2004
./build.bash

cd $SCRIPT_DIR/ros-noetic
./build.bash
