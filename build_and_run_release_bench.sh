#!/bin/bash

export PARLAY_NUM_THREADS=4

rm -rf build_BfsLab
mkdir build_BfsLab

cd build_BfsLab

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake build .
make -j4 all

./BfsLab