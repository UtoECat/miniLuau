/*
 * Even when this one is for testing purposes only,
 * You can still use functions from this in your program, but
 * i am not 100% sure how much they are good.
 *
 * Copyright (C) UtoECat 2023
 * MIT License. No any warrianty 
 */
#include "extra.hpp"
#include <cstdio>
#include <cstdlib>
#include <cassert>

// this is a C++ SOURCE!!!

#include <string>
#include <exception>
#include <stdexcept>
#include <memory>

using Exception = ::std::runtime_error;

class ExpectedException : public std::exception {
	public :
	ExpectedException() = default;
	virtual ~ExpectedException() = default;
	virtual const char* what() const noexcept {
		return "This exception was expected";
	}
};

#include <vector>
// used by code below + in lib source
bool ignore_errors = false;
bool unsafe = false;
bool repl = true; // do REPL after all files ends?
bool bench_enabled = false; // do benchmark?
static std::vector<const char*> filenames;

static void initLuaState (lua_State* L) {
	if (!L) {
		throw Exception("Can't create Lua state!");
	}
	luaL_openlibs(L);
	luaopen_extra(L);
	if (unsafe) {
		// DO NOT SET REGISTRY DIRECTLY TO THE GLOBALS, IF
		// YOU ARE GOING TO ENABLE SANDBOX
		lua_newtable(L);
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		lua_rawsetfield(L, -2, "_REG"); 
		lua_rawsetfield(L, LUA_GLOBALSINDEX, "unsafe");
	}
	if (!unsafe) luaL_sandbox(L);
	// default values
	opts.optimizationLevel=1;
	opts.debugLevel=1;
}

extern int docodegen;

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
			case 'n' :
				ignore_errors = true;
			break;
			case 'b' :
				bench_enabled = true;
			break;
			case '-' :
				printf("reading from stdin is not supported :(\n");
				throw ExpectedException();
			case 'O' : {
				int kind = arg[2] - '0';
				if (kind < 0 || kind > 3)
					throw Exception("Bad optimisation level requested");
				if (kind == 3) {
					kind = 2;
					docodegen = true;
				} else {
					docodegen = false;
				}
				opts.optimizationLevel = kind;
			}
			break;
			case 'i':
				repl = true;
			break;
			case 'e':
				repl = false;
			break;
			case 'g' : {
				int kind = arg[2] - '0';
				if (kind < 0 || kind > 2)
					throw Exception("Bad debug level requested");
				opts.optimizationLevel = kind;
			}
			break;
			case 'u' :
				unsafe = true;
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
		fprintf(stderr, " not show this warning)\n");
	}
	if (unsafe) {
		fprintf(stderr, "Warning: Unsafe mode is enabled, loaded files");
		fprintf(stderr, " have full access to globals/registry, and ");
		fprintf(stderr, " sandbox is not enabled! (-u flag does this).\n");
	}
		if (filenames.size() == 0 && !repl && !bench_enabled) {
		throw Exception("Input files excepted to be given");
	}
}
int lua_Main(int argc, char** argv); 

#include <signal.h>

void invalidinstrction(int) {
	*((int*)0) = 36;
}
// Don't ask... Here be dragons...

int main(int argc, char** argv) {
	try {
		signal(SIGILL, invalidinstrction);
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

#define HAS_BENCH 1
#if HAS_BENCH == 1
#include "bench.h"
static void dobench(lua_State* L) {
	int i =	luau_load(L, "=bench", __bench_bcode, __bench_size, 0);
	assert(i == LUA_OK);
	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
		fprintf(stderr, "bench error: %s\n", lua_tostring(L, -1));
}
#endif

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
			filename, lua_tostring(L, -1) ?	lua_tostring(L, -1) : luaL_typename(L, -1)
		);
		if (!ignore_errors) {
			throw ExpectedException();
		}
	}
	lua_settop(L, 0);
}

// REPL part
#include <linenoise.h>
#include <string.h>

static void doREPL(lua_State* L) {
	std::string buffer;
	int oldtop = lua_gettop(L);

	// tracebacking...
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2); // leave traceback function at the 'top'


	for(;;) {
		bool show_result = false;
		assert(lua_gettop(L) == 1);
		char* tmp = linenoise(buffer.empty() ? "luau> " : "   ->>");
		if (!tmp) continue;
		buffer += tmp;
		linenoiseFree(tmp);

		// try load this code
		if (buffer.compare("quit") == 0) break;
		int status = luaL_loadbufferx(L, buffer.c_str(), buffer.size(), "=stdin", 0);
		// try return result at first
		if (status != LUA_OK) {
			show_result = true;
			std::string tmp = "return ";
			tmp += buffer;
			status = luaL_loadbufferx(L, tmp.c_str(), tmp.size(),
					"=stdin", 0);
			if (status != LUA_OK) {
				show_result = false;
				lua_pop(L, 1); // remove new message
			} else lua_remove(L, -2); // remove old message
			assert(lua_gettop(L) == 2);
		}
		// check message again
		if (status != LUA_OK) {
			const char* msg = lua_tostring(L, -1);
			if (msg && strstr(msg, "<eof>") != NULL) {
				lua_pop(L, 1); // just need more input
				continue;
			}
			if (!msg) msg = "unknown error!";
			fprintf(stderr, "cmp : %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
			buffer.clear();
			continue;
		}
		// well done. Execute it now!
		linenoiseHistoryAdd(buffer.c_str());
		if (lua_pcall(L, 0, 1, 1) != LUA_OK) {
			// error
			fprintf(stderr, "lua : %s", lua_tostring(L, -1));
		} else {
			if (show_result || !lua_isnoneornil(L, -1)) {
				luaL_serialize(L, -1, false);
				fprintf(stderr, " %s\n", lua_tolstring(L, -1, nullptr));
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 1);
		buffer.clear();
	}
	// remove temporary stuff
	lua_settop(L, oldtop);
};

/*
void compileFiles() {
	std::string buff;
	for (auto& fname : filenames) {
		std::string source;
		if (loadFileContent(L, fname, &source) != LUA_OK) {
			fprintf(stderr, "%s", lua_tostring(L, -1));
			if (!ignore_errors) throw ExpectedException();
		};
		size_t len = 0;
		// check for compile errors
		char* c = luau_compile(source.c_str(), source.size(), opts, &len);
		if (luau_load(L, "?", c, len, 0) != LUA_OK) {
			fprintf(stderr, "%s", lua_tostring(L, -1));				
			if (!ignore_errors) throw ExpectedException();
		}
		lua_pop(L, 1); // don't need this
		buff.append(c, len); // append bytecode
		free(c);
	}
} */

int lua_Main(int argc, char** argv) {
	// init
	RaiiLua state;
	lua_State *L = state.L;
	parseargs(argc, argv);
	initLuaState(L);
	
	#if HAS_BENCH==1
	if (bench_enabled) dobench(L);
	#endif

	// execute files
	lua_settop(L, 0);
	for (auto& fname : filenames) {
		execfile(L, fname);
	}
	if (repl) doREPL(L);
	
	// no lua_close() here, it will be RAII'd!
	return 0;
}
