#!/bin/bash

sudo apt-get update
sudo apt-get install -y gcc g++
sudo apt-get install -y cmake
sudo apt-get install -y libboost-all-dev
git submodule sync --recursive
git submodule update --recursive --init
cmake CMakeLists.txt
make