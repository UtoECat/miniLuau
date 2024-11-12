MiniLuau
====

## ⚠️ HERE BE DRAGONS

This is an experimental WIP branch!
I'm currently working on a new and improved packing system that transforms the massive Luau source tree into a more manageable set of files.
Here’s what you need to know:
- **OLD**: The old version packed everything into just two files: `luau.hpp` and `luau.cpp`. 
  That worked, but not perfectly : 
  - it became painfully slow to recompile everything. 
  - accessing and modifying internal definitions and structures was a headache.
- **NEW**: The new system breaks things down into pairs of `.cpp` and `.hpp` files for each module (which correspond to directories in the Luau source tree). This means you’ll have a lot more files—16 in total!—but it’s designed to make your life easier in the long run.
  Additionally :
  - All comments are KEPT and all file indents are KEPT - makes packed code readable ans self documented. Comments do not affect compile times (on this scale).
  - Files are run trough clang-format after packing for better consistency and readability.
- **WARNING:** The produced source code is still untested for compilation and functionality.
  If you need something stable, please check out version 1.0 on the main branch.

## NOTES/TODO :
- Unfortunately, Analysis module still can't be compiled, i'm still thinking and working on ideas how to work around it...
- Need to setup or pack some minimal executable for runtime tesrting, better than old one.

# Overview

This repository implements the `PACK.lua` script for packing a large number of official Luau sources into a 
**significantly smaller** set of pairs of `.cpp` and `.hpp` files. You can easily integrate these files into
 your C++ project as needed.

The previous version only supported compiling `luau.hpp` and `luau.cpp`. However, real-world requirements
necessitated access to compiler and code generation internals, as well as support for packing type checker 
(analysis) sources. This led to the expansion to **16** files in total. 
This may seem *overwhelming*, but it's worth noting that other well-known "source packed" projects, such as
[ImGui](https://github.com/ocornut/imgui), utilize even more files.


The system remains flexible; you can exclude compiler, code generation, and type checking support by simply
 not downloading or using certain files.

# Integration

To use the files, place the necessary ones into your project and ensure it is compiled with a C++11 compiler 
or higher (C++17 or higher is required for the bytecode compiler, code generation, and type analysis).

## What Files Do What

Each file pair or triplet generally represents a single Luau source tree *module*. You can view the *dependency
tree* of modules (which modules you will need if you want to download specific files) inside the 
[PACK.lua](PACK.lua) file at line 193.
 Below is a simplified representation of the dependency structure (unnecessary lines and formatting have been removed):
```
local modules = {
  {
    name = "Common",
    deps = {}
  }, {
    name = "Ast",
    deps = {"Common"},
  }, {
    name = "Compiler",
    deps = {"Ast"}
  }, {
    name = "Config",
    deps = {"Ast"}
  }, {
    name = "Analysis",
    deps = {"Ast", "Config"}
  }, {
    name = "CodeGen",
    deps = {"Common", "VM"}
  }, {
    name = "VM",
    deps = {"Common"}
  }
}
```

As illustrated, the "Common" module is a prerequisite for all other modules.

# File Naming Conventions
Each module has associated `.cpp` and `.hpp` files, specifically:

- `luau_(modname).hpp`: External header files and API. Provides user-friendly (extern "C") interfaces.
- `luau_(modname)_int.hpp`: Internal headers. Contains internal data structures, functions, and various C++ standard library classes. These are rarely useful but you never know when you will need them :)
- `luau_(modname).cpp`: Implementation of internal and external APIs. For note, "Common" module does not have an associated .cpp file.

# Building
**Note: all source files are ALREADY AUTOMATICLY packed in this repository! Do manual packaging only if you really want to change this process in some way or speed up getting the latest version of Luau.**
If you want to package sources manually, you need to install:
- `Lua 5.4`
- `git` (or download zip)
- `clang-format` ???
- ensure unix `find` and `ls` utilities are available

First clone this repository with this command:
```sh
 $ git clone --recursive https://github.com/UtoECat/miniLuau
```
Then cd to the `luau` submodule directory and run
```sh
$ lua5.4 ../PACK.lua
```
During the build process, you will see errors about `<string.h>` or `<vector>` not found, this is normal.
At the end you will get your packaged luau sources inside `..` directory :)

# Usage
- Put packed files in your project, compile as any other source file
- Add `-lm` to your linker flags (if not done yet)
- include `luau_(modname).hpp` everywhere you need to work with lua.

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

