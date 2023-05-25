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
#include "lua.h"
#include "lualib.h"
#include "luacode.h"
#include <cstdio>
#include <cstdlib>
#include <assert.h>

/*
 * Actually, this may be fixed if add -fexceptions to the C side, but... 
 * I am not 100% sure.
 */
#if LUA_USE_LONGJMP
static_assert(!"This baseio activly uses c++ exceptions and RAII stuff\
	that disabled by EXTERN_C. Use C++ instead of C or don't use this library.\
	Any attempt to workaround this will 100% result into UB.");
#endif

// this is a C++ SOURCE!!!

#include <string>

struct CodeBuffer {
	const char* data;
	size_t len;
	const char* name;
};

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

// applies sanbox metatable to the table at the top of the stack
// (using enviroment table at index glob, or GLOBALS proxy instead)
// glob MUST NOT BE PSEUDO!
void applySandboxMT(lua_State* L, int glob) {
	lua_createtable(L, 0, 1);
	if (glob)
		lua_pushvalue(L, glob);
	else
    lua_pushvalue(L, LUA_GLOBALSINDEX);
  lua_setfield(L, -2, "__index");
  lua_setreadonly(L, -1, true);
  lua_setmetatable(L, -2);
}

/*
 * Default sandboxthread() forget to change _G :D
 * Also here we can use not only new table as new enviroment,
 * But precreated by user (and in lua too)
 */
void doSandboxThread(lua_State* L, int envidx) {
		if (envidx) {
				lua_pushvalue(L, envidx);
		} else {
    	// create new global table that proxies reads to original table
    	lua_createtable(L, 0, 1);
    	lua_setsafeenv(L, -1, true); // it's isolated => not shares globals
			applySandboxMT(L, 0); // 0 to indicate to use LUA_GLOBALSINDEX
			lua_pushvalue(L, -1); // very useful change
			lua_setfield(L, -2, "_G");
		}
		assert(lua_istable(L, -1));
    lua_replace(L, LUA_GLOBALSINDEX);
}

/*
 * dostring/dofile-like() in save enviroment and separate corutine instead of main one.
 * this must be preferrable about loadstring, when you load untrusted code,
 * cause yielding may break your internal code execution a bit, when you do not
 * expect that.
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
	if (env==0 || !lua_istable(L, env)) {
		// no enviroment specified - use isolated and fastest case
		lua_createtable(L, 0, 1);
		env = lua_gettop(L);
		lua_setsafeenv(L, env, true);
		applySandboxMT(L, 0); // use real globals table
		lua_pushvalue(L, -1); // very useful change
		lua_setfield(L, -2, "_G");
	} // in other case user should do all similar stuff (except setsafeenv)
	assert(lua_istable(L, env));
	int status = rawloadcode(L, B, env);
	return status;
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
 * isolationg out enviroment. Using isolated one is preferabble, cause it's a fastest
 * way to do things.
 */
static int (luaB_loadstring) (lua_State* L) { // load string with specified enviroment
	CodeBuffer B;
	B.data = luaL_checklstring(L, 1, &B.len);
	B.name = luaL_optstring(L, 2, B.data);
	lua_settop(L, 3); // we nned third argument, or nil
	
	int status = safeloadcodeenv(L, B, 3);

	if (status != LUA_OK) {
		lua_pushnil(L); // [nil] [errmsg]
		lua_insert(L, -2);
	} else lua_pushnil(L); // [res] [nil]
	return 2;
}

/* struct LoadRes {
	const char* filename  = nullptr;
	std::string chunkname = "=";
	RAILfree<char*, true> codebuff;
	size_t len = 0;
}; */ // DEPRECATED

// TODO:
// This is editable fileIO implementation. You can change
// FILE* IO to ZIP archinve readonly IO and everything will still
// work, but just in the archive now :p.
// WARNING: this class MUST NOT throw!
class FileIO {
	private:
	FILE* f = nullptr;
	public:
	FileIO() = default;
	~FileIO() {
		close();
	}
	// FILE* clonning is not supported on some
	// unix systems, and in most non-unix ones
	FileIO(const FileIO&) = delete;
	public:
	using pos_type = decltype(ftell(nullptr));
	using size_type = decltype(fread(nullptr, 0, 0, nullptr));
	enum SeekBase {
		SET = SEEK_SET,
		END = SEEK_END,
		CUR = SEEK_CUR
	};
	public:
	// status
	bool isOpened() const {return !!f;}
	bool hasError() const {return f ? ferror(f) : false;}
	bool isEOF() {return !f || feof(f);}
	operator bool() const {return isOpened() && !hasError();}

	// opening/closing
	void close() {if (f) fclose(f); f = nullptr;}
	bool open(const char* file, const char* mode) {
		f = fopen(file, mode);
		return isOpened();
	}

	// seeking
	pos_type tell() {return ftell(f);}	
	bool seek(pos_type pos, SeekBase base) {
		return fseek(f, pos, (int)base) >= 0;
	}
	void rewind() {
		fseek(f, 0, (int)SeekBase::SET);
		clearerr(f);
	}

	// IO

	// dst space must be not less than itemsz*cnt!
	// returns number of readed elements (in simple case
	// aka two args only, returns 1 in sucess reading, 0 if 
	// EOF or other shit, -1 in case of error (and sets error flag)
	// ((c lib does this by itself!!!1)).
	size_type read(void* dst, size_t itemsz, size_t num = 1) {
		return fread(dst, itemsz, num, f);
	}
	
	// same things aas above, but for writing.
	size_type write(const void* src, size_t itemsz, size_t num = 1) {
		return fwrite(src, itemsz, num, f);
	}
};

/*
 * Reads a whole file into memory (std::string)
 */
static int loadfilecontent(lua_State* L, const char* finm, std::string& buff) {
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
	int status = loadfilecontent(L, filename, buffer);
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
	int status = loadfilecontent(L, filename, buffer);
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

static const luaL_Reg extra_funcs[] = {
	{"loadfile", luaB_loadfile},
	{"dofile", luaB_dofile},
	{"dostring", luaB_dostring},
	{"loadstring", luaB_loadstring},
	{"vector", luaB_vector},
	{"collectgarbage", luaB_collectgarbage},
	{nullptr, nullptr}
};

int luaopen_extra(lua_State *L) {
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, nullptr, extra_funcs);
	opts.vectorCtor = "vector";
	return 0;
}
