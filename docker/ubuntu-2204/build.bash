#!/usr/bin/env bash

set -e
SCRIPT_DIR=$(cd $(dirname $0); pwd)
docker build --rm -t erl/common:22.04 . \
  --build-arg BASE_IMAGE=ubuntu:22.04 $@
