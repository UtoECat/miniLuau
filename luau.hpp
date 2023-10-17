/*
Luau programming language. (Packed version using PACK.LUA)
MIT License

Copyright (c) 2019-2022 Roblox Corporation
Copyright (c) 1994–2019 Lua.org, PUC-Rio.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */
// Comment this out to not build AST and Compiler
#define LUAU_ENABLE_COMPILER 1

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#if defined(_MSC_VER) && !defined(__clang__)
#define LUAU_FASTMATH_BEGIN __pragma(float_control(precise, off, push))
#define LUAU_FASTMATH_END __pragma(float_control(pop))
#else
#define LUAU_FASTMATH_BEGIN
#define LUAU_FASTMATH_END
#endif
#if (defined(__x86_64__) || defined(_M_X64)) && !defined(__SSE4_1__) && !defined(__AVX__)
#if defined(_MSC_VER) && !defined(__clang__)
#define LUAU_TARGET_SSE41
#elif defined(__GNUC__) && defined(__has_attribute)
#if __has_attribute(target)
#define LUAU_TARGET_SSE41 __attribute__((target("sse4.1")))
#endif
#endif
#endif
#if defined(__GNUC__)
#define LUA_PRINTF_ATTR(fmt, arg) __attribute__((format(printf, fmt, arg)))
#else
#define LUA_PRINTF_ATTR(fmt, arg)
#endif
#ifdef _MSC_VER
#define LUA_NORETURN __declspec(noreturn)
#else
#define LUA_NORETURN __attribute__((__noreturn__))
#endif
#ifndef LUA_API
#define LUA_API extern
#endif
#define LUALIB_API LUA_API
#if defined(__GNUC__)
#define LUAI_FUNC __attribute__((visibility("hidden"))) extern
#define LUAI_DATA LUAI_FUNC
#else
#define LUAI_FUNC extern
#define LUAI_DATA extern
#endif
#ifndef LUA_USE_LONGJMP
#define LUA_USE_LONGJMP 0
#endif
#ifndef LUA_IDSIZE
#define LUA_IDSIZE 256
#endif
#ifndef LUA_MINSTACK
#define LUA_MINSTACK 20
#endif
#ifndef LUAI_MAXCSTACK
#define LUAI_MAXCSTACK 8000
#endif
#ifndef LUAI_MAXCALLS
#define LUAI_MAXCALLS 20000
#endif
#ifndef LUAI_MAXCCALLS
#define LUAI_MAXCCALLS 200
#endif
#ifndef LUA_BUFFERSIZE
#define LUA_BUFFERSIZE 512
#endif
#ifndef LUA_UTAG_LIMIT
#define LUA_UTAG_LIMIT 128
#endif
#ifndef LUA_SIZECLASSES
#define LUA_SIZECLASSES 32
#endif
#ifndef LUA_MEMORY_CATEGORIES
#define LUA_MEMORY_CATEGORIES 256
#endif
#ifndef LUA_MINSTRTABSIZE
#define LUA_MINSTRTABSIZE 32
#endif
#ifndef LUA_MAXCAPTURES
#define LUA_MAXCAPTURES 32
#endif
#define LUAI_USER_ALIGNMENT_T union { double u; void* s; long l; }
#ifndef LUA_VECTOR_SIZE
#define LUA_VECTOR_SIZE 3
#endif
#define LUA_EXTRA_SIZE (LUA_VECTOR_SIZE - 2)
#define LUA_MULTRET (-1)
#define LUA_REGISTRYINDEX (-LUAI_MAXCSTACK - 2000)
#define LUA_ENVIRONINDEX (-LUAI_MAXCSTACK - 2001)
#define LUA_GLOBALSINDEX (-LUAI_MAXCSTACK - 2002)
#define lua_upvalueindex(i) (LUA_GLOBALSINDEX - (i))
#define lua_ispseudo(i) ((i) <= LUA_REGISTRYINDEX)
enum lua_Status
{
 LUA_OK = 0,
 LUA_YIELD,
 LUA_ERRRUN,
 LUA_ERRSYNTAX,
 LUA_ERRMEM,
 LUA_ERRERR,
 LUA_BREAK,
};
enum lua_CoStatus
{
 LUA_CORUN = 0,
 LUA_COSUS, // suspended
 LUA_CONOR,
 LUA_COFIN, // finished
 LUA_COERR,
};
typedef struct lua_State lua_State;
typedef int (*lua_CFunction)(lua_State* L);
typedef int (*lua_Continuation)(lua_State* L, int status);
typedef void* (*lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);
#define l_noret void LUA_NORETURN
#define LUA_TNONE (-1)
enum lua_Type
{
 LUA_TNIL = 0,
 LUA_TBOOLEAN = 1, // must be 1 due to l_isfalse
 LUA_TLIGHTUSERDATA,
 LUA_TNUMBER,
 LUA_TVECTOR,
 LUA_TSTRING,
 LUA_TTABLE,
 LUA_TFUNCTION,
 LUA_TUSERDATA,
 LUA_TTHREAD,
 LUA_TPROTO,
 LUA_TUPVAL,
 LUA_TDEADKEY,
 LUA_T_COUNT = LUA_TPROTO
};
typedef double lua_Number;
typedef int lua_Integer;
typedef unsigned lua_Unsigned;
LUA_API lua_State* lua_newstate(lua_Alloc f, void* ud);
LUA_API void lua_close(lua_State* L);
LUA_API lua_State* lua_newthread(lua_State* L);
LUA_API lua_State* lua_mainthread(lua_State* L);
LUA_API void lua_resetthread(lua_State* L);
LUA_API int lua_isthreadreset(lua_State* L);
LUA_API int lua_absindex(lua_State* L, int idx);
LUA_API int lua_gettop(lua_State* L);
LUA_API void lua_settop(lua_State* L, int idx);
LUA_API void lua_pushvalue(lua_State* L, int idx);
LUA_API void lua_remove(lua_State* L, int idx);
LUA_API void lua_insert(lua_State* L, int idx);
LUA_API void lua_replace(lua_State* L, int idx);
LUA_API int lua_checkstack(lua_State* L, int sz);
LUA_API void lua_rawcheckstack(lua_State* L, int sz);
LUA_API void lua_xmove(lua_State* from, lua_State* to, int n);
LUA_API void lua_xpush(lua_State* from, lua_State* to, int idx);
LUA_API int lua_isnumber(lua_State* L, int idx);
LUA_API int lua_isstring(lua_State* L, int idx);
LUA_API int lua_iscfunction(lua_State* L, int idx);
LUA_API int lua_isLfunction(lua_State* L, int idx);
LUA_API int lua_isuserdata(lua_State* L, int idx);
LUA_API int lua_type(lua_State* L, int idx);
LUA_API const char* lua_typename(lua_State* L, int tp);
LUA_API int lua_equal(lua_State* L, int idx1, int idx2);
LUA_API int lua_rawequal(lua_State* L, int idx1, int idx2);
LUA_API int lua_lessthan(lua_State* L, int idx1, int idx2);
LUA_API double lua_tonumberx(lua_State* L, int idx, int* isnum);
LUA_API int lua_tointegerx(lua_State* L, int idx, int* isnum);
LUA_API unsigned lua_tounsignedx(lua_State* L, int idx, int* isnum);
LUA_API const float* lua_tovector(lua_State* L, int idx);
LUA_API int lua_toboolean(lua_State* L, int idx);
LUA_API const char* lua_tolstring(lua_State* L, int idx, size_t* len);
LUA_API const char* lua_tostringatom(lua_State* L, int idx, int* atom);
LUA_API const char* lua_namecallatom(lua_State* L, int* atom);
LUA_API int lua_objlen(lua_State* L, int idx);
LUA_API lua_CFunction lua_tocfunction(lua_State* L, int idx);
LUA_API void* lua_tolightuserdata(lua_State* L, int idx);
LUA_API void* lua_touserdata(lua_State* L, int idx);
LUA_API void* lua_touserdatatagged(lua_State* L, int idx, int tag);
LUA_API int lua_userdatatag(lua_State* L, int idx);
LUA_API lua_State* lua_tothread(lua_State* L, int idx);
LUA_API const void* lua_topointer(lua_State* L, int idx);
LUA_API void lua_pushnil(lua_State* L);
LUA_API void lua_pushnumber(lua_State* L, double n);
LUA_API void lua_pushinteger(lua_State* L, int n);
LUA_API void lua_pushunsigned(lua_State* L, unsigned n);
#if LUA_VECTOR_SIZE == 4
LUA_API void lua_pushvector(lua_State* L, float x, float y, float z, float w);
#else
LUA_API void lua_pushvector(lua_State* L, float x, float y, float z);
#endif
LUA_API void lua_pushlstring(lua_State* L, const char* s, size_t l);
LUA_API void lua_pushstring(lua_State* L, const char* s);
LUA_API const char* lua_pushvfstring(lua_State* L, const char* fmt, va_list argp);
LUA_API LUA_PRINTF_ATTR(2, 3) const char* lua_pushfstringL(lua_State* L, const char* fmt, ...);
LUA_API void lua_pushcclosurek(lua_State* L, lua_CFunction fn, const char* debugname, int nup, lua_Continuation cont);
LUA_API void lua_pushboolean(lua_State* L, int b);
LUA_API int lua_pushthread(lua_State* L);
LUA_API void lua_pushlightuserdata(lua_State* L, void* p);
LUA_API void* lua_newuserdatatagged(lua_State* L, size_t sz, int tag);
LUA_API void* lua_newuserdatadtor(lua_State* L, size_t sz, void (*dtor)(void*));
LUA_API int lua_gettable(lua_State* L, int idx);
LUA_API int lua_getfield(lua_State* L, int idx, const char* k);
LUA_API int lua_rawgetfield(lua_State* L, int idx, const char* k);
LUA_API int lua_rawget(lua_State* L, int idx);
LUA_API int lua_rawgeti(lua_State* L, int idx, int n);
LUA_API void lua_createtable(lua_State* L, int narr, int nrec);
LUA_API void lua_setreadonly(lua_State* L, int idx, int enabled);
LUA_API int lua_getreadonly(lua_State* L, int idx);
LUA_API void lua_setsafeenv(lua_State* L, int idx, int enabled);
LUA_API int lua_getmetatable(lua_State* L, int objindex);
LUA_API void lua_getfenv(lua_State* L, int idx);
LUA_API void lua_settable(lua_State* L, int idx);
LUA_API void lua_setfield(lua_State* L, int idx, const char* k);
LUA_API void lua_rawsetfield(lua_State* L, int idx, const char* k);
LUA_API void lua_rawset(lua_State* L, int idx);
LUA_API void lua_rawseti(lua_State* L, int idx, int n);
LUA_API int lua_setmetatable(lua_State* L, int objindex);
LUA_API int lua_setfenv(lua_State* L, int idx);
LUA_API int luau_load(lua_State* L, const char* chunkname, const char* data, size_t size, int env);
LUA_API void lua_call(lua_State* L, int nargs, int nresults);
LUA_API int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);
LUA_API int lua_yield(lua_State* L, int nresults);
LUA_API int lua_break(lua_State* L);
LUA_API int lua_resume(lua_State* L, lua_State* from, int narg);
LUA_API int lua_resumeerror(lua_State* L, lua_State* from);
LUA_API int lua_status(lua_State* L);
LUA_API int lua_isyieldable(lua_State* L);
LUA_API void* lua_getthreaddata(lua_State* L);
LUA_API void lua_setthreaddata(lua_State* L, void* data);
LUA_API int lua_costatus(lua_State* L, lua_State* co);
enum lua_GCOp
{
 LUA_GCSTOP,
 LUA_GCRESTART,
 LUA_GCCOLLECT,
 LUA_GCCOUNT,
 LUA_GCCOUNTB,
 LUA_GCISRUNNING,
 LUA_GCSTEP,
 LUA_GCSETGOAL,
 LUA_GCSETSTEPMUL,
 LUA_GCSETSTEPSIZE,
};
LUA_API int lua_gc(lua_State* L, int what, int data);
LUA_API void lua_setmemcat(lua_State* L, int category);
LUA_API size_t lua_totalbytes(lua_State* L, int category);
LUA_API l_noret lua_error(lua_State* L);
LUA_API int lua_next(lua_State* L, int idx);
LUA_API int lua_rawiter(lua_State* L, int idx, int iter);
LUA_API void lua_concat(lua_State* L, int n);
LUA_API uintptr_t lua_encodepointer(lua_State* L, uintptr_t p);
LUA_API double lua_clock();
LUA_API void lua_setuserdatatag(lua_State* L, int idx, int tag);
typedef void (*lua_Destructor)(lua_State* L, void* userdata);
LUA_API void lua_setuserdatadtor(lua_State* L, int tag, lua_Destructor dtor);
LUA_API lua_Destructor lua_getuserdatadtor(lua_State* L, int tag);
LUA_API void lua_clonefunction(lua_State* L, int idx);
LUA_API void lua_cleartable(lua_State* L, int idx);
LUA_API lua_Alloc lua_getallocf(lua_State* L, void** ud);
#define LUA_NOREF -1
#define LUA_REFNIL 0
LUA_API int lua_ref(lua_State* L, int idx);
LUA_API void lua_unref(lua_State* L, int ref);
#define lua_getref(L, ref) lua_rawgeti(L, LUA_REGISTRYINDEX, (ref))
#define lua_tonumber(L, i) lua_tonumberx(L, i, NULL)
#define lua_tointeger(L, i) lua_tointegerx(L, i, NULL)
#define lua_tounsigned(L, i) lua_tounsignedx(L, i, NULL)
#define lua_pop(L, n) lua_settop(L, -(n)-1)
#define lua_newtable(L) lua_createtable(L, 0, 0)
#define lua_newuserdata(L, s) lua_newuserdatatagged(L, s, 0)
#define lua_strlen(L, i) lua_objlen(L, (i))
#define lua_isfunction(L, n) (lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L, n) (lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L, n) (lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L, n) (lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L, n) (lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isvector(L, n) (lua_type(L, (n)) == LUA_TVECTOR)
#define lua_isthread(L, n) (lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L, n) (lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n) (lua_type(L, (n)) <= LUA_TNIL)
#define lua_pushliteral(L, s) lua_pushlstring(L, "" s, (sizeof(s) / sizeof(char)) - 1)
#define lua_pushcfunction(L, fn, debugname) lua_pushcclosurek(L, fn, debugname, 0, NULL)
#define lua_pushcclosure(L, fn, debugname, nup) lua_pushcclosurek(L, fn, debugname, nup, NULL)
#define lua_setglobal(L, s) lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L, s) lua_getfield(L, LUA_GLOBALSINDEX, (s))
#define lua_tostring(L, i) lua_tolstring(L, (i), NULL)
#define lua_pushfstring(L, fmt, ...) lua_pushfstringL(L, fmt, ##__VA_ARGS__)
typedef struct lua_Debug lua_Debug;
typedef void (*lua_Hook)(lua_State* L, lua_Debug* ar);
LUA_API int lua_stackdepth(lua_State* L);
LUA_API int lua_getinfo(lua_State* L, int level, const char* what, lua_Debug* ar);
LUA_API int lua_getargument(lua_State* L, int level, int n);
LUA_API const char* lua_getlocal(lua_State* L, int level, int n);
LUA_API const char* lua_setlocal(lua_State* L, int level, int n);
LUA_API const char* lua_getupvalue(lua_State* L, int funcindex, int n);
LUA_API const char* lua_setupvalue(lua_State* L, int funcindex, int n);
LUA_API void lua_singlestep(lua_State* L, int enabled);
LUA_API int lua_breakpoint(lua_State* L, int funcindex, int line, int enabled);
typedef void (*lua_Coverage)(void* context, const char* function, int linedefined, int depth, const int* hits, size_t size);
LUA_API void lua_getcoverage(lua_State* L, int funcindex, void* context, lua_Coverage callback);
LUA_API const char* lua_debugtrace(lua_State* L);
struct lua_Debug
{
 const char* name;
 const char* what; // (s) `Lua', `C', `main', `tail'
 const char* source;
 const char* short_src; // (s)
 int linedefined;
 int currentline; // (l)
 unsigned char nupvals;
 unsigned char nparams; // (a) number of parameters
 char isvararg;
 void* userdata; // only valid in luau_callhook
 char ssbuf[LUA_IDSIZE];
};
struct lua_Callbacks
{
 void* userdata;
 void (*interrupt)(lua_State* L, int gc);
 void (*panic)(lua_State* L, int errcode); // gets called when an unprotected error is raised (if longjmp is used)
 void (*userthread)(lua_State* LP, lua_State* L);
 int16_t (*useratom)(const char* s, size_t l); // gets called when a string is created; returned atom can be retrieved via tostringatom
 void (*debugbreak)(lua_State* L, lua_Debug* ar);
 void (*debugstep)(lua_State* L, lua_Debug* ar); // gets called after each instruction in single step mode
 void (*debuginterrupt)(lua_State* L, lua_Debug* ar);
 void (*debugprotectederror)(lua_State* L); // gets called when protected call results in an error
};
typedef struct lua_Callbacks lua_Callbacks;
LUA_API lua_Callbacks* lua_callbacks(lua_State* L);
#define luaL_error(L, fmt, ...) luaL_errorL(L, fmt, ##__VA_ARGS__)
#define luaL_typeerror(L, narg, tname) luaL_typeerrorL(L, narg, tname)
#define luaL_argerror(L, narg, extramsg) luaL_argerrorL(L, narg, extramsg)
struct luaL_Reg
{
 const char* name;
 lua_CFunction func;
};
typedef struct luaL_Reg luaL_Reg;
LUALIB_API void luaL_register(lua_State* L, const char* libname, const luaL_Reg* l);
LUALIB_API int luaL_getmetafield(lua_State* L, int obj, const char* e);
LUALIB_API int luaL_callmeta(lua_State* L, int obj, const char* e);
LUALIB_API l_noret luaL_typeerrorL(lua_State* L, int narg, const char* tname);
LUALIB_API l_noret luaL_argerrorL(lua_State* L, int narg, const char* extramsg);
LUALIB_API const char* luaL_checklstring(lua_State* L, int numArg, size_t* l);
LUALIB_API const char* luaL_optlstring(lua_State* L, int numArg, const char* def, size_t* l);
LUALIB_API double luaL_checknumber(lua_State* L, int numArg);
LUALIB_API double luaL_optnumber(lua_State* L, int nArg, double def);
LUALIB_API int luaL_checkboolean(lua_State* L, int narg);
LUALIB_API int luaL_optboolean(lua_State* L, int narg, int def);
LUALIB_API int luaL_checkinteger(lua_State* L, int numArg);
LUALIB_API int luaL_optinteger(lua_State* L, int nArg, int def);
LUALIB_API unsigned luaL_checkunsigned(lua_State* L, int numArg);
LUALIB_API unsigned luaL_optunsigned(lua_State* L, int numArg, unsigned def);
LUALIB_API const float* luaL_checkvector(lua_State* L, int narg);
LUALIB_API const float* luaL_optvector(lua_State* L, int narg, const float* def);
LUALIB_API void luaL_checkstack(lua_State* L, int sz, const char* msg);
LUALIB_API void luaL_checktype(lua_State* L, int narg, int t);
LUALIB_API void luaL_checkany(lua_State* L, int narg);
LUALIB_API int luaL_newmetatable(lua_State* L, const char* tname);
LUALIB_API void* luaL_checkudata(lua_State* L, int ud, const char* tname);
LUALIB_API void luaL_where(lua_State* L, int lvl);
LUALIB_API LUA_PRINTF_ATTR(2, 3) l_noret luaL_errorL(lua_State* L, const char* fmt, ...);
LUALIB_API int luaL_checkoption(lua_State* L, int narg, const char* def, const char* const lst[]);
LUALIB_API const char* luaL_tolstring(lua_State* L, int idx, size_t* len);
LUALIB_API lua_State* luaL_newstate(void);
LUALIB_API const char* luaL_findtable(lua_State* L, int idx, const char* fname, int szhint);
LUALIB_API const char* luaL_typename(lua_State* L, int idx);
#define luaL_argcheck(L, cond, arg, extramsg) ((void)((cond) ? (void)0 : luaL_argerror(L, arg, extramsg)))
#define luaL_argexpected(L, cond, arg, tname) ((void)((cond) ? (void)0 : luaL_typeerror(L, arg, tname)))
#define luaL_checkstring(L, n) (luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L, n, d) (luaL_optlstring(L, (n), (d), NULL))
#define luaL_getmetatable(L, n) (lua_getfield(L, LUA_REGISTRYINDEX, (n)))
#define luaL_opt(L, f, n, d) (lua_isnoneornil(L, (n)) ? (d) : f(L, (n)))
struct luaL_Buffer
{
 char* p;
 char* end; // end of the current buffer
 lua_State* L;
 struct TString* storage;
 char buffer[LUA_BUFFERSIZE];
};
typedef struct luaL_Buffer luaL_Buffer;
#define luaL_addchar(B, c) ((void)((B)->p < (B)->end || luaL_extendbuffer(B, 1, -1)), (*(B)->p++ = (char)(c)))
#define luaL_addstring(B, s) luaL_addlstring(B, s, strlen(s), -1)
LUALIB_API void luaL_buffinit(lua_State* L, luaL_Buffer* B);
LUALIB_API char* luaL_buffinitsize(lua_State* L, luaL_Buffer* B, size_t size);
LUALIB_API char* luaL_extendbuffer(luaL_Buffer* B, size_t additionalsize, int boxloc);
LUALIB_API void luaL_reservebuffer(luaL_Buffer* B, size_t size, int boxloc);
LUALIB_API void luaL_addlstring(luaL_Buffer* B, const char* s, size_t l, int boxloc);
LUALIB_API void luaL_addvalue(luaL_Buffer* B);
LUALIB_API void luaL_addvalueany(luaL_Buffer* B, int idx);
LUALIB_API void luaL_pushresult(luaL_Buffer* B);
LUALIB_API void luaL_pushresultsize(luaL_Buffer* B, size_t size);
LUALIB_API int luaopen_base(lua_State* L);
#define LUA_COLIBNAME "coroutine"
LUALIB_API int luaopen_coroutine(lua_State* L);
#define LUA_TABLIBNAME "table"
LUALIB_API int luaopen_table(lua_State* L);
#define LUA_OSLIBNAME "os"
LUALIB_API int luaopen_os(lua_State* L);
#define LUA_STRLIBNAME "string"
LUALIB_API int luaopen_string(lua_State* L);
#define LUA_BITLIBNAME "bit32"
LUALIB_API int luaopen_bit32(lua_State* L);
#define LUA_UTF8LIBNAME "utf8"
LUALIB_API int luaopen_utf8(lua_State* L);
#define LUA_MATHLIBNAME "math"
LUALIB_API int luaopen_math(lua_State* L);
#define LUA_DBLIBNAME "debug"
LUALIB_API int luaopen_debug(lua_State* L);
LUALIB_API void luaL_openlibs(lua_State* L);
LUALIB_API void luaL_sandbox(lua_State* L);
LUALIB_API void luaL_sandboxthread(lua_State* L);
#ifndef LUACODE_API
#define LUACODE_API extern
#endif
typedef struct lua_CompileOptions lua_CompileOptions;
struct lua_CompileOptions
{
 int optimizationLevel; // default=1
 int debugLevel; // default=1
 int coverageLevel; // default=0
 const char* vectorLib;
 const char* vectorCtor;
 const char* vectorType;
 const char* const* mutableGlobals;
};
LUACODE_API char* luau_compile(const char* source, size_t size, lua_CompileOptions* options, size_t* outsize);
