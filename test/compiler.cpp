/*
 * Luau bytecode compiler to C header
 *
 * Copyright (C) UtoECat 2023
 * MIT License. No any warrianty 
 */
#include "extra.hpp"
#include <cstdio>
#include <cstdlib>
#include <cassert>

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
static std::vector<const char*> filenames;
bool rawoutput = false; // compile to C by default

static void initLuaState (lua_State* L) {
	if (!L) {
		throw Exception("Can't create Lua state!");
	}
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_setreadonly(L, -1, 1);
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
			case 'r' : rawoutput = true; break;
			case 'c' : rawoutput = false; break;
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
	if (rawoutput) {
		fprintf(stderr, "Warning: There is no default way to load raw\n");
		fprintf(stderr, "Warning: bytecode from sandbxed eniroment in Luau.\n");
		fprintf(stderr, "Warning: Mallicious bytecode WILL Crash VM!\n");
		fprintf(stderr, "Warning: So not use it, include it directly to C\n");
		fprintf(stderr, "Warning: from *.h files using -c flag instead.\n");
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

static const char* header_format = 
"/* this file is autogenerated!\n"
" * don't edit! (Luauc - luau compiler)\n"
" * file : %s \n"
" */\n"
"\n"
"static const char %s_bcode[] = {\n "
;

static const char* footer_format = 
"};\n"
"\nstatic const long int %s_size = %li;\n"
"/* end of file!\n"
" */";

#include <ctype.h>

void compFile(lua_State*L, const char* fname) {
	std::string source;
	if (loadFileContent(L, fname, source) != LUA_OK) {
		fprintf(stderr, "%s", lua_tostring(L, -1));
		throw ExpectedException();
	};
	size_t len = 0;
	// check for compile errors
	char* c = luau_compile(source.c_str(), source.size(), &opts, &len);
	if (luau_load(L, "?", c, len, 0) != LUA_OK) {
		fprintf(stderr, "%s", lua_tostring(L, -1));				
		throw ExpectedException();
	}
	lua_pop(L, 1); // don't need this
	source.clear();
	source.append(c, len); // set bytecode
	free(c);

	std::string newname = fname;
	std::string naming = "";
	auto pos = newname.find(".lua");
	if (pos != std::string::npos) newname.resize(pos);
	naming = newname;
	newname += rawoutput ? ".bin" : ".h" ;

	// fix naming
	for (char& c : naming) {
		if (!isalnum(c) && c != '_') c = '_';
	}
	if (isdigit(naming[0])) naming.insert(0, "_");

	FileIO out;
	if (!out.open(newname.c_str(), rawoutput ? "wb" : "w"))
		throw Exception(strformat("Can't open file %s!", newname.c_str()));
	if (rawoutput) {
		if (!out.write(source)) throw Exception("Writing output error!");
	} else {
		if (!out.write(strformat(header_format, fname, naming.c_str())))
				throw Exception("Can't write header");
		for (size_t i = 0; i < source.size(); i++) {
			int c = source[i];
			out.write(strformat("%i%s", c, (i+1<source.size())?", ":" "));
			if ((i+1) % 5 == 0) {
				out.write("\n ", 1, 2);
			}
		}
		if (!out.write(strformat(footer_format, naming.c_str(), source.size()))) {
				throw Exception("Can't write end of file!");
		};
	}
	out.close();
}

int lua_Main(int argc, char** argv) {
	// init
	RaiiLua state;
	lua_State *L = state.L;
	parseargs(argc, argv);
	initLuaState(L);
	
	// execute files compilation
	lua_settop(L, 0);
	for (auto& fname : filenames) {
		compFile(L, fname);
	}
	
	// no lua_close() here, it will be RAII'd!
	return 0;
}
