#!/bin/bash
# runs on Ubuntu Github Actions workflow

# need lua
sudo apt install lua

cd ./luau

lua ../PACK.lua

cp luau.hpp ../luau.hpp
cp luau.cpp ../luau.cpp

cd ..

mkdir cmake
cd cmake
cmake ..
