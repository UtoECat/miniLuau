/*
 * A headeronly require function implementation for Luau. 
 * Copyright (C) UtoECat 2023
 * MIT License. No any warrianty 
 */

#include "lua.h"
#include "lualib.h"

// can load lua files?
#if LUA_REQUIRE_HASDOFILE
/*
 * WARNING: Your dofile function MUST follow this specification :
 * ([] is optional arguments)
 * function dofile(name:string[, chunkname:string, table:enviroment]) : any
 * returns as many values, as code in the file returns.
 * throws error if can't load/execute file.
 * FILE MUST BE LOADED AND EXECUTED IN SEPARATE PROTECTED COROUTINE.
 * IT'S IMPORTANT REQUIREMENT. SEE BELOW.
 */
int luaB_dofile(lua_State* L);
#else
#warning no luaB_dofle() function specified. \
require will not be able to load modules from files, only use preloaded ones.
#endif

/*
 * Initializes REGISTRY for this module.
 * And loads (safe)require() and package.* modules into lua.
 * you can't just use this require() in your lua code outside of C require() Call!
 * Read below (in implementation) for more info.
 */
int luaopen_package(lua_State* L);

/* Allows and calls require from C API.
 * Why we have this and need this? Because of security.
 * Read below (in implementation) for more info.
 * 
 * This function throws error if can't load specified module. So
 * wrap it into lua_Cfunction and call through pcall() if you need to
 * catch this error in place. 
 *
 * Returns 1.
 */
int luaR_require(lua_State* L, char* modname, char* mode);

// wrapper around luaR_require to make it possible to push this into
// lua. WARNING : DO NOT GIVE THIS FUNCTION IN ANY WAY FOR UNSAFE CODE!
// it's not pushed into lua by default at all. Do this manually, and ONLY if
// you really need it. (you can simply call luaR_require from CAPI :D)
int luaB_unsafeRequire(lua_State* L);

#ifdef LUA_REQUIRE_IMPLEMENTATION
/*
 * Useful functions for require
 */
static int luaL_getsubtable (lua_State *L, int idx, const char *fname) {
  if (lua_getfield(L, idx, fname) == LUA_TTABLE) return 1; // ok
	lua_pop(L, 1); // remove getfield result
	idx = lua_absindex(L, idx);
	assert(!lua_getreadonly(L, idx));
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setfield(L, idx, fname); // assign into table
	// returns +1 table on the stack
	return 0; // table was not exist.
}

/*
 * Fos safe enviroments it's important to dissalow unsafe code to
 * require lua files like libraries, that may be required later
 * in safe code. + Safe code sometimes may want to require something
 * to be used by himself only, not shared with unsafe...
 * 
 * As result, we need ability to require not only globally, but
 * locally too. This makes implementation (and usage) a bit more
 * complex, but that's nicer than nothing.
 * Require mode to require locally must be default one.
 *
 * REQUIRE implementation.
 *
 * Require uses now not only table in REGISTRY to store loaded modules, but
 * many tables at many levels :
 * _LOADED  - readed only. Contains loaded libs from CAPI. (ONLY IN WRAPPER)
 * _REQENV?  - read/write. Contains [COROUTINE] = Loaded table.
 * _MODULES? - read/write. Contains unsafe globally loaded libraries
 *
 * (p.s marked with ? mark means that they may be implemented in other way)
 *
 * Require specification :
 * function require(modname : string [, mode : string]) : any
 *
 * mode here can be "local"(default) or "global" and it's specifies how
 * loaded module should be visible (locally or globally).
 *
 * When module is already required, mode, if specified, may limit way where
 * require will search for this module. In other case all (except C) loaded
 * tables are checked, starting from local.
 *
 * Additional info :
 * _REQENV is WEAK (__mode="kvs" table.)
 *
 * Q: Why dofile() must execute and load script in coroutine?
 * A: As you can see, to make it possible for local requiring to work we
 * need to get ENVIROMENT table (aka GLOBALS), BUT it's gonna be really
 * bad to do some getfenv() shit from CAPI. SO, BECAUSE OF THAT, WE USE
 * COROUTINES IN DOFILE(), that may be created with different GLOBALS
 * table in Luau. 
 */
/*
 * BRAINSTORM :
 * Of course, that makes string in loadstring() and other stuff calling
 * require inside load*() functions (that NOT USES COROUTINE - UNSAFE).
 * Also, when code is loaded, it's functions NO MORE EXECUTED IN COROUTINE,
 * so that WILL be a bad security shit.
 *
 * Q: Is there any simple and efficient workaround for this?
 * A: Yes. Kinda. Here it is :
 * Require can be splitted down into two functions :
 * - Require wrapper : executed in safe enviroment to ALLOW and START
 *   actual require().
 * - Require worker  : executed by starter AND BY ALL REQUIRED SCRPTS
 * DIRECTLY. If script will try to run this function again (outside
 * of requireStart() call), it will throw error about it and do nothing.
 * 
 * Of course, this means that we need to somehow track this fact.
 * Flag about is require allowed(_REQSTAT) will be INCREASED in REGISTRY by 1
 * before call to actual require, and will be decreased after return.
 *
 * This is also why script cannot yield during require (btw, yielding
 * across require makes no sense).
 *
 * Require wrapper MUST protect first actual call, return loaded module
 * in case of OK, rethrow error in case of ERROR. And in both cases, MUST
 * DECREASE _REQ_STAT!
 */
#define LUA_REQUIRE "_REQUIRE"
#define LUA_REQSTAT "_REQSTAT"

// this index is reserved and MUST NOT be used as module name
#define LUA_LOADERSNAME "_LOADERS" 

enum {
	LOADED_UNSAFE = -1, // PSEUDO. (LOCAL|GLOBAL)
	LOADED_C      = 0, // Real && Pseudo (C|LOCAL|GLOBAL)
	LOADED_LOCAL  = 1,
	LOADED_GLOBAL = 2
};

/* Since it seems useless to check for C modules only, 
 * they are turns to PSEUDO (union of LOCAL && GLOBAL) in
 * loaded-specific functions
 */

static void makelocaltbl(lua_State* L, int idx) {
	idx = lua_absindex(L, idx);
	lua_createtable(L, 0, 1); // we will insert 1 module soon...
	if (luaL_newmetatable(L, "_localrequiremeta")) { // meta for local mode
		// for loaders we will also check in local enviroments table
		lua_pushstring(L, "__index");
		lua_pushvalue(L, idx);
		lua_rawset(L, -3);
		lua_setreadonly(L, -1, 1);
	}
	lua_setmetatable(L, -2);
}

// RAW. Not checks is that ok to return C loaded table.
// Not supports pseudo indicies.
// returns loaded table, or creates, inserts and returns it.
static void luaR_pushloaded(lua_State* L, int kind) {
	lua_getfield(L, LUA_REGISTRYINDEX, LUA_REQUIRE);
	lua_rawgeti(L, -1, kind);
	lua_remove(L, -2); // don't need require internal anymore
	if (kind == LOADED_LOCAL) { // hard one
		lua_pushthread(L); // key
		if (lua_rawget(L, -2) != LUA_TTABLE) {
			lua_pop(L, 1); // pop nil
			lua_pushthread(L); // key
			makelocaltbl(L, -2);
			lua_rawset(L, -3);
		};
		lua_remove(L, -2); // we don't need loaded table anymore
	}
	assert(lua_istable(L, -1)); // hmmm
}

/*
 * LOADED_UNSAFE sets check range [LOADED_LOCAL .. LOADED_GLOBAL]
 * LOADED_C      sets check range [LOADED_C     .. LOADED_GLOBAL]
 * LOADED_LOCAL  sets single-value LOADED_LOCAL range
 * LOADED_GLOBAL sets single value LOADED_GLOBAL range
 */
static void pseudocheck(int mode, int *start, int* end) {
	*start = mode;
	*end   = LOADED_GLOBAL;
	// check for pseudo
	if (mode == LOADED_LOCAL) *end = LOADED_LOCAL; // check only local loaded
	if (mode == LOADED_UNSAFE) *start = LOADED_LOCAL; 
	assert(mode >= LOADED_UNSAFE && mode <= LOADED_GLOBAL);
}

/*
 * Checks is given module was loaded at modes mode.
 */
static int luaR_isLoaded(lua_State* L, const char* modname, int mode) {
	#ifndef NDEBUG
	int top = lua_gettop(L);
	#endif
	int kind, max;
	pseudocheck(mode, &kind, &max);

	// make actual checks
	for (; kind <= max; kind++) {
		luaR_pushloaded(L, kind);
		int t = lua_getfield(L, -1, modname);
		lua_remove(L, -2); // remove loaded table
		if (t != LUA_TNONE && t != LUA_TNIL) {
			return 1; // module is already loaded
		}
		lua_pop(L, 1); // remove nil
	}
	#ifndef NDEBUG
	assert(lua_gettop(L) == top);
	#endif
	return 0; // push nothing, and nothing was found :(
}

/*
 * Run loaders for specified mode
 */
static int luaR_runLoaders(lua_State* L) {
	if (lua_getfield(L, 2, LUA_LOADERSNAME) != LUA_TTABLE) {
		luaL_error(L, "no loaders table found!");				
	};
	lua_remove(L, 2);

	int i = 1;
	for (;lua_rawgeti(L, -1, i) == LUA_TFUNCTION; i++) {
		lua_pushvalue(L, 1); // push module name
		if (lua_pcall(L, 1, 1) == LUA_OK) {
			return 1;
		} else luaL_error(L, "loader[%i] : %s", i, lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	luaL_error(L, "load failed");
}

static const char* mode_name[LOADED_GLOBAL + 1] = {"C", "local", "global"};	

/*
 * Loads given module using loaders function at modes mode.
 */
static int luaR_loadModule(lua_State* L, const char* modname, int mode) {
	int kind, max;
	pseudocheck(mode, &kind, &max);
	luaL_Buffer B; // loaders error log
	luaL_buffinit(L, &B);

	// do loaders
	for (; kind <= max; kind++) {
		lua_pushfstring(L, " Loader error for mode '%s' : \n\t", mode_name[kind]);
		luaL_addvalue(&B);
		lua_pushcfunction(L, luaR_runLoaders, "runLoaders");
		lua_pushstring(L, modname); // 1
		luaR_pushloaded(L, kind);   // 2
		if (lua_pcall(L, 2, 1, 0) == LUA_OK && lua_isfunction(L, -1)) {
			return 1; // return compiled && loaded module :D
		};
		luaL_addvalue(&B); // add error message
		luaL_addstring(&B, "\n"); // add newline
	}
	luaL_pushresult(&B);
	lua_pushfstring(L, "Can't load module %s : \n", tostring(L, -1));
	return -1;
}

static const char* req_opts[] = {"unsafe", "safe", "local", "global", nullptr};

#include <string.h>
/* accept LOADED_UNSAFE and LOADED_C in def ONLY!*/
static int luaR_checkmode(lua_State* L, char* mode, int def) {
	int opt = 0; mode = mode ? mode : "local";

	// search
	while (req_opts[opt]) {
		if (strcmp(req_opts[opt], mode) == 0) break;
		opt++;
	};
	if (!req_opts[opt]) luaL_error(L, "bad require mode %s", mode);

	opt--;
	if (LOADED_UNSAFE && opt == LOADED_C)
		 luaL_error(L, "unsafe code cannot require %s modules!", "C");
	return opt;
}

static int luaR_status(lua_State* L, int incr) {
	lua_getfield(L, LUA_REGISTRYINDEX, LUA_REQSTAT);
	int stat = lua_tointeger(L, -1); lua_pop(L, 1);
	if (incr == 0) return stat;
	stat += incr;
	assert(stat >= 0);
	lua_pushinteger(L, stat);
	lua_setfield(L, LUA_REGISTRYINDEX, LUA_REQSTAT);
	return !!stat;
}

/* actual require worker.
 * retutns -1 at error, nresults otherwise */
static int luaR_dorequire(lua_State* L, char* modname, int mode) {
	if (!luaR_status(L, 0)) {
		lua_pushstring(L, "require() is called outside of safe coroutine. "
			"Make require() calls in the main script function, not later in the code."
		); return -1;
	}
	// can't use _LOADERS as module name
	if (strcmp(modname, LUA_LOADERSNAME) == 0) {
		lua_pushstring(L, "can't use '_LOADERS' as a module name!");
		return -1;
	}
	// check is loaded already
	if (luaR_isLoaded(L, modname, mode)) return 1;
	// load it in other case
	return luaR_loadModule(L, modname, mode);
}

/* Allows to require and starts require worker from CAPI */
static int luaR_require(lua_State* L, char* modname, char* strmode) {
	assert(modname);
	int mode = luaR_checkmode(L, strmode, LOADED_C);

	// increase require status, run worker, decrease status
	luaR_status(L, 1);
	int nres = luaR_dorequire(L, modname, mode);
	luaR_status(L, -1);

	if (nres < 0) lua_error(L); // rethrow
	return nres;
}

// not a REQUIRE_WRAPPER, but wrapper around worker ._.
// used in unsafe code
static int luaB_require (lua_State* L) {
	char* modname = luaL_checkstring(L, 1);
	int kind = luaR_checkmode(L, lua_tostring(L, 2), LOADED_UNSAFE); 
	int nres = luaR_dorequire(L, modname, kind);
	if (nres < 0) {
		lua_error(L); // rethrow
	}
	return nres;
}

// real REQUIRE_WRAPPER, but wrapped to make it possible to push this into
// lua. WARNING : DO NOT GIVE THIS FUNCTION IN ANY WAY FOR UNSAFE CODE!
// it's not pushed into lua by default at all. Do this manually, and ONLY if
// you really need it. (you can simply call luaR_require from CAPI :D)
int luaB_unsafeRequire(lua_State* L) {
	char* modname = luaL_checkstring(L, 1);
	int kind = luaR_checkmode(L, lua_tostring(L, 2), LOADED_C);
	luaR_require(L, modname, kind);
	return 1;	
}

static const luaL_Reg require_funcs[] = {
	{"require", luaB_require},
	{nullptr, nullptr}
};

int luaopen_package(lua_State* L) {
	lua_settop(L, 0);
	luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_REQUIRE); // hehe
		// setup C
		luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED"); // assume it exists
		lua_rawseti(L, -2, LOADED_C);
		// setup local
		lua_newtable(L);
		lua_createtable(L, 0, 1);
		lua_pushstring(L, "kvs");
		lua_setfield(L, -2, "__mode"); // to collect unused enviroments
		lua_setmetatable(L, -2);
		// add default local loader
		#if LUA_REQUIRE_HASDOFILE 
		lua_newtable(L, 1, 0);
		lua_pushcfunction(L, luaB_dofile, "fileloader");
		lua_rawseti(L, -2, 1);
		lua_setfield(L, -2, LUA_LOADERSNAME);
		#endif
		lua_rawseti(L, -2, LOADED_LOCAL);
		// setup global
		lua_newtable(L);
		lua_rawseti(L, -2, LOADED_GLOBAL);

	// remove require internal table
	lua_pop(L, 1);
	// precreate flag
	lua_pushinteger(L, 0);
	lua_setfield(L, LUA_REGISTRYINDEX, LUA_REQSTAT);
	luaL_register(L, "_G", require_funcs);
	return 1;
}
#endif

