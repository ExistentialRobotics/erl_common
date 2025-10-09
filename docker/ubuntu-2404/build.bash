#!/usr/bin/env bash

set -e
SCRIPT_DIR=$(cd $(dirname $0); pwd)
BASE_IMAGE=${BASE_IMAGE:-ubuntu:24.04}
TAG=${TAG:-24.04}

docker build --rm -t erl/common:${TAG} . \
  --build-arg BASE_IMAGE=${BASE_IMAGE} $@
