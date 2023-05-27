/*
 * Even when this one is for testing purposes only,
 * You can still use functions from this in your program, but
 * i am not 100% sure how much they are good.
 * WARNING TODO FIXME: this MAY NOT WORK IF LUAU COMPILED TO USE LONGJUMP
 * INSTEAD OF C++ EXCEPTIONS! (aka with EXTERN_C)
 *
 * loadfile() and loadstring() are behaves DIFFERENTLY from vanilla
 * lua behaviour : if no enviroment specidied, not _G setted, but
 * setted proxy of _G, so module can still set "globals", even if they
 * are now just locals :D
 *
 * Copyright (C) UtoECat 2023
 * MIT License. No any warrianty 
 */
#pragma once
#include "luau.hpp"
#include <cstdio>
#include <cstdlib>

/*
 * Actually, this may be fixed if add -fexceptions to the C side,
 * and still use exceptions in build for C but... 
 * I am not 100% sure.
 */
#if LUA_USE_LONGJMP
static_assert(!"This baseio activly uses c++ exceptions and RAII stuff\
	that disabled by EXTERN_C. Use C++ instead of C or don't use this library.\
	Any attempt to workaround this will 100% result into UB.");
#endif

// any fields here should not be manuallu allocated/deallocated
// by functions, that uses this struct
struct CodeBuffer {
	const char* data;
	size_t len;
	const char* name;
};

/*
 * Global compile options (cause i am lazy xD)
 */
extern lua_CompileOptions opts; 


// TODO:
// This is editable fileIO implementation. You can change
// FILE* IO to ZIP archinve readonly IO and everything will still
// work, but just in the archive now :p.
// WARNING: this class MUST NOT throw ANYTHING!
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

#include <string>
/*
 * Reads a whole file into memory (std::string)
 */
int loadFileContent(lua_State* L, const char* finm, std::string& buff);

/*
 * hehe
 */
int luaL_loadbufferx (lua_State *L, const char *buff, size_t size,
                                 const char *name, int env);

/*
 * Opens extra stuff :)
 */
int luaopen_extra(lua_State *L);
