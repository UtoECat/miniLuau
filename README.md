MiniLuau
====

This repo implements lua script `PACK.lua`, to pack official Luau sources
*(compiler & vm only at this point)* into **TWO FILES** : `luau.hpp` and `luau.cpp`, that can be just drag&dropped to your C++ project.


# Building

**Notice : luau.h and luau.c are ALREADY prebuild in this repo! Do packing manually only if you really want to change this process somehow, or to fetch with latest Luau version faster.**
If you want to pack source manually, you need to install Lua 5.4, git.

At first clone this repo with this command :
```sh
 $ git clone --recursive https://github.com/UtoECat/miniLuau
```
Then cd into `luau` submodule directory, and run
```sh
$ lua5.4 ../PACK.lua
```
You will see errors during building process about not founded `<string.h>` or `<vector>`, that's normal.
At the end you will get your own packed luau sources :)  

# Dependencies

Luau uses C++ as its implementation language. The runtime requires C++11, whereas the compiler and analysis components require C++17. It should build without issues using Microsoft Visual Studio 2017 or later, or gcc-7 or clang-7 or later.

Other than the STL/CRT, Luau library components don't have external dependencies. 

# License

Luau implementation is distributed under the terms of [MIT License](https://github.com/Roblox/luau/blob/master/LICENSE.txt). It is based on Lua 5.x implementation that is MIT licensed as well.

When Luau is integrated into external projects, we ask to honor the license agreement and include Luau attribution into the user-facing product documentation. The attribution using [Luau logo](https://github.com/Roblox/luau/blob/master/docs/logo.svg) is also encouraged.

Pack.lua licensed under MIT too.

Luau 
====

Luau (lowercase u, /ˈlu.aʊ/) is a fast, small, safe, gradually typed embeddable scripting language derived from [Lua](https://lua.org).

This is not original Luau repository.
Visit [Original Luau repository](https://github.com/Roblox/luau) for more information about LuaU.

This repository hosts source code for the VM, LUAAPI and COMPILER implementation, to make it easy to embend in your projects.

Also it has optional luau-test utility, to check if compilation is done correctly and API working as intended.

The documentation portion of this repository can be viewed at [Lua-Lang website](https://luau-lang.org/)

# Baselib documentation
[Is here (click)](https://luau-lang.org/library)

