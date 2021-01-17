#!/bin/bash

brew install cmake
brew install libboost-all-dev
git submodule sync --recursive
git submodule update --recursive --init
cd zlib && git checkout 162771a9bdf6cf2b17914a6b4ea0fcfe8367207c && cd ..
cmake CMakeLists.txt
make