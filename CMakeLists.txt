set(CMAKE_BUILD_TYPE Release)
cmake_minimum_required(VERSION 3.0) # FIXME : 3.19 actually!

##
## Cunstomizable flags for this build
## LUAU_BUILD_SHARED - makes libraruies in shared mode, instead of static
## LUAU_TEST_EXEC - build test lua code runner utility (for testing)
## LUA_EXTERN_C - to make LuaAPI available in pure C without headache
##
option(LUAU_BUILD_SHARED "Build Shared libraries instead of static" OFF)
option(LUAU_BUILD_TEST_EXEC "Build luau-test executable" ON)
option(LUAU_EXTERN_C "Use extern C for all APIs" OFF)

project(Luau LANGUAGES CXX)
add_library(Luau.Common INTERFACE)

if(LUAU_BUILD_SHARED)
	add_library(Luau.Compiler SHARED)
	add_library(Luau.VM SHARED)
else()
	add_library(Luau.Compiler STATIC)
	add_library(Luau.VM STATIC)
endif()


target_include_directories(Luau.Common INTERFACE include/Luau)

target_compile_features(Luau.Compiler PUBLIC cxx_std_17)
target_include_directories(Luau.Compiler PUBLIC include/ src/Luau)
target_link_libraries(Luau.Compiler PUBLIC Luau.Common)

target_compile_features(Luau.VM PRIVATE cxx_std_11)
target_include_directories(Luau.VM PUBLIC include/ src/)
target_link_libraries(Luau.VM PUBLIC Luau.Common)

set(LUAU_OPTIONS)

if(MSVC)
    list(APPEND LUAU_OPTIONS /D_CRT_SECURE_NO_WARNINGS)
		list(APPEND LUAU_OPTIONS "/we4018") # Signed/unsigned mismatch
    list(APPEND LUAU_OPTIONS "/we4388") # Also signed/unsigned mismatch
else()
    list(APPEND LUAU_OPTIONS -Wall) # All warnings
    list(APPEND LUAU_OPTIONS -Wsign-compare)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND LUAU_OPTIONS /MP) # compilation across multiple cores
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Some gcc versions treat var in `if (type var = val)` as unused
    list(APPEND LUAU_OPTIONS -Wno-unused)
endif()

target_compile_options(Luau.VM PRIVATE ${LUAU_OPTIONS})

