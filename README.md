MiniLuau
====
## ⚠️ HERE BE DRAGONS

This is an experimental WIP branch!
I'm currently working on a new and improved packing system that transforms the massive Luau source tree into a more manageable set of files.
Here’s what you need to know:

### OLD
The old version packed everything into just two files: `luau.hpp` and `luau.cpp`.
That worked, but not perfectly:
- it became painfully slow to recompile everything.
- accessing and modifying internal definitions and structures was a headache.

### NEW
The new system breaks things down into pairs of `.cpp` and `.hpp` files for each module (which correspond to directories in the Luau source tree).
This means you’ll have a lot more files—16 in total!—but it’s designed to make your life easier in the long run.
Additionally:
- All comments are kept, and all file indents are kept - makes packed code readable and self-documented. Comments do not affect compile times (on this scale).
- Files are run through clang-format after packing for better consistency and readability.
- Packing sources of some parts of the Luau CLI toolchain is possible now! 

### WARNING
The produced source code is still not well tested for compilation and functionality.
If you need something stable, please check out version 1.0 on the main branch.

## NOTES/TODO
- [+] CLI Tools: Compiler and Repl sources packed and verified to build and seemingly run well
- [ ] Unfortunately, the Analysis module still can't be compiled, I'm still thinking and working on ideas how to work around it...
- [ ] Need to set up or pack some minimal executable for runtime testing, better than the old one.
- [ ] Check 2 can't be done with default luau's test suite, because it retuires analysis module be kept up and ready + has it's own shenanigans.

# Overview

This repository implements the `PACK.lua` script for packing a large number of official Luau sources into a
**significantly smaller** set of pairs of `.cpp` and `.hpp` files. You can easily integrate these files into
your C++ project as needed.

Script supports building only a specific module by its name, a complete list of modules can be found at the top of the script.
By default, only Luau sources for *integration into another application* are packed, including VM, Codegen, Ast.
Standalone CLI tools are available for packing too, but only when requested!

> The previous version only supported compiling `luau.hpp` and `luau.cpp`. However, real-world requirements
> necessitated access to compiler and code generation internals, as well as support for packing type checker
> (analysis) sources. This led to the expansion to **16** files in total.
> This may seem *overwhelming*, but it's worth noting that other well-known "source packed" projects, such as
> [ImGui](https://github.com/ocornut/imgui), utilize even more files.
> The system remains flexible; you can exclude compiler, code generation, and type checking support by simply
> not downloading or using certain files.


# Tutorial

## Step 1 - Get or Generate Packed Sources

### Option 1 - Download from GitHub

Download the archive zip folder or clone this repository in your terminal. 
Luau sources, including CLI sources, are periodically automatically updated and repacked here. 

> $ git clone --recursive https://github.com/UtoECat/miniLuau

### Option 2 - Use Script to Pack Luau Sources

For your customized setups or to pack a more recent version/patched version of Luau sources.

> **Hint**: You mostly would want to stick to a specific Luau version and not run for the new shiny version every time. 
> Luau is constantly evolving, and while it may not break your stuff, it will likely break this script for sure :) 
> So I **DO NOT RECOMMEND** using this script as part of an automated build system for getting the latest and greatest Luau version! 

Requirements :
- Unix/Linux/Posix OS.
- Find, ls utilities awailable
- clang-format and gcc compiler installed (form the formatting and checks)

Usage: Change directory to the `luau` submodule directory and run:

> ```
> $ lua5.4 ../PACK.lua
> ```

This should produce output in the `../pack-out` directory.

If you want to build specific module(s):

> ```
> $ lua5.4 ../PACK.lua MODNAME1 MODNAME2 ... MODNAMEN
> ```

You only need to specify the final module names you want; all dependencies will be resolved and packed in the correct order automatically.


### Q: What Does the Term Module Mean?

A module is an individual set of source and header files from the Luau source tree. 
The output of a module (packed sources, which is what you are interested in) is usually in `luau_(modname).cpp`, 
`luau_(modname).hpp`, and `luau_(modname)_int.hpp` files. 

- `luau_(modname).hpp`: External header files and API. Provides public interfaces, functions and methods.
- `luau_(modname)_int.hpp`: Internal headers. Contains internal data structures, functions, and various C++ standard library classes. These are rarely useful, but you never know when you will need them :)
- `luau_(modname).cpp`: Implementation of internal and external APIs. Note that the "Common" module does not have an associated .cpp file.


### Q: What Files Do I Need?

Generally, here are the options:
For embedding into your app:
  - Minimal: Common, Ast, VM modules
  - With the ability to compile scripts at runtime: +Compiler module
  - With native code generation for performance boost on supported platforms: +Codegen module

CLI Tools are a little more difficult.
The easiest way to find out dependencies is to look into the `Pack.lua` script module table and `deps` fields.
Another way is to follow `#include` statements at the top of the files you are interested in until you find all the modules you need.

Also, you *can* just download everything, but beware to compile all `luau_cli_*.cpp` files (except `luau_cli_base.cpp`) separately
(or you will get linker errors about duplicated main symbols).
In cases where you don't want CLI tools, simply remove all files with `cli` in their names :)


## Step 2A: Embedding/Integration

So, you want to embed Luau.
Step one - put the necessary files into your project.
I recommend keeping `*_int.hpp` as private headers and `*.hpp` as public API headers, but you may dump everything in one directory,
even with source files.

Step 2 - ensure they are compiled with at least a C++11 compiler or higher (C++17 or higher is required 
for the bytecode compiler, code generation, and type analysis).
Ensure you have exceptions enabled as well.

For foreign interfaces, although I'm not recommending doing that, you will need to change/add some definitions in `vm_int.hpp` and `vm.hpp`.
Refer to makefiles for that; I don't remember them, and I haven't used them seriously.

## Step 2B: Building Luau Toolchain

With this script, there is optional support for packing and building parts of the CLI toolchain too!

Example for CLI Compiler:

> ```
> $ cd luau
>
> # Asking to pack a specific module with all dependencies
> $ lua5.4 ../PACK.lua CLICompiler
> $ cd ../pack-out
>
> # Build it
> $ g++ luau_cli_compiler.cpp luau_cli_base.cpp luau_ast.cpp luau_codegen.cpp luau_compiler.cpp luau_vm.cpp -o cli_compiler -I. 
>
> $ ./cli_compiler --help
> Usage: ./cli_compiler [--mode] [options] [file list]
>
> Available modes:
>    binary, text, remarks, codegen
> ...
> ```

Example for REPL (interactive command prompt):
> ```
> $ cd luau
>
> # Asking to pack a specific module with all dependencies
> $ lua5.4 ../PACK.lua CLIRepl
> $ cd ../pack-out
>
> g++ -o out_repl -I. luau_ast.cpp luau_cli_base.cpp luau_cli_repl.cpp luau_codegen.cpp luau_compiler.cpp luau_config.cpp luau_isocline.cpp luau_vm.cpp 
> ```

Testing utility and Analysis utility are cannot be packed into the correct c++ code yet.

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

