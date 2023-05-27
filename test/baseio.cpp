/*
 * Even when this one is for testing purposes only,
 * You can still use functions from this in your program, but
 * i am not 100% sure how much they are good.
 * WARNING TODO FIXME: this MAY NOT WORK IF LUAU COMPILED TO USE LONGJUMP
 * INSTEAD OF C++ EXCEPTIONS! (aka with EXTERN_C)
 *
 * Copyright (C) UtoECat 2023
 * MIT License. No any warrianty 
 */
#include "extra.hpp"
#include <assert.h>

// this is a C++ SOURCE!!!

#include <string>
lua_CompileOptions opts = {0};

/*
 * RAW. Must not be used directly in your programs.
 */
static int rawloadcode(lua_State* L, CodeBuffer& B, int env) {
	size_t len = 0;
	char* c = luau_compile(B.data, B.len, &opts, &len);
	int status = luau_load(L, B.name, c, len, env);
	free(c);
	return status;
}

/*
 * More OOP stuff in the future...
 * MUST REMOVE ALL ADDED BY ITSELF TO STACK STUFF, EXCEPT FOR NEW
 * TABLE!
 */
static void lua_inherit(lua_State* L) {
	int top = lua_gettop(L);
	lua_createtable(L, 0, 1); // new object
  lua_createtable(L, 0, 1); // new meta
	lua_pushvalue(L, -3); // copy table above new object
  lua_setfield(L, -2, "__index"); // proxy reads
  lua_setreadonly(L, -1, true); // make metatable readonly
  lua_setmetatable(L, -2); // apply
	assert(top+1 == lua_gettop(L));
}

static int luaB_inherit(lua_State* L) {
	if (!lua_istable(L, -1)) luaL_error(L, "bad argument #%i : Table excepted!", 1);
	lua_inherit(L);	
	return 1;
}

/*
 * Default sandboxthread() forget to change _G :D
 * Also here we can use not only new _G proxy table as new enviroment,
 * But precreated by user (and in lua too)
 */
static void doSandboxEnv(lua_State* L, int env) {
	int top = lua_gettop(L);
	if (env && lua_istable(L, env))
		// just use specified enviroment
		lua_pushvalue(L, env);
	else {
		lua_pushvalue(L, LUA_GLOBALSINDEX);
		lua_inherit(L);
		lua_remove(L, -2); // remove old globals
    lua_setsafeenv(L, -1, true); // it's isolated => not shares globals
		env = lua_gettop(L);
	}
	if (!lua_getreadonly(L, -1)) { // add _G
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "_G");
	}
	assert(lua_istable(L, -1));
	assert(lua_gettop(L) == top+1);
}

static void doSandboxThread(lua_State* L, int env) {
		doSandboxEnv(L, env);
		assert(lua_istable(L, -1));
    lua_replace(L, LUA_GLOBALSINDEX);
}

/*
 * dostring/dofile-like() in save enviroment and separate corutine instead of main one.
 * this must be preferrable about loadstring, when you load untrusted code.
 * (and trusted code too, since setsafeenv() allows VM to do a lot of
 * additional optimisations, by exepting that all baselib functions are
 * always reachable from global table (assumes readonly))
 *
 * dostring() throws errors instead of returning them, this is vanilla lua behaviour
 */
static int safedocode(lua_State* L, CodeBuffer& B, int env = 0) {
	// create new coroutine from main threrad with protected enviroment
	lua_State* ML = lua_newthread(lua_mainthread(L));

	if (env) { // move enviroment to the new coro
		lua_xmove(L, ML, 1);
		env = lua_gettop(ML);
	}
	doSandboxThread(ML, env);

	int status = rawloadcode(ML, B, 0); // enviroment is already changed
	if (status != LUA_OK) {
		lua_xmove(ML, L, 1);
		lua_error(L); // throw compile error again
	}

	int oldtop = lua_gettop(ML);
	status = lua_resume(ML, L, 0); // call function in new coro
	int nresults = lua_gettop(ML) - oldtop + 1;
	if (nresults > 0) {
		lua_checkstack(L, lua_gettop(L) + nresults);
		lua_xmove(ML, L, nresults); // move results to the main thread
	}
	lua_resetthread(ML); // release coro resources (kinda)

	if (status == LUA_YIELD) {
		lua_pushstring(L, "attempt to yield from isolated coroutine!");
		lua_error(L);
	} else if (status == LUA_OK) {
		return nresults; // return results
	} else {
		lua_error(L);
	}
}

/*
 * loads code in specified enviroment (or creates fully isolated and safe one)
 * returns errors like rawloadcode() does
 */
static int (safeloadcodeenv) (lua_State* L, CodeBuffer& B, int env) {
	int top = lua_gettop(L);
	doSandboxEnv(L, env);
	assert(lua_istable(L, -1));
	int status = rawloadcode(L, B, -1);
	lua_remove(L, -2); // remove enviroment
	assert(lua_gettop(L) == top+1);
	return status;
}

int luaL_loadbufferx (lua_State *L, const char *buff, size_t size,
  const char *name, int env) {
	assert(buff != nullptr);
	CodeBuffer b;
	b.data = buff;
	b.len = size;
	b.name = name ? name : "?";
	return safeloadcodeenv(L, b, env);
}

/*
 * real dostring() implementation.
 */
static int luaB_dostring(lua_State* L) {
	CodeBuffer B;
	B.data = luaL_checklstring(L, 1, &B.len);
	B.name = luaL_optstring(L, 2, B.data);
	lua_settop(L, 3);
	return safedocode(L, B, lua_istable(L, 3) ? 3 : 0);
}

/*
 * This function is so different because of optimisations that we can get from
 * isolating out enviroment. Using isolated one is preferabble, cause it's a fastest
 * way to do things.
 */
static int (luaB_loadstring) (lua_State* L) { // load string with specified enviroment
	CodeBuffer B;
	B.data = luaL_checklstring(L, 1, &B.len);
	B.name = luaL_optstring(L, 2, B.data);
	lua_settop(L, 3); // we nned third argument, or nil
	if (!lua_istable(L, 3) && !lua_isnoneornil(L, 3)) {
		luaL_error(L, "bad argument #3 : table or nil excepted!");
	}
	
	int status = safeloadcodeenv(L, B, 3);

	if (status != LUA_OK) {
		lua_pushnil(L); // [nil] [errmsg]
		lua_insert(L, -2);
	} else lua_pushnil(L); // [res] [nil]
	return 2;
}

/*
 * Reads a whole file into memory (std::string)
 */
int loadFileContent(lua_State* L, const char* finm, std::string& buff) {
	FileIO file; // RAIL stdio file wrapper
	file.open(finm, "r");	
	if (!file) { // checks for error && is file opened
		lua_pushstring(L, "Can't open file!");
		return LUA_ERRRUN;		
	};

	// get file size (TODO: not suitable for stdin!)
	file.seek(0, ::FileIO::SeekBase::END);
	size_t len = file.tell();

	// allocate buffer to load file source
	// (actually preallocating std::string internal buffer)
	buff.clear();
	try {
		buff.reserve(len + 2);
	} catch (...) {
		lua_pushstring(L, "NOMEM");
		return LUA_ERRMEM;
	}

	file.rewind();
	// reading by fixed size block, since this is really good for disk IO
	const size_t block_size = 256;
	size_t blocks_num = len / block_size;
	assert(blocks_num * block_size <= len);

	try {
		char tmp[block_size];
		for (size_t bli = 0; bli < blocks_num; bli++) {
			if (!file) throw (bli * block_size); // IO error
			auto cnt = file.read(tmp, block_size);
			if (cnt < 1) throw (bli * block_size + cnt); //file shrinked?
			buff.append(tmp, block_size);
		};
		// read rest
		size_t n = len - (blocks_num * block_size);
		assert(n < block_size);
		auto cnt = file.read(tmp, 1, block_size);
		if (cnt != n) throw n;
		buff.append(tmp, cnt);
	} catch (size_t i) {
		lua_pushfstring(L, "Can't read file after opening" 
			"(pos %i/%i)", (int)i, (int)len);
		return LUA_ERRERR;
	}

	file.close();
	return LUA_OK;
}

// chunk name for file name
static std::string chnameForfname(const char* filename) {
	return std::string("=") += filename;
}

static int luaB_loadfile(lua_State* L) {
	const char* filename = luaL_checkstring(L, 1);
	std::string chunkname = chnameForfname(filename);

	CodeBuffer b; // will be filled later
	b.name = luaL_optstring(L, 2, chunkname.data());
	lua_settop(L, 3); // we ned third argument, or nil
	
	// load file content
	std::string buffer;
	int status = loadFileContent(L, filename, buffer);
	if (status != LUA_OK) goto errorcheck;

	// load code now
	b.data = buffer.data();
	b.len  = buffer.size();

	status = safeloadcodeenv(L, b, 3); 

	errorcheck:
	lua_pushnil(L); // [res] [nil]
	if (status != LUA_OK) {
		lua_insert(L, -2); // [nil] [errmsg]
	}
	return 2;
}

int luaB_dofile(lua_State* L) {
	const char* filename = luaL_checkstring(L, 1);
	std::string chunkname = chnameForfname(filename);

	CodeBuffer b; // will be filled later
	b.name = luaL_optstring(L, 2, chunkname.data());
	lua_settop(L, 3);

	// load file content
	std::string buffer;
	int status = loadFileContent(L, filename, buffer);
	if (status != LUA_OK) lua_error(L);

	// load code now
	b.data = buffer.data();
	b.len  = buffer.size();
	return safedocode(L, b, lua_istable(L, 3) ? 3 : 0);
	// if there is something to free, it should be RAII'd
};


static int luaB_vector(lua_State* L) {
  float x = luaL_checknumber(L, 1);
  float y = luaL_checknumber(L, 2);
  float z = luaL_checknumber(L, 3);
  lua_pushvector(L, x, y, z);
  return 1;
}

#include <cstring>

static int luaB_collectgarbage(lua_State* L) {
	const char* option = luaL_optstring(L, 1, "collect");
	if (strcmp(option, "collect") == 0) {
		lua_gc(L, LUA_GCCOLLECT, 0); return 0;
	}
	if (strcmp(option, "count") == 0) {
		int c = lua_gc(L, LUA_GCCOUNT, 0);
		lua_pushnumber(L, c); return 1;
	}
	luaL_error(L, "collectgarbage must be called with 'count' or 'collect'");
}

struct Serialize {
	int deep;
	int cidx;
	std::string buffer;
	bool loadable; // is result should be available to load without errors?
};

static void ser_str(lua_State* L, Serialize* ser) {
		size_t len = 0;
		const char* str = lua_tolstring(L, -1, &len);

		ser->buffer += '\"';
		while (len--) {
			switch (*str) {
				case '\"' :
				case '\\' :
				case '\n' :
					ser->buffer += '\\';
					ser->buffer += *str;
				break;
				case '\r' :
					ser->buffer += "\\r";
				break;
				case '\0' :
					ser->buffer += "\\000";
				break;
				default :
					ser->buffer += *str;
				break;
			};
		str++;	
	}
	ser->buffer += '\"';
}

static void addcache(lua_State* L, Serialize* ser) {
	lua_pushvalue(L, ser->cidx);
	lua_pushvalue(L, -2);
	lua_pushvalue(L, -1);
	luaL_tolstring(L, -1, nullptr); // simple
	lua_rawset(L, -3);
	lua_pop(L, 1);
}

void luaL_serialize(lua_State* L, Serialize* ser);
	
static void servalue(lua_State* L, Serialize* ser, int idx) {
	ser->deep++;
	if (ser->deep > 20) luaL_error(L, "Table is too deep!");
	int oldtop = lua_gettop(L);
	lua_pushvalue(L, idx);
	luaL_serialize(L, ser);
	lua_pop(L, 1);
	if (oldtop < lua_gettop(L)) {
		luaL_error(L, "unbalanced servalue!");
	}
}

void luaL_serialize(lua_State* L, Serialize* ser) {
	assert(ser && L);
	lua_checkstack(L, lua_gettop(L) + 5);

	// CHECK FOR REURSION
	lua_pushvalue(L, ser->cidx);
	lua_pushvalue(L, -2);
	if (lua_rawget(L, -2) != LUA_TNIL) {
		lua_remove(L, -2); // remove cache table
		lua_remove(L, -2); // remove original value
		return; // return cached value
	};
	lua_pop(L, 2);

	switch(lua_type(L, -1)) {
		case LUA_TSTRING:
		ser_str(L, ser);
		break;
		case LUA_TTABLE: {
		addcache(L, ser);	
		int idx = lua_gettop(L);
		lua_pushnil(L);
		int oldi = 0;
		int cnt = 0;
		ser->buffer += "{";

		while (lua_next(L, idx)) { // next will remove key and value at the end
			if (lua_tonumber(L, -2) - oldi <= 1) { // array part
				oldi++;
			} else {
				ser->buffer += "["; 
				servalue(L, ser, -2);
				ser->buffer += "] = "; 
			}
			servalue(L, ser, -1);
			ser->buffer += ", "; 
			cnt++;
			if (cnt % 5) ser->buffer += "\n";
		}
		ser->buffer += "{";
		}
		break;
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
		if (ser->loadable)
			luaL_error(L, "Cannot serialize type %s", luaL_typename(L, -1));
		ser->buffer += luaL_tolstring(L, -1, nullptr); // in other case
		break;
		default :
		ser->buffer += luaL_tolstring(L, -1, nullptr);
		break;
	}
}

static int luaB_serialize(lua_State* L) {
	Serialize tmp, *ser;
	ser = &tmp;
	ser->loadable = lua_toboolean(L, 2);
	lua_settop(L, 1);

	lua_createtable(L, 1, 1);
	ser->cidx = lua_gettop(L);
	lua_pushvalue(L, -2);
	ser->deep = 0;
	servalue(L, ser, 1);
	return 1;
}

static const luaL_Reg extra_funcs[] = {
	{"loadfile", luaB_loadfile},
	{"dofile", luaB_dofile},
	{"dostring", luaB_dostring},
	{"loadstring", luaB_loadstring},
	{"vector", luaB_vector},
	{"collectgarbage", luaB_collectgarbage},
	{"serialize", luaB_serialize},
	{nullptr, nullptr}
};

int luaopen_extra(lua_State *L) {
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, nullptr, extra_funcs);
	opts.vectorCtor = "vector";
	if (lua_getfield(L, LUA_GLOBALSINDEX, "table") != LUA_TTABLE) {
		lua_pop(L, 1); 
		lua_createtable(L, 1, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, LUA_GLOBALSINDEX, "table");		
	};
	lua_pushcfunction(L, luaB_inherit, "inherit");
	lua_setfield(L, -2, "inherit");
	lua_pop(L, 1); 
	return 0;
}
