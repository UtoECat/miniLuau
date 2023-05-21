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

if(LUAU_BUILD_TEST_EXEC)
	add_executable(Luau.Test)
	set_target_properties(Luau.Test PROPERTIES OUTPUT_NAME luau-test)
	target_compile_options(Luau.Test PRIVATE ${LUAU_OPTIONS})
	target_compile_features(Luau.Test PRIVATE cxx_std_11)
	target_include_directories(Luau.Test PUBLIC include/)
	target_sources(Luau.Test PRIVATE test.cpp)
	target_link_libraries(Luau.Test PRIVATE Luau.Compiler Luau.VM)
endif()

if(LUAU_EXTERN_C)
    # note that we enable LUA_USE_LONGJMP=1 as well;
		#	otherwise functions like luaL_error will throw C++ 
		# exceptions, which can't be done from extern "C" functions
    target_compile_definitions(Luau.VM PUBLIC LUA_USE_LONGJMP=1)
    target_compile_definitions(Luau.VM PUBLIC LUA_API=extern\"C\")
    target_compile_definitions(Luau.Compiler PUBLIC LUACODE_API=extern\"C\")
endif()

target_sources(Luau.Common PRIVATE 
	include/Luau/Common.h
	include/Luau/Bytecode.h
  include/Luau/DenseHash.h
  include/Luau/ExperimentalFlags.h
)

target_sources(Luau.Compiler PRIVATE
	include/Luau/BytecodeBuilder.h
	include/Luau/Compiler.h
	include/luacode.h

	# AST is here now... yeah...
	src/Luau/Ast.cpp
	src/Luau/Confusables.cpp
	src/Luau/Lexer.cpp
	src/Luau/Location.cpp
	src/Luau/Parser.cpp
	src/Luau/StringUtils.cpp
	src/Luau/TimeTrace.cpp

	src/Luau/BytecodeBuilder.cpp
	src/Luau/Compiler.cpp
	src/Luau/Builtins.cpp
	src/Luau/BuiltinFolding.cpp
	src/Luau/ConstantFolding.cpp
	src/Luau/CostModel.cpp
	src/Luau/TableShape.cpp
	src/Luau/ValueTracking.cpp
	src/Luau/lcode.cpp
	src/Luau/Builtins.h
	src/Luau/BuiltinFolding.h
	src/Luau/ConstantFolding.h
	src/Luau/CostModel.h
	src/Luau/TableShape.h
	src/Luau/ValueTracking.h
)

target_sources(Luau.VM PRIVATE
    include/lua.h
    include/luaconf.h
    include/lualib.h

    src/lapi.cpp
    src/laux.cpp
    src/lbaselib.cpp
    src/lbitlib.cpp
    src/lbuiltins.cpp
    src/lcorolib.cpp
    src/ldblib.cpp
    src/ldebug.cpp
    src/ldo.cpp
    src/lfunc.cpp
    src/lgc.cpp
    src/lgcdebug.cpp
    src/linit.cpp
    src/lmathlib.cpp
    src/lmem.cpp
    src/lnumprint.cpp
    src/lobject.cpp
    src/loslib.cpp
    src/lperf.cpp
    src/lstate.cpp
    src/lstring.cpp
    src/lstrlib.cpp
    src/ltable.cpp
    src/ltablib.cpp
    src/ltm.cpp
    src/ludata.cpp
    src/lutf8lib.cpp
    src/lvmexecute.cpp
    src/lvmload.cpp
    src/lvmutils.cpp

    src/lapi.h
    src/lbuiltins.h
    src/lbytecode.h
    src/lcommon.h
    src/ldebug.h
    src/ldo.h
    src/lfunc.h
    src/lgc.h
    src/lmem.h
    src/lnumutils.h
    src/lobject.h
    src/lstate.h
    src/lstring.h
    src/ltable.h
    src/ltm.h
    src/ludata.h
    src/lvm.h
)

