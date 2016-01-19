#!/bin/bash

mkdir -p dependencies
cd dependencies

git clone https://github.com/google/benchmark google-benchmark-framework

cd google-benchmark-framework
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(pwd)/build .
make && make install
