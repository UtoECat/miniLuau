MiniLuau
====

This repository implements the `PACK.lua` lua script for packing official Luau sources.
*(only compiler and vm at the moment)* into **TWO FILES**: `luau.hpp` and `luau.cpp`, which you can simply drag and drop into your C++ project.

# Building
**Note: luau.h and luau.c are ALREADY AUTOMATICLY packed in this repository! Do manual packaging only if you really want to change this process in some way or speed up getting the latest version of Luau.**
 If you want to package sources manually, you need to install Lua 5.4, git.

First clone this repository with this command:
```sh
 $ git clone --recursive https://github.com/UtoECat/miniLuau
```
Then cd to the `luau` submodule directory and run
```sh
$ lua5.4 ../PACK.lua
```
During the build process, you will see errors about `<string.h>` or `<vector>` not found, this is normal.
At the end you will get your packaged luau sources :)

# Dependencies

Luau uses C++ as its implementation language. The runtime requires C++11, whereas the compiler and analysis components require C++17. It should build without issues using Microsoft Visual Studio 2017 or later, or gcc-7 or clang-7 or later.

Other than the STL/CRT, Luau library components don't have external dependencies. 

# License

Luau implementation is distributed under the terms of [MIT License](https://github.com/Roblox/luau/blob/master/LICENSE.txt). It is based on Lua 5.x implementation that is MIT licensed as well.

When Luau is integrated into external projects, we ask to honor the license agreement and include Luau attribution into the user-facing product documentation. The attribution using [Luau logo](https://github.com/Roblox/luau/blob/master/docs/logo.svg) is also encouraged.

`Pack.lua` is also under the MIT license.

Luau 
====

Luau (lowercase u, /ˈlu.aʊ/) is a fast, small, safe, gradually typed embeddable scripting language derived from [Lua](https://lua.org).

This is not the original Luau repository.
Visit [Original Luau repository](https://github.com/Roblox/luau) for more information about LuaU.

The documentation portion for Luau language can be viewed at [Luau-Lang website](https://luau-lang.org/)

# Baselib documentation
[Is here (click)](https://luau-lang.org/library)

