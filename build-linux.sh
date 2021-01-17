#!/bin/bash

sudo apt-get install cmake
sudo apt-get install libboost-all-dev
git submodule sync --recursive
git submodule update --recursive --init
cmake CMakeLists.txt
make