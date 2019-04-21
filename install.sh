#!/bin/bash

sudo add-apt-repository ppa:ubuntu-toolchain-r/test

sudo apt-get update

sudo apt-get install g++-8

sudo apt-get install make cmake libgmp-dev libssl-dev libboost-all-dev

wget "https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz"

tar -xvf boost_1_66_0.tar.gz

cd boost_1_66_0/

sudo ./bootstrap.sh

sudo ./b2 install

#Boost is installed in /usr/local/lib and headers in /usr/local/include/boost

cd ..

git clone https://github.com/encryptogroup/ABY.git

cd ABY/

mkdir build && cd build

cmake --version

cd ../..
