#!/bin/bash

wget "https://github.com/Kitware/CMake/releases/download/v3.14.2/cmake-3.14.2.tar.gz"

tar -xvf cmake-3.14.2.tar.gz

cd cmake-3.14.2/

sudo ./bootstrap

sudo make -j4

sudo make install

sudo cp /usr/local/bin/cmake /usr/bin/cmake

cmake --version

cd ..
