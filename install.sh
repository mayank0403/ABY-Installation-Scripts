#!/bin/bash

add-apt-repository ppa:ubuntu-toolchain-r/test

apt-get update

apt-get install -y g++-8

apt-get install -y make 
apt-get install -y cmake 
apt-get install -y libgmp-dev 
apt-get install -y libssl-dev 
#apt-get install -y libboost-all-dev

wget "https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz"

tar -xvf boost_1_66_0.tar.gz

cd boost_1_66_0/

./bootstrap.sh

./b2 install

#Boost is installed in /usr/local/lib and headers in /usr/local/include/boost

cd ..

git clone https://github.com/encryptogroup/ABY.git

cd ABY/

mkdir build && cd build

cmake --version

cd ../..
