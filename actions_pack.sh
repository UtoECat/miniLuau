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
gcc -c luau.cpp -o tmp.o -std=c++17 -Wall

echo "Make ready to commit"
git config --global user.email "patch@auto.bot"
git config --global user.name "Patch Bot"
git add . 
git commit -m "auto commit"
