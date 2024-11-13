#!/bin/bash
# runs on Ubuntu Github Actions workflow

# need lua
sudo apt-get install lua5.4 clang-format

cd ./luau

lua ../PACK.lua

cd ../pack-out

echo "Check for valid packing"

# TODO : does not test tools really yet
g++ -fsyntax-only -c *.cpp  -std=c++17 -Wall
