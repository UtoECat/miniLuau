#!/bin/bash
# runs on Ubuntu Github Actions workflow

# need lua
sudo apt-get install lua5.4 clang-format

cd ./luau

lua ../PACK.lua

cd ..

echo "Check for valid packing"

g++ -fsyntax-only -c *.cpp  -std=c++17 -Wall -DLUAU_ENABLE_COMPILER -DLUAU_ENABLE_CODEGEN
