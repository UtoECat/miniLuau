/*
 * Even when this one is for testing purposes only,
 * You can still use functions from this in your program, but
 * i am not 100% sure how much they are good.
 *
 * Copyright (C) UtoECat 2023
 * MIT License. No any warrianty 
 */
#include "lua.h"
#include "lualib.h"
#include "luacode.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>

// this is a C++ SOURCE!!!

#include <string>
#include <exception>
#include <stdexcept>
#include <cstdarg>
#include <memory>

const std::string strformat(const char * const fmt, ...) {
    va_list args, args2;

    va_start(args, fmt);
    va_copy(args2, args);

    const int len = std::vsnprintf(NULL, 0, fmt, args2);
    va_end(args2);

		if (len <= 0) throw "formatting error";
		auto tmp = std::make_unique<char[]>(len+1);

    std::vsnprintf(tmp.get(), len+1, fmt, args);
    va_end(args);
    return std::string(tmp.get(), len+1);
}

using Exception = ::std::runtime_error;

class ExpectedException : public std::exception {
	public :
	ExpectedException() = default;
	virtual ~ExpectedException() = default;
	virtual const char* what() const noexcept {
		return "This exception was expected";
	}
};

extern int luaopen_extra(lua_State *L);

#include <vector>
// used by code below + in lib source
extern lua_CompileOptions opts;
bool ignore_errors = false;
static std::vector<const char*> filenames;

static void initLuaState (lua_State* L) {
	if (!L) {
		throw Exception("Can't create Lua state!");
	}
	luaL_openlibs(L);
	luaopen_extra(L);
	luaL_sandbox(L);
	// default values
	opts.optimizationLevel=1;
	opts.debugLevel=1;
}


static void parseargs (int argc, char** argv) {
	for (char** p = argv+1; p < argv + argc; p++) {
		const char* arg = p[0];
		if (arg[0] == '-') switch (arg[1]) {
			case '\0':
				throw Exception("Flag type excepted after character '-'");
			break;
			case 'h' : 
				printf("No help 4u <3\n");
				throw ExpectedException();
			case 'v' :
				printf("lua version is 5.1 (LuaU)\n");
				break;
			case 'c' :
				ignore_errors = true;
			break;
			case '-' :
				printf("reading from stdin is not supported :(\n");
				throw ExpectedException();
			case 'O' : {
				int kind = arg[2] - 0;
				if (kind < 0 || kind > 2)
					throw Exception("Bad optimisation level requested");
				opts.optimizationLevel = kind;
			}
			break;
			case 'g' : {
				int kind = arg[2] - 0;
				if (kind < 0 || kind > 2)
					throw Exception("Bad debug level requested");
				opts.optimizationLevel = kind;
			}
			break;
			default  :
				throw Exception(strformat("Option %c is not supported", arg[1]));
		} else {
			filenames.push_back(arg);
		}
	}
	if (opts.optimizationLevel == 2 && opts.debugLevel > 1) {
		fprintf(stderr, "Warning: Debug information may be useless with");
		fprintf(stderr, " optimization level 2. (Run with -g0 or -O1 to");
		fprintf(stderr, " not show this warning)");
	}
	if (filenames.size() == 0) {
		throw Exception("Input files excepted to be given");
	}
}
int lua_Main(int argc, char** argv); 

int main(int argc, char** argv) {
	try {
		return lua_Main(argc, argv);
	} catch (ExpectedException& e) {
		return 0; // all is OK
	} catch (std::exception& e) {
		fprintf(stderr, "Fatal : %s!\n", e.what());
		return -1;
	} catch (...) {
		fprintf(stderr, "Fatal : Unknown exception type!\n");
		return -1;
	}
	return 0;
}

typedef struct RAIISTATE {
	lua_State* L;
	RAIISTATE() {L = luaL_newstate();}
	~RAIISTATE() {lua_close(L);}
} RaiiLua;


static void execfile(lua_State* L, const char* filename) {
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2); // leave traceback function at the 'top'
	assert(lua_gettop(L) == 1);

	// function to be called
	lua_getfield(L, LUA_GLOBALSINDEX, "dofile");
	lua_pushstring(L, filename);
	if (lua_pcall(L, 1, 0, -3) != LUA_OK) {
		fprintf(stderr, "%s : Can't execute %s : %s\n", 
			ignore_errors ? "Warning" : "Error",
			filename, lua_tostring(L, -1)			
		);
		if (!ignore_errors) {
			throw ExpectedException();
		}
	}
}

int lua_Main(int argc, char** argv) {
	// init
	RaiiLua state;
	lua_State *L = state.L;
	initLuaState(L);
	parseargs(argc, argv);
	
	// execute files
	lua_settop(L, 0);
	for (auto& fname : filenames) {
		execfile(L, fname);
	}
	
	// no lua_close() here, it will be RAII'd!
	return 0;
}
