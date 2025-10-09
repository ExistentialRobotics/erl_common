#!/usr/bin/env bash

set -e
SCRIPT_DIR=$(cd $(dirname $0); pwd)
BASE_IMAGE=${BASE_IMAGE:-nvidia/cuda:12.9.0-cudnn-devel-ubuntu22.04}
TAG=${TAG:-22.04-cuda-12.9.0}

docker build --rm -t erl/common:${TAG} . \
  --build-arg BASE_IMAGE=${BASE_IMAGE} $@
