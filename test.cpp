/*
 * Even when this one is for testing purposes only,
 * You can still use functions from this in your program, but
 * i am not 100% sure how much they are good.
 * MIT License. No any warrianty 
 */
#include "lua.h"
#include "lualib.h"
#include "luacode.h"
#include <cstdio>
#include <cstdlib>

// this is a C++ SOURCE!!!

#include <string>

struct CodeBuffer {
	char* data;
	size_t len;
	char* name;
};

/*
 * RAW. Must not be used directly.
 */
static int rawloadcode(lua_State* L, CodeBuffer& B, int env) {
	size_t len = 0;
	RAILfree<char*> bcbuff(luau_compile(B.data, B.len, &opts, &len));
	return luau_load(L, B.name, bcbuff, len, 0);
}

/*
 * dostring() in save enviroment and separate corutine instead of global one
 */
static int safedocode(lua_State* L, CodeBuffer& B) {
	// create new coroutine from main threrad with protected enviroment
	lua_State* ML = lua_newthread(lua_mainthread(L));
	luaL_sandboxthread(ML);

	int status = rawloadcode(ML, B, env);
	if (status != LUA_OK) {
		lua_xmove(ML, L, 1); // move error to main thread
		lua_error(L); // throw again
	}
	status = lua_resume(ML, L, 0); // call
	if (status == LUA_YIELD) {
		luaL_error("attempt to yield from isolated coroutine");
	} else if (status == LUA_OK) {
		
	} else {
		lua_error(L);
	}
}

/*
 * Loads code in separate enviroment
 */
static int lua_safeloadcode(lua_State* L, const char* code, size_t len, const char* name) {
	
}


template <typename T, bool USE_DELETE = false>
class RAILfree {
	T ptr = nullptr;
	public :
	RAILfree() = default;
	RAILfree(T p) : ptr(p) {}
	~RAILfree() {
		if (USE_DELETE) {
			delete[] ptr;
		} else free(ptr);
	}
	RAILfree(const RAILfree&) = delete;
	RAILfree(RAILfree&&) = default;
	const RAILfree& operator=(T p) {
		ptr = p;
		return *this;
	}
	operator T() {
		return ptr;
	}
};

struct LoadRes {
	const char* filename  = nullptr;
	std::string chunkname = "=";
	RAILfree<char*, true> codebuff;
	size_t len = 0;
};

static lua_CompileOptions opts = {0};

static int doloadfile(lua_State* L, struct LoadRes& res) {
	const char* filename = res.filename;
	FILE* f = fopen(filename, "r");
	if (!f) {
		lua_pushstring(L, "Can't open file!");
		return LUA_ERRRUN;		
	};
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);

	// allocate buffer to load file source
	try {
		res.codebuff = new char[len + 2];
		if (!((char*)res.codebuff)) throw "h";
	} catch (...) {
		lua_pushstring(L, "NOMEM");
		return LUA_ERRMEM;
	}

	fseek(f, 0, SEEK_SET);
	int result = fread((char*)res.codebuff, 1, len, f);
	if (result < len - 5) {
		lua_pushstring(L, "Can't read file after opening.");
		return LUA_ERRRUN;
	}
	fclose(f);
	res.len = len;
	return LUA_OK;
}

static int getenv(lua_State* L) {
	if (lua_istable(L, 3)) return 3;
	return 0;
}

static int Bloadfile(lua_State* L) {
	const char* name = luaL_checkstring(L, 1);
	struct LoadRes res = {name, "=" + (std::string)name};
	int env = getenv(L);

	int result = doloadfile(L, res);
	if (result == LUA_OK) { // file loaded? Nice, load code now
		result = loadcode(L, res.codebuff, res.len, res.chunkname.c_str(), env);
	}
	if (result != LUA_OK) {
		// swap args => [nil, errmsg]
		if (lua_tostring(L, -1) == nullptr) {
			lua_pushstring(L, "unknown error");
		}
		lua_pushnil(L);
		lua_insert(L, -2);
 		return 2;
	} else {
		lua_pushnil(L);
		return 2;
	};
}

#include <assert.h>

static int Bloadstring(lua_State* L) {
    size_t l = 0;
    const char* s = luaL_checklstring(L, 1, &l);
    const char* chunkname = luaL_optstring(L, 2, s);
		int env = getenv(L);

    //lua_setsafeenv(L, LUA_ENVIRONINDEX, false);
		if (loadcode(L, s, l, chunkname, env) == LUA_OK) {
			return 1; // nice
		}
    lua_pushnil(L);
    lua_insert(L, -2); // put before error message
    return 2;          // return nil plus error message
}

int main(int argc, char** argv) {
	lua_State* L = luaL_newstate();	
	luaL_openlibs(L);
	lua_pushcfunction(L, Bloadfile, "loadfile");
	lua_setfield(L, LUA_GLOBALSINDEX, "loadfile");
	lua_pushcfunction(L, Bloadstring, "loadstring");
	lua_setfield(L, LUA_GLOBALSINDEX, "loadstring");
	luaL_sandbox(L);
	const char* filename = nullptr;
	opts.optimizationLevel=2;
	opts.debugLevel=1;

	if (argc > 1) {
		if (argv[1][0] == '-' && argv[1][1] == 'O') {
			int opt = argv[1][2] - '0';
			if (opt > 2 || opt < 0) {
				fprintf(stderr, "bad optimisation level!\n");
				return -1;
			}
			if (argc > 2) filename = argv[2];
			else goto nofile;
		} else filename = argv[1];
	} else {
		nofile:
		fprintf(stderr, "Error : file argument excepted!\n");
		fprintf(stderr, "Error : retrying eith test.lua...\n");
		filename = "test.lua";
	}
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	lua_pushcfunction(L, Bloadfile, ".");
	lua_pushstring(L, filename);
	lua_call(L, 1, 2);
	if (!lua_isfunction(L, -2)) {
		fprintf(stderr, "Can't load file : %s\n", lua_tostring(L, -1));
		return -1;		
	};
	lua_pop(L, 1); // pop nil
	//assert(lua_isfunction(L, -2) && lua_isfunction(L, -1));
	if (lua_pcall(L, 0, 0, -2) != LUA_OK) {
		fprintf(stderr, "Execution error %s\n", lua_tostring(L, -1));
	};
	lua_close(L);
	return 0;
}
