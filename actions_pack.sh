#!/bin/bash
# runs on Ubuntu Github Actions workflow

# need lua
sudo apt-get install lua5.4

cd ./luau

lua ../PACK.lua

cp luau.hpp ../luau.hpp
cp luau.cpp ../luau.cpp

cd ..

echo "Check for valid packing"
gcc -c luau.cpp -o tmp.o -std=c++17 -Wall -DLUAU_ENABLE_COMPILER -DLUAU_ENABLE_CODEGEN
