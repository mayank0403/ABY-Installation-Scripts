#!/bin/bash

cd ABY/

cd build/

CXX=/usr/bin/g++-8 cmake .. -DABY_BUILD_EXE=On -DBOOST_ROOT=/usr/local/include/boost -DBOOST_LIBRARYDIR=/usr/local/lib

make
