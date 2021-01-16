#!/bin/bash

brew install cmake
brew install libboost-all-dev
git clone https://github.com/kennyfrc/lc0
cd lc0
git checkout 55326758fe
cd ..
git clone https://github.com/kennyfrc/polyglot
cd polyglot
git checkout 6e8af49a18
cd ..
git clone https://github.com/kennyfrc/zlib
cd zlib
git checkout 162771a9bd
cd ..
cmake CMakeLists.txt
make