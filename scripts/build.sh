#!/usr/bin/env bash

set -e

git submodule update --init --recursive
mkdir -p build
cd build
cmake ..
make -j$(nproc)

set +e
