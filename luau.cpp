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
#include "luau.hpp"

enum LuauOpcode
{
 LOP_NOP,
 LOP_BREAK,
 LOP_LOADNIL,
 LOP_LOADB,
 LOP_LOADN,
 LOP_LOADK,
 LOP_MOVE,
 LOP_GETGLOBAL,
 LOP_SETGLOBAL,
 LOP_GETUPVAL,
 LOP_SETUPVAL,
 LOP_CLOSEUPVALS,
 LOP_GETIMPORT,
 LOP_GETTABLE,
 LOP_SETTABLE,
 LOP_GETTABLEKS,
 LOP_SETTABLEKS,
 LOP_GETTABLEN,
 LOP_SETTABLEN,
 LOP_NEWCLOSURE,
 LOP_NAMECALL,
 LOP_CALL,
 LOP_RETURN,
 LOP_JUMP,
 LOP_JUMPBACK,
 LOP_JUMPIF,
 LOP_JUMPIFNOT,
 LOP_JUMPIFEQ,
 LOP_JUMPIFLE,
 LOP_JUMPIFLT,
 LOP_JUMPIFNOTEQ,
 LOP_JUMPIFNOTLE,
 LOP_JUMPIFNOTLT,
 LOP_ADD,
 LOP_SUB,
 LOP_MUL,
 LOP_DIV,
 LOP_MOD,
 LOP_POW,
 LOP_ADDK,
 LOP_SUBK,
 LOP_MULK,
 LOP_DIVK,
 LOP_MODK,
 LOP_POWK,
 LOP_AND,
 LOP_OR,
 LOP_ANDK,
 LOP_ORK,
 LOP_CONCAT,
 LOP_NOT,
 LOP_MINUS,
 LOP_LENGTH,
 LOP_NEWTABLE,
 LOP_DUPTABLE,
 LOP_SETLIST,
 LOP_FORNPREP,
 LOP_FORNLOOP,
 LOP_FORGLOOP,
 LOP_FORGPREP_INEXT,
 LOP_DEP_FORGLOOP_INEXT,
 LOP_FORGPREP_NEXT,
 LOP_NATIVECALL,
 LOP_GETVARARGS,
 LOP_DUPCLOSURE,
 LOP_PREPVARARGS,
 LOP_LOADKX,
 LOP_JUMPX,
 LOP_FASTCALL,
 LOP_COVERAGE,
 LOP_CAPTURE,
 LOP_DEP_JUMPIFEQK,
 LOP_DEP_JUMPIFNOTEQK,
 LOP_FASTCALL1,
 LOP_FASTCALL2,
 LOP_FASTCALL2K,
 LOP_FORGPREP,
 LOP_JUMPXEQKNIL,
 LOP_JUMPXEQKB,
 LOP_JUMPXEQKN,
 LOP_JUMPXEQKS,
 LOP__COUNT
};
#define LUAU_INSN_OP(insn) ((insn) & 0xff)
#define LUAU_INSN_A(insn) (((insn) >> 8) & 0xff)
#define LUAU_INSN_B(insn) (((insn) >> 16) & 0xff)
#define LUAU_INSN_C(insn) (((insn) >> 24) & 0xff)
#define LUAU_INSN_D(insn) (int32_t(insn) >> 16)
#define LUAU_INSN_E(insn) (int32_t(insn) >> 8)
enum LuauBytecodeTag
{
 LBC_VERSION_MIN = 3,
 LBC_VERSION_MAX = 3,
 LBC_VERSION_TARGET = 3,
 LBC_CONSTANT_NIL = 0,
 LBC_CONSTANT_BOOLEAN,
 LBC_CONSTANT_NUMBER,
 LBC_CONSTANT_STRING,
 LBC_CONSTANT_IMPORT,
 LBC_CONSTANT_TABLE,
 LBC_CONSTANT_CLOSURE,
};
enum LuauBuiltinFunction
{
 LBF_NONE = 0,
 LBF_ASSERT,
 LBF_MATH_ABS,
 LBF_MATH_ACOS,
 LBF_MATH_ASIN,
 LBF_MATH_ATAN2,
 LBF_MATH_ATAN,
 LBF_MATH_CEIL,
 LBF_MATH_COSH,
 LBF_MATH_COS,
 LBF_MATH_DEG,
 LBF_MATH_EXP,
 LBF_MATH_FLOOR,
 LBF_MATH_FMOD,
 LBF_MATH_FREXP,
 LBF_MATH_LDEXP,
 LBF_MATH_LOG10,
 LBF_MATH_LOG,
 LBF_MATH_MAX,
 LBF_MATH_MIN,
 LBF_MATH_MODF,
 LBF_MATH_POW,
 LBF_MATH_RAD,
 LBF_MATH_SINH,
 LBF_MATH_SIN,
 LBF_MATH_SQRT,
 LBF_MATH_TANH,
 LBF_MATH_TAN,
 LBF_BIT32_ARSHIFT,
 LBF_BIT32_BAND,
 LBF_BIT32_BNOT,
 LBF_BIT32_BOR,
 LBF_BIT32_BXOR,
 LBF_BIT32_BTEST,
 LBF_BIT32_EXTRACT,
 LBF_BIT32_LROTATE,
 LBF_BIT32_LSHIFT,
 LBF_BIT32_REPLACE,
 LBF_BIT32_RROTATE,
 LBF_BIT32_RSHIFT,
 LBF_TYPE,
 LBF_STRING_BYTE,
 LBF_STRING_CHAR,
 LBF_STRING_LEN,
 LBF_TYPEOF,
 LBF_STRING_SUB,
 LBF_MATH_CLAMP,
 LBF_MATH_SIGN,
 LBF_MATH_ROUND,
 LBF_RAWSET,
 LBF_RAWGET,
 LBF_RAWEQUAL,
 LBF_TABLE_INSERT,
 LBF_TABLE_UNPACK,
 LBF_VECTOR,
 LBF_BIT32_COUNTLZ,
 LBF_BIT32_COUNTRZ,
 LBF_SELECT_VARARG,
 LBF_RAWLEN,
 LBF_BIT32_EXTRACTK,
 LBF_GETMETATABLE,
 LBF_SETMETATABLE,
};
enum LuauCaptureType
{
 LCT_VAL = 0,
 LCT_REF,
 LCT_UPVAL,
};
#ifdef _MSC_VER
#define LUAU_NORETURN __declspec(noreturn)
#define LUAU_NOINLINE __declspec(noinline)
#define LUAU_FORCEINLINE __forceinline
#define LUAU_LIKELY(x) x
#define LUAU_UNLIKELY(x) x
#define LUAU_UNREACHABLE() __assume(false)
#define LUAU_DEBUGBREAK() __debugbreak()
#else
#define LUAU_NORETURN __attribute__((__noreturn__))
#define LUAU_NOINLINE __attribute__((noinline))
#define LUAU_FORCEINLINE inline __attribute__((always_inline))
#define LUAU_LIKELY(x) __builtin_expect(x, 1)
#define LUAU_UNLIKELY(x) __builtin_expect(x, 0)
#define LUAU_UNREACHABLE() __builtin_unreachable()
#define LUAU_DEBUGBREAK() __builtin_trap()
#endif
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define LUAU_BIG_ENDIAN
#endif
namespace Luau
{
using AssertHandler = int (*)(const char* expression, const char* file, int line, const char* function);
inline AssertHandler& assertHandler()
{
 static AssertHandler handler = nullptr;
 return handler;
}
LUAU_NOINLINE inline int assertCallHandler(const char* expression, const char* file, int line, const char* function)
{
 if (AssertHandler handler = assertHandler())
 return handler(expression, file, line, function);
 return 1;
}
}
#if !defined(NDEBUG) || defined(LUAU_ENABLE_ASSERT)
#define LUAU_ASSERT(expr) ((void)(!!(expr) || (Luau::assertCallHandler(#expr, __FILE__, __LINE__, __FUNCTION__) && (LUAU_DEBUGBREAK(), 0))))
#define LUAU_ASSERTENABLED
#else
#define LUAU_ASSERT(expr) (void)sizeof(!!(expr))
#endif
namespace Luau
{
template<typename T>
struct FValue
{
 static FValue* list;
 T value;
 bool dynamic;
 const char* name;
 FValue* next;
 FValue(const char* name, T def, bool dynamic)
 : value(def)
 , dynamic(dynamic)
 , name(name)
 , next(list)
 {
 list = this;
 }
 operator T() const
 {
 return value;
 }
};
template<typename T>
FValue<T>* FValue<T>::list = nullptr;
}
#define LUAU_FASTFLAG(flag) namespace FFlag { extern Luau::FValue<bool> flag; }
#define LUAU_FASTFLAGVARIABLE(flag, def) namespace FFlag { Luau::FValue<bool> flag(#flag, def, false); }
#define LUAU_FASTINT(flag) namespace FInt { extern Luau::FValue<int> flag; }
#define LUAU_FASTINTVARIABLE(flag, def) namespace FInt { Luau::FValue<int> flag(#flag, def, false); }
#define LUAU_DYNAMIC_FASTFLAG(flag) namespace DFFlag { extern Luau::FValue<bool> flag; }
#define LUAU_DYNAMIC_FASTFLAGVARIABLE(flag, def) namespace DFFlag { Luau::FValue<bool> flag(#flag, def, true); }
#define LUAU_DYNAMIC_FASTINT(flag) namespace DFInt { extern Luau::FValue<int> flag; }
#define LUAU_DYNAMIC_FASTINTVARIABLE(flag, def) namespace DFInt { Luau::FValue<int> flag(#flag, def, true); }
#if defined(__GNUC__)
#define LUAU_PRINTF_ATTR(fmt, arg) __attribute__((format(printf, fmt, arg)))
#else
#define LUAU_PRINTF_ATTR(fmt, arg)
#endif
#include <functional>
#include <utility>
#include <type_traits>
#include <stdint.h>
namespace Luau
{
struct DenseHashPointer
{
 size_t operator()(const void* key) const
 {
 return (uintptr_t(key) >> 4) ^ (uintptr_t(key) >> 9);
 }
};
namespace detail
{
template<typename T>
using DenseHashDefault = std::conditional_t<std::is_pointer_v<T>, DenseHashPointer, std::hash<T>>;
template<typename Key, typename Item, typename MutableItem, typename ItemInterface, typename Hash, typename Eq>
class DenseHashTable
{
public:
 class const_iterator;
 class iterator;
 DenseHashTable(const Key& empty_key, size_t buckets = 0)
 : data(nullptr)
 , capacity(0)
 , count(0)
 , empty_key(empty_key)
 {
 LUAU_ASSERT(eq(empty_key, empty_key));
 LUAU_ASSERT((buckets & (buckets - 1)) == 0);
 if (buckets)
 {
 data = static_cast<Item*>(::operator new(sizeof(Item) * buckets));
 capacity = buckets;
 ItemInterface::fill(data, buckets, empty_key);
 }
 }
 ~DenseHashTable()
 {
 if (data)
 destroy();
 }
 DenseHashTable(const DenseHashTable& other)
 : data(nullptr)
 , capacity(0)
 , count(other.count)
 , empty_key(other.empty_key)
 {
 if (other.capacity)
 {
 data = static_cast<Item*>(::operator new(sizeof(Item) * other.capacity));
 for (size_t i = 0; i < other.capacity; ++i)
 {
 new (&data[i]) Item(other.data[i]);
 capacity = i + 1;
 }
 }
 }
 DenseHashTable(DenseHashTable&& other)
 : data(other.data)
 , capacity(other.capacity)
 , count(other.count)
 , empty_key(other.empty_key)
 {
 other.data = nullptr;
 other.capacity = 0;
 other.count = 0;
 }
 DenseHashTable& operator=(DenseHashTable&& other)
 {
 if (this != &other)
 {
 if (data)
 destroy();
 data = other.data;
 capacity = other.capacity;
 count = other.count;
 empty_key = other.empty_key;
 other.data = nullptr;
 other.capacity = 0;
 other.count = 0;
 }
 return *this;
 }
 DenseHashTable& operator=(const DenseHashTable& other)
 {
 if (this != &other)
 {
 DenseHashTable copy(other);
 *this = std::move(copy);
 }
 return *this;
 }
 void clear()
 {
 if (count == 0)
 return;
 if (capacity > 32)
 {
 destroy();
 }
 else
 {
 ItemInterface::destroy(data, capacity);
 ItemInterface::fill(data, capacity, empty_key);
 }
 count = 0;
 }
 void destroy()
 {
 ItemInterface::destroy(data, capacity);
 ::operator delete(data);
 data = nullptr;
 capacity = 0;
 }
 Item* insert_unsafe(const Key& key)
 {
 LUAU_ASSERT(!eq(key, empty_key));
 size_t hashmod = capacity - 1;
 size_t bucket = hasher(key) & hashmod;
 for (size_t probe = 0; probe <= hashmod; ++probe)
 {
 Item& probe_item = data[bucket];
 if (eq(ItemInterface::getKey(probe_item), empty_key))
 {
 ItemInterface::setKey(probe_item, key);
 count++;
 return &probe_item;
 }
 if (eq(ItemInterface::getKey(probe_item), key))
 {
 return &probe_item;
 }
 bucket = (bucket + probe + 1) & hashmod;
 }
 LUAU_ASSERT(false);
 return NULL;
 }
 const Item* find(const Key& key) const
 {
 if (count == 0)
 return 0;
 if (eq(key, empty_key))
 return 0;
 size_t hashmod = capacity - 1;
 size_t bucket = hasher(key) & hashmod;
 for (size_t probe = 0; probe <= hashmod; ++probe)
 {
 const Item& probe_item = data[bucket];
 if (eq(ItemInterface::getKey(probe_item), key))
 return &probe_item;
 if (eq(ItemInterface::getKey(probe_item), empty_key))
 return NULL;
 bucket = (bucket + probe + 1) & hashmod;
 }
 LUAU_ASSERT(false);
 return NULL;
 }
 void rehash()
 {
 size_t newsize = capacity == 0 ? 16 : capacity * 2;
 DenseHashTable newtable(empty_key, newsize);
 for (size_t i = 0; i < capacity; ++i)
 {
 const Key& key = ItemInterface::getKey(data[i]);
 if (!eq(key, empty_key))
 {
 Item* item = newtable.insert_unsafe(key);
 *item = std::move(data[i]);
 }
 }
 LUAU_ASSERT(count == newtable.count);
 std::swap(data, newtable.data);
 std::swap(capacity, newtable.capacity);
 }
 void rehash_if_full(const Key& key)
 {
 if (count >= capacity * 3 / 4 && !find(key))
 {
 rehash();
 }
 }
 const_iterator begin() const
 {
 size_t start = 0;
 while (start < capacity && eq(ItemInterface::getKey(data[start]), empty_key))
 start++;
 return const_iterator(this, start);
 }
 const_iterator end() const
 {
 return const_iterator(this, capacity);
 }
 iterator begin()
 {
 size_t start = 0;
 while (start < capacity && eq(ItemInterface::getKey(data[start]), empty_key))
 start++;
 return iterator(this, start);
 }
 iterator end()
 {
 return iterator(this, capacity);
 }
 size_t size() const
 {
 return count;
 }
 class const_iterator
 {
 public:
 const_iterator()
 : set(0)
 , index(0)
 {
 }
 const_iterator(const DenseHashTable<Key, Item, MutableItem, ItemInterface, Hash, Eq>* set, size_t index)
 : set(set)
 , index(index)
 {
 }
 const Item& operator*() const
 {
 return set->data[index];
 }
 const Item* operator->() const
 {
 return &set->data[index];
 }
 bool operator==(const const_iterator& other) const
 {
 return set == other.set && index == other.index;
 }
 bool operator!=(const const_iterator& other) const
 {
 return set != other.set || index != other.index;
 }
 const_iterator& operator++()
 {
 size_t size = set->capacity;
 do
 {
 index++;
 } while (index < size && set->eq(ItemInterface::getKey(set->data[index]), set->empty_key));
 return *this;
 }
 const_iterator operator++(int)
 {
 const_iterator res = *this;
 ++*this;
 return res;
 }
 private:
 const DenseHashTable<Key, Item, MutableItem, ItemInterface, Hash, Eq>* set;
 size_t index;
 };
 class iterator
 {
 public:
 iterator()
 : set(0)
 , index(0)
 {
 }
 iterator(DenseHashTable<Key, Item, MutableItem, ItemInterface, Hash, Eq>* set, size_t index)
 : set(set)
 , index(index)
 {
 }
 MutableItem& operator*() const
 {
 return *reinterpret_cast<MutableItem*>(&set->data[index]);
 }
 MutableItem* operator->() const
 {
 return reinterpret_cast<MutableItem*>(&set->data[index]);
 }
 bool operator==(const iterator& other) const
 {
 return set == other.set && index == other.index;
 }
 bool operator!=(const iterator& other) const
 {
 return set != other.set || index != other.index;
 }
 iterator& operator++()
 {
 size_t size = set->capacity;
 do
 {
 index++;
 } while (index < size && set->eq(ItemInterface::getKey(set->data[index]), set->empty_key));
 return *this;
 }
 iterator operator++(int)
 {
 iterator res = *this;
 ++*this;
 return res;
 }
 private:
 DenseHashTable<Key, Item, MutableItem, ItemInterface, Hash, Eq>* set;
 size_t index;
 };
private:
 Item* data;
 size_t capacity;
 size_t count;
 Key empty_key;
 Hash hasher;
 Eq eq;
};
template<typename Key>
struct ItemInterfaceSet
{
 static const Key& getKey(const Key& item)
 {
 return item;
 }
 static void setKey(Key& item, const Key& key)
 {
 item = key;
 }
 static void fill(Key* data, size_t count, const Key& key)
 {
 for (size_t i = 0; i < count; ++i)
 new (&data[i]) Key(key);
 }
 static void destroy(Key* data, size_t count)
 {
 for (size_t i = 0; i < count; ++i)
 data[i].~Key();
 }
};
template<typename Key, typename Value>
struct ItemInterfaceMap
{
 static const Key& getKey(const std::pair<Key, Value>& item)
 {
 return item.first;
 }
 static void setKey(std::pair<Key, Value>& item, const Key& key)
 {
 item.first = key;
 }
 static void fill(std::pair<Key, Value>* data, size_t count, const Key& key)
 {
 for (size_t i = 0; i < count; ++i)
 {
 new (&data[i].first) Key(key);
 new (&data[i].second) Value();
 }
 }
 static void destroy(std::pair<Key, Value>* data, size_t count)
 {
 for (size_t i = 0; i < count; ++i)
 {
 data[i].first.~Key();
 data[i].second.~Value();
 }
 }
};
}
template<typename Key, typename Hash = detail::DenseHashDefault<Key>, typename Eq = std::equal_to<Key>>
class DenseHashSet
{
 typedef detail::DenseHashTable<Key, Key, Key, detail::ItemInterfaceSet<Key>, Hash, Eq> Impl;
 Impl impl;
public:
 typedef typename Impl::const_iterator const_iterator;
 typedef typename Impl::iterator iterator;
 DenseHashSet(const Key& empty_key, size_t buckets = 0)
 : impl(empty_key, buckets)
 {
 }
 void clear()
 {
 impl.clear();
 }
 const Key& insert(const Key& key)
 {
 impl.rehash_if_full(key);
 return *impl.insert_unsafe(key);
 }
 const Key* find(const Key& key) const
 {
 return impl.find(key);
 }
 bool contains(const Key& key) const
 {
 return impl.find(key) != 0;
 }
 size_t size() const
 {
 return impl.size();
 }
 bool empty() const
 {
 return impl.size() == 0;
 }
 const_iterator begin() const
 {
 return impl.begin();
 }
 const_iterator end() const
 {
 return impl.end();
 }
 iterator begin()
 {
 return impl.begin();
 }
 iterator end()
 {
 return impl.end();
 }
};
template<typename Key, typename Value, typename Hash = detail::DenseHashDefault<Key>, typename Eq = std::equal_to<Key>>
class DenseHashMap
{
 typedef detail::DenseHashTable<Key, std::pair<Key, Value>, std::pair<const Key, Value>, detail::ItemInterfaceMap<Key, Value>, Hash, Eq> Impl;
 Impl impl;
public:
 typedef typename Impl::const_iterator const_iterator;
 typedef typename Impl::iterator iterator;
 DenseHashMap(const Key& empty_key, size_t buckets = 0)
 : impl(empty_key, buckets)
 {
 }
 void clear()
 {
 impl.clear();
 }
 Value& operator[](const Key& key)
 {
 impl.rehash_if_full(key);
 return impl.insert_unsafe(key)->second;
 }
 const Value* find(const Key& key) const
 {
 const std::pair<Key, Value>* result = impl.find(key);
 return result ? &result->second : NULL;
 }
 Value* find(const Key& key)
 {
 const std::pair<Key, Value>* result = impl.find(key);
 return result ? const_cast<Value*>(&result->second) : NULL;
 }
 bool contains(const Key& key) const
 {
 return impl.find(key) != 0;
 }
 size_t size() const
 {
 return impl.size();
 }
 bool empty() const
 {
 return impl.size() == 0;
 }
 const_iterator begin() const
 {
 return impl.begin();
 }
 const_iterator end() const
 {
 return impl.end();
 }
 iterator begin()
 {
 return impl.begin();
 }
 iterator end()
 {
 return impl.end();
 }
};
}
#include <string.h>
namespace Luau
{
inline bool isFlagExperimental(const char* flag)
{
 static const char* const kList[] = {
 "LuauInstantiateInSubtyping", // requires some fixes to lua-apps code
 "LuauTypecheckTypeguards", // requires some fixes to lua-apps code (CLI-67030)
 "LuauTinyControlFlowAnalysis", // waiting for updates to packages depended by internal builtin plugins
 nullptr,
 };
 for (const char* item : kList)
 if (item && strcmp(item, flag) == 0)
 return true;
 return false;
}
}
#include <limits.h>
typedef LUAI_USER_ALIGNMENT_T L_Umaxalign;
#define check_exp(c, e) (LUAU_ASSERT(c), (e))
#define api_check(l, e) LUAU_ASSERT(e)
#ifndef cast_to
#define cast_to(t, exp) ((t)(exp))
#endif
#define cast_byte(i) cast_to(uint8_t, (i))
#define cast_num(i) cast_to(double, (i))
#define cast_int(i) cast_to(int, (i))
typedef uint32_t Instruction;
#if defined(HARDSTACKTESTS) && HARDSTACKTESTS
#define condhardstacktests(x) (x)
#else
#define condhardstacktests(x) ((void)0)
#endif
#if defined(HARDMEMTESTS) && HARDMEMTESTS
#define condhardmemtests(x, l) (HARDMEMTESTS >= l ? (x) : (void)0)
#else
#define condhardmemtests(x, l) ((void)0)
#endif
typedef union GCObject GCObject;
#define CommonHeader uint8_t tt; uint8_t marked; uint8_t memcat
typedef struct GCheader
{
 CommonHeader;
} GCheader;
typedef union
{
 GCObject* gc;
 void* p;
 double n;
 int b;
 float v[2];
} Value;
typedef struct lua_TValue
{
 Value value;
 int extra[LUA_EXTRA_SIZE];
 int tt;
} TValue;
#define ttisnil(o) (ttype(o) == LUA_TNIL)
#define ttisnumber(o) (ttype(o) == LUA_TNUMBER)
#define ttisstring(o) (ttype(o) == LUA_TSTRING)
#define ttistable(o) (ttype(o) == LUA_TTABLE)
#define ttisfunction(o) (ttype(o) == LUA_TFUNCTION)
#define ttisboolean(o) (ttype(o) == LUA_TBOOLEAN)
#define ttisuserdata(o) (ttype(o) == LUA_TUSERDATA)
#define ttisthread(o) (ttype(o) == LUA_TTHREAD)
#define ttislightuserdata(o) (ttype(o) == LUA_TLIGHTUSERDATA)
#define ttisvector(o) (ttype(o) == LUA_TVECTOR)
#define ttisupval(o) (ttype(o) == LUA_TUPVAL)
#define ttype(o) ((o)->tt)
#define gcvalue(o) check_exp(iscollectable(o), (o)->value.gc)
#define pvalue(o) check_exp(ttislightuserdata(o), (o)->value.p)
#define nvalue(o) check_exp(ttisnumber(o), (o)->value.n)
#define vvalue(o) check_exp(ttisvector(o), (o)->value.v)
#define tsvalue(o) check_exp(ttisstring(o), &(o)->value.gc->ts)
#define uvalue(o) check_exp(ttisuserdata(o), &(o)->value.gc->u)
#define clvalue(o) check_exp(ttisfunction(o), &(o)->value.gc->cl)
#define hvalue(o) check_exp(ttistable(o), &(o)->value.gc->h)
#define bvalue(o) check_exp(ttisboolean(o), (o)->value.b)
#define thvalue(o) check_exp(ttisthread(o), &(o)->value.gc->th)
#define upvalue(o) check_exp(ttisupval(o), &(o)->value.gc->uv)
#define l_isfalse(o) (ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))
#define checkconsistency(obj) LUAU_ASSERT(!iscollectable(obj) || (ttype(obj) == (obj)->value.gc->gch.tt))
#define checkliveness(g, obj) LUAU_ASSERT(!iscollectable(obj) || ((ttype(obj) == (obj)->value.gc->gch.tt) && !isdead(g, (obj)->value.gc)))
#define setnilvalue(obj) ((obj)->tt = LUA_TNIL)
#define setnvalue(obj, x) { TValue* i_o = (obj); i_o->value.n = (x); i_o->tt = LUA_TNUMBER; }
#if LUA_VECTOR_SIZE == 4
#define setvvalue(obj, x, y, z, w) { TValue* i_o = (obj); float* i_v = i_o->value.v; i_v[0] = (x); i_v[1] = (y); i_v[2] = (z); i_v[3] = (w); i_o->tt = LUA_TVECTOR; }
#else
#define setvvalue(obj, x, y, z, w) { TValue* i_o = (obj); float* i_v = i_o->value.v; i_v[0] = (x); i_v[1] = (y); i_v[2] = (z); i_o->tt = LUA_TVECTOR; }
#endif
#define setpvalue(obj, x) { TValue* i_o = (obj); i_o->value.p = (x); i_o->tt = LUA_TLIGHTUSERDATA; }
#define setbvalue(obj, x) { TValue* i_o = (obj); i_o->value.b = (x); i_o->tt = LUA_TBOOLEAN; }
#define setsvalue(L, obj, x) { TValue* i_o = (obj); i_o->value.gc = cast_to(GCObject*, (x)); i_o->tt = LUA_TSTRING; checkliveness(L->global, i_o); }
#define setuvalue(L, obj, x) { TValue* i_o = (obj); i_o->value.gc = cast_to(GCObject*, (x)); i_o->tt = LUA_TUSERDATA; checkliveness(L->global, i_o); }
#define setthvalue(L, obj, x) { TValue* i_o = (obj); i_o->value.gc = cast_to(GCObject*, (x)); i_o->tt = LUA_TTHREAD; checkliveness(L->global, i_o); }
#define setclvalue(L, obj, x) { TValue* i_o = (obj); i_o->value.gc = cast_to(GCObject*, (x)); i_o->tt = LUA_TFUNCTION; checkliveness(L->global, i_o); }
#define sethvalue(L, obj, x) { TValue* i_o = (obj); i_o->value.gc = cast_to(GCObject*, (x)); i_o->tt = LUA_TTABLE; checkliveness(L->global, i_o); }
#define setptvalue(L, obj, x) { TValue* i_o = (obj); i_o->value.gc = cast_to(GCObject*, (x)); i_o->tt = LUA_TPROTO; checkliveness(L->global, i_o); }
#define setupvalue(L, obj, x) { TValue* i_o = (obj); i_o->value.gc = cast_to(GCObject*, (x)); i_o->tt = LUA_TUPVAL; checkliveness(L->global, i_o); }
#define setobj(L, obj1, obj2) { const TValue* o2 = (obj2); TValue* o1 = (obj1); *o1 = *o2; checkliveness(L->global, o1); }
#define setobj2s setobj
#define setobjt2t setobj
#define setobj2t setobj
#define setobj2n setobj
#define setttype(obj, tt) (ttype(obj) = (tt))
#define iscollectable(o) (ttype(o) >= LUA_TSTRING)
typedef TValue* StkId;
typedef struct TString
{
 CommonHeader;
 int16_t atom;
 TString* next;
 unsigned int hash;
 unsigned int len;
 char data[1];
} TString;
#define getstr(ts) (ts)->data
#define svalue(o) getstr(tsvalue(o))
typedef struct Udata
{
 CommonHeader;
 uint8_t tag;
 int len;
 struct Table* metatable;
 union
 {
 char data[1];
 L_Umaxalign dummy; // ensures maximum alignment for data
 };
} Udata;
typedef struct Proto
{
 CommonHeader;
 TValue* k;
 Instruction* code; // function bytecode
 struct Proto** p;
 uint8_t* lineinfo; // for each instruction, line number as a delta from baseline
 int* abslineinfo;
 struct LocVar* locvars; // information about local variables
 TString** upvalues;
 TString* source;
 TString* debugname;
 uint8_t* debuginsn;
 const Instruction* codeentry;
 void* execdata;
 uintptr_t exectarget;
 GCObject* gclist;
 int sizecode;
 int sizep;
 int sizelocvars;
 int sizeupvalues;
 int sizek;
 int sizelineinfo;
 int linegaplog2;
 int linedefined;
 int bytecodeid;
 uint8_t nups;
 uint8_t numparams;
 uint8_t is_vararg;
 uint8_t maxstacksize;
} Proto;
typedef struct LocVar
{
 TString* varname;
 int startpc;
 int endpc; // first point where variable is dead
 uint8_t reg;
} LocVar;
typedef struct UpVal
{
 CommonHeader;
 uint8_t markedopen;
 TValue* v;
 union
 {
 TValue value;
 struct
 {
 struct UpVal* prev;
 struct UpVal* next;
 struct UpVal* threadnext;
 } open;
 } u;
} UpVal;
#define upisopen(up) ((up)->v != &(up)->u.value)
typedef struct Closure
{
 CommonHeader;
 uint8_t isC;
 uint8_t nupvalues;
 uint8_t stacksize;
 uint8_t preload;
 GCObject* gclist;
 struct Table* env;
 union
 {
 struct
 {
 lua_CFunction f;
 lua_Continuation cont;
 const char* debugname;
 TValue upvals[1];
 } c;
 struct
 {
 struct Proto* p;
 TValue uprefs[1];
 } l;
 };
} Closure;
#define iscfunction(o) (ttype(o) == LUA_TFUNCTION && clvalue(o)->isC)
#define isLfunction(o) (ttype(o) == LUA_TFUNCTION && !clvalue(o)->isC)
typedef struct TKey
{
 ::Value value;
 int extra[LUA_EXTRA_SIZE];
 unsigned tt : 4;
 int next : 28;
} TKey;
typedef struct LuaNode
{
 TValue val;
 TKey key;
} LuaNode;
#define setnodekey(L, node, obj) { LuaNode* n_ = (node); const TValue* i_o = (obj); n_->key.value = i_o->value; memcpy(n_->key.extra, i_o->extra, sizeof(n_->key.extra)); n_->key.tt = i_o->tt; checkliveness(L->global, i_o); }
#define getnodekey(L, obj, node) { TValue* i_o = (obj); const LuaNode* n_ = (node); i_o->value = n_->key.value; memcpy(i_o->extra, n_->key.extra, sizeof(i_o->extra)); i_o->tt = n_->key.tt; checkliveness(L->global, i_o); }
typedef struct Table
{
 CommonHeader;
 uint8_t tmcache;
 uint8_t readonly; // sandboxing feature to prohibit writes to table
 uint8_t safeenv;
 uint8_t lsizenode; // log2 of size of `node' array
 uint8_t nodemask8;
 int sizearray;
 union
 {
 int lastfree;
 int aboundary; // negated 'boundary' of `array' array; iff aboundary < 0
 };
 struct Table* metatable;
 TValue* array;
 LuaNode* node;
 GCObject* gclist;
} Table;
#define lmod(s, size) (check_exp((size & (size - 1)) == 0, (cast_to(int, (s) & ((size)-1)))))
#define twoto(x) ((int)(1 << (x)))
#define sizenode(t) (twoto((t)->lsizenode))
#define luaO_nilobject (&luaO_nilobject_)
LUAI_DATA const TValue luaO_nilobject_;
#define ceillog2(x) (luaO_log2((x)-1) + 1)
LUAI_FUNC int luaO_log2(unsigned int x);
LUAI_FUNC int luaO_rawequalObj(const TValue* t1, const TValue* t2);
LUAI_FUNC int luaO_rawequalKey(const TKey* t1, const TValue* t2);
LUAI_FUNC int luaO_str2d(const char* s, double* result);
LUAI_FUNC const char* luaO_pushvfstring(lua_State* L, const char* fmt, va_list argp);
LUAI_FUNC const char* luaO_pushfstring(lua_State* L, const char* fmt, ...);
LUAI_FUNC const char* luaO_chunkid(char* buf, size_t buflen, const char* source, size_t srclen);
LUAI_FUNC const TValue* luaA_toobject(lua_State* L, int idx);
LUAI_FUNC void luaA_pushobject(lua_State* L, const TValue* o);
typedef enum
{
 TM_INDEX,
 TM_NEWINDEX,
 TM_MODE,
 TM_NAMECALL,
 TM_CALL,
 TM_ITER,
 TM_LEN,
 TM_EQ,
 TM_ADD,
 TM_SUB,
 TM_MUL,
 TM_DIV,
 TM_MOD,
 TM_POW,
 TM_UNM,
 TM_LT,
 TM_LE,
 TM_CONCAT,
 TM_TYPE,
 TM_METATABLE,
 TM_N
} TMS;
#define gfasttm(g, et, e) ((et) == NULL ? NULL : ((et)->tmcache & (1u << (e))) ? NULL : luaT_gettm(et, e, (g)->tmname[e]))
#define fasttm(l, et, e) gfasttm(l->global, et, e)
#define fastnotm(et, e) ((et) == NULL || ((et)->tmcache & (1u << (e))))
LUAI_DATA const char* const luaT_typenames[];
LUAI_DATA const char* const luaT_eventname[];
LUAI_FUNC const TValue* luaT_gettm(Table* events, TMS event, TString* ename);
LUAI_FUNC const TValue* luaT_gettmbyobj(lua_State* L, const TValue* o, TMS event);
LUAI_FUNC const TString* luaT_objtypenamestr(lua_State* L, const TValue* o);
LUAI_FUNC const char* luaT_objtypename(lua_State* L, const TValue* o);
LUAI_FUNC void luaT_init(lua_State* L);
#define registry(L) (&L->global->registry)
#define EXTRA_STACK 5
#define BASIC_CI_SIZE 8
#define BASIC_STACK_SIZE (2 * LUA_MINSTACK)
typedef struct stringtable
{
 TString** hash;
 uint32_t nuse;
 int size;
} stringtable;
typedef struct CallInfo
{
 StkId base;
 StkId func; // function index in the stack
 StkId top;
 const Instruction* savedpc;
 int nresults;
 unsigned int flags; // call frame flags, see LUA_CALLINFO_*
} CallInfo;
#define LUA_CALLINFO_RETURN (1 << 0)
#define LUA_CALLINFO_HANDLE (1 << 1) // should the error thrown during execution get handled by continuation from this callinfo? func must be C
#define LUA_CALLINFO_NATIVE (1 << 2)
#define curr_func(L) (clvalue(L->ci->func))
#define ci_func(ci) (clvalue((ci)->func))
#define f_isLua(ci) (!ci_func(ci)->isC)
#define isLua(ci) (ttisfunction((ci)->func) && f_isLua(ci))
struct GCStats
{
 int32_t triggerterms[32] = {0};
 uint32_t triggertermpos = 0;
 int32_t triggerintegral = 0;
 size_t atomicstarttotalsizebytes = 0;
 size_t endtotalsizebytes = 0;
 size_t heapgoalsizebytes = 0;
 double starttimestamp = 0;
 double atomicstarttimestamp = 0;
 double endtimestamp = 0;
};
#ifdef LUAI_GCMETRICS
struct GCCycleMetrics
{
 size_t starttotalsizebytes = 0;
 size_t heaptriggersizebytes = 0;
 double pausetime = 0.0;
 double starttimestamp = 0.0;
 double endtimestamp = 0.0;
 double marktime = 0.0;
 double markassisttime = 0.0;
 double markmaxexplicittime = 0.0;
 size_t markexplicitsteps = 0;
 size_t markwork = 0;
 double atomicstarttimestamp = 0.0;
 size_t atomicstarttotalsizebytes = 0;
 double atomictime = 0.0;
 double atomictimeupval = 0.0;
 double atomictimeweak = 0.0;
 double atomictimegray = 0.0;
 double atomictimeclear = 0.0;
 double sweeptime = 0.0;
 double sweepassisttime = 0.0;
 double sweepmaxexplicittime = 0.0;
 size_t sweepexplicitsteps = 0;
 size_t sweepwork = 0;
 size_t assistwork = 0;
 size_t explicitwork = 0;
 size_t propagatework = 0;
 size_t propagateagainwork = 0;
 size_t endtotalsizebytes = 0;
};
struct GCMetrics
{
 double stepexplicittimeacc = 0.0;
 double stepassisttimeacc = 0.0;
 uint64_t completedcycles = 0;
 GCCycleMetrics lastcycle;
 GCCycleMetrics currcycle;
};
#endif
struct lua_ExecutionCallbacks
{
 void* context;
 void (*close)(lua_State* L);
 void (*destroy)(lua_State* L, Proto* proto); // called when function is destroyed
 int (*enter)(lua_State* L, Proto* proto);
 void (*setbreakpoint)(lua_State* L, Proto* proto, int line); // called when a breakpoint is set in a function
};
typedef struct global_State
{
 stringtable strt;
 lua_Alloc frealloc;
 void* ud; // auxiliary data to `frealloc'
 uint8_t currentwhite;
 uint8_t gcstate;
 GCObject* gray;
 GCObject* grayagain; // list of objects to be traversed atomically
 GCObject* weak;
 size_t GCthreshold;
 size_t totalbytes; // number of bytes currently allocated
 int gcgoal;
 int gcstepmul; // see LUAI_GCSTEPMUL
 int gcstepsize;
 struct lua_Page* freepages[LUA_SIZECLASSES];
 struct lua_Page* freegcopages[LUA_SIZECLASSES]; // free page linked list for each size class for collectable objects
 struct lua_Page* allgcopages;
 struct lua_Page* sweepgcopage; // position of the sweep in `allgcopages'
 size_t memcatbytes[LUA_MEMORY_CATEGORIES];
 struct lua_State* mainthread;
 UpVal uvhead;
 struct Table* mt[LUA_T_COUNT]; // metatables for basic types
 TString* ttname[LUA_T_COUNT];
 TString* tmname[TM_N]; // array with tag-method names
 TValue pseudotemp;
 TValue registry;
 int registryfree; // next free slot in registry
 struct lua_jmpbuf* errorjmp;
 uint64_t rngstate;
 uint64_t ptrenckey[4]; // pointer encoding key for display
 lua_Callbacks cb;
 lua_ExecutionCallbacks ecb;
 void (*udatagc[LUA_UTAG_LIMIT])(lua_State*, void*);
 GCStats gcstats;
#ifdef LUAI_GCMETRICS
 GCMetrics gcmetrics;
#endif
} global_State;
struct lua_State
{
 CommonHeader;
 uint8_t status;
 uint8_t activememcat;
 bool isactive;
 bool singlestep; // call debugstep hook after each instruction
 StkId top;
 StkId base; // base of current function
 global_State* global;
 CallInfo* ci;
 StkId stack_last; // last free slot in the stack
 StkId stack;
 CallInfo* end_ci;
 CallInfo* base_ci; // array of CallInfo's
 int stacksize;
 int size_ci;
 unsigned short nCcalls;
 unsigned short baseCcalls; // nested C calls when resuming coroutine
 int cachedslot;
 Table* gt;
 UpVal* openupval; // list of open upvalues in this stack
 GCObject* gclist;
 TString* namecall;
 void* userdata;
};
union GCObject
{
 GCheader gch;
 struct TString ts;
 struct Udata u;
 struct Closure cl;
 struct Table h;
 struct Proto p;
 struct UpVal uv;
 struct lua_State th;
};
#define gco2ts(o) check_exp((o)->gch.tt == LUA_TSTRING, &((o)->ts))
#define gco2u(o) check_exp((o)->gch.tt == LUA_TUSERDATA, &((o)->u))
#define gco2cl(o) check_exp((o)->gch.tt == LUA_TFUNCTION, &((o)->cl))
#define gco2h(o) check_exp((o)->gch.tt == LUA_TTABLE, &((o)->h))
#define gco2p(o) check_exp((o)->gch.tt == LUA_TPROTO, &((o)->p))
#define gco2uv(o) check_exp((o)->gch.tt == LUA_TUPVAL, &((o)->uv))
#define gco2th(o) check_exp((o)->gch.tt == LUA_TTHREAD, &((o)->th))
#define obj2gco(v) check_exp(iscollectable(v), cast_to(GCObject*, (v) + 0))
LUAI_FUNC lua_State* luaE_newthread(lua_State* L);
LUAI_FUNC void luaE_freethread(lua_State* L, lua_State* L1, struct lua_Page* page);
#define MAXSSIZE (1 << 30)
#define ATOM_UNDEF -32768
#define sizestring(len) (offsetof(TString, data) + len + 1)
#define luaS_new(L, s) (luaS_newlstr(L, s, strlen(s)))
#define luaS_newliteral(L, s) (luaS_newlstr(L, "" s, (sizeof(s) / sizeof(char)) - 1))
#define luaS_fix(s) l_setbit((s)->marked, FIXEDBIT)
LUAI_FUNC unsigned int luaS_hash(const char* str, size_t len);
LUAI_FUNC void luaS_resize(lua_State* L, int newsize);
LUAI_FUNC TString* luaS_newlstr(lua_State* L, const char* str, size_t l);
LUAI_FUNC void luaS_free(lua_State* L, TString* ts, struct lua_Page* page);
LUAI_FUNC TString* luaS_bufstart(lua_State* L, size_t size);
LUAI_FUNC TString* luaS_buffinish(lua_State* L, TString* ts);
#define gnode(t, i) (&(t)->node[i])
#define gkey(n) (&(n)->key)
#define gval(n) (&(n)->val)
#define gnext(n) ((n)->key.next)
#define gval2slot(t, v) int(cast_to(LuaNode*, static_cast<const TValue*>(v)) - t->node)
#define invalidateTMcache(t) t->tmcache = 0
LUAI_FUNC const TValue* luaH_getnum(Table* t, int key);
LUAI_FUNC TValue* luaH_setnum(lua_State* L, Table* t, int key);
LUAI_FUNC const TValue* luaH_getstr(Table* t, TString* key);
LUAI_FUNC TValue* luaH_setstr(lua_State* L, Table* t, TString* key);
LUAI_FUNC const TValue* luaH_get(Table* t, const TValue* key);
LUAI_FUNC TValue* luaH_set(lua_State* L, Table* t, const TValue* key);
LUAI_FUNC TValue* luaH_newkey(lua_State* L, Table* t, const TValue* key);
LUAI_FUNC Table* luaH_new(lua_State* L, int narray, int lnhash);
LUAI_FUNC void luaH_resizearray(lua_State* L, Table* t, int nasize);
LUAI_FUNC void luaH_resizehash(lua_State* L, Table* t, int nhsize);
LUAI_FUNC void luaH_free(lua_State* L, Table* t, struct lua_Page* page);
LUAI_FUNC int luaH_next(lua_State* L, Table* t, StkId key);
LUAI_FUNC int luaH_getn(Table* t);
LUAI_FUNC Table* luaH_clone(lua_State* L, Table* tt);
LUAI_FUNC void luaH_clear(Table* tt);
#define luaH_setslot(L, t, slot, key) (invalidateTMcache(t), (slot == luaO_nilobject ? luaH_newkey(L, t, key) : cast_to(TValue*, slot)))
extern const LuaNode luaH_dummynode;
#define sizeCclosure(n) (offsetof(Closure, c.upvals) + sizeof(TValue) * (n))
#define sizeLclosure(n) (offsetof(Closure, l.uprefs) + sizeof(TValue) * (n))
LUAI_FUNC Proto* luaF_newproto(lua_State* L);
LUAI_FUNC Closure* luaF_newLclosure(lua_State* L, int nelems, Table* e, Proto* p);
LUAI_FUNC Closure* luaF_newCclosure(lua_State* L, int nelems, Table* e);
LUAI_FUNC UpVal* luaF_findupval(lua_State* L, StkId level);
LUAI_FUNC void luaF_close(lua_State* L, StkId level);
LUAI_FUNC void luaF_closeupval(lua_State* L, UpVal* uv, bool dead);
LUAI_FUNC void luaF_freeproto(lua_State* L, Proto* f, struct lua_Page* page);
LUAI_FUNC void luaF_freeclosure(lua_State* L, Closure* c, struct lua_Page* page);
LUAI_FUNC void luaF_freeupval(lua_State* L, UpVal* uv, struct lua_Page* page);
LUAI_FUNC const LocVar* luaF_getlocal(const Proto* func, int local_number, int pc);
LUAI_FUNC const LocVar* luaF_findlocal(const Proto* func, int local_reg, int pc);
#define pcRel(pc, p) ((pc) ? cast_to(int, (pc) - (p)->code) - 1 : 0)
#define luaG_typeerror(L, o, opname) luaG_typeerrorL(L, o, opname)
#define luaG_forerror(L, o, what) luaG_forerrorL(L, o, what)
#define luaG_runerror(L, fmt, ...) luaG_runerrorL(L, fmt, ##__VA_ARGS__)
#define LUA_MEMERRMSG "not enough memory"
#define LUA_ERRERRMSG "error in error handling"
LUAI_FUNC l_noret luaG_typeerrorL(lua_State* L, const TValue* o, const char* opname);
LUAI_FUNC l_noret luaG_forerrorL(lua_State* L, const TValue* o, const char* what);
LUAI_FUNC l_noret luaG_concaterror(lua_State* L, StkId p1, StkId p2);
LUAI_FUNC l_noret luaG_aritherror(lua_State* L, const TValue* p1, const TValue* p2, TMS op);
LUAI_FUNC l_noret luaG_ordererror(lua_State* L, const TValue* p1, const TValue* p2, TMS op);
LUAI_FUNC l_noret luaG_indexerror(lua_State* L, const TValue* p1, const TValue* p2);
LUAI_FUNC l_noret luaG_methoderror(lua_State* L, const TValue* p1, const TString* p2);
LUAI_FUNC l_noret luaG_readonlyerror(lua_State* L);
LUAI_FUNC LUA_PRINTF_ATTR(2, 3) l_noret luaG_runerrorL(lua_State* L, const char* fmt, ...);
LUAI_FUNC void luaG_pusherror(lua_State* L, const char* error);
LUAI_FUNC void luaG_breakpoint(lua_State* L, Proto* p, int line, bool enable);
LUAI_FUNC bool luaG_onbreak(lua_State* L);
LUAI_FUNC int luaG_getline(Proto* p, int pc);
#define luaD_checkstack(L, n) if ((char*)L->stack_last - (char*)L->top <= (n) * (int)sizeof(TValue)) luaD_growstack(L, n); else condhardstacktests(luaD_reallocstack(L, L->stacksize - EXTRA_STACK));
#define incr_top(L) { luaD_checkstack(L, 1); L->top++; }
#define savestack(L, p) ((char*)(p) - (char*)L->stack)
#define restorestack(L, n) ((TValue*)((char*)L->stack + (n)))
#define expandstacklimit(L, p) { LUAU_ASSERT((p) <= (L)->stack_last); if ((L)->ci->top < (p)) (L)->ci->top = (p); }
#define incr_ci(L) ((L->ci == L->end_ci) ? luaD_growCI(L) : (condhardstacktests(luaD_reallocCI(L, L->size_ci)), ++L->ci))
#define saveci(L, p) ((char*)(p) - (char*)L->base_ci)
#define restoreci(L, n) ((CallInfo*)((char*)L->base_ci + (n)))
#define PCRLUA 0 // initiated a call to a Lua function
#define PCRC 1
#define PCRYIELD 2 // C function yielded
typedef void (*Pfunc)(lua_State* L, void* ud);
LUAI_FUNC CallInfo* luaD_growCI(lua_State* L);
LUAI_FUNC void luaD_call(lua_State* L, StkId func, int nresults);
LUAI_FUNC int luaD_pcall(lua_State* L, Pfunc func, void* u, ptrdiff_t oldtop, ptrdiff_t ef);
LUAI_FUNC void luaD_reallocCI(lua_State* L, int newsize);
LUAI_FUNC void luaD_reallocstack(lua_State* L, int newsize);
LUAI_FUNC void luaD_growstack(lua_State* L, int n);
LUAI_FUNC void luaD_checkCstack(lua_State* L);
LUAI_FUNC l_noret luaD_throw(lua_State* L, int errcode);
LUAI_FUNC int luaD_rawrunprotected(lua_State* L, Pfunc f, void* ud);
#define LUAI_GCGOAL 200
#define LUAI_GCSTEPMUL 200 // GC runs 'twice the speed' of memory allocation
#define LUAI_GCSTEPSIZE 1
#define GCSpause 0
#define GCSpropagate 1
#define GCSpropagateagain 2
#define GCSatomic 3
#define GCSsweep 4
#define keepinvariant(g) ((g)->gcstate == GCSpropagate || (g)->gcstate == GCSpropagateagain || (g)->gcstate == GCSatomic)
#define resetbits(x, m) ((x) &= cast_to(uint8_t, ~(m)))
#define setbits(x, m) ((x) |= (m))
#define testbits(x, m) ((x) & (m))
#define bitmask(b) (1 << (b))
#define bit2mask(b1, b2) (bitmask(b1) | bitmask(b2))
#define l_setbit(x, b) setbits(x, bitmask(b))
#define resetbit(x, b) resetbits(x, bitmask(b))
#define testbit(x, b) testbits(x, bitmask(b))
#define set2bits(x, b1, b2) setbits(x, (bit2mask(b1, b2)))
#define reset2bits(x, b1, b2) resetbits(x, (bit2mask(b1, b2)))
#define test2bits(x, b1, b2) testbits(x, (bit2mask(b1, b2)))
#define WHITE0BIT 0
#define WHITE1BIT 1
#define BLACKBIT 2
#define FIXEDBIT 3
#define WHITEBITS bit2mask(WHITE0BIT, WHITE1BIT)
#define iswhite(x) test2bits((x)->gch.marked, WHITE0BIT, WHITE1BIT)
#define isblack(x) testbit((x)->gch.marked, BLACKBIT)
#define isgray(x) (!testbits((x)->gch.marked, WHITEBITS | bitmask(BLACKBIT)))
#define isfixed(x) testbit((x)->gch.marked, FIXEDBIT)
#define otherwhite(g) (g->currentwhite ^ WHITEBITS)
#define isdead(g, v) (((v)->gch.marked & (WHITEBITS | bitmask(FIXEDBIT))) == (otherwhite(g) & WHITEBITS))
#define changewhite(x) ((x)->gch.marked ^= WHITEBITS)
#define gray2black(x) l_setbit((x)->gch.marked, BLACKBIT)
#define luaC_white(g) cast_to(uint8_t, ((g)->currentwhite) & WHITEBITS)
#define luaC_checkGC(L) { condhardstacktests(luaD_reallocstack(L, L->stacksize - EXTRA_STACK)); if (L->global->totalbytes >= L->global->GCthreshold) { condhardmemtests(luaC_validate(L), 1); luaC_step(L, true); } else { condhardmemtests(luaC_validate(L), 2); } }
#define luaC_barrier(L, p, v) { if (iscollectable(v) && isblack(obj2gco(p)) && iswhite(gcvalue(v))) luaC_barrierf(L, obj2gco(p), gcvalue(v)); }
#define luaC_barriert(L, t, v) { if (iscollectable(v) && isblack(obj2gco(t)) && iswhite(gcvalue(v))) luaC_barriertable(L, t, gcvalue(v)); }
#define luaC_barrierfast(L, t) { if (isblack(obj2gco(t))) luaC_barrierback(L, obj2gco(t), &t->gclist); }
#define luaC_objbarrier(L, p, o) { if (isblack(obj2gco(p)) && iswhite(obj2gco(o))) luaC_barrierf(L, obj2gco(p), obj2gco(o)); }
#define luaC_threadbarrier(L) { if (isblack(obj2gco(L))) luaC_barrierback(L, obj2gco(L), &L->gclist); }
#define luaC_init(L, o, tt_) { o->marked = luaC_white(L->global); o->tt = tt_; o->memcat = L->activememcat; }
LUAI_FUNC void luaC_freeall(lua_State* L);
LUAI_FUNC size_t luaC_step(lua_State* L, bool assist);
LUAI_FUNC void luaC_fullgc(lua_State* L);
LUAI_FUNC void luaC_initobj(lua_State* L, GCObject* o, uint8_t tt);
LUAI_FUNC void luaC_upvalclosed(lua_State* L, UpVal* uv);
LUAI_FUNC void luaC_barrierf(lua_State* L, GCObject* o, GCObject* v);
LUAI_FUNC void luaC_barriertable(lua_State* L, Table* t, GCObject* v);
LUAI_FUNC void luaC_barrierback(lua_State* L, GCObject* o, GCObject** gclist);
LUAI_FUNC void luaC_validate(lua_State* L);
LUAI_FUNC void luaC_dump(lua_State* L, void* file, const char* (*categoryName)(lua_State* L, uint8_t memcat));
LUAI_FUNC int64_t luaC_allocationrate(lua_State* L);
LUAI_FUNC const char* luaC_statename(int state);
#define UTAG_IDTOR LUA_UTAG_LIMIT
#define UTAG_PROXY (LUA_UTAG_LIMIT + 1)
#define sizeudata(len) (offsetof(Udata, data) + len)
LUAI_FUNC Udata* luaU_newudata(lua_State* L, size_t s, int tag);
LUAI_FUNC void luaU_freeudata(lua_State* L, Udata* u, struct lua_Page* page);
#define tostring(L, o) ((ttype(o) == LUA_TSTRING) || (luaV_tostring(L, o)))
#define tonumber(o, n) (ttype(o) == LUA_TNUMBER || (((o) = luaV_tonumber(o, n)) != NULL))
#define equalobj(L, o1, o2) (ttype(o1) == ttype(o2) && luaV_equalval(L, o1, o2))
LUAI_FUNC int luaV_strcmp(const TString* ls, const TString* rs);
LUAI_FUNC int luaV_lessthan(lua_State* L, const TValue* l, const TValue* r);
LUAI_FUNC int luaV_lessequal(lua_State* L, const TValue* l, const TValue* r);
LUAI_FUNC int luaV_equalval(lua_State* L, const TValue* t1, const TValue* t2);
LUAI_FUNC void luaV_doarith(lua_State* L, StkId ra, const TValue* rb, const TValue* rc, TMS op);
LUAI_FUNC void luaV_dolen(lua_State* L, StkId ra, const TValue* rb);
LUAI_FUNC const TValue* luaV_tonumber(const TValue* obj, TValue* n);
LUAI_FUNC const float* luaV_tovector(const TValue* obj);
LUAI_FUNC int luaV_tostring(lua_State* L, StkId obj);
LUAI_FUNC void luaV_gettable(lua_State* L, const TValue* t, TValue* key, StkId val);
LUAI_FUNC void luaV_settable(lua_State* L, const TValue* t, TValue* key, StkId val);
LUAI_FUNC void luaV_concat(lua_State* L, int total, int last);
LUAI_FUNC void luaV_getimport(lua_State* L, Table* env, TValue* k, StkId res, uint32_t id, bool propagatenil);
LUAI_FUNC void luaV_getimport_dep(lua_State* L, Table* env, TValue* k, uint32_t id, bool propagatenil);
LUAI_FUNC void luaV_prepareFORN(lua_State* L, StkId plimit, StkId pstep, StkId pinit);
LUAI_FUNC void luaV_callTM(lua_State* L, int nparams, int res);
LUAI_FUNC void luaV_tryfuncTM(lua_State* L, StkId func);
LUAI_FUNC void luau_execute(lua_State* L);
LUAI_FUNC int luau_precall(lua_State* L, struct lua_TValue* func, int nresults);
LUAI_FUNC void luau_poscall(lua_State* L, StkId first);
LUAI_FUNC void luau_callhook(lua_State* L, lua_Hook hook, void* userdata);
#include <math.h>
#define luai_numadd(a, b) ((a) + (b))
#define luai_numsub(a, b) ((a) - (b))
#define luai_nummul(a, b) ((a) * (b))
#define luai_numdiv(a, b) ((a) / (b))
#define luai_numpow(a, b) (pow(a, b))
#define luai_numunm(a) (-(a))
#define luai_numisnan(a) ((a) != (a))
#define luai_numeq(a, b) ((a) == (b))
#define luai_numlt(a, b) ((a) < (b))
#define luai_numle(a, b) ((a) <= (b))
inline bool luai_veceq(const float* a, const float* b)
{
#if LUA_VECTOR_SIZE == 4
 return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
#else
 return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
#endif
}
inline bool luai_vecisnan(const float* a)
{
#if LUA_VECTOR_SIZE == 4
 return a[0] != a[0] || a[1] != a[1] || a[2] != a[2] || a[3] != a[3];
#else
 return a[0] != a[0] || a[1] != a[1] || a[2] != a[2];
#endif
}
LUAU_FASTMATH_BEGIN
inline double luai_nummod(double a, double b)
{
 return a - floor(a / b) * b;
}
LUAU_FASTMATH_END
#define luai_num2int(i, d) ((i) = (int)(d))
#if defined(_MSC_VER) && defined(_M_IX86)
#define luai_num2unsigned(i, n) { __int64 l; __asm { __asm fld n __asm fistp l} ; i = (unsigned int)l; }
#else
#define luai_num2unsigned(i, n) ((i) = (unsigned)(long long)(n))
#endif
#define LUAI_MAXNUM2STR 48
LUAI_FUNC char* luai_num2str(char* buf, double n);
#define luai_str2num(s, p) strtod((s), (p))
const char* lua_ident = "$Lua: Lua 5.1.4 Copyright (C) 1994-2008 Lua.org, PUC-Rio $\n"
 "$Authors: R. Ierusalimschy, L. H. de Figueiredo & W. Celes $\n"
 "$URL: www.lua.org $\n";
const char* luau_ident = "$Luau: Copyright (C) 2019-2022 Roblox Corporation $\n"
 "$URL: luau-lang.org $\n";
#define api_checknelems(L, n) api_check(L, (n) <= (L->top - L->base))
#define api_checkvalidindex(L, i) api_check(L, (i) != luaO_nilobject)
#define api_incr_top(L) { api_check(L, L->top < L->ci->top); L->top++; }
#define api_update_top(L, p) { api_check(L, p >= L->base && p < L->ci->top); L->top = p; }
#define updateatom(L, ts) { if (ts->atom == ATOM_UNDEF) ts->atom = L->global->cb.useratom ? L->global->cb.useratom(ts->data, ts->len) : -1; }
static Table* getcurrenv(lua_State* L)
{
 if (L->ci == L->base_ci)
 return L->gt; // use global table as environment
 else
 return curr_func(L)->env;
}
static LUAU_NOINLINE TValue* pseudo2addr(lua_State* L, int idx)
{
 api_check(L, lua_ispseudo(idx));
 switch (idx)
 {
 case LUA_REGISTRYINDEX:
 return registry(L);
 case LUA_ENVIRONINDEX:
 {
 sethvalue(L, &L->global->pseudotemp, getcurrenv(L));
 return &L->global->pseudotemp;
 }
 case LUA_GLOBALSINDEX:
 {
 sethvalue(L, &L->global->pseudotemp, L->gt);
 return &L->global->pseudotemp;
 }
 default:
 {
 Closure* func = curr_func(L);
 idx = LUA_GLOBALSINDEX - idx;
 return (idx <= func->nupvalues) ? &func->c.upvals[idx - 1] : cast_to(TValue*, luaO_nilobject);
 }
 }
}
static LUAU_FORCEINLINE TValue* index2addr(lua_State* L, int idx)
{
 if (idx > 0)
 {
 TValue* o = L->base + (idx - 1);
 api_check(L, idx <= L->ci->top - L->base);
 if (o >= L->top)
 return cast_to(TValue*, luaO_nilobject);
 else
 return o;
 }
 else if (idx > LUA_REGISTRYINDEX)
 {
 api_check(L, idx != 0 && -idx <= L->top - L->base);
 return L->top + idx;
 }
 else
 {
 return pseudo2addr(L, idx);
 }
}
const TValue* luaA_toobject(lua_State* L, int idx)
{
 StkId p = index2addr(L, idx);
 return (p == luaO_nilobject) ? NULL : p;
}
void luaA_pushobject(lua_State* L, const TValue* o)
{
 setobj2s(L, L->top, o);
 api_incr_top(L);
}
int lua_checkstack(lua_State* L, int size)
{
 int res = 1;
 if (size > LUAI_MAXCSTACK || (L->top - L->base + size) > LUAI_MAXCSTACK)
 res = 0;
 else if (size > 0)
 {
 luaD_checkstack(L, size);
 expandstacklimit(L, L->top + size);
 }
 return res;
}
void lua_rawcheckstack(lua_State* L, int size)
{
 luaD_checkstack(L, size);
 expandstacklimit(L, L->top + size);
}
void lua_xmove(lua_State* from, lua_State* to, int n)
{
 if (from == to)
 return;
 api_checknelems(from, n);
 api_check(from, from->global == to->global);
 api_check(from, to->ci->top - to->top >= n);
 luaC_threadbarrier(to);
 StkId ttop = to->top;
 StkId ftop = from->top - n;
 for (int i = 0; i < n; i++)
 setobj2s(to, ttop + i, ftop + i);
 from->top = ftop;
 to->top = ttop + n;
}
void lua_xpush(lua_State* from, lua_State* to, int idx)
{
 api_check(from, from->global == to->global);
 luaC_threadbarrier(to);
 setobj2s(to, to->top, index2addr(from, idx));
 api_incr_top(to);
}
lua_State* lua_newthread(lua_State* L)
{
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 lua_State* L1 = luaE_newthread(L);
 setthvalue(L, L->top, L1);
 api_incr_top(L);
 global_State* g = L->global;
 if (g->cb.userthread)
 g->cb.userthread(L, L1);
 return L1;
}
lua_State* lua_mainthread(lua_State* L)
{
 return L->global->mainthread;
}
int lua_absindex(lua_State* L, int idx)
{
 api_check(L, (idx > 0 && idx <= L->top - L->base) || (idx < 0 && -idx <= L->top - L->base) || lua_ispseudo(idx));
 return idx > 0 || lua_ispseudo(idx) ? idx : cast_int(L->top - L->base) + idx + 1;
}
int lua_gettop(lua_State* L)
{
 return cast_int(L->top - L->base);
}
void lua_settop(lua_State* L, int idx)
{
 if (idx >= 0)
 {
 api_check(L, idx <= L->stack_last - L->base);
 while (L->top < L->base + idx)
 setnilvalue(L->top++);
 L->top = L->base + idx;
 }
 else
 {
 api_check(L, -(idx + 1) <= (L->top - L->base));
 L->top += idx + 1;
 }
}
void lua_remove(lua_State* L, int idx)
{
 StkId p = index2addr(L, idx);
 api_checkvalidindex(L, p);
 while (++p < L->top)
 setobj2s(L, p - 1, p);
 L->top--;
}
void lua_insert(lua_State* L, int idx)
{
 luaC_threadbarrier(L);
 StkId p = index2addr(L, idx);
 api_checkvalidindex(L, p);
 for (StkId q = L->top; q > p; q--)
 setobj2s(L, q, q - 1);
 setobj2s(L, p, L->top);
}
void lua_replace(lua_State* L, int idx)
{
 api_checknelems(L, 1);
 luaC_threadbarrier(L);
 StkId o = index2addr(L, idx);
 api_checkvalidindex(L, o);
 if (idx == LUA_ENVIRONINDEX)
 {
 api_check(L, L->ci != L->base_ci);
 Closure* func = curr_func(L);
 api_check(L, ttistable(L->top - 1));
 func->env = hvalue(L->top - 1);
 luaC_barrier(L, func, L->top - 1);
 }
 else if (idx == LUA_GLOBALSINDEX)
 {
 api_check(L, ttistable(L->top - 1));
 L->gt = hvalue(L->top - 1);
 }
 else
 {
 setobj(L, o, L->top - 1);
 if (idx < LUA_GLOBALSINDEX)
 luaC_barrier(L, curr_func(L), L->top - 1);
 }
 L->top--;
}
void lua_pushvalue(lua_State* L, int idx)
{
 luaC_threadbarrier(L);
 StkId o = index2addr(L, idx);
 setobj2s(L, L->top, o);
 api_incr_top(L);
}
int lua_type(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 return (o == luaO_nilobject) ? LUA_TNONE : ttype(o);
}
const char* lua_typename(lua_State* L, int t)
{
 return (t == LUA_TNONE) ? "no value" : luaT_typenames[t];
}
int lua_iscfunction(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 return iscfunction(o);
}
int lua_isLfunction(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 return isLfunction(o);
}
int lua_isnumber(lua_State* L, int idx)
{
 TValue n;
 const TValue* o = index2addr(L, idx);
 return tonumber(o, &n);
}
int lua_isstring(lua_State* L, int idx)
{
 int t = lua_type(L, idx);
 return (t == LUA_TSTRING || t == LUA_TNUMBER);
}
int lua_isuserdata(lua_State* L, int idx)
{
 const TValue* o = index2addr(L, idx);
 return (ttisuserdata(o) || ttislightuserdata(o));
}
int lua_rawequal(lua_State* L, int index1, int index2)
{
 StkId o1 = index2addr(L, index1);
 StkId o2 = index2addr(L, index2);
 return (o1 == luaO_nilobject || o2 == luaO_nilobject) ? 0 : luaO_rawequalObj(o1, o2);
}
int lua_equal(lua_State* L, int index1, int index2)
{
 StkId o1, o2;
 int i;
 o1 = index2addr(L, index1);
 o2 = index2addr(L, index2);
 i = (o1 == luaO_nilobject || o2 == luaO_nilobject) ? 0 : equalobj(L, o1, o2);
 return i;
}
int lua_lessthan(lua_State* L, int index1, int index2)
{
 StkId o1, o2;
 int i;
 o1 = index2addr(L, index1);
 o2 = index2addr(L, index2);
 i = (o1 == luaO_nilobject || o2 == luaO_nilobject) ? 0 : luaV_lessthan(L, o1, o2);
 return i;
}
double lua_tonumberx(lua_State* L, int idx, int* isnum)
{
 TValue n;
 const TValue* o = index2addr(L, idx);
 if (tonumber(o, &n))
 {
 if (isnum)
 *isnum = 1;
 return nvalue(o);
 }
 else
 {
 if (isnum)
 *isnum = 0;
 return 0;
 }
}
int lua_tointegerx(lua_State* L, int idx, int* isnum)
{
 TValue n;
 const TValue* o = index2addr(L, idx);
 if (tonumber(o, &n))
 {
 int res;
 double num = nvalue(o);
 luai_num2int(res, num);
 if (isnum)
 *isnum = 1;
 return res;
 }
 else
 {
 if (isnum)
 *isnum = 0;
 return 0;
 }
}
unsigned lua_tounsignedx(lua_State* L, int idx, int* isnum)
{
 TValue n;
 const TValue* o = index2addr(L, idx);
 if (tonumber(o, &n))
 {
 unsigned res;
 double num = nvalue(o);
 luai_num2unsigned(res, num);
 if (isnum)
 *isnum = 1;
 return res;
 }
 else
 {
 if (isnum)
 *isnum = 0;
 return 0;
 }
}
int lua_toboolean(lua_State* L, int idx)
{
 const TValue* o = index2addr(L, idx);
 return !l_isfalse(o);
}
const char* lua_tolstring(lua_State* L, int idx, size_t* len)
{
 StkId o = index2addr(L, idx);
 if (!ttisstring(o))
 {
 luaC_threadbarrier(L);
 if (!luaV_tostring(L, o))
 {
 if (len != NULL)
 *len = 0;
 return NULL;
 }
 luaC_checkGC(L);
 o = index2addr(L, idx);
 }
 if (len != NULL)
 *len = tsvalue(o)->len;
 return svalue(o);
}
const char* lua_tostringatom(lua_State* L, int idx, int* atom)
{
 StkId o = index2addr(L, idx);
 if (!ttisstring(o))
 return NULL;
 TString* s = tsvalue(o);
 if (atom)
 {
 updateatom(L, s);
 *atom = s->atom;
 }
 return getstr(s);
}
const char* lua_namecallatom(lua_State* L, int* atom)
{
 TString* s = L->namecall;
 if (!s)
 return NULL;
 if (atom)
 {
 updateatom(L, s);
 *atom = s->atom;
 }
 return getstr(s);
}
const float* lua_tovector(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 if (!ttisvector(o))
 return NULL;
 return vvalue(o);
}
int lua_objlen(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 switch (ttype(o))
 {
 case LUA_TSTRING:
 return tsvalue(o)->len;
 case LUA_TUSERDATA:
 return uvalue(o)->len;
 case LUA_TTABLE:
 return luaH_getn(hvalue(o));
 default:
 return 0;
 }
}
lua_CFunction lua_tocfunction(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 return (!iscfunction(o)) ? NULL : cast_to(lua_CFunction, clvalue(o)->c.f);
}
void* lua_tolightuserdata(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 return (!ttislightuserdata(o)) ? NULL : pvalue(o);
}
void* lua_touserdata(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 if (ttisuserdata(o))
 return uvalue(o)->data;
 else if (ttislightuserdata(o))
 return pvalue(o);
 else
 return NULL;
}
void* lua_touserdatatagged(lua_State* L, int idx, int tag)
{
 StkId o = index2addr(L, idx);
 return (ttisuserdata(o) && uvalue(o)->tag == tag) ? uvalue(o)->data : NULL;
}
int lua_userdatatag(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 if (ttisuserdata(o))
 return uvalue(o)->tag;
 return -1;
}
lua_State* lua_tothread(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 return (!ttisthread(o)) ? NULL : thvalue(o);
}
const void* lua_topointer(lua_State* L, int idx)
{
 StkId o = index2addr(L, idx);
 switch (ttype(o))
 {
 case LUA_TSTRING:
 return tsvalue(o);
 case LUA_TTABLE:
 return hvalue(o);
 case LUA_TFUNCTION:
 return clvalue(o);
 case LUA_TTHREAD:
 return thvalue(o);
 case LUA_TUSERDATA:
 return uvalue(o)->data;
 case LUA_TLIGHTUSERDATA:
 return pvalue(o);
 default:
 return NULL;
 }
}
void lua_pushnil(lua_State* L)
{
 setnilvalue(L->top);
 api_incr_top(L);
}
void lua_pushnumber(lua_State* L, double n)
{
 setnvalue(L->top, n);
 api_incr_top(L);
}
void lua_pushinteger(lua_State* L, int n)
{
 setnvalue(L->top, cast_num(n));
 api_incr_top(L);
}
void lua_pushunsigned(lua_State* L, unsigned u)
{
 setnvalue(L->top, cast_num(u));
 api_incr_top(L);
}
#if LUA_VECTOR_SIZE == 4
void lua_pushvector(lua_State* L, float x, float y, float z, float w)
{
 setvvalue(L->top, x, y, z, w);
 api_incr_top(L);
}
#else
void lua_pushvector(lua_State* L, float x, float y, float z)
{
 setvvalue(L->top, x, y, z, 0.0f);
 api_incr_top(L);
}
#endif
void lua_pushlstring(lua_State* L, const char* s, size_t len)
{
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 setsvalue(L, L->top, luaS_newlstr(L, s, len));
 api_incr_top(L);
}
void lua_pushstring(lua_State* L, const char* s)
{
 if (s == NULL)
 lua_pushnil(L);
 else
 lua_pushlstring(L, s, strlen(s));
}
const char* lua_pushvfstring(lua_State* L, const char* fmt, va_list argp)
{
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 const char* ret = luaO_pushvfstring(L, fmt, argp);
 return ret;
}
const char* lua_pushfstringL(lua_State* L, const char* fmt, ...)
{
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 va_list argp;
 va_start(argp, fmt);
 const char* ret = luaO_pushvfstring(L, fmt, argp);
 va_end(argp);
 return ret;
}
void lua_pushcclosurek(lua_State* L, lua_CFunction fn, const char* debugname, int nup, lua_Continuation cont)
{
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 api_checknelems(L, nup);
 Closure* cl = luaF_newCclosure(L, nup, getcurrenv(L));
 cl->c.f = fn;
 cl->c.cont = cont;
 cl->c.debugname = debugname;
 L->top -= nup;
 while (nup--)
 setobj2n(L, &cl->c.upvals[nup], L->top + nup);
 setclvalue(L, L->top, cl);
 LUAU_ASSERT(iswhite(obj2gco(cl)));
 api_incr_top(L);
}
void lua_pushboolean(lua_State* L, int b)
{
 setbvalue(L->top, (b != 0));
 api_incr_top(L);
}
void lua_pushlightuserdata(lua_State* L, void* p)
{
 setpvalue(L->top, p);
 api_incr_top(L);
}
int lua_pushthread(lua_State* L)
{
 luaC_threadbarrier(L);
 setthvalue(L, L->top, L);
 api_incr_top(L);
 return L->global->mainthread == L;
}
int lua_gettable(lua_State* L, int idx)
{
 luaC_threadbarrier(L);
 StkId t = index2addr(L, idx);
 api_checkvalidindex(L, t);
 luaV_gettable(L, t, L->top - 1, L->top - 1);
 return ttype(L->top - 1);
}
int lua_getfield(lua_State* L, int idx, const char* k)
{
 luaC_threadbarrier(L);
 StkId t = index2addr(L, idx);
 api_checkvalidindex(L, t);
 TValue key;
 setsvalue(L, &key, luaS_new(L, k));
 luaV_gettable(L, t, &key, L->top);
 api_incr_top(L);
 return ttype(L->top - 1);
}
int lua_rawgetfield(lua_State* L, int idx, const char* k)
{
 luaC_threadbarrier(L);
 StkId t = index2addr(L, idx);
 api_check(L, ttistable(t));
 TValue key;
 setsvalue(L, &key, luaS_new(L, k));
 setobj2s(L, L->top, luaH_getstr(hvalue(t), tsvalue(&key)));
 api_incr_top(L);
 return ttype(L->top - 1);
}
int lua_rawget(lua_State* L, int idx)
{
 luaC_threadbarrier(L);
 StkId t = index2addr(L, idx);
 api_check(L, ttistable(t));
 setobj2s(L, L->top - 1, luaH_get(hvalue(t), L->top - 1));
 return ttype(L->top - 1);
}
int lua_rawgeti(lua_State* L, int idx, int n)
{
 luaC_threadbarrier(L);
 StkId t = index2addr(L, idx);
 api_check(L, ttistable(t));
 setobj2s(L, L->top, luaH_getnum(hvalue(t), n));
 api_incr_top(L);
 return ttype(L->top - 1);
}
void lua_createtable(lua_State* L, int narray, int nrec)
{
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 sethvalue(L, L->top, luaH_new(L, narray, nrec));
 api_incr_top(L);
}
void lua_setreadonly(lua_State* L, int objindex, int enabled)
{
 const TValue* o = index2addr(L, objindex);
 api_check(L, ttistable(o));
 Table* t = hvalue(o);
 api_check(L, t != hvalue(registry(L)));
 t->readonly = bool(enabled);
}
int lua_getreadonly(lua_State* L, int objindex)
{
 const TValue* o = index2addr(L, objindex);
 api_check(L, ttistable(o));
 Table* t = hvalue(o);
 int res = t->readonly;
 return res;
}
void lua_setsafeenv(lua_State* L, int objindex, int enabled)
{
 const TValue* o = index2addr(L, objindex);
 api_check(L, ttistable(o));
 Table* t = hvalue(o);
 t->safeenv = bool(enabled);
}
int lua_getmetatable(lua_State* L, int objindex)
{
 luaC_threadbarrier(L);
 Table* mt = NULL;
 const TValue* obj = index2addr(L, objindex);
 switch (ttype(obj))
 {
 case LUA_TTABLE:
 mt = hvalue(obj)->metatable;
 break;
 case LUA_TUSERDATA:
 mt = uvalue(obj)->metatable;
 break;
 default:
 mt = L->global->mt[ttype(obj)];
 break;
 }
 if (mt)
 {
 sethvalue(L, L->top, mt);
 api_incr_top(L);
 }
 return mt != NULL;
}
void lua_getfenv(lua_State* L, int idx)
{
 luaC_threadbarrier(L);
 StkId o = index2addr(L, idx);
 api_checkvalidindex(L, o);
 switch (ttype(o))
 {
 case LUA_TFUNCTION:
 sethvalue(L, L->top, clvalue(o)->env);
 break;
 case LUA_TTHREAD:
 sethvalue(L, L->top, thvalue(o)->gt);
 break;
 default:
 setnilvalue(L->top);
 break;
 }
 api_incr_top(L);
}
void lua_settable(lua_State* L, int idx)
{
 api_checknelems(L, 2);
 StkId t = index2addr(L, idx);
 api_checkvalidindex(L, t);
 luaV_settable(L, t, L->top - 2, L->top - 1);
 L->top -= 2;
}
void lua_setfield(lua_State* L, int idx, const char* k)
{
 api_checknelems(L, 1);
 StkId t = index2addr(L, idx);
 api_checkvalidindex(L, t);
 TValue key;
 setsvalue(L, &key, luaS_new(L, k));
 luaV_settable(L, t, &key, L->top - 1);
 L->top--;
}
void lua_rawsetfield(lua_State* L, int idx, const char* k)
{
 api_checknelems(L, 1);
 StkId t = index2addr(L, idx);
 api_check(L, ttistable(t));
 if (hvalue(t)->readonly)
 luaG_readonlyerror(L);
 setobj2t(L, luaH_setstr(L, hvalue(t), luaS_new(L, k)), L->top - 1);
 luaC_barriert(L, hvalue(t), L->top - 1);
 L->top--;
}
void lua_rawset(lua_State* L, int idx)
{
 api_checknelems(L, 2);
 StkId t = index2addr(L, idx);
 api_check(L, ttistable(t));
 if (hvalue(t)->readonly)
 luaG_readonlyerror(L);
 setobj2t(L, luaH_set(L, hvalue(t), L->top - 2), L->top - 1);
 luaC_barriert(L, hvalue(t), L->top - 1);
 L->top -= 2;
}
void lua_rawseti(lua_State* L, int idx, int n)
{
 api_checknelems(L, 1);
 StkId o = index2addr(L, idx);
 api_check(L, ttistable(o));
 if (hvalue(o)->readonly)
 luaG_readonlyerror(L);
 setobj2t(L, luaH_setnum(L, hvalue(o), n), L->top - 1);
 luaC_barriert(L, hvalue(o), L->top - 1);
 L->top--;
}
int lua_setmetatable(lua_State* L, int objindex)
{
 api_checknelems(L, 1);
 TValue* obj = index2addr(L, objindex);
 api_checkvalidindex(L, obj);
 Table* mt = NULL;
 if (!ttisnil(L->top - 1))
 {
 api_check(L, ttistable(L->top - 1));
 mt = hvalue(L->top - 1);
 }
 switch (ttype(obj))
 {
 case LUA_TTABLE:
 {
 if (hvalue(obj)->readonly)
 luaG_readonlyerror(L);
 hvalue(obj)->metatable = mt;
 if (mt)
 luaC_objbarrier(L, hvalue(obj), mt);
 break;
 }
 case LUA_TUSERDATA:
 {
 uvalue(obj)->metatable = mt;
 if (mt)
 luaC_objbarrier(L, uvalue(obj), mt);
 break;
 }
 default:
 {
 L->global->mt[ttype(obj)] = mt;
 break;
 }
 }
 L->top--;
 return 1;
}
int lua_setfenv(lua_State* L, int idx)
{
 int res = 1;
 api_checknelems(L, 1);
 StkId o = index2addr(L, idx);
 api_checkvalidindex(L, o);
 api_check(L, ttistable(L->top - 1));
 switch (ttype(o))
 {
 case LUA_TFUNCTION:
 clvalue(o)->env = hvalue(L->top - 1);
 break;
 case LUA_TTHREAD:
 thvalue(o)->gt = hvalue(L->top - 1);
 break;
 default:
 res = 0;
 break;
 }
 if (res)
 {
 luaC_objbarrier(L, &gcvalue(o)->gch, hvalue(L->top - 1));
 }
 L->top--;
 return res;
}
#define adjustresults(L, nres) { if (nres == LUA_MULTRET && L->top >= L->ci->top) L->ci->top = L->top; }
#define checkresults(L, na, nr) api_check(L, (nr) == LUA_MULTRET || (L->ci->top - L->top >= (nr) - (na)))
void lua_call(lua_State* L, int nargs, int nresults)
{
 StkId func;
 api_checknelems(L, nargs + 1);
 api_check(L, L->status == 0);
 checkresults(L, nargs, nresults);
 func = L->top - (nargs + 1);
 luaD_call(L, func, nresults);
 adjustresults(L, nresults);
}
struct CallS
{
 StkId func;
 int nresults;
};
static void f_call(lua_State* L, void* ud)
{
 struct CallS* c = cast_to(struct CallS*, ud);
 luaD_call(L, c->func, c->nresults);
}
int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc)
{
 api_checknelems(L, nargs + 1);
 api_check(L, L->status == 0);
 checkresults(L, nargs, nresults);
 ptrdiff_t func = 0;
 if (errfunc != 0)
 {
 StkId o = index2addr(L, errfunc);
 api_checkvalidindex(L, o);
 func = savestack(L, o);
 }
 struct CallS c;
 c.func = L->top - (nargs + 1);
 c.nresults = nresults;
 int status = luaD_pcall(L, f_call, &c, savestack(L, c.func), func);
 adjustresults(L, nresults);
 return status;
}
int lua_status(lua_State* L)
{
 return L->status;
}
int lua_costatus(lua_State* L, lua_State* co)
{
 if (co == L)
 return LUA_CORUN;
 if (co->status == LUA_YIELD)
 return LUA_COSUS;
 if (co->status == LUA_BREAK)
 return LUA_CONOR;
 if (co->status != 0)
 return LUA_COERR;
 if (co->ci != co->base_ci)
 return LUA_CONOR;
 if (co->top == co->base)
 return LUA_COFIN;
 return LUA_COSUS;
}
void* lua_getthreaddata(lua_State* L)
{
 return L->userdata;
}
void lua_setthreaddata(lua_State* L, void* data)
{
 L->userdata = data;
}
int lua_gc(lua_State* L, int what, int data)
{
 int res = 0;
 condhardmemtests(luaC_validate(L), 1);
 global_State* g = L->global;
 switch (what)
 {
 case LUA_GCSTOP:
 {
 g->GCthreshold = SIZE_MAX;
 break;
 }
 case LUA_GCRESTART:
 {
 g->GCthreshold = g->totalbytes;
 break;
 }
 case LUA_GCCOLLECT:
 {
 luaC_fullgc(L);
 break;
 }
 case LUA_GCCOUNT:
 {
 res = cast_int(g->totalbytes >> 10);
 break;
 }
 case LUA_GCCOUNTB:
 {
 res = cast_int(g->totalbytes & 1023);
 break;
 }
 case LUA_GCISRUNNING:
 {
 res = (g->GCthreshold != SIZE_MAX);
 break;
 }
 case LUA_GCSTEP:
 {
 size_t amount = (cast_to(size_t, data) << 10);
 ptrdiff_t oldcredit = g->gcstate == GCSpause ? 0 : g->GCthreshold - g->totalbytes;
 if (amount <= g->totalbytes)
 g->GCthreshold = g->totalbytes - amount;
 else
 g->GCthreshold = 0;
#ifdef LUAI_GCMETRICS
 double startmarktime = g->gcmetrics.currcycle.marktime;
 double startsweeptime = g->gcmetrics.currcycle.sweeptime;
#endif
 size_t actualwork = 0;
 while (g->GCthreshold <= g->totalbytes)
 {
 size_t stepsize = luaC_step(L, false);
 actualwork += stepsize;
 if (g->gcstate == GCSpause)
 {
 res = 1; // signal it
 break;
 }
 }
#ifdef LUAI_GCMETRICS
 GCCycleMetrics* cyclemetrics = g->gcstate == GCSpause ? &g->gcmetrics.lastcycle : &g->gcmetrics.currcycle;
 double totalmarktime = cyclemetrics->marktime - startmarktime;
 double totalsweeptime = cyclemetrics->sweeptime - startsweeptime;
 if (totalmarktime > 0.0)
 {
 cyclemetrics->markexplicitsteps++;
 if (totalmarktime > cyclemetrics->markmaxexplicittime)
 cyclemetrics->markmaxexplicittime = totalmarktime;
 }
 if (totalsweeptime > 0.0)
 {
 cyclemetrics->sweepexplicitsteps++;
 if (totalsweeptime > cyclemetrics->sweepmaxexplicittime)
 cyclemetrics->sweepmaxexplicittime = totalsweeptime;
 }
#endif
 if (g->gcstate != GCSpause)
 {
 ptrdiff_t newthreshold = g->totalbytes + actualwork + oldcredit;
 g->GCthreshold = newthreshold < 0 ? 0 : newthreshold;
 }
 break;
 }
 case LUA_GCSETGOAL:
 {
 res = g->gcgoal;
 g->gcgoal = data;
 break;
 }
 case LUA_GCSETSTEPMUL:
 {
 res = g->gcstepmul;
 g->gcstepmul = data;
 break;
 }
 case LUA_GCSETSTEPSIZE:
 {
 res = g->gcstepsize >> 10;
 g->gcstepsize = data << 10;
 break;
 }
 default:
 res = -1;
 }
 return res;
}
l_noret lua_error(lua_State* L)
{
 api_checknelems(L, 1);
 luaD_throw(L, LUA_ERRRUN);
}
int lua_next(lua_State* L, int idx)
{
 luaC_threadbarrier(L);
 StkId t = index2addr(L, idx);
 api_check(L, ttistable(t));
 int more = luaH_next(L, hvalue(t), L->top - 1);
 if (more)
 {
 api_incr_top(L);
 }
 else
 L->top -= 1; // remove key
 return more;
}
int lua_rawiter(lua_State* L, int idx, int iter)
{
 luaC_threadbarrier(L);
 StkId t = index2addr(L, idx);
 api_check(L, ttistable(t));
 api_check(L, iter >= 0);
 Table* h = hvalue(t);
 int sizearray = h->sizearray;
 for (; unsigned(iter) < unsigned(sizearray); ++iter)
 {
 TValue* e = &h->array[iter];
 if (!ttisnil(e))
 {
 StkId top = L->top;
 setnvalue(top + 0, double(iter + 1));
 setobj2s(L, top + 1, e);
 api_update_top(L, top + 2);
 return iter + 1;
 }
 }
 int sizenode = 1 << h->lsizenode;
 for (; unsigned(iter - sizearray) < unsigned(sizenode); ++iter)
 {
 LuaNode* n = &h->node[iter - sizearray];
 if (!ttisnil(gval(n)))
 {
 StkId top = L->top;
 getnodekey(L, top + 0, n);
 setobj2s(L, top + 1, gval(n));
 api_update_top(L, top + 2);
 return iter + 1;
 }
 }
 return -1;
}
void lua_concat(lua_State* L, int n)
{
 api_checknelems(L, n);
 if (n >= 2)
 {
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 luaV_concat(L, n, cast_int(L->top - L->base) - 1);
 L->top -= (n - 1);
 }
 else if (n == 0)
 {
 luaC_threadbarrier(L);
 setsvalue(L, L->top, luaS_newlstr(L, "", 0));
 api_incr_top(L);
 }
}
void* lua_newuserdatatagged(lua_State* L, size_t sz, int tag)
{
 api_check(L, unsigned(tag) < LUA_UTAG_LIMIT || tag == UTAG_PROXY);
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 Udata* u = luaU_newudata(L, sz, tag);
 setuvalue(L, L->top, u);
 api_incr_top(L);
 return u->data;
}
void* lua_newuserdatadtor(lua_State* L, size_t sz, void (*dtor)(void*))
{
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 size_t as = sz < SIZE_MAX - sizeof(dtor) ? sz + sizeof(dtor) : SIZE_MAX;
 Udata* u = luaU_newudata(L, as, UTAG_IDTOR);
 memcpy(&u->data + sz, &dtor, sizeof(dtor));
 setuvalue(L, L->top, u);
 api_incr_top(L);
 return u->data;
}
static const char* aux_upvalue(StkId fi, int n, TValue** val)
{
 Closure* f;
 if (!ttisfunction(fi))
 return NULL;
 f = clvalue(fi);
 if (f->isC)
 {
 if (!(1 <= n && n <= f->nupvalues))
 return NULL;
 *val = &f->c.upvals[n - 1];
 return "";
 }
 else
 {
 Proto* p = f->l.p;
 if (!(1 <= n && n <= p->nups))
 return NULL;
 TValue* r = &f->l.uprefs[n - 1];
 *val = ttisupval(r) ? upvalue(r)->v : r;
 if (!(1 <= n && n <= p->sizeupvalues))
 return "";
 return getstr(p->upvalues[n - 1]);
 }
}
const char* lua_getupvalue(lua_State* L, int funcindex, int n)
{
 luaC_threadbarrier(L);
 TValue* val;
 const char* name = aux_upvalue(index2addr(L, funcindex), n, &val);
 if (name)
 {
 setobj2s(L, L->top, val);
 api_incr_top(L);
 }
 return name;
}
const char* lua_setupvalue(lua_State* L, int funcindex, int n)
{
 api_checknelems(L, 1);
 StkId fi = index2addr(L, funcindex);
 TValue* val;
 const char* name = aux_upvalue(fi, n, &val);
 if (name)
 {
 L->top--;
 setobj(L, val, L->top);
 luaC_barrier(L, clvalue(fi), L->top);
 }
 return name;
}
uintptr_t lua_encodepointer(lua_State* L, uintptr_t p)
{
 global_State* g = L->global;
 return uintptr_t((g->ptrenckey[0] * p + g->ptrenckey[2]) ^ (g->ptrenckey[1] * p + g->ptrenckey[3]));
}
int lua_ref(lua_State* L, int idx)
{
 api_check(L, idx != LUA_REGISTRYINDEX);
 int ref = LUA_REFNIL;
 global_State* g = L->global;
 StkId p = index2addr(L, idx);
 if (!ttisnil(p))
 {
 Table* reg = hvalue(registry(L));
 if (g->registryfree != 0)
 {
 ref = g->registryfree;
 }
 else
 {
 ref = luaH_getn(reg);
 ref++;
 }
 TValue* slot = luaH_setnum(L, reg, ref);
 if (g->registryfree != 0)
 g->registryfree = int(nvalue(slot));
 setobj2t(L, slot, p);
 luaC_barriert(L, reg, p);
 }
 return ref;
}
void lua_unref(lua_State* L, int ref)
{
 if (ref <= LUA_REFNIL)
 return;
 global_State* g = L->global;
 Table* reg = hvalue(registry(L));
 TValue* slot = luaH_setnum(L, reg, ref);
 setnvalue(slot, g->registryfree);
 g->registryfree = ref;
}
void lua_setuserdatatag(lua_State* L, int idx, int tag)
{
 api_check(L, unsigned(tag) < LUA_UTAG_LIMIT);
 StkId o = index2addr(L, idx);
 api_check(L, ttisuserdata(o));
 uvalue(o)->tag = uint8_t(tag);
}
void lua_setuserdatadtor(lua_State* L, int tag, lua_Destructor dtor)
{
 api_check(L, unsigned(tag) < LUA_UTAG_LIMIT);
 L->global->udatagc[tag] = dtor;
}
lua_Destructor lua_getuserdatadtor(lua_State* L, int tag)
{
 api_check(L, unsigned(tag) < LUA_UTAG_LIMIT);
 return L->global->udatagc[tag];
}
void lua_clonefunction(lua_State* L, int idx)
{
 luaC_checkGC(L);
 luaC_threadbarrier(L);
 StkId p = index2addr(L, idx);
 api_check(L, isLfunction(p));
 Closure* cl = clvalue(p);
 Closure* newcl = luaF_newLclosure(L, cl->nupvalues, L->gt, cl->l.p);
 for (int i = 0; i < cl->nupvalues; ++i)
 setobj2n(L, &newcl->l.uprefs[i], &cl->l.uprefs[i]);
 setclvalue(L, L->top, newcl);
 api_incr_top(L);
}
void lua_cleartable(lua_State* L, int idx)
{
 StkId t = index2addr(L, idx);
 api_check(L, ttistable(t));
 Table* tt = hvalue(t);
 if (tt->readonly)
 luaG_readonlyerror(L);
 luaH_clear(tt);
}
lua_Callbacks* lua_callbacks(lua_State* L)
{
 return &L->global->cb;
}
void lua_setmemcat(lua_State* L, int category)
{
 api_check(L, unsigned(category) < LUA_MEMORY_CATEGORIES);
 L->activememcat = uint8_t(category);
}
size_t lua_totalbytes(lua_State* L, int category)
{
 api_check(L, category < LUA_MEMORY_CATEGORIES);
 return category < 0 ? L->global->totalbytes : L->global->memcatbytes[category];
}
#define abs_index(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
static const char* currfuncname(lua_State* L)
{
 Closure* cl = L->ci > L->base_ci ? curr_func(L) : NULL;
 const char* debugname = cl && cl->isC ? cl->c.debugname + 0 : NULL;
 if (debugname && strcmp(debugname, "__namecall") == 0)
 return L->namecall ? getstr(L->namecall) : NULL;
 else
 return debugname;
}
l_noret luaL_argerrorL(lua_State* L, int narg, const char* extramsg)
{
 const char* fname = currfuncname(L);
 if (fname)
 luaL_error(L, "invalid argument #%d to '%s' (%s)", narg, fname, extramsg);
 else
 luaL_error(L, "invalid argument #%d (%s)", narg, extramsg);
}
l_noret luaL_typeerrorL(lua_State* L, int narg, const char* tname)
{
 const char* fname = currfuncname(L);
 const TValue* obj = luaA_toobject(L, narg);
 if (obj)
 {
 if (fname)
 luaL_error(L, "invalid argument #%d to '%s' (%s expected, got %s)", narg, fname, tname, luaT_objtypename(L, obj));
 else
 luaL_error(L, "invalid argument #%d (%s expected, got %s)", narg, tname, luaT_objtypename(L, obj));
 }
 else
 {
 if (fname)
 luaL_error(L, "missing argument #%d to '%s' (%s expected)", narg, fname, tname);
 else
 luaL_error(L, "missing argument #%d (%s expected)", narg, tname);
 }
}
static l_noret tag_error(lua_State* L, int narg, int tag)
{
 luaL_typeerrorL(L, narg, lua_typename(L, tag));
}
void luaL_where(lua_State* L, int level)
{
 lua_Debug ar;
 if (lua_getinfo(L, level, "sl", &ar) && ar.currentline > 0)
 {
 lua_pushfstring(L, "%s:%d: ", ar.short_src, ar.currentline);
 return;
 }
 lua_pushliteral(L, ""); // else, no information available...
}
l_noret luaL_errorL(lua_State* L, const char* fmt, ...)
{
 va_list argp;
 va_start(argp, fmt);
 luaL_where(L, 1);
 lua_pushvfstring(L, fmt, argp);
 va_end(argp);
 lua_concat(L, 2);
 lua_error(L);
}
int luaL_checkoption(lua_State* L, int narg, const char* def, const char* const lst[])
{
 const char* name = (def) ? luaL_optstring(L, narg, def) : luaL_checkstring(L, narg);
 int i;
 for (i = 0; lst[i]; i++)
 if (strcmp(lst[i], name) == 0)
 return i;
 const char* msg = lua_pushfstring(L, "invalid option '%s'", name);
 luaL_argerrorL(L, narg, msg);
}
int luaL_newmetatable(lua_State* L, const char* tname)
{
 lua_getfield(L, LUA_REGISTRYINDEX, tname);
 if (!lua_isnil(L, -1)) // name already in use?
 return 0;
 lua_pop(L, 1);
 lua_newtable(L);
 lua_pushvalue(L, -1);
 lua_setfield(L, LUA_REGISTRYINDEX, tname);
 return 1;
}
void* luaL_checkudata(lua_State* L, int ud, const char* tname)
{
 void* p = lua_touserdata(L, ud);
 if (p != NULL)
 {
 if (lua_getmetatable(L, ud))
 {
 lua_getfield(L, LUA_REGISTRYINDEX, tname); // get correct metatable
 if (lua_rawequal(L, -1, -2))
 {
 lua_pop(L, 2); // remove both metatables
 return p;
 }
 }
 }
 luaL_typeerrorL(L, ud, tname);
}
void luaL_checkstack(lua_State* L, int space, const char* mes)
{
 if (!lua_checkstack(L, space))
 luaL_error(L, "stack overflow (%s)", mes);
}
void luaL_checktype(lua_State* L, int narg, int t)
{
 if (lua_type(L, narg) != t)
 tag_error(L, narg, t);
}
void luaL_checkany(lua_State* L, int narg)
{
 if (lua_type(L, narg) == LUA_TNONE)
 luaL_error(L, "missing argument #%d", narg);
}
const char* luaL_checklstring(lua_State* L, int narg, size_t* len)
{
 const char* s = lua_tolstring(L, narg, len);
 if (!s)
 tag_error(L, narg, LUA_TSTRING);
 return s;
}
const char* luaL_optlstring(lua_State* L, int narg, const char* def, size_t* len)
{
 if (lua_isnoneornil(L, narg))
 {
 if (len)
 *len = (def ? strlen(def) : 0);
 return def;
 }
 else
 return luaL_checklstring(L, narg, len);
}
double luaL_checknumber(lua_State* L, int narg)
{
 int isnum;
 double d = lua_tonumberx(L, narg, &isnum);
 if (!isnum)
 tag_error(L, narg, LUA_TNUMBER);
 return d;
}
double luaL_optnumber(lua_State* L, int narg, double def)
{
 return luaL_opt(L, luaL_checknumber, narg, def);
}
int luaL_checkboolean(lua_State* L, int narg)
{
 if (!lua_isboolean(L, narg))
 tag_error(L, narg, LUA_TBOOLEAN);
 return lua_toboolean(L, narg);
}
int luaL_optboolean(lua_State* L, int narg, int def)
{
 return luaL_opt(L, luaL_checkboolean, narg, def);
}
int luaL_checkinteger(lua_State* L, int narg)
{
 int isnum;
 int d = lua_tointegerx(L, narg, &isnum);
 if (!isnum)
 tag_error(L, narg, LUA_TNUMBER);
 return d;
}
int luaL_optinteger(lua_State* L, int narg, int def)
{
 return luaL_opt(L, luaL_checkinteger, narg, def);
}
unsigned luaL_checkunsigned(lua_State* L, int narg)
{
 int isnum;
 unsigned d = lua_tounsignedx(L, narg, &isnum);
 if (!isnum)
 tag_error(L, narg, LUA_TNUMBER);
 return d;
}
unsigned luaL_optunsigned(lua_State* L, int narg, unsigned def)
{
 return luaL_opt(L, luaL_checkunsigned, narg, def);
}
const float* luaL_checkvector(lua_State* L, int narg)
{
 const float* v = lua_tovector(L, narg);
 if (!v)
 tag_error(L, narg, LUA_TVECTOR);
 return v;
}
const float* luaL_optvector(lua_State* L, int narg, const float* def)
{
 return luaL_opt(L, luaL_checkvector, narg, def);
}
int luaL_getmetafield(lua_State* L, int obj, const char* event)
{
 if (!lua_getmetatable(L, obj))
 return 0;
 lua_pushstring(L, event);
 lua_rawget(L, -2);
 if (lua_isnil(L, -1))
 {
 lua_pop(L, 2);
 return 0;
 }
 else
 {
 lua_remove(L, -2);
 return 1;
 }
}
int luaL_callmeta(lua_State* L, int obj, const char* event)
{
 obj = abs_index(L, obj);
 if (!luaL_getmetafield(L, obj, event))
 return 0;
 lua_pushvalue(L, obj);
 lua_call(L, 1, 1);
 return 1;
}
static int libsize(const luaL_Reg* l)
{
 int size = 0;
 for (; l->name; l++)
 size++;
 return size;
}
void luaL_register(lua_State* L, const char* libname, const luaL_Reg* l)
{
 if (libname)
 {
 int size = libsize(l);
 luaL_findtable(L, LUA_REGISTRYINDEX, "_LOADED", 1);
 lua_getfield(L, -1, libname);
 if (!lua_istable(L, -1))
 {
 lua_pop(L, 1); // remove previous result
 if (luaL_findtable(L, LUA_GLOBALSINDEX, libname, size) != NULL)
 luaL_error(L, "name conflict for module '%s'", libname);
 lua_pushvalue(L, -1);
 lua_setfield(L, -3, libname);
 }
 lua_remove(L, -2);
 }
 for (; l->name; l++)
 {
 lua_pushcfunction(L, l->func, l->name);
 lua_setfield(L, -2, l->name);
 }
}
const char* luaL_findtable(lua_State* L, int idx, const char* fname, int szhint)
{
 const char* e;
 lua_pushvalue(L, idx);
 do
 {
 e = strchr(fname, '.');
 if (e == NULL)
 e = fname + strlen(fname);
 lua_pushlstring(L, fname, e - fname);
 lua_rawget(L, -2);
 if (lua_isnil(L, -1))
 {
 lua_pop(L, 1); // remove this nil
 lua_createtable(L, 0, (*e == '.' ? 1 : szhint));
 lua_pushlstring(L, fname, e - fname);
 lua_pushvalue(L, -2);
 lua_settable(L, -4);
 }
 else if (!lua_istable(L, -1))
 {
 lua_pop(L, 2); // remove table and value
 return fname;
 }
 lua_remove(L, -2);
 fname = e + 1;
 } while (*e == '.');
 return NULL;
}
const char* luaL_typename(lua_State* L, int idx)
{
 const TValue* obj = luaA_toobject(L, idx);
 return luaT_objtypename(L, obj);
}
static size_t getnextbuffersize(lua_State* L, size_t currentsize, size_t desiredsize)
{
 size_t newsize = currentsize + currentsize / 2;
 if (SIZE_MAX - desiredsize < currentsize)
 luaL_error(L, "buffer too large");
 if (newsize < desiredsize)
 newsize = desiredsize;
 return newsize;
}
void luaL_buffinit(lua_State* L, luaL_Buffer* B)
{
 B->p = B->buffer;
 B->end = B->p + LUA_BUFFERSIZE;
 B->L = L;
 B->storage = nullptr;
}
char* luaL_buffinitsize(lua_State* L, luaL_Buffer* B, size_t size)
{
 luaL_buffinit(L, B);
 luaL_reservebuffer(B, size, -1);
 return B->p;
}
char* luaL_extendbuffer(luaL_Buffer* B, size_t additionalsize, int boxloc)
{
 lua_State* L = B->L;
 if (B->storage)
 LUAU_ASSERT(B->storage == tsvalue(L->top + boxloc));
 char* base = B->storage ? B->storage->data : B->buffer;
 size_t capacity = B->end - base;
 size_t nextsize = getnextbuffersize(B->L, capacity, capacity + additionalsize);
 TString* newStorage = luaS_bufstart(L, nextsize);
 memcpy(newStorage->data, base, B->p - base);
 if (base == B->buffer)
 {
 lua_pushnil(L);
 lua_insert(L, boxloc);
 }
 setsvalue(L, L->top + boxloc, newStorage);
 B->p = newStorage->data + (B->p - base);
 B->end = newStorage->data + nextsize;
 B->storage = newStorage;
 return B->p;
}
void luaL_reservebuffer(luaL_Buffer* B, size_t size, int boxloc)
{
 if (size_t(B->end - B->p) < size)
 luaL_extendbuffer(B, size - (B->end - B->p), boxloc);
}
void luaL_addlstring(luaL_Buffer* B, const char* s, size_t len, int boxloc)
{
 if (size_t(B->end - B->p) < len)
 luaL_extendbuffer(B, len - (B->end - B->p), boxloc);
 memcpy(B->p, s, len);
 B->p += len;
}
void luaL_addvalue(luaL_Buffer* B)
{
 lua_State* L = B->L;
 size_t vl;
 if (const char* s = lua_tolstring(L, -1, &vl))
 {
 if (size_t(B->end - B->p) < vl)
 luaL_extendbuffer(B, vl - (B->end - B->p), -2);
 memcpy(B->p, s, vl);
 B->p += vl;
 lua_pop(L, 1);
 }
}
void luaL_pushresult(luaL_Buffer* B)
{
 lua_State* L = B->L;
 if (TString* storage = B->storage)
 {
 luaC_checkGC(L);
 if (B->p == B->end)
 {
 setsvalue(L, L->top - 1, luaS_buffinish(L, storage));
 }
 else
 {
 setsvalue(L, L->top - 1, luaS_newlstr(L, storage->data, B->p - storage->data));
 }
 }
 else
 {
 lua_pushlstring(L, B->buffer, B->p - B->buffer);
 }
}
void luaL_pushresultsize(luaL_Buffer* B, size_t size)
{
 B->p += size;
 luaL_pushresult(B);
}
const char* luaL_tolstring(lua_State* L, int idx, size_t* len)
{
 if (luaL_callmeta(L, idx, "__tostring")) // is there a metafield?
 {
 if (!lua_isstring(L, -1))
 luaL_error(L, "'__tostring' must return a string");
 return lua_tolstring(L, -1, len);
 }
 switch (lua_type(L, idx))
 {
 case LUA_TNUMBER:
 {
 double n = lua_tonumber(L, idx);
 char s[LUAI_MAXNUM2STR];
 char* e = luai_num2str(s, n);
 lua_pushlstring(L, s, e - s);
 break;
 }
 case LUA_TSTRING:
 lua_pushvalue(L, idx);
 break;
 case LUA_TBOOLEAN:
 lua_pushstring(L, (lua_toboolean(L, idx) ? "true" : "false"));
 break;
 case LUA_TNIL:
 lua_pushliteral(L, "nil");
 break;
 case LUA_TVECTOR:
 {
 const float* v = lua_tovector(L, idx);
 char s[LUAI_MAXNUM2STR * LUA_VECTOR_SIZE];
 char* e = s;
 for (int i = 0; i < LUA_VECTOR_SIZE; ++i)
 {
 if (i != 0)
 {
 *e++ = ',';
 *e++ = ' ';
 }
 e = luai_num2str(e, v[i]);
 }
 lua_pushlstring(L, s, e - s);
 break;
 }
 default:
 {
 const void* ptr = lua_topointer(L, idx);
 unsigned long long enc = lua_encodepointer(L, uintptr_t(ptr));
 lua_pushfstring(L, "%s: 0x%016llx", luaL_typename(L, idx), enc);
 break;
 }
 }
 return lua_tolstring(L, -1, len);
}
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
static void writestring(const char* s, size_t l)
{
 fwrite(s, 1, l, stdout);
}
static int luaB_print(lua_State* L)
{
 int n = lua_gettop(L);
 for (int i = 1; i <= n; i++)
 {
 size_t l;
 const char* s = luaL_tolstring(L, i, &l);
 if (i > 1)
 writestring("\t", 1);
 writestring(s, l);
 lua_pop(L, 1);
 }
 writestring("\n", 1);
 return 0;
}
static int luaB_tonumber(lua_State* L)
{
 int base = luaL_optinteger(L, 2, 10);
 if (base == 10)
 {
 int isnum = 0;
 double n = lua_tonumberx(L, 1, &isnum);
 if (isnum)
 {
 lua_pushnumber(L, n);
 return 1;
 }
 luaL_checkany(L, 1);
 }
 else
 {
 const char* s1 = luaL_checkstring(L, 1);
 luaL_argcheck(L, 2 <= base && base <= 36, 2, "base out of range");
 char* s2;
 unsigned long long n;
 n = strtoull(s1, &s2, base);
 if (s1 != s2)
 {
 while (isspace((unsigned char)(*s2)))
 s2++;
 if (*s2 == '\0')
 {
 lua_pushnumber(L, (double)n);
 return 1;
 }
 }
 }
 lua_pushnil(L);
 return 1;
}
static int luaB_error(lua_State* L)
{
 int level = luaL_optinteger(L, 2, 1);
 lua_settop(L, 1);
 if (lua_isstring(L, 1) && level > 0)
 {
 luaL_where(L, level);
 lua_pushvalue(L, 1);
 lua_concat(L, 2);
 }
 lua_error(L);
}
static int luaB_getmetatable(lua_State* L)
{
 luaL_checkany(L, 1);
 if (!lua_getmetatable(L, 1))
 {
 lua_pushnil(L);
 return 1;
 }
 luaL_getmetafield(L, 1, "__metatable");
 return 1;
}
static int luaB_setmetatable(lua_State* L)
{
 int t = lua_type(L, 2);
 luaL_checktype(L, 1, LUA_TTABLE);
 luaL_argexpected(L, t == LUA_TNIL || t == LUA_TTABLE, 2, "nil or table");
 if (luaL_getmetafield(L, 1, "__metatable"))
 luaL_error(L, "cannot change a protected metatable");
 lua_settop(L, 2);
 lua_setmetatable(L, 1);
 return 1;
}
static void getfunc(lua_State* L, int opt)
{
 if (lua_isfunction(L, 1))
 lua_pushvalue(L, 1);
 else
 {
 lua_Debug ar;
 int level = opt ? luaL_optinteger(L, 1, 1) : luaL_checkinteger(L, 1);
 luaL_argcheck(L, level >= 0, 1, "level must be non-negative");
 if (lua_getinfo(L, level, "f", &ar) == 0)
 luaL_argerror(L, 1, "invalid level");
 if (lua_isnil(L, -1))
 luaL_error(L, "no function environment for tail call at level %d", level);
 }
}
static int luaB_getfenv(lua_State* L)
{
 getfunc(L, 1);
 if (lua_iscfunction(L, -1))
 lua_pushvalue(L, LUA_GLOBALSINDEX); // return the thread's global env.
 else
 lua_getfenv(L, -1);
 lua_setsafeenv(L, -1, false);
 return 1;
}
static int luaB_setfenv(lua_State* L)
{
 luaL_checktype(L, 2, LUA_TTABLE);
 getfunc(L, 0);
 lua_pushvalue(L, 2);
 lua_setsafeenv(L, -1, false);
 if (lua_isnumber(L, 1) && lua_tonumber(L, 1) == 0)
 {
 lua_pushthread(L);
 lua_insert(L, -2);
 lua_setfenv(L, -2);
 return 0;
 }
 else if (lua_iscfunction(L, -2) || lua_setfenv(L, -2) == 0)
 luaL_error(L, "'setfenv' cannot change environment of given object");
 return 1;
}
static int luaB_rawequal(lua_State* L)
{
 luaL_checkany(L, 1);
 luaL_checkany(L, 2);
 lua_pushboolean(L, lua_rawequal(L, 1, 2));
 return 1;
}
static int luaB_rawget(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 luaL_checkany(L, 2);
 lua_settop(L, 2);
 lua_rawget(L, 1);
 return 1;
}
static int luaB_rawset(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 luaL_checkany(L, 2);
 luaL_checkany(L, 3);
 lua_settop(L, 3);
 lua_rawset(L, 1);
 return 1;
}
static int luaB_rawlen(lua_State* L)
{
 int tt = lua_type(L, 1);
 luaL_argcheck(L, tt == LUA_TTABLE || tt == LUA_TSTRING, 1, "table or string expected");
 int len = lua_objlen(L, 1);
 lua_pushinteger(L, len);
 return 1;
}
static int luaB_gcinfo(lua_State* L)
{
 lua_pushinteger(L, lua_gc(L, LUA_GCCOUNT, 0));
 return 1;
}
static int luaB_type(lua_State* L)
{
 luaL_checkany(L, 1);
 lua_pushstring(L, lua_typename(L, lua_type(L, 1)));
 return 1;
}
static int luaB_typeof(lua_State* L)
{
 luaL_checkany(L, 1);
 lua_pushstring(L, luaL_typename(L, 1));
 return 1;
}
int luaB_next(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 lua_settop(L, 2);
 if (lua_next(L, 1))
 return 2;
 else
 {
 lua_pushnil(L);
 return 1;
 }
}
static int luaB_pairs(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 lua_pushvalue(L, lua_upvalueindex(1));
 lua_pushvalue(L, 1); // state,
 lua_pushnil(L);
 return 3;
}
int luaB_inext(lua_State* L)
{
 int i = luaL_checkinteger(L, 2);
 luaL_checktype(L, 1, LUA_TTABLE);
 i++;
 lua_pushinteger(L, i);
 lua_rawgeti(L, 1, i);
 return (lua_isnil(L, -1)) ? 0 : 2;
}
static int luaB_ipairs(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 lua_pushvalue(L, lua_upvalueindex(1));
 lua_pushvalue(L, 1); // state,
 lua_pushinteger(L, 0);
 return 3;
}
static int luaB_assert(lua_State* L)
{
 luaL_checkany(L, 1);
 if (!lua_toboolean(L, 1))
 luaL_error(L, "%s", luaL_optstring(L, 2, "assertion failed!"));
 return lua_gettop(L);
}
static int luaB_select(lua_State* L)
{
 int n = lua_gettop(L);
 if (lua_type(L, 1) == LUA_TSTRING && *lua_tostring(L, 1) == '#')
 {
 lua_pushinteger(L, n - 1);
 return 1;
 }
 else
 {
 int i = luaL_checkinteger(L, 1);
 if (i < 0)
 i = n + i;
 else if (i > n)
 i = n;
 luaL_argcheck(L, 1 <= i, 1, "index out of range");
 return n - i;
 }
}
static void luaB_pcallrun(lua_State* L, void* ud)
{
 StkId func = (StkId)ud;
 luaD_call(L, func, LUA_MULTRET);
}
static int luaB_pcally(lua_State* L)
{
 luaL_checkany(L, 1);
 StkId func = L->base;
 L->ci->flags |= LUA_CALLINFO_HANDLE;
 L->baseCcalls++;
 int status = luaD_pcall(L, luaB_pcallrun, func, savestack(L, func), 0);
 L->baseCcalls--;
 expandstacklimit(L, L->top);
 if (status == 0 && (L->status == LUA_YIELD || L->status == LUA_BREAK))
 return -1;
 lua_rawcheckstack(L, 1);
 lua_pushboolean(L, status == 0);
 lua_insert(L, 1);
 return lua_gettop(L);
}
static int luaB_pcallcont(lua_State* L, int status)
{
 if (status == 0)
 {
 lua_rawcheckstack(L, 1);
 lua_pushboolean(L, true);
 lua_insert(L, 1);
 return lua_gettop(L);
 }
 else
 {
 lua_rawcheckstack(L, 1);
 lua_pushboolean(L, false);
 lua_insert(L, -2);
 return 2;
 }
}
static int luaB_xpcally(lua_State* L)
{
 luaL_checktype(L, 2, LUA_TFUNCTION);
 lua_pushvalue(L, 1);
 lua_pushvalue(L, 2);
 lua_replace(L, 1);
 lua_replace(L, 2);
 L->ci->flags |= LUA_CALLINFO_HANDLE;
 StkId errf = L->base;
 StkId func = L->base + 1;
 L->baseCcalls++;
 int status = luaD_pcall(L, luaB_pcallrun, func, savestack(L, func), savestack(L, errf));
 L->baseCcalls--;
 expandstacklimit(L, L->top);
 if (status == 0 && (L->status == LUA_YIELD || L->status == LUA_BREAK))
 return -1;
 lua_rawcheckstack(L, 1);
 lua_pushboolean(L, status == 0);
 lua_replace(L, 1);
 return lua_gettop(L); // return status + all results
}
static void luaB_xpcallerr(lua_State* L, void* ud)
{
 StkId func = (StkId)ud;
 luaD_call(L, func, 1);
}
static int luaB_xpcallcont(lua_State* L, int status)
{
 if (status == 0)
 {
 lua_rawcheckstack(L, 1);
 lua_pushboolean(L, true);
 lua_replace(L, 1);
 return lua_gettop(L); // return status + all results
 }
 else
 {
 lua_rawcheckstack(L, 3);
 lua_pushboolean(L, false);
 lua_pushvalue(L, 1);
 lua_pushvalue(L, -3); // push error object (that was on top of the stack before)
 StkId res = L->top - 3;
 StkId errf = L->top - 2;
 luaD_pcall(L, luaB_xpcallerr, errf, savestack(L, errf), savestack(L, res));
 return 2;
 }
}
static int luaB_tostring(lua_State* L)
{
 luaL_checkany(L, 1);
 luaL_tolstring(L, 1, NULL);
 return 1;
}
static int luaB_newproxy(lua_State* L)
{
 int t = lua_type(L, 1);
 luaL_argexpected(L, t == LUA_TNONE || t == LUA_TNIL || t == LUA_TBOOLEAN, 1, "nil or boolean");
 bool needsmt = lua_toboolean(L, 1);
 lua_newuserdatatagged(L, 0, UTAG_PROXY);
 if (needsmt)
 {
 lua_newtable(L);
 lua_setmetatable(L, -2);
 }
 return 1;
}
static const luaL_Reg base_funcs[] = {
 {"assert", luaB_assert},
 {"error", luaB_error},
 {"gcinfo", luaB_gcinfo},
 {"getfenv", luaB_getfenv},
 {"getmetatable", luaB_getmetatable},
 {"next", luaB_next},
 {"newproxy", luaB_newproxy},
 {"print", luaB_print},
 {"rawequal", luaB_rawequal},
 {"rawget", luaB_rawget},
 {"rawset", luaB_rawset},
 {"rawlen", luaB_rawlen},
 {"select", luaB_select},
 {"setfenv", luaB_setfenv},
 {"setmetatable", luaB_setmetatable},
 {"tonumber", luaB_tonumber},
 {"tostring", luaB_tostring},
 {"type", luaB_type},
 {"typeof", luaB_typeof},
 {NULL, NULL},
};
static void auxopen(lua_State* L, const char* name, lua_CFunction f, lua_CFunction u)
{
 lua_pushcfunction(L, u, NULL);
 lua_pushcclosure(L, f, name, 1);
 lua_setfield(L, -2, name);
}
int luaopen_base(lua_State* L)
{
 lua_pushvalue(L, LUA_GLOBALSINDEX);
 lua_setglobal(L, "_G");
 luaL_register(L, "_G", base_funcs);
 lua_pushliteral(L, "Luau");
 lua_setglobal(L, "_VERSION"); // set global _VERSION
 auxopen(L, "ipairs", luaB_ipairs, luaB_inext);
 auxopen(L, "pairs", luaB_pairs, luaB_next);
 lua_pushcclosurek(L, luaB_pcally, "pcall", 0, luaB_pcallcont);
 lua_setfield(L, -2, "pcall");
 lua_pushcclosurek(L, luaB_xpcally, "xpcall", 0, luaB_xpcallcont);
 lua_setfield(L, -2, "xpcall");
 return 1;
}
#define ALLONES ~0u
#define NBITS int(8 * sizeof(unsigned))
#define trim(x) ((x)&ALLONES)
#define mask(n) (~((ALLONES << 1) << ((n)-1)))
typedef unsigned b_uint;
static b_uint andaux(lua_State* L)
{
 int i, n = lua_gettop(L);
 b_uint r = ~(b_uint)0;
 for (i = 1; i <= n; i++)
 r &= luaL_checkunsigned(L, i);
 return trim(r);
}
static int b_and(lua_State* L)
{
 b_uint r = andaux(L);
 lua_pushunsigned(L, r);
 return 1;
}
static int b_test(lua_State* L)
{
 b_uint r = andaux(L);
 lua_pushboolean(L, r != 0);
 return 1;
}
static int b_or(lua_State* L)
{
 int i, n = lua_gettop(L);
 b_uint r = 0;
 for (i = 1; i <= n; i++)
 r |= luaL_checkunsigned(L, i);
 lua_pushunsigned(L, trim(r));
 return 1;
}
static int b_xor(lua_State* L)
{
 int i, n = lua_gettop(L);
 b_uint r = 0;
 for (i = 1; i <= n; i++)
 r ^= luaL_checkunsigned(L, i);
 lua_pushunsigned(L, trim(r));
 return 1;
}
static int b_not(lua_State* L)
{
 b_uint r = ~luaL_checkunsigned(L, 1);
 lua_pushunsigned(L, trim(r));
 return 1;
}
static int b_shift(lua_State* L, b_uint r, int i)
{
 if (i < 0)
 {
 i = -i;
 r = trim(r);
 if (i >= NBITS)
 r = 0;
 else
 r >>= i;
 }
 else
 {
 if (i >= NBITS)
 r = 0;
 else
 r <<= i;
 r = trim(r);
 }
 lua_pushunsigned(L, r);
 return 1;
}
static int b_lshift(lua_State* L)
{
 return b_shift(L, luaL_checkunsigned(L, 1), luaL_checkinteger(L, 2));
}
static int b_rshift(lua_State* L)
{
 return b_shift(L, luaL_checkunsigned(L, 1), -luaL_checkinteger(L, 2));
}
static int b_arshift(lua_State* L)
{
 b_uint r = luaL_checkunsigned(L, 1);
 int i = luaL_checkinteger(L, 2);
 if (i < 0 || !(r & ((b_uint)1 << (NBITS - 1))))
 return b_shift(L, r, -i);
 else
 {
 if (i >= NBITS)
 r = ALLONES;
 else
 r = trim((r >> i) | ~(~(b_uint)0 >> i));
 lua_pushunsigned(L, r);
 return 1;
 }
}
static int b_rot(lua_State* L, int i)
{
 b_uint r = luaL_checkunsigned(L, 1);
 i &= (NBITS - 1);
 r = trim(r);
 if (i != 0)
 r = (r << i) | (r >> (NBITS - i));
 lua_pushunsigned(L, trim(r));
 return 1;
}
static int b_lrot(lua_State* L)
{
 return b_rot(L, luaL_checkinteger(L, 2));
}
static int b_rrot(lua_State* L)
{
 return b_rot(L, -luaL_checkinteger(L, 2));
}
static int fieldargs(lua_State* L, int farg, int* width)
{
 int f = luaL_checkinteger(L, farg);
 int w = luaL_optinteger(L, farg + 1, 1);
 luaL_argcheck(L, 0 <= f, farg, "field cannot be negative");
 luaL_argcheck(L, 0 < w, farg + 1, "width must be positive");
 if (f + w > NBITS)
 luaL_error(L, "trying to access non-existent bits");
 *width = w;
 return f;
}
static int b_extract(lua_State* L)
{
 int w;
 b_uint r = luaL_checkunsigned(L, 1);
 int f = fieldargs(L, 2, &w);
 r = (r >> f) & mask(w);
 lua_pushunsigned(L, r);
 return 1;
}
static int b_replace(lua_State* L)
{
 int w;
 b_uint r = luaL_checkunsigned(L, 1);
 b_uint v = luaL_checkunsigned(L, 2);
 int f = fieldargs(L, 3, &w);
 int m = mask(w);
 v &= m;
 r = (r & ~(m << f)) | (v << f);
 lua_pushunsigned(L, r);
 return 1;
}
static int b_countlz(lua_State* L)
{
 b_uint v = luaL_checkunsigned(L, 1);
 b_uint r = NBITS;
 for (int i = 0; i < NBITS; ++i)
 if (v & (1u << (NBITS - 1 - i)))
 {
 r = i;
 break;
 }
 lua_pushunsigned(L, r);
 return 1;
}
static int b_countrz(lua_State* L)
{
 b_uint v = luaL_checkunsigned(L, 1);
 b_uint r = NBITS;
 for (int i = 0; i < NBITS; ++i)
 if (v & (1u << i))
 {
 r = i;
 break;
 }
 lua_pushunsigned(L, r);
 return 1;
}
static const luaL_Reg bitlib[] = {
 {"arshift", b_arshift},
 {"band", b_and},
 {"bnot", b_not},
 {"bor", b_or},
 {"bxor", b_xor},
 {"btest", b_test},
 {"extract", b_extract},
 {"lrotate", b_lrot},
 {"lshift", b_lshift},
 {"replace", b_replace},
 {"rrotate", b_rrot},
 {"rshift", b_rshift},
 {"countlz", b_countlz},
 {"countrz", b_countrz},
 {NULL, NULL},
};
int luaopen_bit32(lua_State* L)
{
 luaL_register(L, LUA_BITLIBNAME, bitlib);
 return 1;
}
typedef int (*luau_FastFunction)(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams);
extern const luau_FastFunction luauF_table[256];
#ifdef _MSC_VER
#include <intrin.h>
#endif
#ifdef LUAU_TARGET_SSE41
#include <smmintrin.h>
#ifndef _MSC_VER
#include <cpuid.h>
#endif
#endif
static int luauF_assert(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults == 0 && !l_isfalse(arg0))
 {
 return 0;
 }
 return -1;
}
static int luauF_abs(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, fabs(a1));
 return 1;
 }
 return -1;
}
static int luauF_acos(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, acos(a1));
 return 1;
 }
 return -1;
}
static int luauF_asin(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, asin(a1));
 return 1;
 }
 return -1;
}
static int luauF_atan2(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 setnvalue(res, atan2(a1, a2));
 return 1;
 }
 return -1;
}
static int luauF_atan(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, atan(a1));
 return 1;
 }
 return -1;
}
LUAU_FASTMATH_BEGIN
static int luauF_ceil(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, ceil(a1));
 return 1;
 }
 return -1;
}
LUAU_FASTMATH_END
static int luauF_cosh(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, cosh(a1));
 return 1;
 }
 return -1;
}
static int luauF_cos(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, cos(a1));
 return 1;
 }
 return -1;
}
static int luauF_deg(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 const double rpd = (3.14159265358979323846 / 180.0);
 setnvalue(res, a1 / rpd);
 return 1;
 }
 return -1;
}
static int luauF_exp(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, exp(a1));
 return 1;
 }
 return -1;
}
LUAU_FASTMATH_BEGIN
static int luauF_floor(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, floor(a1));
 return 1;
 }
 return -1;
}
LUAU_FASTMATH_END
static int luauF_fmod(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 setnvalue(res, fmod(a1, a2));
 return 1;
 }
 return -1;
}
static int luauF_frexp(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 2 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 int e;
 double f = frexp(a1, &e);
 setnvalue(res, f);
 setnvalue(res + 1, double(e));
 return 2;
 }
 return -1;
}
static int luauF_ldexp(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 setnvalue(res, ldexp(a1, int(a2)));
 return 1;
 }
 return -1;
}
static int luauF_log10(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, log10(a1));
 return 1;
 }
 return -1;
}
static int luauF_log(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 if (nparams == 1)
 {
 setnvalue(res, log(a1));
 return 1;
 }
 else if (ttisnumber(args))
 {
 double a2 = nvalue(args);
 if (a2 == 2.0)
 {
 setnvalue(res, log2(a1));
 return 1;
 }
 else if (a2 == 10.0)
 {
 setnvalue(res, log10(a1));
 return 1;
 }
 else
 {
 setnvalue(res, log(a1) / log(a2));
 return 1;
 }
 }
 }
 return -1;
}
static int luauF_max(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 double r = (a2 > a1) ? a2 : a1;
 for (int i = 3; i <= nparams; ++i)
 {
 if (!ttisnumber(args + (i - 2)))
 return -1;
 double a = nvalue(args + (i - 2));
 r = (a > r) ? a : r;
 }
 setnvalue(res, r);
 return 1;
 }
 return -1;
}
static int luauF_min(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 double r = (a2 < a1) ? a2 : a1;
 for (int i = 3; i <= nparams; ++i)
 {
 if (!ttisnumber(args + (i - 2)))
 return -1;
 double a = nvalue(args + (i - 2));
 r = (a < r) ? a : r;
 }
 setnvalue(res, r);
 return 1;
 }
 return -1;
}
static int luauF_modf(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 2 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 double ip;
 double fp = modf(a1, &ip);
 setnvalue(res, ip);
 setnvalue(res + 1, fp);
 return 2;
 }
 return -1;
}
static int luauF_pow(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 setnvalue(res, pow(a1, a2));
 return 1;
 }
 return -1;
}
static int luauF_rad(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 const double rpd = (3.14159265358979323846 / 180.0);
 setnvalue(res, a1 * rpd);
 return 1;
 }
 return -1;
}
static int luauF_sinh(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, sinh(a1));
 return 1;
 }
 return -1;
}
static int luauF_sin(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, sin(a1));
 return 1;
 }
 return -1;
}
LUAU_FASTMATH_BEGIN
static int luauF_sqrt(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, sqrt(a1));
 return 1;
 }
 return -1;
}
LUAU_FASTMATH_END
static int luauF_tanh(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, tanh(a1));
 return 1;
 }
 return -1;
}
static int luauF_tan(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, tan(a1));
 return 1;
 }
 return -1;
}
static int luauF_arshift(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned u;
 luai_num2unsigned(u, a1);
 int s = int(a2);
 if (unsigned(s) < 32)
 {
 uint32_t r = int32_t(u) >> s;
 setnvalue(res, double(r));
 return 1;
 }
 }
 return -1;
}
static int luauF_band(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned u1, u2;
 luai_num2unsigned(u1, a1);
 luai_num2unsigned(u2, a2);
 uint32_t r = u1 & u2;
 for (int i = 3; i <= nparams; ++i)
 {
 if (!ttisnumber(args + (i - 2)))
 return -1;
 double a = nvalue(args + (i - 2));
 unsigned u;
 luai_num2unsigned(u, a);
 r &= u;
 }
 setnvalue(res, double(r));
 return 1;
 }
 return -1;
}
static int luauF_bnot(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 unsigned u;
 luai_num2unsigned(u, a1);
 uint32_t r = ~u;
 setnvalue(res, double(r));
 return 1;
 }
 return -1;
}
static int luauF_bor(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned u1, u2;
 luai_num2unsigned(u1, a1);
 luai_num2unsigned(u2, a2);
 uint32_t r = u1 | u2;
 for (int i = 3; i <= nparams; ++i)
 {
 if (!ttisnumber(args + (i - 2)))
 return -1;
 double a = nvalue(args + (i - 2));
 unsigned u;
 luai_num2unsigned(u, a);
 r |= u;
 }
 setnvalue(res, double(r));
 return 1;
 }
 return -1;
}
static int luauF_bxor(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned u1, u2;
 luai_num2unsigned(u1, a1);
 luai_num2unsigned(u2, a2);
 uint32_t r = u1 ^ u2;
 for (int i = 3; i <= nparams; ++i)
 {
 if (!ttisnumber(args + (i - 2)))
 return -1;
 double a = nvalue(args + (i - 2));
 unsigned u;
 luai_num2unsigned(u, a);
 r ^= u;
 }
 setnvalue(res, double(r));
 return 1;
 }
 return -1;
}
static int luauF_btest(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned u1, u2;
 luai_num2unsigned(u1, a1);
 luai_num2unsigned(u2, a2);
 uint32_t r = u1 & u2;
 for (int i = 3; i <= nparams; ++i)
 {
 if (!ttisnumber(args + (i - 2)))
 return -1;
 double a = nvalue(args + (i - 2));
 unsigned u;
 luai_num2unsigned(u, a);
 r &= u;
 }
 setbvalue(res, r != 0);
 return 1;
 }
 return -1;
}
static int luauF_extract(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned n;
 luai_num2unsigned(n, a1);
 int f = int(a2);
 if (nparams == 2)
 {
 if (unsigned(f) < 32)
 {
 uint32_t m = 1;
 uint32_t r = (n >> f) & m;
 setnvalue(res, double(r));
 return 1;
 }
 }
 else if (ttisnumber(args + 1))
 {
 double a3 = nvalue(args + 1);
 int w = int(a3);
 if (f >= 0 && w > 0 && f + w <= 32)
 {
 uint32_t m = ~(0xfffffffeu << (w - 1));
 uint32_t r = (n >> f) & m;
 setnvalue(res, double(r));
 return 1;
 }
 }
 }
 return -1;
}
static int luauF_lrotate(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned u;
 luai_num2unsigned(u, a1);
 int s = int(a2);
#ifdef _MSC_VER
 uint32_t r = _rotl(u, s);
#else
 uint32_t r = (u << (s & 31)) | (u >> ((32 - s) & 31));
#endif
 setnvalue(res, double(r));
 return 1;
 }
 return -1;
}
static int luauF_lshift(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned u;
 luai_num2unsigned(u, a1);
 int s = int(a2);
 if (unsigned(s) < 32)
 {
 uint32_t r = u << s;
 setnvalue(res, double(r));
 return 1;
 }
 }
 return -1;
}
static int luauF_replace(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 3 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args) && ttisnumber(args + 1))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 double a3 = nvalue(args + 1);
 unsigned n, v;
 luai_num2unsigned(n, a1);
 luai_num2unsigned(v, a2);
 int f = int(a3);
 if (nparams == 3)
 {
 if (unsigned(f) < 32)
 {
 uint32_t m = 1;
 uint32_t r = (n & ~(m << f)) | ((v & m) << f);
 setnvalue(res, double(r));
 return 1;
 }
 }
 else if (ttisnumber(args + 2))
 {
 double a4 = nvalue(args + 2);
 int w = int(a4);
 if (f >= 0 && w > 0 && f + w <= 32)
 {
 uint32_t m = ~(0xfffffffeu << (w - 1));
 uint32_t r = (n & ~(m << f)) | ((v & m) << f);
 setnvalue(res, double(r));
 return 1;
 }
 }
 }
 return -1;
}
static int luauF_rrotate(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned u;
 luai_num2unsigned(u, a1);
 int s = int(a2);
#ifdef _MSC_VER
 uint32_t r = _rotr(u, s);
#else
 uint32_t r = (u >> (s & 31)) | (u << ((32 - s) & 31));
#endif
 setnvalue(res, double(r));
 return 1;
 }
 return -1;
}
static int luauF_rshift(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned u;
 luai_num2unsigned(u, a1);
 int s = int(a2);
 if (unsigned(s) < 32)
 {
 uint32_t r = u >> s;
 setnvalue(res, double(r));
 return 1;
 }
 }
 return -1;
}
static int luauF_type(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1)
 {
 int tt = ttype(arg0);
 TString* ttname = L->global->ttname[tt];
 setsvalue(L, res, ttname);
 return 1;
 }
 return -1;
}
static int luauF_byte(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && ttisstring(arg0) && ttisnumber(args))
 {
 TString* ts = tsvalue(arg0);
 int i = int(nvalue(args));
 int j = (nparams >= 3) ? (ttisnumber(args + 1) ? int(nvalue(args + 1)) : 0) : i;
 if (i >= 1 && j >= i && j <= int(ts->len))
 {
 int c = j - i + 1;
 const char* s = getstr(ts);
 if (c == (nresults < 0 ? 1 : nresults))
 {
 for (int k = 0; k < c; ++k)
 {
 setnvalue(res + k, uint8_t(s[i + k - 1]));
 }
 return c;
 }
 }
 }
 return -1;
}
static int luauF_char(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 char buffer[8];
 if (nparams < int(sizeof(buffer)) && nresults <= 1)
 {
 if (nparams >= 1)
 {
 if (!ttisnumber(arg0))
 return -1;
 int ch = int(nvalue(arg0));
 if ((unsigned char)(ch) != ch)
 return -1;
 buffer[0] = ch;
 }
 for (int i = 2; i <= nparams; ++i)
 {
 if (!ttisnumber(args + (i - 2)))
 return -1;
 int ch = int(nvalue(args + (i - 2)));
 if ((unsigned char)(ch) != ch)
 return -1;
 buffer[i - 1] = ch;
 }
 buffer[nparams] = 0;
 setsvalue(L, res, luaS_newlstr(L, buffer, nparams));
 return 1;
 }
 return -1;
}
static int luauF_len(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisstring(arg0))
 {
 TString* ts = tsvalue(arg0);
 setnvalue(res, int(ts->len));
 return 1;
 }
 return -1;
}
static int luauF_typeof(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1)
 {
 const TString* ttname = luaT_objtypenamestr(L, arg0);
 setsvalue(L, res, ttname);
 return 1;
 }
 return -1;
}
static int luauF_sub(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 3 && nresults <= 1 && ttisstring(arg0) && ttisnumber(args) && ttisnumber(args + 1))
 {
 TString* ts = tsvalue(arg0);
 int i = int(nvalue(args));
 int j = int(nvalue(args + 1));
 if (i >= 1 && j >= i && unsigned(j - 1) < unsigned(ts->len))
 {
 setsvalue(L, res, luaS_newlstr(L, getstr(ts) + (i - 1), j - i + 1));
 return 1;
 }
 }
 return -1;
}
static int luauF_clamp(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 3 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args) && ttisnumber(args + 1))
 {
 double v = nvalue(arg0);
 double min = nvalue(args);
 double max = nvalue(args + 1);
 if (min <= max)
 {
 double r = v < min ? min : v;
 r = r > max ? max : r;
 setnvalue(res, r);
 return 1;
 }
 }
 return -1;
}
static int luauF_sign(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double v = nvalue(arg0);
 setnvalue(res, v > 0.0 ? 1.0 : v < 0.0 ? -1.0 : 0.0);
 return 1;
 }
 return -1;
}
LUAU_FASTMATH_BEGIN
static int luauF_round(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double v = nvalue(arg0);
 setnvalue(res, round(v));
 return 1;
 }
 return -1;
}
LUAU_FASTMATH_END
static int luauF_rawequal(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1)
 {
 setbvalue(res, luaO_rawequalObj(arg0, args));
 return 1;
 }
 return -1;
}
static int luauF_rawget(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttistable(arg0))
 {
 setobj2s(L, res, luaH_get(hvalue(arg0), args));
 return 1;
 }
 return -1;
}
static int luauF_rawset(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 3 && nresults <= 1 && ttistable(arg0))
 {
 const TValue* key = args;
 if (ttisnil(key))
 return -1;
 else if (ttisnumber(key) && luai_numisnan(nvalue(key)))
 return -1;
 else if (ttisvector(key) && luai_vecisnan(vvalue(key)))
 return -1;
 Table* t = hvalue(arg0);
 if (t->readonly)
 return -1;
 setobj2s(L, res, arg0);
 setobj2t(L, luaH_set(L, t, args), args + 1);
 luaC_barriert(L, t, args + 1);
 return 1;
 }
 return -1;
}
static int luauF_tinsert(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams == 2 && nresults <= 0 && ttistable(arg0))
 {
 Table* t = hvalue(arg0);
 if (t->readonly)
 return -1;
 int pos = luaH_getn(t) + 1;
 setobj2t(L, luaH_setnum(L, t, pos), args);
 luaC_barriert(L, t, args);
 return 0;
 }
 return -1;
}
static int luauF_tunpack(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults < 0 && ttistable(arg0))
 {
 Table* t = hvalue(arg0);
 int n = -1;
 if (nparams == 1)
 n = luaH_getn(t);
 else if (nparams == 3 && ttisnumber(args) && ttisnumber(args + 1) && nvalue(args) == 1.0)
 n = int(nvalue(args + 1));
 if (n >= 0 && n <= t->sizearray && cast_int(L->stack_last - res) >= n && n + nparams <= LUAI_MAXCSTACK)
 {
 TValue* array = t->array;
 for (int i = 0; i < n; ++i)
 setobj2s(L, res + i, array + i);
 expandstacklimit(L, res + n);
 return n;
 }
 }
 return -1;
}
static int luauF_vector(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 3 && nresults <= 1 && ttisnumber(arg0) && ttisnumber(args) && ttisnumber(args + 1))
 {
 double x = nvalue(arg0);
 double y = nvalue(args);
 double z = nvalue(args + 1);
#if LUA_VECTOR_SIZE == 4
 double w = 0.0;
 if (nparams >= 4)
 {
 if (!ttisnumber(args + 2))
 return -1;
 w = nvalue(args + 2);
 }
 setvvalue(res, float(x), float(y), float(z), float(w));
#else
 setvvalue(res, float(x), float(y), float(z), 0.0f);
#endif
 return 1;
 }
 return -1;
}
static int luauF_countlz(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 unsigned n;
 luai_num2unsigned(n, a1);
#ifdef _MSC_VER
 unsigned long rl;
 int r = _BitScanReverse(&rl, n) ? 31 - int(rl) : 32;
#else
 int r = n == 0 ? 32 : __builtin_clz(n);
#endif
 setnvalue(res, double(r));
 return 1;
 }
 return -1;
}
static int luauF_countrz(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 unsigned n;
 luai_num2unsigned(n, a1);
#ifdef _MSC_VER
 unsigned long rl;
 int r = _BitScanForward(&rl, n) ? int(rl) : 32;
#else
 int r = n == 0 ? 32 : __builtin_ctz(n);
#endif
 setnvalue(res, double(r));
 return 1;
 }
 return -1;
}
static int luauF_select(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams == 1 && nresults == 1)
 {
 int n = cast_int(L->base - L->ci->func) - clvalue(L->ci->func)->l.p->numparams - 1;
 if (ttisnumber(arg0))
 {
 int i = int(nvalue(arg0));
 if (unsigned(i - 1) < unsigned(n))
 {
 setobj2s(L, res, L->base - n + (i - 1));
 return 1;
 }
 }
 else if (ttisstring(arg0) && *svalue(arg0) == '#')
 {
 setnvalue(res, double(n));
 return 1;
 }
 }
 return -1;
}
static int luauF_rawlen(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1)
 {
 if (ttistable(arg0))
 {
 Table* h = hvalue(arg0);
 setnvalue(res, double(luaH_getn(h)));
 return 1;
 }
 else if (ttisstring(arg0))
 {
 TString* ts = tsvalue(arg0);
 setnvalue(res, double(ts->len));
 return 1;
 }
 }
 return -1;
}
static int luauF_extractk(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 double a2 = nvalue(args);
 unsigned n;
 luai_num2unsigned(n, a1);
 int fw = int(a2);
 int f = fw & 31;
 int w1 = fw >> 5;
 uint32_t m = ~(0xfffffffeu << w1);
 uint32_t r = (n >> f) & m;
 setnvalue(res, double(r));
 return 1;
 }
 return -1;
}
static int luauF_getmetatable(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1)
 {
 Table* mt = NULL;
 if (ttistable(arg0))
 mt = hvalue(arg0)->metatable;
 else if (ttisuserdata(arg0))
 mt = uvalue(arg0)->metatable;
 else
 mt = L->global->mt[ttype(arg0)];
 const TValue* mtv = mt ? luaH_getstr(mt, L->global->tmname[TM_METATABLE]) : luaO_nilobject;
 if (!ttisnil(mtv))
 {
 setobj2s(L, res, mtv);
 return 1;
 }
 if (mt)
 {
 sethvalue(L, res, mt);
 return 1;
 }
 else
 {
 setnilvalue(res);
 return 1;
 }
 }
 return -1;
}
static int luauF_setmetatable(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 2 && nresults <= 1 && ttistable(arg0) && ttistable(args))
 {
 Table* t = hvalue(arg0);
 if (t->readonly || t->metatable != NULL)
 return -1;
 Table* mt = hvalue(args);
 t->metatable = mt;
 luaC_objbarrier(L, t, mt);
 sethvalue(L, res, t);
 return 1;
 }
 return -1;
}
static int luauF_missing(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 return -1;
}
#ifdef LUAU_TARGET_SSE41
template<int Rounding>
LUAU_TARGET_SSE41 inline double roundsd_sse41(double v)
{
 __m128d av = _mm_set_sd(v);
 __m128d rv = _mm_round_sd(av, av, Rounding | _MM_FROUND_NO_EXC);
 return _mm_cvtsd_f64(rv);
}
LUAU_TARGET_SSE41 static int luauF_floor_sse41(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, roundsd_sse41<_MM_FROUND_TO_NEG_INF>(a1));
 return 1;
 }
 return -1;
}
LUAU_TARGET_SSE41 static int luauF_ceil_sse41(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 setnvalue(res, roundsd_sse41<_MM_FROUND_TO_POS_INF>(a1));
 return 1;
 }
 return -1;
}
LUAU_TARGET_SSE41 static int luauF_round_sse41(lua_State* L, StkId res, TValue* arg0, int nresults, StkId args, int nparams)
{
 if (nparams >= 1 && nresults <= 1 && ttisnumber(arg0))
 {
 double a1 = nvalue(arg0);
 const double offset = 0.49999999999999994;
 setnvalue(res, roundsd_sse41<_MM_FROUND_TO_ZERO>(a1 + (a1 < 0 ? -offset : offset)));
 return 1;
 }
 return -1;
}
static bool luau_hassse41()
{
 int cpuinfo[4] = {};
#ifdef _MSC_VER
 __cpuid(cpuinfo, 1);
#else
 __cpuid(1, cpuinfo[0], cpuinfo[1], cpuinfo[2], cpuinfo[3]);
#endif
 return (cpuinfo[2] & (1 << 19)) != 0;
}
#endif
const luau_FastFunction luauF_table[256] = {
 NULL,
 luauF_assert,
 luauF_abs,
 luauF_acos,
 luauF_asin,
 luauF_atan2,
 luauF_atan,
#ifdef LUAU_TARGET_SSE41
 luau_hassse41() ? luauF_ceil_sse41 : luauF_ceil,
#else
 luauF_ceil,
#endif
 luauF_cosh,
 luauF_cos,
 luauF_deg,
 luauF_exp,
#ifdef LUAU_TARGET_SSE41
 luau_hassse41() ? luauF_floor_sse41 : luauF_floor,
#else
 luauF_floor,
#endif
 luauF_fmod,
 luauF_frexp,
 luauF_ldexp,
 luauF_log10,
 luauF_log,
 luauF_max,
 luauF_min,
 luauF_modf,
 luauF_pow,
 luauF_rad,
 luauF_sinh,
 luauF_sin,
 luauF_sqrt,
 luauF_tanh,
 luauF_tan,
 luauF_arshift,
 luauF_band,
 luauF_bnot,
 luauF_bor,
 luauF_bxor,
 luauF_btest,
 luauF_extract,
 luauF_lrotate,
 luauF_lshift,
 luauF_replace,
 luauF_rrotate,
 luauF_rshift,
 luauF_type,
 luauF_byte,
 luauF_char,
 luauF_len,
 luauF_typeof,
 luauF_sub,
 luauF_clamp,
 luauF_sign,
#ifdef LUAU_TARGET_SSE41
 luau_hassse41() ? luauF_round_sse41 : luauF_round,
#else
 luauF_round,
#endif
 luauF_rawset,
 luauF_rawget,
 luauF_rawequal,
 luauF_tinsert,
 luauF_tunpack,
 luauF_vector,
 luauF_countlz,
 luauF_countrz,
 luauF_select,
 luauF_rawlen,
 luauF_extractk,
 luauF_getmetatable,
 luauF_setmetatable,
#define MISSING8 luauF_missing, luauF_missing, luauF_missing, luauF_missing, luauF_missing, luauF_missing, luauF_missing, luauF_missing
 MISSING8,
 MISSING8,
 MISSING8,
 MISSING8,
 MISSING8,
 MISSING8,
 MISSING8,
 MISSING8,
#undef MISSING8
};
#define CO_STATUS_ERROR -1
#define CO_STATUS_BREAK -2
static const char* const statnames[] = {"running", "suspended", "normal", "dead", "dead"}; // dead appears twice for LUA_COERR and LUA_COFIN
static int costatus(lua_State* L)
{
 lua_State* co = lua_tothread(L, 1);
 luaL_argexpected(L, co, 1, "thread");
 lua_pushstring(L, statnames[lua_costatus(L, co)]);
 return 1;
}
static int auxresume(lua_State* L, lua_State* co, int narg)
{
 if (co->status != LUA_YIELD)
 {
 int status = lua_costatus(L, co);
 if (status != LUA_COSUS)
 {
 lua_pushfstring(L, "cannot resume %s coroutine", statnames[status]);
 return CO_STATUS_ERROR;
 }
 }
 if (narg)
 {
 if (!lua_checkstack(co, narg))
 luaL_error(L, "too many arguments to resume");
 lua_xmove(L, co, narg);
 }
 co->singlestep = L->singlestep;
 int status = lua_resume(co, L, narg);
 if (status == 0 || status == LUA_YIELD)
 {
 int nres = cast_int(co->top - co->base);
 if (nres)
 {
 if (nres + 1 > LUA_MINSTACK && !lua_checkstack(L, nres + 1))
 luaL_error(L, "too many results to resume");
 lua_xmove(co, L, nres);
 }
 return nres;
 }
 else if (status == LUA_BREAK)
 {
 return CO_STATUS_BREAK;
 }
 else
 {
 lua_xmove(co, L, 1);
 return CO_STATUS_ERROR;
 }
}
static int interruptThread(lua_State* L, lua_State* co)
{
 if (L->global->cb.debuginterrupt)
 luau_callhook(L, L->global->cb.debuginterrupt, co);
 return lua_break(L);
}
static int auxresumecont(lua_State* L, lua_State* co)
{
 if (co->status == 0 || co->status == LUA_YIELD)
 {
 int nres = cast_int(co->top - co->base);
 if (!lua_checkstack(L, nres + 1))
 luaL_error(L, "too many results to resume");
 lua_xmove(co, L, nres);
 return nres;
 }
 else
 {
 lua_rawcheckstack(L, 2);
 lua_xmove(co, L, 1);
 return CO_STATUS_ERROR;
 }
}
static int coresumefinish(lua_State* L, int r)
{
 if (r < 0)
 {
 lua_pushboolean(L, 0);
 lua_insert(L, -2);
 return 2;
 }
 else
 {
 lua_pushboolean(L, 1);
 lua_insert(L, -(r + 1));
 return r + 1;
 }
}
static int coresumey(lua_State* L)
{
 lua_State* co = lua_tothread(L, 1);
 luaL_argexpected(L, co, 1, "thread");
 int narg = cast_int(L->top - L->base) - 1;
 int r = auxresume(L, co, narg);
 if (r == CO_STATUS_BREAK)
 return interruptThread(L, co);
 return coresumefinish(L, r);
}
static int coresumecont(lua_State* L, int status)
{
 lua_State* co = lua_tothread(L, 1);
 luaL_argexpected(L, co, 1, "thread");
 if (co->status == LUA_BREAK)
 return interruptThread(L, co);
 int r = auxresumecont(L, co);
 return coresumefinish(L, r);
}
static int auxwrapfinish(lua_State* L, int r)
{
 if (r < 0)
 {
 if (lua_isstring(L, -1))
 {
 luaL_where(L, 1); // add extra info
 lua_insert(L, -2);
 lua_concat(L, 2);
 }
 lua_error(L);
 }
 return r;
}
static int auxwrapy(lua_State* L)
{
 lua_State* co = lua_tothread(L, lua_upvalueindex(1));
 int narg = cast_int(L->top - L->base);
 int r = auxresume(L, co, narg);
 if (r == CO_STATUS_BREAK)
 return interruptThread(L, co);
 return auxwrapfinish(L, r);
}
static int auxwrapcont(lua_State* L, int status)
{
 lua_State* co = lua_tothread(L, lua_upvalueindex(1));
 if (co->status == LUA_BREAK)
 return interruptThread(L, co);
 int r = auxresumecont(L, co);
 return auxwrapfinish(L, r);
}
static int cocreate(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TFUNCTION);
 lua_State* NL = lua_newthread(L);
 lua_xpush(L, NL, 1);
 return 1;
}
static int cowrap(lua_State* L)
{
 cocreate(L);
 lua_pushcclosurek(L, auxwrapy, NULL, 1, auxwrapcont);
 return 1;
}
static int coyield(lua_State* L)
{
 int nres = cast_int(L->top - L->base);
 return lua_yield(L, nres);
}
static int corunning(lua_State* L)
{
 if (lua_pushthread(L))
 lua_pushnil(L);
 return 1;
}
static int coyieldable(lua_State* L)
{
 lua_pushboolean(L, lua_isyieldable(L));
 return 1;
}
static int coclose(lua_State* L)
{
 lua_State* co = lua_tothread(L, 1);
 luaL_argexpected(L, co, 1, "thread");
 int status = lua_costatus(L, co);
 if (status != LUA_COFIN && status != LUA_COERR && status != LUA_COSUS)
 luaL_error(L, "cannot close %s coroutine", statnames[status]);
 if (co->status == LUA_OK || co->status == LUA_YIELD)
 {
 lua_pushboolean(L, true);
 lua_resetthread(co);
 return 1;
 }
 else
 {
 lua_pushboolean(L, false);
 if (lua_gettop(co))
 lua_xmove(co, L, 1);
 lua_resetthread(co);
 return 2;
 }
}
static const luaL_Reg co_funcs[] = {
 {"create", cocreate},
 {"running", corunning},
 {"status", costatus},
 {"wrap", cowrap},
 {"yield", coyield},
 {"isyieldable", coyieldable},
 {"close", coclose},
 {NULL, NULL},
};
int luaopen_coroutine(lua_State* L)
{
 luaL_register(L, LUA_COLIBNAME, co_funcs);
 lua_pushcclosurek(L, coresumey, "resume", 0, coresumecont);
 lua_setfield(L, -2, "resume");
 return 1;
}
static lua_State* getthread(lua_State* L, int* arg)
{
 if (lua_isthread(L, 1))
 {
 *arg = 1;
 return lua_tothread(L, 1);
 }
 else
 {
 *arg = 0;
 return L;
 }
}
static int db_info(lua_State* L)
{
 int arg;
 lua_State* L1 = getthread(L, &arg);
 if (L != L1)
 lua_rawcheckstack(L1, 1);
 int level;
 if (lua_isnumber(L, arg + 1))
 {
 level = (int)lua_tointeger(L, arg + 1);
 luaL_argcheck(L, level >= 0, arg + 1, "level can't be negative");
 }
 else if (arg == 0 && lua_isfunction(L, 1))
 {
 level = -lua_gettop(L);
 }
 else
 luaL_argerror(L, arg + 1, "function or level expected");
 const char* options = luaL_checkstring(L, arg + 2);
 lua_Debug ar;
 if (!lua_getinfo(L1, level, options, &ar))
 return 0;
 int results = 0;
 bool occurs[26] = {};
 for (const char* it = options; *it; ++it)
 {
 if (unsigned(*it - 'a') < 26)
 {
 if (occurs[*it - 'a'])
 luaL_argerror(L, arg + 2, "duplicate option");
 occurs[*it - 'a'] = true;
 }
 switch (*it)
 {
 case 's':
 lua_pushstring(L, ar.short_src);
 results++;
 break;
 case 'l':
 lua_pushinteger(L, ar.currentline);
 results++;
 break;
 case 'n':
 lua_pushstring(L, ar.name ? ar.name : "");
 results++;
 break;
 case 'f':
 if (L1 == L)
 lua_pushvalue(L, -1 - results);
 else
 lua_xmove(L1, L, 1);
 results++;
 break;
 case 'a':
 lua_pushinteger(L, ar.nparams);
 lua_pushboolean(L, ar.isvararg);
 results += 2;
 break;
 default:
 luaL_argerror(L, arg + 2, "invalid option");
 }
 }
 return results;
}
static int db_traceback(lua_State* L)
{
 int arg;
 lua_State* L1 = getthread(L, &arg);
 const char* msg = luaL_optstring(L, arg + 1, NULL);
 int level = luaL_optinteger(L, arg + 2, (L == L1) ? 1 : 0);
 luaL_argcheck(L, level >= 0, arg + 2, "level can't be negative");
 luaL_Buffer buf;
 luaL_buffinit(L, &buf);
 if (msg)
 {
 luaL_addstring(&buf, msg);
 luaL_addstring(&buf, "\n");
 }
 lua_Debug ar;
 for (int i = level; lua_getinfo(L1, i, "sln", &ar); ++i)
 {
 if (strcmp(ar.what, "C") == 0)
 continue;
 if (ar.source)
 luaL_addstring(&buf, ar.short_src);
 if (ar.currentline > 0)
 {
 char line[32];
 char* lineend = line + sizeof(line);
 char* lineptr = lineend;
 for (unsigned int r = ar.currentline; r > 0; r /= 10)
 *--lineptr = '0' + (r % 10);
 luaL_addchar(&buf, ':');
 luaL_addlstring(&buf, lineptr, lineend - lineptr, -1);
 }
 if (ar.name)
 {
 luaL_addstring(&buf, " function ");
 luaL_addstring(&buf, ar.name);
 }
 luaL_addchar(&buf, '\n');
 }
 luaL_pushresult(&buf);
 return 1;
}
static const luaL_Reg dblib[] = {
 {"info", db_info},
 {"traceback", db_traceback},
 {NULL, NULL},
};
int luaopen_debug(lua_State* L)
{
 luaL_register(L, LUA_DBLIBNAME, dblib);
 return 1;
}
struct lua_Page;
union GCObject;
#define luaM_newgco(L, t, size, memcat) cast_to(t*, luaM_newgco_(L, size, memcat))
#define luaM_freegco(L, p, size, memcat, page) luaM_freegco_(L, obj2gco(p), size, memcat, page)
#define luaM_arraysize_(L, n, e) ((cast_to(size_t, (n)) <= SIZE_MAX / (e)) ? (n) * (e) : (luaM_toobig(L), SIZE_MAX))
#define luaM_newarray(L, n, t, memcat) cast_to(t*, luaM_new_(L, luaM_arraysize_(L, n, sizeof(t)), memcat))
#define luaM_freearray(L, b, n, t, memcat) luaM_free_(L, (b), (n) * sizeof(t), memcat)
#define luaM_reallocarray(L, v, oldn, n, t, memcat) ((v) = cast_to(t*, luaM_realloc_(L, v, (oldn) * sizeof(t), luaM_arraysize_(L, n, sizeof(t)), memcat)))
LUAI_FUNC void* luaM_new_(lua_State* L, size_t nsize, uint8_t memcat);
LUAI_FUNC GCObject* luaM_newgco_(lua_State* L, size_t nsize, uint8_t memcat);
LUAI_FUNC void luaM_free_(lua_State* L, void* block, size_t osize, uint8_t memcat);
LUAI_FUNC void luaM_freegco_(lua_State* L, GCObject* block, size_t osize, uint8_t memcat, lua_Page* page);
LUAI_FUNC void* luaM_realloc_(lua_State* L, void* block, size_t osize, size_t nsize, uint8_t memcat);
LUAI_FUNC l_noret luaM_toobig(lua_State* L);
LUAI_FUNC void luaM_getpagewalkinfo(lua_Page* page, char** start, char** end, int* busyBlocks, int* blockSize);
LUAI_FUNC lua_Page* luaM_getnextgcopage(lua_Page* page);
LUAI_FUNC void luaM_visitpage(lua_Page* page, void* context, bool (*visitor)(void* context, lua_Page* page, GCObject* gco));
LUAI_FUNC void luaM_visitgco(lua_State* L, void* context, bool (*visitor)(void* context, lua_Page* page, GCObject* gco));
static const char* getfuncname(Closure* f);
static int currentpc(lua_State* L, CallInfo* ci)
{
 return pcRel(ci->savedpc, ci_func(ci)->l.p);
}
static int currentline(lua_State* L, CallInfo* ci)
{
 return luaG_getline(ci_func(ci)->l.p, currentpc(L, ci));
}
static Proto* getluaproto(CallInfo* ci)
{
 return (isLua(ci) ? cast_to(Proto*, ci_func(ci)->l.p) : NULL);
}
int lua_getargument(lua_State* L, int level, int n)
{
 if (unsigned(level) >= unsigned(L->ci - L->base_ci))
 return 0;
 CallInfo* ci = L->ci - level;
 Proto* fp = getluaproto(ci);
 int res = 0;
 if (fp && n > 0)
 {
 if (n <= fp->numparams)
 {
 luaC_threadbarrier(L);
 luaA_pushobject(L, ci->base + (n - 1));
 res = 1;
 }
 else if (fp->is_vararg && n < ci->base - ci->func)
 {
 luaC_threadbarrier(L);
 luaA_pushobject(L, ci->func + n);
 res = 1;
 }
 }
 return res;
}
const char* lua_getlocal(lua_State* L, int level, int n)
{
 if (unsigned(level) >= unsigned(L->ci - L->base_ci))
 return 0;
 CallInfo* ci = L->ci - level;
 Proto* fp = getluaproto(ci);
 const LocVar* var = fp ? luaF_getlocal(fp, n, currentpc(L, ci)) : NULL;
 if (var)
 {
 luaC_threadbarrier(L);
 luaA_pushobject(L, ci->base + var->reg);
 }
 const char* name = var ? getstr(var->varname) : NULL;
 return name;
}
const char* lua_setlocal(lua_State* L, int level, int n)
{
 if (unsigned(level) >= unsigned(L->ci - L->base_ci))
 return 0;
 CallInfo* ci = L->ci - level;
 Proto* fp = getluaproto(ci);
 const LocVar* var = fp ? luaF_getlocal(fp, n, currentpc(L, ci)) : NULL;
 if (var)
 setobj2s(L, ci->base + var->reg, L->top - 1);
 L->top--;
 const char* name = var ? getstr(var->varname) : NULL;
 return name;
}
static Closure* auxgetinfo(lua_State* L, const char* what, lua_Debug* ar, Closure* f, CallInfo* ci)
{
 Closure* cl = NULL;
 for (; *what; what++)
 {
 switch (*what)
 {
 case 's':
 {
 if (f->isC)
 {
 ar->source = "=[C]";
 ar->what = "C";
 ar->linedefined = -1;
 ar->short_src = "[C]";
 }
 else
 {
 TString* source = f->l.p->source;
 ar->source = getstr(source);
 ar->what = "Lua";
 ar->linedefined = f->l.p->linedefined;
 ar->short_src = luaO_chunkid(ar->ssbuf, sizeof(ar->ssbuf), getstr(source), source->len);
 }
 break;
 }
 case 'l':
 {
 if (ci)
 {
 ar->currentline = isLua(ci) ? currentline(L, ci) : -1;
 }
 else
 {
 ar->currentline = f->isC ? -1 : f->l.p->linedefined;
 }
 break;
 }
 case 'u':
 {
 ar->nupvals = f->nupvalues;
 break;
 }
 case 'a':
 {
 if (f->isC)
 {
 ar->isvararg = 1;
 ar->nparams = 0;
 }
 else
 {
 ar->isvararg = f->l.p->is_vararg;
 ar->nparams = f->l.p->numparams;
 }
 break;
 }
 case 'n':
 {
 ar->name = ci ? getfuncname(ci_func(ci)) : getfuncname(f);
 break;
 }
 case 'f':
 {
 cl = f;
 break;
 }
 default:;
 }
 }
 return cl;
}
int lua_stackdepth(lua_State* L)
{
 return int(L->ci - L->base_ci);
}
int lua_getinfo(lua_State* L, int level, const char* what, lua_Debug* ar)
{
 Closure* f = NULL;
 CallInfo* ci = NULL;
 if (level < 0)
 {
 const TValue* func = luaA_toobject(L, level);
 api_check(L, ttisfunction(func));
 f = clvalue(func);
 }
 else if (unsigned(level) < unsigned(L->ci - L->base_ci))
 {
 ci = L->ci - level;
 LUAU_ASSERT(ttisfunction(ci->func));
 f = clvalue(ci->func);
 }
 if (f)
 {
 if (Closure* fcl = auxgetinfo(L, what, ar, f, ci))
 {
 luaC_threadbarrier(L);
 setclvalue(L, L->top, fcl);
 incr_top(L);
 }
 }
 return f ? 1 : 0;
}
static const char* getfuncname(Closure* cl)
{
 if (cl->isC)
 {
 if (cl->c.debugname)
 {
 return cl->c.debugname;
 }
 }
 else
 {
 Proto* p = cl->l.p;
 if (p->debugname)
 {
 return getstr(p->debugname);
 }
 }
 return nullptr;
}
l_noret luaG_typeerrorL(lua_State* L, const TValue* o, const char* op)
{
 const char* t = luaT_objtypename(L, o);
 luaG_runerror(L, "attempt to %s a %s value", op, t);
}
l_noret luaG_forerrorL(lua_State* L, const TValue* o, const char* what)
{
 const char* t = luaT_objtypename(L, o);
 luaG_runerror(L, "invalid 'for' %s (number expected, got %s)", what, t);
}
l_noret luaG_concaterror(lua_State* L, StkId p1, StkId p2)
{
 const char* t1 = luaT_objtypename(L, p1);
 const char* t2 = luaT_objtypename(L, p2);
 luaG_runerror(L, "attempt to concatenate %s with %s", t1, t2);
}
l_noret luaG_aritherror(lua_State* L, const TValue* p1, const TValue* p2, TMS op)
{
 const char* t1 = luaT_objtypename(L, p1);
 const char* t2 = luaT_objtypename(L, p2);
 const char* opname = luaT_eventname[op] + 2;
 if (t1 == t2)
 luaG_runerror(L, "attempt to perform arithmetic (%s) on %s", opname, t1);
 else
 luaG_runerror(L, "attempt to perform arithmetic (%s) on %s and %s", opname, t1, t2);
}
l_noret luaG_ordererror(lua_State* L, const TValue* p1, const TValue* p2, TMS op)
{
 const char* t1 = luaT_objtypename(L, p1);
 const char* t2 = luaT_objtypename(L, p2);
 const char* opname = (op == TM_LT) ? "<" : (op == TM_LE) ? "<=" : "==";
 luaG_runerror(L, "attempt to compare %s %s %s", t1, opname, t2);
}
l_noret luaG_indexerror(lua_State* L, const TValue* p1, const TValue* p2)
{
 const char* t1 = luaT_objtypename(L, p1);
 const char* t2 = luaT_objtypename(L, p2);
 const TString* key = ttisstring(p2) ? tsvalue(p2) : 0;
 if (key && key->len <= 64)
 luaG_runerror(L, "attempt to index %s with '%s'", t1, getstr(key));
 else
 luaG_runerror(L, "attempt to index %s with %s", t1, t2);
}
l_noret luaG_methoderror(lua_State* L, const TValue* p1, const TString* p2)
{
 const char* t1 = luaT_objtypename(L, p1);
 luaG_runerror(L, "attempt to call missing method '%s' of %s", getstr(p2), t1);
}
l_noret luaG_readonlyerror(lua_State* L)
{
 luaG_runerror(L, "attempt to modify a readonly table");
}
static void pusherror(lua_State* L, const char* msg)
{
 CallInfo* ci = L->ci;
 if (isLua(ci))
 {
 TString* source = getluaproto(ci)->source;
 char chunkbuf[LUA_IDSIZE];
 const char* chunkid = luaO_chunkid(chunkbuf, sizeof(chunkbuf), getstr(source), source->len);
 int line = currentline(L, ci);
 luaO_pushfstring(L, "%s:%d: %s", chunkid, line, msg);
 }
 else
 {
 lua_pushstring(L, msg);
 }
}
l_noret luaG_runerrorL(lua_State* L, const char* fmt, ...)
{
 va_list argp;
 va_start(argp, fmt);
 char result[LUA_BUFFERSIZE];
 vsnprintf(result, sizeof(result), fmt, argp);
 va_end(argp);
 pusherror(L, result);
 luaD_throw(L, LUA_ERRRUN);
}
void luaG_pusherror(lua_State* L, const char* error)
{
 pusherror(L, error);
}
void luaG_breakpoint(lua_State* L, Proto* p, int line, bool enable)
{
 if (p->lineinfo)
 {
 for (int i = 0; i < p->sizecode; ++i)
 {
 if (LUAU_INSN_OP(p->code[i]) == LOP_PREPVARARGS)
 continue;
 if (luaG_getline(p, i) != line)
 continue;
 if (!p->debuginsn)
 {
 p->debuginsn = luaM_newarray(L, p->sizecode, uint8_t, p->memcat);
 for (int j = 0; j < p->sizecode; ++j)
 p->debuginsn[j] = LUAU_INSN_OP(p->code[j]);
 }
 uint8_t op = enable ? LOP_BREAK : LUAU_INSN_OP(p->debuginsn[i]);
 p->code[i] &= ~0xff;
 p->code[i] |= op;
 LUAU_ASSERT(LUAU_INSN_OP(p->code[i]) == op);
#if LUA_CUSTOM_EXECUTION
 if (L->global->ecb.setbreakpoint)
 L->global->ecb.setbreakpoint(L, p, i);
#endif
 break;
 }
 }
 for (int i = 0; i < p->sizep; ++i)
 {
 luaG_breakpoint(L, p->p[i], line, enable);
 }
}
bool luaG_onbreak(lua_State* L)
{
 if (L->ci == L->base_ci)
 return false;
 if (!isLua(L->ci))
 return false;
 return LUAU_INSN_OP(*L->ci->savedpc) == LOP_BREAK;
}
int luaG_getline(Proto* p, int pc)
{
 LUAU_ASSERT(pc >= 0 && pc < p->sizecode);
 if (!p->lineinfo)
 return 0;
 return p->abslineinfo[pc >> p->linegaplog2] + p->lineinfo[pc];
}
void lua_singlestep(lua_State* L, int enabled)
{
 L->singlestep = bool(enabled);
}
static int getmaxline(Proto* p)
{
 int result = -1;
 for (int i = 0; i < p->sizecode; ++i)
 {
 int line = luaG_getline(p, i);
 result = result < line ? line : result;
 }
 for (int i = 0; i < p->sizep; ++i)
 {
 int psize = getmaxline(p->p[i]);
 result = result < psize ? psize : result;
 }
 return result;
}
static int getnextline(Proto* p, int line)
{
 int closest = -1;
 if (p->lineinfo)
 {
 for (int i = 0; i < p->sizecode; ++i)
 {
 if (LUAU_INSN_OP(p->code[i]) == LOP_PREPVARARGS)
 continue;
 int candidate = luaG_getline(p, i);
 if (candidate == line)
 return line;
 if (candidate > line && (closest == -1 || candidate < closest))
 closest = candidate;
 }
 }
 for (int i = 0; i < p->sizep; ++i)
 {
 int candidate = getnextline(p->p[i], line);
 if (candidate == line)
 return line;
 if (candidate > line && (closest == -1 || candidate < closest))
 closest = candidate;
 }
 return closest;
}
int lua_breakpoint(lua_State* L, int funcindex, int line, int enabled)
{
 const TValue* func = luaA_toobject(L, funcindex);
 api_check(L, ttisfunction(func) && !clvalue(func)->isC);
 Proto* p = clvalue(func)->l.p;
 int target = getnextline(p, line);
 if (target != -1)
 {
 luaG_breakpoint(L, p, target, bool(enabled));
 }
 return target;
}
static void getcoverage(Proto* p, int depth, int* buffer, size_t size, void* context, lua_Coverage callback)
{
 memset(buffer, -1, size * sizeof(int));
 for (int i = 0; i < p->sizecode; ++i)
 {
 Instruction insn = p->code[i];
 if (LUAU_INSN_OP(insn) != LOP_COVERAGE)
 continue;
 int line = luaG_getline(p, i);
 int hits = LUAU_INSN_E(insn);
 LUAU_ASSERT(size_t(line) < size);
 buffer[line] = buffer[line] < hits ? hits : buffer[line];
 }
 const char* debugname = p->debugname ? getstr(p->debugname) : NULL;
 int linedefined = p->linedefined;
 callback(context, debugname, linedefined, depth, buffer, size);
 for (int i = 0; i < p->sizep; ++i)
 getcoverage(p->p[i], depth + 1, buffer, size, context, callback);
}
void lua_getcoverage(lua_State* L, int funcindex, void* context, lua_Coverage callback)
{
 const TValue* func = luaA_toobject(L, funcindex);
 api_check(L, ttisfunction(func) && !clvalue(func)->isC);
 Proto* p = clvalue(func)->l.p;
 size_t size = getmaxline(p) + 1;
 if (size == 0)
 return;
 int* buffer = luaM_newarray(L, size, int, 0);
 getcoverage(p, 0, buffer, size, context, callback);
 luaM_freearray(L, buffer, size, int, 0);
}
static size_t append(char* buf, size_t bufsize, size_t offset, const char* data)
{
 size_t size = strlen(data);
 size_t copy = offset + size >= bufsize ? bufsize - offset - 1 : size;
 memcpy(buf + offset, data, copy);
 return offset + copy;
}
const char* lua_debugtrace(lua_State* L)
{
 static char buf[4096];
 const int limit1 = 10;
 const int limit2 = 10;
 int depth = int(L->ci - L->base_ci);
 size_t offset = 0;
 lua_Debug ar;
 for (int level = 0; lua_getinfo(L, level, "sln", &ar); ++level)
 {
 if (ar.source)
 offset = append(buf, sizeof(buf), offset, ar.short_src);
 if (ar.currentline > 0)
 {
 char line[32];
 snprintf(line, sizeof(line), ":%d", ar.currentline);
 offset = append(buf, sizeof(buf), offset, line);
 }
 if (ar.name)
 {
 offset = append(buf, sizeof(buf), offset, " function ");
 offset = append(buf, sizeof(buf), offset, ar.name);
 }
 offset = append(buf, sizeof(buf), offset, "\n");
 if (depth > limit1 + limit2 && level == limit1 - 1)
 {
 char skip[32];
 snprintf(skip, sizeof(skip), "... (+%d frames)\n", int(depth - limit1 - limit2));
 offset = append(buf, sizeof(buf), offset, skip);
 level = depth - limit2 - 1;
 }
 }
 LUAU_ASSERT(offset < sizeof(buf));
 buf[offset] = '\0';
 return buf;
}
#if LUA_USE_LONGJMP
#include <setjmp.h>
#else
#include <stdexcept>
#endif
#if LUA_USE_LONGJMP
struct lua_jmpbuf
{
 lua_jmpbuf* volatile prev;
 volatile int status;
 jmp_buf buf;
};
#if defined(__linux__) || defined(__APPLE__)
#define LUAU_SETJMP(buf) _setjmp(buf)
#define LUAU_LONGJMP(buf, code) _longjmp(buf, code)
#else
#define LUAU_SETJMP(buf) setjmp(buf)
#define LUAU_LONGJMP(buf, code) longjmp(buf, code)
#endif
int luaD_rawrunprotected(lua_State* L, Pfunc f, void* ud)
{
 lua_jmpbuf jb;
 jb.prev = L->global->errorjmp;
 jb.status = 0;
 L->global->errorjmp = &jb;
 if (LUAU_SETJMP(jb.buf) == 0)
 f(L, ud);
 L->global->errorjmp = jb.prev;
 return jb.status;
}
l_noret luaD_throw(lua_State* L, int errcode)
{
 if (lua_jmpbuf* jb = L->global->errorjmp)
 {
 jb->status = errcode;
 LUAU_LONGJMP(jb->buf, 1);
 }
 if (L->global->cb.panic)
 L->global->cb.panic(L, errcode);
 abort();
}
#else
class lua_exception : public std::exception
{
public:
 lua_exception(lua_State* L, int status)
 : L(L)
 , status(status)
 {
 }
 const char* what() const throw() override
 {
 if (status == LUA_ERRRUN)
 if (const char* str = lua_tostring(L, -1))
 return str;
 switch (status)
 {
 case LUA_ERRRUN:
 return "lua_exception: runtime error";
 case LUA_ERRSYNTAX:
 return "lua_exception: syntax error";
 case LUA_ERRMEM:
 return "lua_exception: " LUA_MEMERRMSG;
 case LUA_ERRERR:
 return "lua_exception: " LUA_ERRERRMSG;
 default:
 return "lua_exception: unexpected exception status";
 }
 }
 int getStatus() const
 {
 return status;
 }
private:
 lua_State* L;
 int status;
};
int luaD_rawrunprotected(lua_State* L, Pfunc f, void* ud)
{
 int status = 0;
 try
 {
 f(L, ud);
 return 0;
 }
 catch (lua_exception& e)
 {
 status = e.getStatus();
 }
 catch (std::exception& e)
 {
 try
 {
 luaG_pusherror(L, e.what());
 status = LUA_ERRRUN;
 }
 catch (std::exception&)
 {
 status = LUA_ERRMEM;
 }
 }
 return status;
}
l_noret luaD_throw(lua_State* L, int errcode)
{
 throw lua_exception(L, errcode);
}
#endif
static void correctstack(lua_State* L, TValue* oldstack)
{
 L->top = (L->top - oldstack) + L->stack;
 for (UpVal* up = L->openupval; up != NULL; up = up->u.open.threadnext)
 up->v = (up->v - oldstack) + L->stack;
 for (CallInfo* ci = L->base_ci; ci <= L->ci; ci++)
 {
 ci->top = (ci->top - oldstack) + L->stack;
 ci->base = (ci->base - oldstack) + L->stack;
 ci->func = (ci->func - oldstack) + L->stack;
 }
 L->base = (L->base - oldstack) + L->stack;
}
void luaD_reallocstack(lua_State* L, int newsize)
{
 TValue* oldstack = L->stack;
 int realsize = newsize + EXTRA_STACK;
 LUAU_ASSERT(L->stack_last - L->stack == L->stacksize - EXTRA_STACK);
 luaM_reallocarray(L, L->stack, L->stacksize, realsize, TValue, L->memcat);
 TValue* newstack = L->stack;
 for (int i = L->stacksize; i < realsize; i++)
 setnilvalue(newstack + i);
 L->stacksize = realsize;
 L->stack_last = newstack + newsize;
 correctstack(L, oldstack);
}
void luaD_reallocCI(lua_State* L, int newsize)
{
 CallInfo* oldci = L->base_ci;
 luaM_reallocarray(L, L->base_ci, L->size_ci, newsize, CallInfo, L->memcat);
 L->size_ci = newsize;
 L->ci = (L->ci - oldci) + L->base_ci;
 L->end_ci = L->base_ci + L->size_ci - 1;
}
void luaD_growstack(lua_State* L, int n)
{
 if (n <= L->stacksize)
 luaD_reallocstack(L, 2 * L->stacksize);
 else
 luaD_reallocstack(L, L->stacksize + n);
}
CallInfo* luaD_growCI(lua_State* L)
{
 const int hardlimit = LUAI_MAXCALLS + (LUAI_MAXCALLS >> 3);
 if (L->size_ci >= hardlimit)
 luaD_throw(L, LUA_ERRERR);
 int request = L->size_ci * 2;
 luaD_reallocCI(L, L->size_ci >= LUAI_MAXCALLS ? hardlimit : request < LUAI_MAXCALLS ? request : LUAI_MAXCALLS);
 if (L->size_ci > LUAI_MAXCALLS)
 luaG_runerror(L, "stack overflow");
 return ++L->ci;
}
void luaD_checkCstack(lua_State* L)
{
 const int hardlimit = LUAI_MAXCCALLS + (LUAI_MAXCCALLS >> 3);
 if (L->nCcalls == LUAI_MAXCCALLS)
 luaG_runerror(L, "C stack overflow");
 else if (L->nCcalls >= hardlimit)
 luaD_throw(L, LUA_ERRERR);
}
void luaD_call(lua_State* L, StkId func, int nresults)
{
 if (++L->nCcalls >= LUAI_MAXCCALLS)
 luaD_checkCstack(L);
 ptrdiff_t old_func = savestack(L, func);
 if (luau_precall(L, func, nresults) == PCRLUA)
 {
 L->ci->flags |= LUA_CALLINFO_RETURN; // luau_execute will stop after returning from the stack frame
 bool oldactive = L->isactive;
 L->isactive = true;
 luaC_threadbarrier(L);
 luau_execute(L);
 if (!oldactive)
 L->isactive = false;
 }
 if (nresults != LUA_MULTRET)
 L->top = restorestack(L, old_func) + nresults;
 L->nCcalls--;
 luaC_checkGC(L);
}
static void seterrorobj(lua_State* L, int errcode, StkId oldtop)
{
 switch (errcode)
 {
 case LUA_ERRMEM:
 {
 setsvalue(L, oldtop, luaS_newliteral(L, LUA_MEMERRMSG));
 break;
 }
 case LUA_ERRERR:
 {
 setsvalue(L, oldtop, luaS_newliteral(L, LUA_ERRERRMSG));
 break;
 }
 case LUA_ERRSYNTAX:
 case LUA_ERRRUN:
 {
 setobj2s(L, oldtop, L->top - 1);
 break;
 }
 }
 L->top = oldtop + 1;
}
static void resume_continue(lua_State* L)
{
 while (L->status == 0 && L->ci > L->base_ci)
 {
 LUAU_ASSERT(L->baseCcalls == L->nCcalls);
 Closure* cl = curr_func(L);
 if (cl->isC)
 {
 LUAU_ASSERT(cl->c.cont);
 int n = cl->c.cont(L, 0);
 if (L->status == LUA_BREAK)
 break;
 luau_poscall(L, L->top - n);
 }
 else
 {
 luau_execute(L);
 }
 }
}
static void resume(lua_State* L, void* ud)
{
 StkId firstArg = cast_to(StkId, ud);
 if (L->status == 0)
 {
 LUAU_ASSERT(L->ci == L->base_ci && firstArg >= L->base);
 if (firstArg == L->base)
 luaG_runerror(L, "cannot resume dead coroutine");
 if (luau_precall(L, firstArg - 1, LUA_MULTRET) != PCRLUA)
 return;
 L->ci->flags |= LUA_CALLINFO_RETURN;
 }
 else
 {
 LUAU_ASSERT(L->status == LUA_YIELD || L->status == LUA_BREAK);
 L->status = 0;
 Closure* cl = curr_func(L);
 if (cl->isC)
 {
 if (!cl->c.cont)
 {
 luau_poscall(L, firstArg);
 }
 }
 else
 {
 L->base = L->ci->base;
 }
 }
 resume_continue(L);
}
static CallInfo* resume_findhandler(lua_State* L)
{
 CallInfo* ci = L->ci;
 while (ci > L->base_ci)
 {
 if (ci->flags & LUA_CALLINFO_HANDLE)
 return ci;
 ci--;
 }
 return NULL;
}
static void resume_handle(lua_State* L, void* ud)
{
 CallInfo* ci = (CallInfo*)ud;
 Closure* cl = ci_func(ci);
 LUAU_ASSERT(ci->flags & LUA_CALLINFO_HANDLE);
 LUAU_ASSERT(cl->isC && cl->c.cont);
 LUAU_ASSERT(L->status != 0);
 L->nCcalls = L->baseCcalls;
 ci->flags &= ~LUA_CALLINFO_HANDLE;
 int status = L->status;
 L->status = 0;
 if (status != LUA_ERRRUN)
 seterrorobj(L, status, L->top);
 L->base = ci->base;
 ci->top = L->top;
 ptrdiff_t old_ci = saveci(L, ci);
 int n = cl->c.cont(L, status);
 L->ci = restoreci(L, old_ci);
 luaF_close(L, L->base);
 luau_poscall(L, L->top - n);
 resume_continue(L);
}
static int resume_error(lua_State* L, const char* msg)
{
 L->top = L->ci->base;
 setsvalue(L, L->top, luaS_new(L, msg));
 incr_top(L);
 return LUA_ERRRUN;
}
static void resume_finish(lua_State* L, int status)
{
 L->nCcalls = L->baseCcalls;
 L->isactive = false;
 if (status != 0)
 {
 L->status = cast_byte(status); // mark thread as `dead'
 seterrorobj(L, status, L->top);
 L->ci->top = L->top;
 }
 else if (L->status == 0)
 {
 expandstacklimit(L, L->top);
 }
}
int lua_resume(lua_State* L, lua_State* from, int nargs)
{
 int status;
 if (L->status != LUA_YIELD && L->status != LUA_BREAK && (L->status != 0 || L->ci != L->base_ci))
 return resume_error(L, "cannot resume non-suspended coroutine");
 L->nCcalls = from ? from->nCcalls : 0;
 if (L->nCcalls >= LUAI_MAXCCALLS)
 return resume_error(L, "C stack overflow");
 L->baseCcalls = ++L->nCcalls;
 L->isactive = true;
 luaC_threadbarrier(L);
 status = luaD_rawrunprotected(L, resume, L->top - nargs);
 CallInfo* ch = NULL;
 while (status != 0 && (ch = resume_findhandler(L)) != NULL)
 {
 L->status = cast_byte(status);
 status = luaD_rawrunprotected(L, resume_handle, ch);
 }
 resume_finish(L, status);
 --L->nCcalls;
 return L->status;
}
int lua_resumeerror(lua_State* L, lua_State* from)
{
 int status;
 if (L->status != LUA_YIELD && L->status != LUA_BREAK && (L->status != 0 || L->ci != L->base_ci))
 return resume_error(L, "cannot resume non-suspended coroutine");
 L->nCcalls = from ? from->nCcalls : 0;
 if (L->nCcalls >= LUAI_MAXCCALLS)
 return resume_error(L, "C stack overflow");
 L->baseCcalls = ++L->nCcalls;
 L->isactive = true;
 luaC_threadbarrier(L);
 status = LUA_ERRRUN;
 CallInfo* ch = NULL;
 while (status != 0 && (ch = resume_findhandler(L)) != NULL)
 {
 L->status = cast_byte(status);
 status = luaD_rawrunprotected(L, resume_handle, ch);
 }
 resume_finish(L, status);
 --L->nCcalls;
 return L->status;
}
int lua_yield(lua_State* L, int nresults)
{
 if (L->nCcalls > L->baseCcalls)
 luaG_runerror(L, "attempt to yield across metamethod/C-call boundary");
 L->base = L->top - nresults;
 L->status = LUA_YIELD;
 return -1;
}
int lua_break(lua_State* L)
{
 if (L->nCcalls > L->baseCcalls)
 luaG_runerror(L, "attempt to break across metamethod/C-call boundary");
 L->status = LUA_BREAK;
 return -1;
}
int lua_isyieldable(lua_State* L)
{
 return (L->nCcalls <= L->baseCcalls);
}
static void callerrfunc(lua_State* L, void* ud)
{
 StkId errfunc = cast_to(StkId, ud);
 setobj2s(L, L->top, L->top - 1);
 setobj2s(L, L->top - 1, errfunc);
 incr_top(L);
 luaD_call(L, L->top - 2, 1);
}
static void restore_stack_limit(lua_State* L)
{
 LUAU_ASSERT(L->stack_last - L->stack == L->stacksize - EXTRA_STACK);
 if (L->size_ci > LUAI_MAXCALLS)
 {
 int inuse = cast_int(L->ci - L->base_ci);
 if (inuse + 1 < LUAI_MAXCALLS)
 luaD_reallocCI(L, LUAI_MAXCALLS);
 }
}
int luaD_pcall(lua_State* L, Pfunc func, void* u, ptrdiff_t old_top, ptrdiff_t ef)
{
 unsigned short oldnCcalls = L->nCcalls;
 ptrdiff_t old_ci = saveci(L, L->ci);
 bool oldactive = L->isactive;
 int status = luaD_rawrunprotected(L, func, u);
 if (status != 0)
 {
 int errstatus = status;
 if (ef)
 {
 if (status != LUA_ERRRUN)
 seterrorobj(L, status, L->top);
 int err = luaD_rawrunprotected(L, callerrfunc, restorestack(L, ef));
 if (err == 0)
 errstatus = LUA_ERRRUN;
 else if (status == LUA_ERRMEM && err == LUA_ERRMEM)
 errstatus = LUA_ERRMEM;
 else
 errstatus = status = LUA_ERRERR;
 }
 if (!oldactive)
 L->isactive = false;
 L->nCcalls = oldnCcalls;
 if (L->global->cb.debugprotectederror)
 {
 L->global->cb.debugprotectederror(L);
 if (L->status == LUA_BREAK)
 return 0;
 }
 StkId oldtop = restorestack(L, old_top);
 luaF_close(L, oldtop);
 seterrorobj(L, errstatus, oldtop);
 L->ci = restoreci(L, old_ci);
 L->base = L->ci->base;
 restore_stack_limit(L);
 }
 return status;
}
Proto* luaF_newproto(lua_State* L)
{
 Proto* f = luaM_newgco(L, Proto, sizeof(Proto), L->activememcat);
 luaC_init(L, f, LUA_TPROTO);
 f->k = NULL;
 f->sizek = 0;
 f->p = NULL;
 f->sizep = 0;
 f->code = NULL;
 f->sizecode = 0;
 f->sizeupvalues = 0;
 f->nups = 0;
 f->upvalues = NULL;
 f->numparams = 0;
 f->is_vararg = 0;
 f->maxstacksize = 0;
 f->sizelineinfo = 0;
 f->linegaplog2 = 0;
 f->lineinfo = NULL;
 f->abslineinfo = NULL;
 f->sizelocvars = 0;
 f->locvars = NULL;
 f->source = NULL;
 f->debugname = NULL;
 f->debuginsn = NULL;
 f->codeentry = NULL;
 f->execdata = NULL;
 f->exectarget = 0;
 return f;
}
Closure* luaF_newLclosure(lua_State* L, int nelems, Table* e, Proto* p)
{
 Closure* c = luaM_newgco(L, Closure, sizeLclosure(nelems), L->activememcat);
 luaC_init(L, c, LUA_TFUNCTION);
 c->isC = 0;
 c->env = e;
 c->nupvalues = cast_byte(nelems);
 c->stacksize = p->maxstacksize;
 c->preload = 0;
 c->l.p = p;
 for (int i = 0; i < nelems; ++i)
 setnilvalue(&c->l.uprefs[i]);
 return c;
}
Closure* luaF_newCclosure(lua_State* L, int nelems, Table* e)
{
 Closure* c = luaM_newgco(L, Closure, sizeCclosure(nelems), L->activememcat);
 luaC_init(L, c, LUA_TFUNCTION);
 c->isC = 1;
 c->env = e;
 c->nupvalues = cast_byte(nelems);
 c->stacksize = LUA_MINSTACK;
 c->preload = 0;
 c->c.f = NULL;
 c->c.cont = NULL;
 c->c.debugname = NULL;
 return c;
}
UpVal* luaF_findupval(lua_State* L, StkId level)
{
 global_State* g = L->global;
 UpVal** pp = &L->openupval;
 UpVal* p;
 while (*pp != NULL && (p = *pp)->v >= level)
 {
 LUAU_ASSERT(!isdead(g, obj2gco(p)));
 LUAU_ASSERT(upisopen(p));
 if (p->v == level)
 return p;
 pp = &p->u.open.threadnext;
 }
 LUAU_ASSERT(L->isactive);
 LUAU_ASSERT(!isblack(obj2gco(L)));
 UpVal* uv = luaM_newgco(L, UpVal, sizeof(UpVal), L->activememcat);
 luaC_init(L, uv, LUA_TUPVAL);
 uv->markedopen = 0;
 uv->v = level;
 uv->u.open.threadnext = *pp;
 *pp = uv;
 uv->u.open.prev = &g->uvhead;
 uv->u.open.next = g->uvhead.u.open.next;
 uv->u.open.next->u.open.prev = uv;
 g->uvhead.u.open.next = uv;
 LUAU_ASSERT(uv->u.open.next->u.open.prev == uv && uv->u.open.prev->u.open.next == uv);
 return uv;
}
void luaF_freeupval(lua_State* L, UpVal* uv, lua_Page* page)
{
 luaM_freegco(L, uv, sizeof(UpVal), uv->memcat, page);
}
void luaF_close(lua_State* L, StkId level)
{
 global_State* g = L->global;
 UpVal* uv;
 while (L->openupval != NULL && (uv = L->openupval)->v >= level)
 {
 GCObject* o = obj2gco(uv);
 LUAU_ASSERT(!isblack(o) && upisopen(uv));
 LUAU_ASSERT(!isdead(g, o));
 L->openupval = uv->u.open.threadnext;
 luaF_closeupval(L, uv, false);
 }
}
void luaF_closeupval(lua_State* L, UpVal* uv, bool dead)
{
 LUAU_ASSERT(uv->u.open.next->u.open.prev == uv && uv->u.open.prev->u.open.next == uv);
 uv->u.open.next->u.open.prev = uv->u.open.prev;
 uv->u.open.prev->u.open.next = uv->u.open.next;
 if (dead)
 return;
 setobj(L, &uv->u.value, uv->v);
 uv->v = &uv->u.value;
 luaC_upvalclosed(L, uv);
}
void luaF_freeproto(lua_State* L, Proto* f, lua_Page* page)
{
 luaM_freearray(L, f->code, f->sizecode, Instruction, f->memcat);
 luaM_freearray(L, f->p, f->sizep, Proto*, f->memcat);
 luaM_freearray(L, f->k, f->sizek, TValue, f->memcat);
 if (f->lineinfo)
 luaM_freearray(L, f->lineinfo, f->sizelineinfo, uint8_t, f->memcat);
 luaM_freearray(L, f->locvars, f->sizelocvars, struct LocVar, f->memcat);
 luaM_freearray(L, f->upvalues, f->sizeupvalues, TString*, f->memcat);
 if (f->debuginsn)
 luaM_freearray(L, f->debuginsn, f->sizecode, uint8_t, f->memcat);
#if LUA_CUSTOM_EXECUTION
 if (f->execdata)
 {
 LUAU_ASSERT(L->global->ecb.destroy);
 L->global->ecb.destroy(L, f);
 }
#endif
 luaM_freegco(L, f, sizeof(Proto), f->memcat, page);
}
void luaF_freeclosure(lua_State* L, Closure* c, lua_Page* page)
{
 int size = c->isC ? sizeCclosure(c->nupvalues) : sizeLclosure(c->nupvalues);
 luaM_freegco(L, c, size, c->memcat, page);
}
const LocVar* luaF_getlocal(const Proto* f, int local_number, int pc)
{
 for (int i = 0; i < f->sizelocvars; i++)
 {
 if (pc >= f->locvars[i].startpc && pc < f->locvars[i].endpc)
 {
 local_number--;
 if (local_number == 0)
 return &f->locvars[i];
 }
 }
 return NULL;
}
const LocVar* luaF_findlocal(const Proto* f, int local_reg, int pc)
{
 for (int i = 0; i < f->sizelocvars; i++)
 if (local_reg == f->locvars[i].reg && pc >= f->locvars[i].startpc && pc < f->locvars[i].endpc)
 return &f->locvars[i];
 return NULL;
}
#define GC_SWEEPPAGESTEPCOST 16
#define GC_INTERRUPT(state) { void (*interrupt)(lua_State*, int) = g->cb.interrupt; if (LUAU_UNLIKELY(!!interrupt)) interrupt(L, state); }
#define maskmarks cast_byte(~(bitmask(BLACKBIT) | WHITEBITS))
#define makewhite(g, x) ((x)->gch.marked = cast_byte(((x)->gch.marked & maskmarks) | luaC_white(g)))
#define white2gray(x) reset2bits((x)->gch.marked, WHITE0BIT, WHITE1BIT)
#define black2gray(x) resetbit((x)->gch.marked, BLACKBIT)
#define stringmark(s) reset2bits((s)->marked, WHITE0BIT, WHITE1BIT)
#define markvalue(g, o) { checkconsistency(o); if (iscollectable(o) && iswhite(gcvalue(o))) reallymarkobject(g, gcvalue(o)); }
#define markobject(g, t) { if (iswhite(obj2gco(t))) reallymarkobject(g, obj2gco(t)); }
#ifdef LUAI_GCMETRICS
static void recordGcStateStep(global_State* g, int startgcstate, double seconds, bool assist, size_t work)
{
 switch (startgcstate)
 {
 case GCSpause:
 if (g->gcstate == GCSpropagate)
 {
 g->gcmetrics.currcycle.marktime += seconds;
 if (assist)
 g->gcmetrics.currcycle.markassisttime += seconds;
 }
 break;
 case GCSpropagate:
 case GCSpropagateagain:
 g->gcmetrics.currcycle.marktime += seconds;
 g->gcmetrics.currcycle.markwork += work;
 if (assist)
 g->gcmetrics.currcycle.markassisttime += seconds;
 break;
 case GCSatomic:
 g->gcmetrics.currcycle.atomictime += seconds;
 break;
 case GCSsweep:
 g->gcmetrics.currcycle.sweeptime += seconds;
 g->gcmetrics.currcycle.sweepwork += work;
 if (assist)
 g->gcmetrics.currcycle.sweepassisttime += seconds;
 break;
 default:
 LUAU_ASSERT(!"Unexpected GC state");
 }
 if (assist)
 {
 g->gcmetrics.stepassisttimeacc += seconds;
 g->gcmetrics.currcycle.assistwork += work;
 }
 else
 {
 g->gcmetrics.stepexplicittimeacc += seconds;
 g->gcmetrics.currcycle.explicitwork += work;
 }
}
static double recordGcDeltaTime(double& timer)
{
 double now = lua_clock();
 double delta = now - timer;
 timer = now;
 return delta;
}
static void startGcCycleMetrics(global_State* g)
{
 g->gcmetrics.currcycle.starttimestamp = lua_clock();
 g->gcmetrics.currcycle.pausetime = g->gcmetrics.currcycle.starttimestamp - g->gcmetrics.lastcycle.endtimestamp;
}
static void finishGcCycleMetrics(global_State* g)
{
 g->gcmetrics.currcycle.endtimestamp = lua_clock();
 g->gcmetrics.currcycle.endtotalsizebytes = g->totalbytes;
 g->gcmetrics.completedcycles++;
 g->gcmetrics.lastcycle = g->gcmetrics.currcycle;
 g->gcmetrics.currcycle = GCCycleMetrics();
 g->gcmetrics.currcycle.starttotalsizebytes = g->totalbytes;
 g->gcmetrics.currcycle.heaptriggersizebytes = g->GCthreshold;
}
#endif
static void removeentry(LuaNode* n)
{
 LUAU_ASSERT(ttisnil(gval(n)));
 if (iscollectable(gkey(n)))
 setttype(gkey(n), LUA_TDEADKEY);
}
static void reallymarkobject(global_State* g, GCObject* o)
{
 LUAU_ASSERT(iswhite(o) && !isdead(g, o));
 white2gray(o);
 switch (o->gch.tt)
 {
 case LUA_TSTRING:
 {
 return;
 }
 case LUA_TUSERDATA:
 {
 Table* mt = gco2u(o)->metatable;
 gray2black(o);
 if (mt)
 markobject(g, mt);
 return;
 }
 case LUA_TUPVAL:
 {
 UpVal* uv = gco2uv(o);
 markvalue(g, uv->v);
 if (!upisopen(uv))
 gray2black(o); // open upvalues are never black
 return;
 }
 case LUA_TFUNCTION:
 {
 gco2cl(o)->gclist = g->gray;
 g->gray = o;
 break;
 }
 case LUA_TTABLE:
 {
 gco2h(o)->gclist = g->gray;
 g->gray = o;
 break;
 }
 case LUA_TTHREAD:
 {
 gco2th(o)->gclist = g->gray;
 g->gray = o;
 break;
 }
 case LUA_TPROTO:
 {
 gco2p(o)->gclist = g->gray;
 g->gray = o;
 break;
 }
 default:
 LUAU_ASSERT(0);
 }
}
static const char* gettablemode(global_State* g, Table* h)
{
 const TValue* mode = gfasttm(g, h->metatable, TM_MODE);
 if (mode && ttisstring(mode))
 return svalue(mode);
 return NULL;
}
static int traversetable(global_State* g, Table* h)
{
 int i;
 int weakkey = 0;
 int weakvalue = 0;
 if (h->metatable)
 markobject(g, cast_to(Table*, h->metatable));
 if (const char* modev = gettablemode(g, h))
 {
 weakkey = (strchr(modev, 'k') != NULL);
 weakvalue = (strchr(modev, 'v') != NULL);
 if (weakkey || weakvalue)
 {
 h->gclist = g->weak; // must be cleared after GC, ...
 g->weak = obj2gco(h);
 }
 }
 if (weakkey && weakvalue)
 return 1;
 if (!weakvalue)
 {
 i = h->sizearray;
 while (i--)
 markvalue(g, &h->array[i]);
 }
 i = sizenode(h);
 while (i--)
 {
 LuaNode* n = gnode(h, i);
 LUAU_ASSERT(ttype(gkey(n)) != LUA_TDEADKEY || ttisnil(gval(n)));
 if (ttisnil(gval(n)))
 removeentry(n);
 else
 {
 LUAU_ASSERT(!ttisnil(gkey(n)));
 if (!weakkey)
 markvalue(g, gkey(n));
 if (!weakvalue)
 markvalue(g, gval(n));
 }
 }
 return weakkey || weakvalue;
}
static void traverseproto(global_State* g, Proto* f)
{
 int i;
 if (f->source)
 stringmark(f->source);
 if (f->debugname)
 stringmark(f->debugname);
 for (i = 0; i < f->sizek; i++)
 markvalue(g, &f->k[i]);
 for (i = 0; i < f->sizeupvalues; i++)
 {
 if (f->upvalues[i])
 stringmark(f->upvalues[i]);
 }
 for (i = 0; i < f->sizep; i++)
 {
 if (f->p[i])
 markobject(g, f->p[i]);
 }
 for (i = 0; i < f->sizelocvars; i++)
 {
 if (f->locvars[i].varname)
 stringmark(f->locvars[i].varname);
 }
}
static void traverseclosure(global_State* g, Closure* cl)
{
 markobject(g, cl->env);
 if (cl->isC)
 {
 int i;
 for (i = 0; i < cl->nupvalues; i++)
 markvalue(g, &cl->c.upvals[i]);
 }
 else
 {
 int i;
 LUAU_ASSERT(cl->nupvalues == cl->l.p->nups);
 markobject(g, cast_to(Proto*, cl->l.p));
 for (i = 0; i < cl->nupvalues; i++)
 markvalue(g, &cl->l.uprefs[i]);
 }
}
static void traversestack(global_State* g, lua_State* l)
{
 markobject(g, l->gt);
 if (l->namecall)
 stringmark(l->namecall);
 for (StkId o = l->stack; o < l->top; o++)
 markvalue(g, o);
 for (UpVal* uv = l->openupval; uv; uv = uv->u.open.threadnext)
 {
 LUAU_ASSERT(upisopen(uv));
 uv->markedopen = 1;
 markobject(g, uv);
 }
}
static void clearstack(lua_State* l)
{
 StkId stack_end = l->stack + l->stacksize;
 for (StkId o = l->top; o < stack_end; o++)
 setnilvalue(o);
}
static void shrinkstack(lua_State* L)
{
 StkId lim = L->top;
 for (CallInfo* ci = L->base_ci; ci <= L->ci; ci++)
 {
 LUAU_ASSERT(ci->top <= L->stack_last);
 if (lim < ci->top)
 lim = ci->top;
 }
 int ci_used = cast_int(L->ci - L->base_ci); // number of `ci' in use
 int s_used = cast_int(lim - L->stack);
 if (L->size_ci > LUAI_MAXCALLS) // handling overflow?
 return;
 if (3 * ci_used < L->size_ci && 2 * BASIC_CI_SIZE < L->size_ci)
 luaD_reallocCI(L, L->size_ci / 2); // still big enough...
 condhardstacktests(luaD_reallocCI(L, ci_used + 1));
 if (3 * s_used < L->stacksize && 2 * (BASIC_STACK_SIZE + EXTRA_STACK) < L->stacksize)
 luaD_reallocstack(L, L->stacksize / 2); // still big enough...
 condhardstacktests(luaD_reallocstack(L, s_used));
}
static size_t propagatemark(global_State* g)
{
 GCObject* o = g->gray;
 LUAU_ASSERT(isgray(o));
 gray2black(o);
 switch (o->gch.tt)
 {
 case LUA_TTABLE:
 {
 Table* h = gco2h(o);
 g->gray = h->gclist;
 if (traversetable(g, h))
 black2gray(o); // keep it gray
 return sizeof(Table) + sizeof(TValue) * h->sizearray + sizeof(LuaNode) * sizenode(h);
 }
 case LUA_TFUNCTION:
 {
 Closure* cl = gco2cl(o);
 g->gray = cl->gclist;
 traverseclosure(g, cl);
 return cl->isC ? sizeCclosure(cl->nupvalues) : sizeLclosure(cl->nupvalues);
 }
 case LUA_TTHREAD:
 {
 lua_State* th = gco2th(o);
 g->gray = th->gclist;
 bool active = th->isactive || th == th->global->mainthread;
 traversestack(g, th);
 if (active)
 {
 th->gclist = g->grayagain;
 g->grayagain = o;
 black2gray(o);
 }
 if (!active || g->gcstate == GCSatomic)
 clearstack(th);
 if (g->gcstate == GCSpropagate)
 shrinkstack(th);
 return sizeof(lua_State) + sizeof(TValue) * th->stacksize + sizeof(CallInfo) * th->size_ci;
 }
 case LUA_TPROTO:
 {
 Proto* p = gco2p(o);
 g->gray = p->gclist;
 traverseproto(g, p);
 return sizeof(Proto) + sizeof(Instruction) * p->sizecode + sizeof(Proto*) * p->sizep + sizeof(TValue) * p->sizek + p->sizelineinfo +
 sizeof(LocVar) * p->sizelocvars + sizeof(TString*) * p->sizeupvalues;
 }
 default:
 LUAU_ASSERT(0);
 return 0;
 }
}
static size_t propagateall(global_State* g)
{
 size_t work = 0;
 while (g->gray)
 {
 work += propagatemark(g);
 }
 return work;
}
static int isobjcleared(GCObject* o)
{
 if (o->gch.tt == LUA_TSTRING)
 {
 stringmark(&o->ts);
 return 0;
 }
 return iswhite(o);
}
#define iscleared(o) (iscollectable(o) && isobjcleared(gcvalue(o)))
static size_t cleartable(lua_State* L, GCObject* l)
{
 size_t work = 0;
 while (l)
 {
 Table* h = gco2h(l);
 work += sizeof(Table) + sizeof(TValue) * h->sizearray + sizeof(LuaNode) * sizenode(h);
 int i = h->sizearray;
 while (i--)
 {
 TValue* o = &h->array[i];
 if (iscleared(o))
 setnilvalue(o); // remove value
 }
 i = sizenode(h);
 int activevalues = 0;
 while (i--)
 {
 LuaNode* n = gnode(h, i);
 if (!ttisnil(gval(n)))
 {
 if (iscleared(gkey(n)) || iscleared(gval(n)))
 {
 setnilvalue(gval(n));
 removeentry(n); // remove entry from table
 }
 else
 {
 activevalues++;
 }
 }
 }
 if (const char* modev = gettablemode(L->global, h))
 {
 if (strchr(modev, 's'))
 {
 if (activevalues < sizenode(h) * 3 / 8)
 luaH_resizehash(L, h, activevalues);
 }
 }
 l = h->gclist;
 }
 return work;
}
static void freeobj(lua_State* L, GCObject* o, lua_Page* page)
{
 switch (o->gch.tt)
 {
 case LUA_TPROTO:
 luaF_freeproto(L, gco2p(o), page);
 break;
 case LUA_TFUNCTION:
 luaF_freeclosure(L, gco2cl(o), page);
 break;
 case LUA_TUPVAL:
 luaF_freeupval(L, gco2uv(o), page);
 break;
 case LUA_TTABLE:
 luaH_free(L, gco2h(o), page);
 break;
 case LUA_TTHREAD:
 LUAU_ASSERT(gco2th(o) != L && gco2th(o) != L->global->mainthread);
 luaE_freethread(L, gco2th(o), page);
 break;
 case LUA_TSTRING:
 luaS_free(L, gco2ts(o), page);
 break;
 case LUA_TUSERDATA:
 luaU_freeudata(L, gco2u(o), page);
 break;
 default:
 LUAU_ASSERT(0);
 }
}
static void shrinkbuffers(lua_State* L)
{
 global_State* g = L->global;
 if (g->strt.nuse < cast_to(uint32_t, g->strt.size / 4) && g->strt.size > LUA_MINSTRTABSIZE * 2)
 luaS_resize(L, g->strt.size / 2); // table is too big
}
static void shrinkbuffersfull(lua_State* L)
{
 global_State* g = L->global;
 int hashsize = g->strt.size;
 while (g->strt.nuse < cast_to(uint32_t, hashsize / 4) && hashsize > LUA_MINSTRTABSIZE * 2)
 hashsize /= 2;
 if (hashsize != g->strt.size)
 luaS_resize(L, hashsize);
}
static bool deletegco(void* context, lua_Page* page, GCObject* gco)
{
 lua_State* L = (lua_State*)context;
 freeobj(L, gco, page);
 return true;
}
void luaC_freeall(lua_State* L)
{
 global_State* g = L->global;
 LUAU_ASSERT(L == g->mainthread);
 luaM_visitgco(L, L, deletegco);
 for (int i = 0; i < g->strt.size; i++)
 LUAU_ASSERT(g->strt.hash[i] == NULL);
 LUAU_ASSERT(L->global->strt.nuse == 0);
}
static void markmt(global_State* g)
{
 int i;
 for (i = 0; i < LUA_T_COUNT; i++)
 if (g->mt[i])
 markobject(g, g->mt[i]);
}
static void markroot(lua_State* L)
{
 global_State* g = L->global;
 g->gray = NULL;
 g->grayagain = NULL;
 g->weak = NULL;
 markobject(g, g->mainthread);
 markobject(g, g->mainthread->gt);
 markvalue(g, registry(L));
 markmt(g);
 g->gcstate = GCSpropagate;
}
static size_t remarkupvals(global_State* g)
{
 size_t work = 0;
 for (UpVal* uv = g->uvhead.u.open.next; uv != &g->uvhead; uv = uv->u.open.next)
 {
 work += sizeof(UpVal);
 LUAU_ASSERT(upisopen(uv));
 LUAU_ASSERT(uv->u.open.next->u.open.prev == uv && uv->u.open.prev->u.open.next == uv);
 LUAU_ASSERT(!isblack(obj2gco(uv)));
 if (isgray(obj2gco(uv)))
 markvalue(g, uv->v);
 }
 return work;
}
static size_t clearupvals(lua_State* L)
{
 global_State* g = L->global;
 size_t work = 0;
 for (UpVal* uv = g->uvhead.u.open.next; uv != &g->uvhead;)
 {
 work += sizeof(UpVal);
 LUAU_ASSERT(upisopen(uv));
 LUAU_ASSERT(uv->u.open.next->u.open.prev == uv && uv->u.open.prev->u.open.next == uv);
 LUAU_ASSERT(!isblack(obj2gco(uv)));
 LUAU_ASSERT(iswhite(obj2gco(uv)) || !iscollectable(uv->v) || !iswhite(gcvalue(uv->v)));
 if (uv->markedopen)
 {
 LUAU_ASSERT(isgray(obj2gco(uv)));
 uv->markedopen = 0;
 uv = uv->u.open.next;
 }
 else
 {
 UpVal* next = uv->u.open.next;
 luaF_closeupval(L, uv, iswhite(obj2gco(uv)));
 uv = next;
 }
 }
 return work;
}
static size_t atomic(lua_State* L)
{
 global_State* g = L->global;
 LUAU_ASSERT(g->gcstate == GCSatomic);
 size_t work = 0;
#ifdef LUAI_GCMETRICS
 double currts = lua_clock();
#endif
 work += remarkupvals(g);
 work += propagateall(g);
#ifdef LUAI_GCMETRICS
 g->gcmetrics.currcycle.atomictimeupval += recordGcDeltaTime(currts);
#endif
 g->gray = g->weak;
 g->weak = NULL;
 LUAU_ASSERT(!iswhite(obj2gco(g->mainthread)));
 markobject(g, L);
 markmt(g); // mark basic metatables (again)
 work += propagateall(g);
#ifdef LUAI_GCMETRICS
 g->gcmetrics.currcycle.atomictimeweak += recordGcDeltaTime(currts);
#endif
 g->gray = g->grayagain;
 g->grayagain = NULL;
 work += propagateall(g);
#ifdef LUAI_GCMETRICS
 g->gcmetrics.currcycle.atomictimegray += recordGcDeltaTime(currts);
#endif
 work += cleartable(L, g->weak);
 g->weak = NULL;
#ifdef LUAI_GCMETRICS
 g->gcmetrics.currcycle.atomictimeclear += recordGcDeltaTime(currts);
#endif
 work += clearupvals(L);
#ifdef LUAI_GCMETRICS
 g->gcmetrics.currcycle.atomictimeupval += recordGcDeltaTime(currts);
#endif
 g->currentwhite = cast_byte(otherwhite(g));
 g->sweepgcopage = g->allgcopages;
 g->gcstate = GCSsweep;
 return work;
}
static int sweepgcopage(lua_State* L, lua_Page* page)
{
 char* start;
 char* end;
 int busyBlocks;
 int blockSize;
 luaM_getpagewalkinfo(page, &start, &end, &busyBlocks, &blockSize);
 LUAU_ASSERT(busyBlocks > 0);
 global_State* g = L->global;
 int deadmask = otherwhite(g);
 LUAU_ASSERT(testbit(deadmask, FIXEDBIT));
 int newwhite = luaC_white(g);
 for (char* pos = start; pos != end; pos += blockSize)
 {
 GCObject* gco = (GCObject*)pos;
 if (gco->gch.tt == LUA_TNIL)
 continue;
 if ((gco->gch.marked ^ WHITEBITS) & deadmask)
 {
 LUAU_ASSERT(!isdead(g, gco));
 gco->gch.marked = cast_byte((gco->gch.marked & maskmarks) | newwhite);
 }
 else
 {
 LUAU_ASSERT(isdead(g, gco));
 freeobj(L, gco, page);
 if (--busyBlocks == 0)
 return int(pos - start) / blockSize + 1;
 }
 }
 return int(end - start) / blockSize;
}
static size_t gcstep(lua_State* L, size_t limit)
{
 size_t cost = 0;
 global_State* g = L->global;
 switch (g->gcstate)
 {
 case GCSpause:
 {
 markroot(L);
 LUAU_ASSERT(g->gcstate == GCSpropagate);
 break;
 }
 case GCSpropagate:
 {
 while (g->gray && cost < limit)
 {
 cost += propagatemark(g);
 }
 if (!g->gray)
 {
#ifdef LUAI_GCMETRICS
 g->gcmetrics.currcycle.propagatework = g->gcmetrics.currcycle.explicitwork + g->gcmetrics.currcycle.assistwork;
#endif
 g->gray = g->grayagain;
 g->grayagain = NULL;
 g->gcstate = GCSpropagateagain;
 }
 break;
 }
 case GCSpropagateagain:
 {
 while (g->gray && cost < limit)
 {
 cost += propagatemark(g);
 }
 if (!g->gray)
 {
#ifdef LUAI_GCMETRICS
 g->gcmetrics.currcycle.propagateagainwork =
 g->gcmetrics.currcycle.explicitwork + g->gcmetrics.currcycle.assistwork - g->gcmetrics.currcycle.propagatework;
#endif
 g->gcstate = GCSatomic;
 }
 break;
 }
 case GCSatomic:
 {
#ifdef LUAI_GCMETRICS
 g->gcmetrics.currcycle.atomicstarttimestamp = lua_clock();
 g->gcmetrics.currcycle.atomicstarttotalsizebytes = g->totalbytes;
#endif
 g->gcstats.atomicstarttimestamp = lua_clock();
 g->gcstats.atomicstarttotalsizebytes = g->totalbytes;
 cost = atomic(L);
 LUAU_ASSERT(g->gcstate == GCSsweep);
 break;
 }
 case GCSsweep:
 {
 while (g->sweepgcopage && cost < limit)
 {
 lua_Page* next = luaM_getnextgcopage(g->sweepgcopage);
 int steps = sweepgcopage(L, g->sweepgcopage);
 g->sweepgcopage = next;
 cost += steps * GC_SWEEPPAGESTEPCOST;
 }
 if (g->sweepgcopage == NULL)
 {
 LUAU_ASSERT(!isdead(g, obj2gco(g->mainthread)));
 makewhite(g, obj2gco(g->mainthread));
 shrinkbuffers(L);
 g->gcstate = GCSpause;
 }
 break;
 }
 default:
 LUAU_ASSERT(!"Unexpected GC state");
 }
 return cost;
}
static int64_t getheaptriggererroroffset(global_State* g)
{
 int32_t errorKb = int32_t((g->gcstats.atomicstarttotalsizebytes - g->gcstats.heapgoalsizebytes) / 1024);
 const size_t triggertermcount = sizeof(g->gcstats.triggerterms) / sizeof(g->gcstats.triggerterms[0]);
 int32_t* slot = &g->gcstats.triggerterms[g->gcstats.triggertermpos % triggertermcount];
 int32_t prev = *slot;
 *slot = errorKb;
 g->gcstats.triggerintegral += errorKb - prev;
 g->gcstats.triggertermpos++;
 const double Ku = 0.9;
 const double Tu = 2.5; // oscillation period (measured)
 const double Kp = 0.45 * Ku;
 const double Ti = 0.8 * Tu;
 const double Ki = 0.54 * Ku / Ti; // integral gain
 double proportionalTerm = Kp * errorKb;
 double integralTerm = Ki * g->gcstats.triggerintegral;
 double totalTerm = proportionalTerm + integralTerm;
 return int64_t(totalTerm * 1024);
}
static size_t getheaptrigger(global_State* g, size_t heapgoal)
{
 const double durationthreshold = 1e-3;
 double allocationduration = g->gcstats.atomicstarttimestamp - g->gcstats.endtimestamp;
 if (allocationduration < durationthreshold)
 return heapgoal;
 double allocationrate = (g->gcstats.atomicstarttotalsizebytes - g->gcstats.endtotalsizebytes) / allocationduration;
 double markduration = g->gcstats.atomicstarttimestamp - g->gcstats.starttimestamp;
 int64_t expectedgrowth = int64_t(markduration * allocationrate);
 int64_t offset = getheaptriggererroroffset(g);
 int64_t heaptrigger = heapgoal - (expectedgrowth + offset);
 return heaptrigger < int64_t(g->totalbytes) ? g->totalbytes : (heaptrigger > int64_t(heapgoal) ? heapgoal : size_t(heaptrigger));
}
size_t luaC_step(lua_State* L, bool assist)
{
 global_State* g = L->global;
 int lim = g->gcstepsize * g->gcstepmul / 100; // how much to work
 LUAU_ASSERT(g->totalbytes >= g->GCthreshold);
 size_t debt = g->totalbytes - g->GCthreshold;
 GC_INTERRUPT(0);
 if (g->gcstate == GCSpause)
 g->gcstats.starttimestamp = lua_clock();
#ifdef LUAI_GCMETRICS
 if (g->gcstate == GCSpause)
 startGcCycleMetrics(g);
 double lasttimestamp = lua_clock();
#endif
 int lastgcstate = g->gcstate;
 size_t work = gcstep(L, lim);
#ifdef LUAI_GCMETRICS
 recordGcStateStep(g, lastgcstate, lua_clock() - lasttimestamp, assist, work);
#endif
 size_t actualstepsize = work * 100 / g->gcstepmul;
 if (g->gcstate == GCSpause)
 {
 size_t heapgoal = (g->totalbytes / 100) * g->gcgoal;
 size_t heaptrigger = getheaptrigger(g, heapgoal);
 g->GCthreshold = heaptrigger;
 g->gcstats.heapgoalsizebytes = heapgoal;
 g->gcstats.endtimestamp = lua_clock();
 g->gcstats.endtotalsizebytes = g->totalbytes;
#ifdef LUAI_GCMETRICS
 finishGcCycleMetrics(g);
#endif
 }
 else
 {
 g->GCthreshold = g->totalbytes + actualstepsize;
 if (g->GCthreshold >= debt)
 g->GCthreshold -= debt;
 }
 GC_INTERRUPT(lastgcstate);
 return actualstepsize;
}
void luaC_fullgc(lua_State* L)
{
 global_State* g = L->global;
#ifdef LUAI_GCMETRICS
 if (g->gcstate == GCSpause)
 startGcCycleMetrics(g);
#endif
 if (keepinvariant(g))
 {
 g->sweepgcopage = g->allgcopages;
 g->gray = NULL;
 g->grayagain = NULL;
 g->weak = NULL;
 g->gcstate = GCSsweep;
 }
 LUAU_ASSERT(g->gcstate == GCSpause || g->gcstate == GCSsweep);
 while (g->gcstate != GCSpause)
 {
 LUAU_ASSERT(g->gcstate == GCSsweep);
 gcstep(L, SIZE_MAX);
 }
 for (UpVal* uv = g->uvhead.u.open.next; uv != &g->uvhead; uv = uv->u.open.next)
 {
 LUAU_ASSERT(upisopen(uv));
 uv->markedopen = 0;
 }
#ifdef LUAI_GCMETRICS
 finishGcCycleMetrics(g);
 startGcCycleMetrics(g);
#endif
 markroot(L);
 while (g->gcstate != GCSpause)
 {
 gcstep(L, SIZE_MAX);
 }
 shrinkbuffersfull(L);
 size_t heapgoalsizebytes = (g->totalbytes / 100) * g->gcgoal;
 g->GCthreshold = g->totalbytes * (g->gcgoal * g->gcstepmul / 100 - 100) / g->gcstepmul;
 if (g->GCthreshold < g->totalbytes)
 g->GCthreshold = g->totalbytes;
 g->gcstats.heapgoalsizebytes = heapgoalsizebytes;
#ifdef LUAI_GCMETRICS
 finishGcCycleMetrics(g);
#endif
}
void luaC_barrierf(lua_State* L, GCObject* o, GCObject* v)
{
 global_State* g = L->global;
 LUAU_ASSERT(isblack(o) && iswhite(v) && !isdead(g, v) && !isdead(g, o));
 LUAU_ASSERT(g->gcstate != GCSpause);
 if (keepinvariant(g))
 reallymarkobject(g, v);
 else // don't mind
 makewhite(g, o);
}
void luaC_barriertable(lua_State* L, Table* t, GCObject* v)
{
 global_State* g = L->global;
 GCObject* o = obj2gco(t);
 if (g->gcstate == GCSpropagateagain)
 {
 LUAU_ASSERT(isblack(o) && iswhite(v) && !isdead(g, v) && !isdead(g, o));
 reallymarkobject(g, v);
 return;
 }
 LUAU_ASSERT(isblack(o) && !isdead(g, o));
 LUAU_ASSERT(g->gcstate != GCSpause);
 black2gray(o);
 t->gclist = g->grayagain;
 g->grayagain = o;
}
void luaC_barrierback(lua_State* L, GCObject* o, GCObject** gclist)
{
 global_State* g = L->global;
 LUAU_ASSERT(isblack(o) && !isdead(g, o));
 LUAU_ASSERT(g->gcstate != GCSpause);
 black2gray(o);
 *gclist = g->grayagain;
 g->grayagain = o;
}
void luaC_upvalclosed(lua_State* L, UpVal* uv)
{
 global_State* g = L->global;
 GCObject* o = obj2gco(uv);
 LUAU_ASSERT(!upisopen(uv));
 if (isgray(o))
 {
 if (keepinvariant(g))
 {
 gray2black(o);
 luaC_barrier(L, uv, uv->v);
 }
 else
 {
 makewhite(g, o);
 LUAU_ASSERT(g->gcstate != GCSpause);
 }
 }
}
int64_t luaC_allocationrate(lua_State* L)
{
 global_State* g = L->global;
 const double durationthreshold = 1e-3;
 if (g->gcstate <= GCSatomic)
 {
 double duration = lua_clock() - g->gcstats.endtimestamp;
 if (duration < durationthreshold)
 return -1;
 return int64_t((g->totalbytes - g->gcstats.endtotalsizebytes) / duration);
 }
 double duration = g->gcstats.atomicstarttimestamp - g->gcstats.endtimestamp;
 if (duration < durationthreshold)
 return -1;
 return int64_t((g->gcstats.atomicstarttotalsizebytes - g->gcstats.endtotalsizebytes) / duration);
}
const char* luaC_statename(int state)
{
 switch (state)
 {
 case GCSpause:
 return "pause";
 case GCSpropagate:
 return "mark";
 case GCSpropagateagain:
 return "remark";
 case GCSatomic:
 return "atomic";
 case GCSsweep:
 return "sweep";
 default:
 return NULL;
 }
}
static void validateobjref(global_State* g, GCObject* f, GCObject* t)
{
 LUAU_ASSERT(!isdead(g, t));
 if (keepinvariant(g))
 {
 LUAU_ASSERT(!(isblack(f) && iswhite(t)));
 }
}
static void validateref(global_State* g, GCObject* f, TValue* v)
{
 if (iscollectable(v))
 {
 LUAU_ASSERT(ttype(v) == gcvalue(v)->gch.tt);
 validateobjref(g, f, gcvalue(v));
 }
}
static void validatetable(global_State* g, Table* h)
{
 int sizenode = 1 << h->lsizenode;
 LUAU_ASSERT(h->lastfree <= sizenode);
 if (h->metatable)
 validateobjref(g, obj2gco(h), obj2gco(h->metatable));
 for (int i = 0; i < h->sizearray; ++i)
 validateref(g, obj2gco(h), &h->array[i]);
 for (int i = 0; i < sizenode; ++i)
 {
 LuaNode* n = &h->node[i];
 LUAU_ASSERT(ttype(gkey(n)) != LUA_TDEADKEY || ttisnil(gval(n)));
 LUAU_ASSERT(i + gnext(n) >= 0 && i + gnext(n) < sizenode);
 if (!ttisnil(gval(n)))
 {
 TValue k = {};
 k.tt = gkey(n)->tt;
 k.value = gkey(n)->value;
 validateref(g, obj2gco(h), &k);
 validateref(g, obj2gco(h), gval(n));
 }
 }
}
static void validateclosure(global_State* g, Closure* cl)
{
 validateobjref(g, obj2gco(cl), obj2gco(cl->env));
 if (cl->isC)
 {
 for (int i = 0; i < cl->nupvalues; ++i)
 validateref(g, obj2gco(cl), &cl->c.upvals[i]);
 }
 else
 {
 LUAU_ASSERT(cl->nupvalues == cl->l.p->nups);
 validateobjref(g, obj2gco(cl), obj2gco(cl->l.p));
 for (int i = 0; i < cl->nupvalues; ++i)
 validateref(g, obj2gco(cl), &cl->l.uprefs[i]);
 }
}
static void validatestack(global_State* g, lua_State* l)
{
 validateobjref(g, obj2gco(l), obj2gco(l->gt));
 for (CallInfo* ci = l->base_ci; ci <= l->ci; ++ci)
 {
 LUAU_ASSERT(l->stack <= ci->base);
 LUAU_ASSERT(ci->func <= ci->base && ci->base <= ci->top);
 LUAU_ASSERT(ci->top <= l->stack_last);
 }
 for (StkId o = l->stack; o < l->top; ++o)
 checkliveness(g, o);
 if (l->namecall)
 validateobjref(g, obj2gco(l), obj2gco(l->namecall));
 for (UpVal* uv = l->openupval; uv; uv = uv->u.open.threadnext)
 {
 LUAU_ASSERT(uv->tt == LUA_TUPVAL);
 LUAU_ASSERT(upisopen(uv));
 LUAU_ASSERT(uv->u.open.next->u.open.prev == uv && uv->u.open.prev->u.open.next == uv);
 LUAU_ASSERT(!isblack(obj2gco(uv)));
 }
}
static void validateproto(global_State* g, Proto* f)
{
 if (f->source)
 validateobjref(g, obj2gco(f), obj2gco(f->source));
 if (f->debugname)
 validateobjref(g, obj2gco(f), obj2gco(f->debugname));
 for (int i = 0; i < f->sizek; ++i)
 validateref(g, obj2gco(f), &f->k[i]);
 for (int i = 0; i < f->sizeupvalues; ++i)
 if (f->upvalues[i])
 validateobjref(g, obj2gco(f), obj2gco(f->upvalues[i]));
 for (int i = 0; i < f->sizep; ++i)
 if (f->p[i])
 validateobjref(g, obj2gco(f), obj2gco(f->p[i]));
 for (int i = 0; i < f->sizelocvars; i++)
 if (f->locvars[i].varname)
 validateobjref(g, obj2gco(f), obj2gco(f->locvars[i].varname));
}
static void validateobj(global_State* g, GCObject* o)
{
 if (isdead(g, o))
 {
 LUAU_ASSERT(g->gcstate == GCSsweep);
 return;
 }
 switch (o->gch.tt)
 {
 case LUA_TSTRING:
 break;
 case LUA_TTABLE:
 validatetable(g, gco2h(o));
 break;
 case LUA_TFUNCTION:
 validateclosure(g, gco2cl(o));
 break;
 case LUA_TUSERDATA:
 if (gco2u(o)->metatable)
 validateobjref(g, o, obj2gco(gco2u(o)->metatable));
 break;
 case LUA_TTHREAD:
 validatestack(g, gco2th(o));
 break;
 case LUA_TPROTO:
 validateproto(g, gco2p(o));
 break;
 case LUA_TUPVAL:
 validateref(g, o, gco2uv(o)->v);
 break;
 default:
 LUAU_ASSERT(!"unexpected object type");
 }
}
static void validategraylist(global_State* g, GCObject* o)
{
 if (!keepinvariant(g))
 return;
 while (o)
 {
 LUAU_ASSERT(isgray(o));
 switch (o->gch.tt)
 {
 case LUA_TTABLE:
 o = gco2h(o)->gclist;
 break;
 case LUA_TFUNCTION:
 o = gco2cl(o)->gclist;
 break;
 case LUA_TTHREAD:
 o = gco2th(o)->gclist;
 break;
 case LUA_TPROTO:
 o = gco2p(o)->gclist;
 break;
 default:
 LUAU_ASSERT(!"unknown object in gray list");
 return;
 }
 }
}
static bool validategco(void* context, lua_Page* page, GCObject* gco)
{
 lua_State* L = (lua_State*)context;
 global_State* g = L->global;
 validateobj(g, gco);
 return false;
}
void luaC_validate(lua_State* L)
{
 global_State* g = L->global;
 LUAU_ASSERT(!isdead(g, obj2gco(g->mainthread)));
 checkliveness(g, &g->registry);
 for (int i = 0; i < LUA_T_COUNT; ++i)
 if (g->mt[i])
 LUAU_ASSERT(!isdead(g, obj2gco(g->mt[i])));
 validategraylist(g, g->weak);
 validategraylist(g, g->gray);
 validategraylist(g, g->grayagain);
 validategco(L, NULL, obj2gco(g->mainthread));
 luaM_visitgco(L, L, validategco);
 for (UpVal* uv = g->uvhead.u.open.next; uv != &g->uvhead; uv = uv->u.open.next)
 {
 LUAU_ASSERT(uv->tt == LUA_TUPVAL);
 LUAU_ASSERT(upisopen(uv));
 LUAU_ASSERT(uv->u.open.next->u.open.prev == uv && uv->u.open.prev->u.open.next == uv);
 LUAU_ASSERT(!isblack(obj2gco(uv)));
 }
}
inline bool safejson(char ch)
{
 return unsigned(ch) < 128 && ch >= 32 && ch != '\\' && ch != '\"';
}
static void dumpref(FILE* f, GCObject* o)
{
 fprintf(f, "\"%p\"", o);
}
static void dumprefs(FILE* f, TValue* data, size_t size)
{
 bool first = true;
 for (size_t i = 0; i < size; ++i)
 {
 if (iscollectable(&data[i]))
 {
 if (!first)
 fputc(',', f);
 first = false;
 dumpref(f, gcvalue(&data[i]));
 }
 }
}
static void dumpstringdata(FILE* f, const char* data, size_t len)
{
 for (size_t i = 0; i < len; ++i)
 fputc(safejson(data[i]) ? data[i] : '?', f);
}
static void dumpstring(FILE* f, TString* ts)
{
 fprintf(f, "{\"type\":\"string\",\"cat\":%d,\"size\":%d,\"data\":\"", ts->memcat, int(sizestring(ts->len)));
 dumpstringdata(f, ts->data, ts->len);
 fprintf(f, "\"}");
}
static void dumptable(FILE* f, Table* h)
{
 size_t size = sizeof(Table) + (h->node == &luaH_dummynode ? 0 : sizenode(h) * sizeof(LuaNode)) + h->sizearray * sizeof(TValue);
 fprintf(f, "{\"type\":\"table\",\"cat\":%d,\"size\":%d", h->memcat, int(size));
 if (h->node != &luaH_dummynode)
 {
 fprintf(f, ",\"pairs\":[");
 bool first = true;
 for (int i = 0; i < sizenode(h); ++i)
 {
 const LuaNode& n = h->node[i];
 if (!ttisnil(&n.val) && (iscollectable(&n.key) || iscollectable(&n.val)))
 {
 if (!first)
 fputc(',', f);
 first = false;
 if (iscollectable(&n.key))
 dumpref(f, gcvalue(&n.key));
 else
 fprintf(f, "null");
 fputc(',', f);
 if (iscollectable(&n.val))
 dumpref(f, gcvalue(&n.val));
 else
 fprintf(f, "null");
 }
 }
 fprintf(f, "]");
 }
 if (h->sizearray)
 {
 fprintf(f, ",\"array\":[");
 dumprefs(f, h->array, h->sizearray);
 fprintf(f, "]");
 }
 if (h->metatable)
 {
 fprintf(f, ",\"metatable\":");
 dumpref(f, obj2gco(h->metatable));
 }
 fprintf(f, "}");
}
static void dumpclosure(FILE* f, Closure* cl)
{
 fprintf(f, "{\"type\":\"function\",\"cat\":%d,\"size\":%d", cl->memcat,
 cl->isC ? int(sizeCclosure(cl->nupvalues)) : int(sizeLclosure(cl->nupvalues)));
 fprintf(f, ",\"env\":");
 dumpref(f, obj2gco(cl->env));
 if (cl->isC)
 {
 if (cl->c.debugname)
 fprintf(f, ",\"name\":\"%s\"", cl->c.debugname + 0);
 if (cl->nupvalues)
 {
 fprintf(f, ",\"upvalues\":[");
 dumprefs(f, cl->c.upvals, cl->nupvalues);
 fprintf(f, "]");
 }
 }
 else
 {
 if (cl->l.p->debugname)
 fprintf(f, ",\"name\":\"%s\"", getstr(cl->l.p->debugname));
 fprintf(f, ",\"proto\":");
 dumpref(f, obj2gco(cl->l.p));
 if (cl->nupvalues)
 {
 fprintf(f, ",\"upvalues\":[");
 dumprefs(f, cl->l.uprefs, cl->nupvalues);
 fprintf(f, "]");
 }
 }
 fprintf(f, "}");
}
static void dumpudata(FILE* f, Udata* u)
{
 fprintf(f, "{\"type\":\"userdata\",\"cat\":%d,\"size\":%d,\"tag\":%d", u->memcat, int(sizeudata(u->len)), u->tag);
 if (u->metatable)
 {
 fprintf(f, ",\"metatable\":");
 dumpref(f, obj2gco(u->metatable));
 }
 fprintf(f, "}");
}
static void dumpthread(FILE* f, lua_State* th)
{
 size_t size = sizeof(lua_State) + sizeof(TValue) * th->stacksize + sizeof(CallInfo) * th->size_ci;
 fprintf(f, "{\"type\":\"thread\",\"cat\":%d,\"size\":%d", th->memcat, int(size));
 fprintf(f, ",\"env\":");
 dumpref(f, obj2gco(th->gt));
 Closure* tcl = 0;
 for (CallInfo* ci = th->base_ci; ci <= th->ci; ++ci)
 {
 if (ttisfunction(ci->func))
 {
 tcl = clvalue(ci->func);
 break;
 }
 }
 if (tcl && !tcl->isC && tcl->l.p->source)
 {
 Proto* p = tcl->l.p;
 fprintf(f, ",\"source\":\"");
 dumpstringdata(f, p->source->data, p->source->len);
 fprintf(f, "\",\"line\":%d", p->linedefined);
 }
 if (th->top > th->stack)
 {
 fprintf(f, ",\"stack\":[");
 dumprefs(f, th->stack, th->top - th->stack);
 fprintf(f, "]");
 CallInfo* ci = th->base_ci;
 bool first = true;
 fprintf(f, ",\"stacknames\":[");
 for (StkId v = th->stack; v < th->top; ++v)
 {
 if (!iscollectable(v))
 continue;
 while (ci < th->ci && v >= (ci + 1)->func)
 ci++;
 if (!first)
 fputc(',', f);
 first = false;
 if (v == ci->func)
 {
 Closure* cl = ci_func(ci);
 if (cl->isC)
 {
 fprintf(f, "\"frame:%s\"", cl->c.debugname ? cl->c.debugname : "[C]");
 }
 else
 {
 Proto* p = cl->l.p;
 fprintf(f, "\"frame:");
 if (p->source)
 dumpstringdata(f, p->source->data, p->source->len);
 fprintf(f, ":%d:%s\"", p->linedefined, p->debugname ? getstr(p->debugname) : "");
 }
 }
 else if (isLua(ci))
 {
 Proto* p = ci_func(ci)->l.p;
 int pc = pcRel(ci->savedpc, p);
 const LocVar* var = luaF_findlocal(p, int(v - ci->base), pc);
 if (var && var->varname)
 fprintf(f, "\"%s\"", getstr(var->varname));
 else
 fprintf(f, "null");
 }
 else
 fprintf(f, "null");
 }
 fprintf(f, "]");
 }
 fprintf(f, "}");
}
static void dumpproto(FILE* f, Proto* p)
{
 size_t size = sizeof(Proto) + sizeof(Instruction) * p->sizecode + sizeof(Proto*) * p->sizep + sizeof(TValue) * p->sizek + p->sizelineinfo +
 sizeof(LocVar) * p->sizelocvars + sizeof(TString*) * p->sizeupvalues;
 fprintf(f, "{\"type\":\"proto\",\"cat\":%d,\"size\":%d", p->memcat, int(size));
 if (p->source)
 {
 fprintf(f, ",\"source\":\"");
 dumpstringdata(f, p->source->data, p->source->len);
 fprintf(f, "\",\"line\":%d", p->abslineinfo ? p->abslineinfo[0] : 0);
 }
 if (p->sizek)
 {
 fprintf(f, ",\"constants\":[");
 dumprefs(f, p->k, p->sizek);
 fprintf(f, "]");
 }
 if (p->sizep)
 {
 fprintf(f, ",\"protos\":[");
 for (int i = 0; i < p->sizep; ++i)
 {
 if (i != 0)
 fputc(',', f);
 dumpref(f, obj2gco(p->p[i]));
 }
 fprintf(f, "]");
 }
 fprintf(f, "}");
}
static void dumpupval(FILE* f, UpVal* uv)
{
 fprintf(f, "{\"type\":\"upvalue\",\"cat\":%d,\"size\":%d,\"open\":%s", uv->memcat, int(sizeof(UpVal)), upisopen(uv) ? "true" : "false");
 if (iscollectable(uv->v))
 {
 fprintf(f, ",\"object\":");
 dumpref(f, gcvalue(uv->v));
 }
 fprintf(f, "}");
}
static void dumpobj(FILE* f, GCObject* o)
{
 switch (o->gch.tt)
 {
 case LUA_TSTRING:
 return dumpstring(f, gco2ts(o));
 case LUA_TTABLE:
 return dumptable(f, gco2h(o));
 case LUA_TFUNCTION:
 return dumpclosure(f, gco2cl(o));
 case LUA_TUSERDATA:
 return dumpudata(f, gco2u(o));
 case LUA_TTHREAD:
 return dumpthread(f, gco2th(o));
 case LUA_TPROTO:
 return dumpproto(f, gco2p(o));
 case LUA_TUPVAL:
 return dumpupval(f, gco2uv(o));
 default:
 LUAU_ASSERT(0);
 }
}
static bool dumpgco(void* context, lua_Page* page, GCObject* gco)
{
 FILE* f = (FILE*)context;
 dumpref(f, gco);
 fputc(':', f);
 dumpobj(f, gco);
 fputc(',', f);
 fputc('\n', f);
 return false;
}
void luaC_dump(lua_State* L, void* file, const char* (*categoryName)(lua_State* L, uint8_t memcat))
{
 global_State* g = L->global;
 FILE* f = static_cast<FILE*>(file);
 fprintf(f, "{\"objects\":{\n");
 dumpgco(f, NULL, obj2gco(g->mainthread));
 luaM_visitgco(L, f, dumpgco);
 fprintf(f, "\"0\":{\"type\":\"userdata\",\"cat\":0,\"size\":0}\n"); // to avoid issues with trailing ,
 fprintf(f, "},\"roots\":{\n");
 fprintf(f, "\"mainthread\":");
 dumpref(f, obj2gco(g->mainthread));
 fprintf(f, ",\"registry\":");
 dumpref(f, gcvalue(&g->registry));
 fprintf(f, "},\"stats\":{\n");
 fprintf(f, "\"size\":%d,\n", int(g->totalbytes));
 fprintf(f, "\"categories\":{\n");
 for (int i = 0; i < LUA_MEMORY_CATEGORIES; i++)
 {
 if (size_t bytes = g->memcatbytes[i])
 {
 if (categoryName)
 fprintf(f, "\"%d\":{\"name\":\"%s\", \"size\":%d},\n", i, categoryName(L, i), int(bytes));
 else
 fprintf(f, "\"%d\":{\"size\":%d},\n", i, int(bytes));
 }
 }
 fprintf(f, "\"none\":{}\n"); // to avoid issues with trailing ,
 fprintf(f, "}\n");
 fprintf(f, "}}\n");
}
static const luaL_Reg lualibs[] = {
 {"", luaopen_base},
 {LUA_COLIBNAME, luaopen_coroutine},
 {LUA_TABLIBNAME, luaopen_table},
 {LUA_OSLIBNAME, luaopen_os},
 {LUA_STRLIBNAME, luaopen_string},
 {LUA_MATHLIBNAME, luaopen_math},
 {LUA_DBLIBNAME, luaopen_debug},
 {LUA_UTF8LIBNAME, luaopen_utf8},
 {LUA_BITLIBNAME, luaopen_bit32},
 {NULL, NULL},
};
void luaL_openlibs(lua_State* L)
{
 const luaL_Reg* lib = lualibs;
 for (; lib->func; lib++)
 {
 lua_pushcfunction(L, lib->func, NULL);
 lua_pushstring(L, lib->name);
 lua_call(L, 1, 0);
 }
}
void luaL_sandbox(lua_State* L)
{
 lua_pushnil(L);
 while (lua_next(L, LUA_GLOBALSINDEX) != 0)
 {
 if (lua_istable(L, -1))
 lua_setreadonly(L, -1, true);
 lua_pop(L, 1);
 }
 lua_pushliteral(L, "");
 lua_getmetatable(L, -1);
 lua_setreadonly(L, -1, true);
 lua_pop(L, 2);
 lua_setreadonly(L, LUA_GLOBALSINDEX, true);
 lua_setsafeenv(L, LUA_GLOBALSINDEX, true);
}
void luaL_sandboxthread(lua_State* L)
{
 lua_newtable(L);
 lua_newtable(L);
 lua_pushvalue(L, LUA_GLOBALSINDEX);
 lua_setfield(L, -2, "__index");
 lua_setreadonly(L, -1, true);
 lua_setmetatable(L, -2);
 lua_replace(L, LUA_GLOBALSINDEX);
 lua_setsafeenv(L, LUA_GLOBALSINDEX, true);
}
static void* l_alloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
 (void)ud;
 (void)osize;
 if (nsize == 0)
 {
 free(ptr);
 return NULL;
 }
 else
 return realloc(ptr, nsize);
}
lua_State* luaL_newstate(void)
{
 return lua_newstate(l_alloc, NULL);
}
#include <time.h>
#undef PI
#define PI (3.14159265358979323846)
#define RADIANS_PER_DEGREE (PI / 180.0)
#define PCG32_INC 105
static uint32_t pcg32_random(uint64_t* state)
{
 uint64_t oldstate = *state;
 *state = oldstate * 6364136223846793005ULL + (PCG32_INC | 1);
 uint32_t xorshifted = uint32_t(((oldstate >> 18u) ^ oldstate) >> 27u);
 uint32_t rot = uint32_t(oldstate >> 59u);
 return (xorshifted >> rot) | (xorshifted << ((-int32_t(rot)) & 31));
}
static void pcg32_seed(uint64_t* state, uint64_t seed)
{
 *state = 0;
 pcg32_random(state);
 *state += seed;
 pcg32_random(state);
}
static int math_abs(lua_State* L)
{
 lua_pushnumber(L, fabs(luaL_checknumber(L, 1)));
 return 1;
}
static int math_sin(lua_State* L)
{
 lua_pushnumber(L, sin(luaL_checknumber(L, 1)));
 return 1;
}
static int math_sinh(lua_State* L)
{
 lua_pushnumber(L, sinh(luaL_checknumber(L, 1)));
 return 1;
}
static int math_cos(lua_State* L)
{
 lua_pushnumber(L, cos(luaL_checknumber(L, 1)));
 return 1;
}
static int math_cosh(lua_State* L)
{
 lua_pushnumber(L, cosh(luaL_checknumber(L, 1)));
 return 1;
}
static int math_tan(lua_State* L)
{
 lua_pushnumber(L, tan(luaL_checknumber(L, 1)));
 return 1;
}
static int math_tanh(lua_State* L)
{
 lua_pushnumber(L, tanh(luaL_checknumber(L, 1)));
 return 1;
}
static int math_asin(lua_State* L)
{
 lua_pushnumber(L, asin(luaL_checknumber(L, 1)));
 return 1;
}
static int math_acos(lua_State* L)
{
 lua_pushnumber(L, acos(luaL_checknumber(L, 1)));
 return 1;
}
static int math_atan(lua_State* L)
{
 lua_pushnumber(L, atan(luaL_checknumber(L, 1)));
 return 1;
}
static int math_atan2(lua_State* L)
{
 lua_pushnumber(L, atan2(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
 return 1;
}
static int math_ceil(lua_State* L)
{
 lua_pushnumber(L, ceil(luaL_checknumber(L, 1)));
 return 1;
}
static int math_floor(lua_State* L)
{
 lua_pushnumber(L, floor(luaL_checknumber(L, 1)));
 return 1;
}
static int math_fmod(lua_State* L)
{
 lua_pushnumber(L, fmod(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
 return 1;
}
static int math_modf(lua_State* L)
{
 double ip;
 double fp = modf(luaL_checknumber(L, 1), &ip);
 lua_pushnumber(L, ip);
 lua_pushnumber(L, fp);
 return 2;
}
static int math_sqrt(lua_State* L)
{
 lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
 return 1;
}
static int math_pow(lua_State* L)
{
 lua_pushnumber(L, pow(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
 return 1;
}
static int math_log(lua_State* L)
{
 double x = luaL_checknumber(L, 1);
 double res;
 if (lua_isnoneornil(L, 2))
 res = log(x);
 else
 {
 double base = luaL_checknumber(L, 2);
 if (base == 2.0)
 res = log2(x);
 else if (base == 10.0)
 res = log10(x);
 else
 res = log(x) / log(base);
 }
 lua_pushnumber(L, res);
 return 1;
}
static int math_log10(lua_State* L)
{
 lua_pushnumber(L, log10(luaL_checknumber(L, 1)));
 return 1;
}
static int math_exp(lua_State* L)
{
 lua_pushnumber(L, exp(luaL_checknumber(L, 1)));
 return 1;
}
static int math_deg(lua_State* L)
{
 lua_pushnumber(L, luaL_checknumber(L, 1) / RADIANS_PER_DEGREE);
 return 1;
}
static int math_rad(lua_State* L)
{
 lua_pushnumber(L, luaL_checknumber(L, 1) * RADIANS_PER_DEGREE);
 return 1;
}
static int math_frexp(lua_State* L)
{
 int e;
 lua_pushnumber(L, frexp(luaL_checknumber(L, 1), &e));
 lua_pushinteger(L, e);
 return 2;
}
static int math_ldexp(lua_State* L)
{
 lua_pushnumber(L, ldexp(luaL_checknumber(L, 1), luaL_checkinteger(L, 2)));
 return 1;
}
static int math_min(lua_State* L)
{
 int n = lua_gettop(L);
 double dmin = luaL_checknumber(L, 1);
 int i;
 for (i = 2; i <= n; i++)
 {
 double d = luaL_checknumber(L, i);
 if (d < dmin)
 dmin = d;
 }
 lua_pushnumber(L, dmin);
 return 1;
}
static int math_max(lua_State* L)
{
 int n = lua_gettop(L);
 double dmax = luaL_checknumber(L, 1);
 int i;
 for (i = 2; i <= n; i++)
 {
 double d = luaL_checknumber(L, i);
 if (d > dmax)
 dmax = d;
 }
 lua_pushnumber(L, dmax);
 return 1;
}
static int math_random(lua_State* L)
{
 global_State* g = L->global;
 switch (lua_gettop(L))
 {
 case 0:
 {
 uint32_t rl = pcg32_random(&g->rngstate);
 uint32_t rh = pcg32_random(&g->rngstate);
 double rd = ldexp(double(rl | (uint64_t(rh) << 32)), -64);
 lua_pushnumber(L, rd);
 break;
 }
 case 1:
 {
 int u = luaL_checkinteger(L, 1);
 luaL_argcheck(L, 1 <= u, 1, "interval is empty");
 uint64_t x = uint64_t(u) * pcg32_random(&g->rngstate);
 int r = int(1 + (x >> 32));
 lua_pushinteger(L, r);
 break;
 }
 case 2:
 {
 int l = luaL_checkinteger(L, 1);
 int u = luaL_checkinteger(L, 2);
 luaL_argcheck(L, l <= u, 2, "interval is empty");
 uint32_t ul = uint32_t(u) - uint32_t(l);
 luaL_argcheck(L, ul < UINT_MAX, 2, "interval is too large"); // -INT_MIN..INT_MAX interval can result in integer overflow
 uint64_t x = uint64_t(ul + 1) * pcg32_random(&g->rngstate);
 int r = int(l + (x >> 32));
 lua_pushinteger(L, r);
 break;
 }
 default:
 luaL_error(L, "wrong number of arguments");
 }
 return 1;
}
static int math_randomseed(lua_State* L)
{
 int seed = luaL_checkinteger(L, 1);
 pcg32_seed(&L->global->rngstate, seed);
 return 0;
}
static const unsigned char kPerlin[512] = {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99,
 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174,
 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41,
 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17,
 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110,
 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14,
 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24,
 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74,
 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65,
 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52,
 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213,
 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112,
 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199,
 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61,
 156, 180};
static float fade(float t)
{
 return t * t * t * (t * (t * 6 - 15) + 10);
}
static float math_lerp(float t, float a, float b)
{
 return a + t * (b - a);
}
static float grad(unsigned char hash, float x, float y, float z)
{
 unsigned char h = hash & 15;
 float u = (h < 8) ? x : y;
 float v = (h < 4) ? y : (h == 12 || h == 14) ? x : z;
 return (h & 1 ? -u : u) + (h & 2 ? -v : v);
}
static float perlin(float x, float y, float z)
{
 float xflr = floorf(x);
 float yflr = floorf(y);
 float zflr = floorf(z);
 int xi = int(xflr) & 255;
 int yi = int(yflr) & 255;
 int zi = int(zflr) & 255;
 float xf = x - xflr;
 float yf = y - yflr;
 float zf = z - zflr;
 float u = fade(xf);
 float v = fade(yf);
 float w = fade(zf);
 const unsigned char* p = kPerlin;
 int a = p[xi] + yi;
 int aa = p[a] + zi;
 int ab = p[a + 1] + zi;
 int b = p[xi + 1] + yi;
 int ba = p[b] + zi;
 int bb = p[b + 1] + zi;
 return math_lerp(w,
 math_lerp(v, math_lerp(u, grad(p[aa], xf, yf, zf), grad(p[ba], xf - 1, yf, zf)),
 math_lerp(u, grad(p[ab], xf, yf - 1, zf), grad(p[bb], xf - 1, yf - 1, zf))),
 math_lerp(v, math_lerp(u, grad(p[aa + 1], xf, yf, zf - 1), grad(p[ba + 1], xf - 1, yf, zf - 1)),
 math_lerp(u, grad(p[ab + 1], xf, yf - 1, zf - 1), grad(p[bb + 1], xf - 1, yf - 1, zf - 1))));
}
static int math_noise(lua_State* L)
{
 double x = luaL_checknumber(L, 1);
 double y = luaL_optnumber(L, 2, 0.0);
 double z = luaL_optnumber(L, 3, 0.0);
 double r = perlin((float)x, (float)y, (float)z);
 lua_pushnumber(L, r);
 return 1;
}
static int math_clamp(lua_State* L)
{
 double v = luaL_checknumber(L, 1);
 double min = luaL_checknumber(L, 2);
 double max = luaL_checknumber(L, 3);
 luaL_argcheck(L, min <= max, 3, "max must be greater than or equal to min");
 double r = v < min ? min : v;
 r = r > max ? max : r;
 lua_pushnumber(L, r);
 return 1;
}
static int math_sign(lua_State* L)
{
 double v = luaL_checknumber(L, 1);
 lua_pushnumber(L, v > 0.0 ? 1.0 : v < 0.0 ? -1.0 : 0.0);
 return 1;
}
static int math_round(lua_State* L)
{
 lua_pushnumber(L, round(luaL_checknumber(L, 1)));
 return 1;
}
static const luaL_Reg mathlib[] = {
 {"abs", math_abs},
 {"acos", math_acos},
 {"asin", math_asin},
 {"atan2", math_atan2},
 {"atan", math_atan},
 {"ceil", math_ceil},
 {"cosh", math_cosh},
 {"cos", math_cos},
 {"deg", math_deg},
 {"exp", math_exp},
 {"floor", math_floor},
 {"fmod", math_fmod},
 {"frexp", math_frexp},
 {"ldexp", math_ldexp},
 {"log10", math_log10},
 {"log", math_log},
 {"max", math_max},
 {"min", math_min},
 {"modf", math_modf},
 {"pow", math_pow},
 {"rad", math_rad},
 {"random", math_random},
 {"randomseed", math_randomseed},
 {"sinh", math_sinh},
 {"sin", math_sin},
 {"sqrt", math_sqrt},
 {"tanh", math_tanh},
 {"tan", math_tan},
 {"noise", math_noise},
 {"clamp", math_clamp},
 {"sign", math_sign},
 {"round", math_round},
 {NULL, NULL},
};
int luaopen_math(lua_State* L)
{
 uint64_t seed = uintptr_t(L);
 seed ^= time(NULL);
 seed ^= clock();
 pcg32_seed(&L->global->rngstate, seed);
 luaL_register(L, LUA_MATHLIBNAME, mathlib);
 lua_pushnumber(L, PI);
 lua_setfield(L, -2, "pi");
 lua_pushnumber(L, HUGE_VAL);
 lua_setfield(L, -2, "huge");
 return 1;
}
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#if __has_feature(address_sanitizer) || defined(LUAU_ENABLE_ASAN)
#include <sanitizer/asan_interface.h>
#define ASAN_POISON_MEMORY_REGION(addr, size) __asan_poison_memory_region((addr), (size))
#define ASAN_UNPOISON_MEMORY_REGION(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
#define ASAN_POISON_MEMORY_REGION(addr, size) (void)0
#define ASAN_UNPOISON_MEMORY_REGION(addr, size) (void)0
#endif
#if defined(__APPLE__)
#define ABISWITCH(x64, ms32, gcc32) (sizeof(void*) == 8 ? x64 : gcc32)
#elif defined(__i386__) && !defined(_MSC_VER)
#define ABISWITCH(x64, ms32, gcc32) (gcc32)
#else
#define ABISWITCH(x64, ms32, gcc32) (sizeof(void*) == 8 ? x64 : ms32)
#endif
#if LUA_VECTOR_SIZE == 4
static_assert(sizeof(TValue) == ABISWITCH(24, 24, 24), "size mismatch for value");
static_assert(sizeof(LuaNode) == ABISWITCH(48, 48, 48), "size mismatch for table entry");
#else
static_assert(sizeof(TValue) == ABISWITCH(16, 16, 16), "size mismatch for value");
static_assert(sizeof(LuaNode) == ABISWITCH(32, 32, 32), "size mismatch for table entry");
#endif
static_assert(offsetof(TString, data) == ABISWITCH(24, 20, 20), "size mismatch for string header");
static_assert(offsetof(Udata, data) == ABISWITCH(16, 16, 12), "size mismatch for userdata header");
static_assert(sizeof(Table) == ABISWITCH(48, 32, 32), "size mismatch for table header");
const size_t kSizeClasses = LUA_SIZECLASSES;
const size_t kMaxSmallSize = 512;
const size_t kPageSize = 16 * 1024 - 24;
const size_t kBlockHeader = sizeof(double) > sizeof(void*) ? sizeof(double) : sizeof(void*);
const size_t kGCOLinkOffset = (sizeof(GCheader) + sizeof(void*) - 1) & ~(sizeof(void*) - 1); // GCO pages contain freelist links after the GC header
struct SizeClassConfig
{
 int sizeOfClass[kSizeClasses];
 int8_t classForSize[kMaxSmallSize + 1];
 int classCount = 0;
 SizeClassConfig()
 {
 memset(sizeOfClass, 0, sizeof(sizeOfClass));
 memset(classForSize, -1, sizeof(classForSize));
 for (int size = 8; size < 64; size += 8)
 sizeOfClass[classCount++] = size;
 for (int size = 64; size < 256; size += 16)
 sizeOfClass[classCount++] = size;
 for (int size = 256; size <= 512; size += 32)
 sizeOfClass[classCount++] = size;
 LUAU_ASSERT(size_t(classCount) <= kSizeClasses);
 for (int klass = 0; klass < classCount; ++klass)
 classForSize[sizeOfClass[klass]] = int8_t(klass);
 for (int size = kMaxSmallSize - 1; size >= 0; --size)
 if (classForSize[size] < 0)
 classForSize[size] = classForSize[size + 1];
 }
};
const SizeClassConfig kSizeClassConfig;
#define sizeclass(sz) (size_t((sz)-1) < kMaxSmallSize ? kSizeClassConfig.classForSize[sz] : -1)
#define metadata(block) (*(void**)(block))
#define freegcolink(block) (*(void**)((char*)block + kGCOLinkOffset))
struct lua_Page
{
 lua_Page* prev;
 lua_Page* next;
 lua_Page* gcolistprev;
 lua_Page* gcolistnext;
 int pageSize;
 int blockSize; // block size in bytes, including block header (for non-GCO)
 void* freeList;
 int freeNext; // next free block offset in this page, in bytes; when negative, freeList is used instead
 int busyBlocks;
 union
 {
 char data[1];
 double align1;
 void* align2;
 };
};
l_noret luaM_toobig(lua_State* L)
{
 luaG_runerror(L, "memory allocation error: block too big");
}
static lua_Page* newpage(lua_State* L, lua_Page** gcopageset, int pageSize, int blockSize, int blockCount)
{
 global_State* g = L->global;
 LUAU_ASSERT(pageSize - int(offsetof(lua_Page, data)) >= blockSize * blockCount);
 lua_Page* page = (lua_Page*)(*g->frealloc)(g->ud, NULL, 0, pageSize);
 if (!page)
 luaD_throw(L, LUA_ERRMEM);
 ASAN_POISON_MEMORY_REGION(page->data, blockSize * blockCount);
 page->prev = NULL;
 page->next = NULL;
 page->gcolistprev = NULL;
 page->gcolistnext = NULL;
 page->pageSize = pageSize;
 page->blockSize = blockSize;
 page->freeList = NULL;
 page->freeNext = (blockCount - 1) * blockSize;
 page->busyBlocks = 0;
 if (gcopageset)
 {
 page->gcolistnext = *gcopageset;
 if (page->gcolistnext)
 page->gcolistnext->gcolistprev = page;
 *gcopageset = page;
 }
 return page;
}
static lua_Page* newclasspage(lua_State* L, lua_Page** freepageset, lua_Page** gcopageset, uint8_t sizeClass, bool storeMetadata)
{
 int blockSize = kSizeClassConfig.sizeOfClass[sizeClass] + (storeMetadata ? kBlockHeader : 0);
 int blockCount = (kPageSize - offsetof(lua_Page, data)) / blockSize;
 lua_Page* page = newpage(L, gcopageset, kPageSize, blockSize, blockCount);
 LUAU_ASSERT(!freepageset[sizeClass]);
 freepageset[sizeClass] = page;
 return page;
}
static void freepage(lua_State* L, lua_Page** gcopageset, lua_Page* page)
{
 global_State* g = L->global;
 if (gcopageset)
 {
 if (page->gcolistnext)
 page->gcolistnext->gcolistprev = page->gcolistprev;
 if (page->gcolistprev)
 page->gcolistprev->gcolistnext = page->gcolistnext;
 else if (*gcopageset == page)
 *gcopageset = page->gcolistnext;
 }
 (*g->frealloc)(g->ud, page, page->pageSize, 0);
}
static void freeclasspage(lua_State* L, lua_Page** freepageset, lua_Page** gcopageset, lua_Page* page, uint8_t sizeClass)
{
 if (page->next)
 page->next->prev = page->prev;
 if (page->prev)
 page->prev->next = page->next;
 else if (freepageset[sizeClass] == page)
 freepageset[sizeClass] = page->next;
 freepage(L, gcopageset, page);
}
static void* newblock(lua_State* L, int sizeClass)
{
 global_State* g = L->global;
 lua_Page* page = g->freepages[sizeClass];
 if (!page)
 page = newclasspage(L, g->freepages, NULL, sizeClass, true);
 LUAU_ASSERT(!page->prev);
 LUAU_ASSERT(page->freeList || page->freeNext >= 0);
 LUAU_ASSERT(size_t(page->blockSize) == kSizeClassConfig.sizeOfClass[sizeClass] + kBlockHeader);
 void* block;
 if (page->freeNext >= 0)
 {
 block = &page->data + page->freeNext;
 ASAN_UNPOISON_MEMORY_REGION(block, page->blockSize);
 page->freeNext -= page->blockSize;
 page->busyBlocks++;
 }
 else
 {
 block = page->freeList;
 ASAN_UNPOISON_MEMORY_REGION(block, page->blockSize);
 page->freeList = metadata(block);
 page->busyBlocks++;
 }
 metadata(block) = page;
 if (!page->freeList && page->freeNext < 0)
 {
 g->freepages[sizeClass] = page->next;
 if (page->next)
 page->next->prev = NULL;
 page->next = NULL;
 }
 return (char*)block + kBlockHeader;
}
static void* newgcoblock(lua_State* L, int sizeClass)
{
 global_State* g = L->global;
 lua_Page* page = g->freegcopages[sizeClass];
 if (!page)
 page = newclasspage(L, g->freegcopages, &g->allgcopages, sizeClass, false);
 LUAU_ASSERT(!page->prev);
 LUAU_ASSERT(page->freeList || page->freeNext >= 0);
 LUAU_ASSERT(page->blockSize == kSizeClassConfig.sizeOfClass[sizeClass]);
 void* block;
 if (page->freeNext >= 0)
 {
 block = &page->data + page->freeNext;
 ASAN_UNPOISON_MEMORY_REGION(block, page->blockSize);
 page->freeNext -= page->blockSize;
 page->busyBlocks++;
 }
 else
 {
 block = page->freeList;
 ASAN_UNPOISON_MEMORY_REGION((char*)block + sizeof(GCheader), page->blockSize - sizeof(GCheader));
 page->freeList = freegcolink(block);
 page->busyBlocks++;
 }
 if (!page->freeList && page->freeNext < 0)
 {
 g->freegcopages[sizeClass] = page->next;
 if (page->next)
 page->next->prev = NULL;
 page->next = NULL;
 }
 return block;
}
static void freeblock(lua_State* L, int sizeClass, void* block)
{
 global_State* g = L->global;
 LUAU_ASSERT(block);
 block = (char*)block - kBlockHeader;
 lua_Page* page = (lua_Page*)metadata(block);
 LUAU_ASSERT(page && page->busyBlocks > 0);
 LUAU_ASSERT(size_t(page->blockSize) == kSizeClassConfig.sizeOfClass[sizeClass] + kBlockHeader);
 LUAU_ASSERT(block >= page->data && block < (char*)page + page->pageSize);
 if (!page->freeList && page->freeNext < 0)
 {
 LUAU_ASSERT(!page->prev);
 LUAU_ASSERT(!page->next);
 page->next = g->freepages[sizeClass];
 if (page->next)
 page->next->prev = page;
 g->freepages[sizeClass] = page;
 }
 metadata(block) = page->freeList;
 page->freeList = block;
 ASAN_POISON_MEMORY_REGION(block, page->blockSize);
 page->busyBlocks--;
 if (page->busyBlocks == 0)
 freeclasspage(L, g->freepages, NULL, page, sizeClass);
}
static void freegcoblock(lua_State* L, int sizeClass, void* block, lua_Page* page)
{
 LUAU_ASSERT(page && page->busyBlocks > 0);
 LUAU_ASSERT(page->blockSize == kSizeClassConfig.sizeOfClass[sizeClass]);
 LUAU_ASSERT(block >= page->data && block < (char*)page + page->pageSize);
 global_State* g = L->global;
 if (!page->freeList && page->freeNext < 0)
 {
 LUAU_ASSERT(!page->prev);
 LUAU_ASSERT(!page->next);
 page->next = g->freegcopages[sizeClass];
 if (page->next)
 page->next->prev = page;
 g->freegcopages[sizeClass] = page;
 }
 freegcolink(block) = page->freeList;
 page->freeList = block;
 ASAN_POISON_MEMORY_REGION((char*)block + sizeof(GCheader), page->blockSize - sizeof(GCheader));
 page->busyBlocks--;
 if (page->busyBlocks == 0)
 freeclasspage(L, g->freegcopages, &g->allgcopages, page, sizeClass);
}
void* luaM_new_(lua_State* L, size_t nsize, uint8_t memcat)
{
 global_State* g = L->global;
 int nclass = sizeclass(nsize);
 void* block = nclass >= 0 ? newblock(L, nclass) : (*g->frealloc)(g->ud, NULL, 0, nsize);
 if (block == NULL && nsize > 0)
 luaD_throw(L, LUA_ERRMEM);
 g->totalbytes += nsize;
 g->memcatbytes[memcat] += nsize;
 return block;
}
GCObject* luaM_newgco_(lua_State* L, size_t nsize, uint8_t memcat)
{
 LUAU_ASSERT(nsize >= kGCOLinkOffset + sizeof(void*));
 global_State* g = L->global;
 int nclass = sizeclass(nsize);
 void* block = NULL;
 if (nclass >= 0)
 {
 block = newgcoblock(L, nclass);
 }
 else
 {
 lua_Page* page = newpage(L, &g->allgcopages, offsetof(lua_Page, data) + int(nsize), int(nsize), 1);
 block = &page->data;
 ASAN_UNPOISON_MEMORY_REGION(block, page->blockSize);
 page->freeNext -= page->blockSize;
 page->busyBlocks++;
 }
 if (block == NULL && nsize > 0)
 luaD_throw(L, LUA_ERRMEM);
 g->totalbytes += nsize;
 g->memcatbytes[memcat] += nsize;
 return (GCObject*)block;
}
void luaM_free_(lua_State* L, void* block, size_t osize, uint8_t memcat)
{
 global_State* g = L->global;
 LUAU_ASSERT((osize == 0) == (block == NULL));
 int oclass = sizeclass(osize);
 if (oclass >= 0)
 freeblock(L, oclass, block);
 else
 (*g->frealloc)(g->ud, block, osize, 0);
 g->totalbytes -= osize;
 g->memcatbytes[memcat] -= osize;
}
void luaM_freegco_(lua_State* L, GCObject* block, size_t osize, uint8_t memcat, lua_Page* page)
{
 global_State* g = L->global;
 LUAU_ASSERT((osize == 0) == (block == NULL));
 int oclass = sizeclass(osize);
 if (oclass >= 0)
 {
 block->gch.tt = LUA_TNIL;
 freegcoblock(L, oclass, block, page);
 }
 else
 {
 LUAU_ASSERT(page->busyBlocks == 1);
 LUAU_ASSERT(size_t(page->blockSize) == osize);
 LUAU_ASSERT((void*)block == page->data);
 freepage(L, &g->allgcopages, page);
 }
 g->totalbytes -= osize;
 g->memcatbytes[memcat] -= osize;
}
void* luaM_realloc_(lua_State* L, void* block, size_t osize, size_t nsize, uint8_t memcat)
{
 global_State* g = L->global;
 LUAU_ASSERT((osize == 0) == (block == NULL));
 int nclass = sizeclass(nsize);
 int oclass = sizeclass(osize);
 void* result;
 if (nclass >= 0 || oclass >= 0)
 {
 result = nclass >= 0 ? newblock(L, nclass) : (*g->frealloc)(g->ud, NULL, 0, nsize);
 if (result == NULL && nsize > 0)
 luaD_throw(L, LUA_ERRMEM);
 if (osize > 0 && nsize > 0)
 memcpy(result, block, osize < nsize ? osize : nsize);
 if (oclass >= 0)
 freeblock(L, oclass, block);
 else
 (*g->frealloc)(g->ud, block, osize, 0);
 }
 else
 {
 result = (*g->frealloc)(g->ud, block, osize, nsize);
 if (result == NULL && nsize > 0)
 luaD_throw(L, LUA_ERRMEM);
 }
 LUAU_ASSERT((nsize == 0) == (result == NULL));
 g->totalbytes = (g->totalbytes - osize) + nsize;
 g->memcatbytes[memcat] += nsize - osize;
 return result;
}
void luaM_getpagewalkinfo(lua_Page* page, char** start, char** end, int* busyBlocks, int* blockSize)
{
 int blockCount = (page->pageSize - offsetof(lua_Page, data)) / page->blockSize;
 LUAU_ASSERT(page->freeNext >= -page->blockSize && page->freeNext <= (blockCount - 1) * page->blockSize);
 char* data = page->data;
 *start = data + page->freeNext + page->blockSize;
 *end = data + blockCount * page->blockSize;
 *busyBlocks = page->busyBlocks;
 *blockSize = page->blockSize;
}
lua_Page* luaM_getnextgcopage(lua_Page* page)
{
 return page->gcolistnext;
}
void luaM_visitpage(lua_Page* page, void* context, bool (*visitor)(void* context, lua_Page* page, GCObject* gco))
{
 char* start;
 char* end;
 int busyBlocks;
 int blockSize;
 luaM_getpagewalkinfo(page, &start, &end, &busyBlocks, &blockSize);
 for (char* pos = start; pos != end; pos += blockSize)
 {
 GCObject* gco = (GCObject*)pos;
 if (gco->gch.tt == LUA_TNIL)
 continue;
 if (visitor(context, page, gco))
 {
 LUAU_ASSERT(busyBlocks > 0);
 if (--busyBlocks == 0)
 break;
 }
 }
}
void luaM_visitgco(lua_State* L, void* context, bool (*visitor)(void* context, lua_Page* page, GCObject* gco))
{
 global_State* g = L->global;
 for (lua_Page* curr = g->allgcopages; curr;)
 {
 lua_Page* next = curr->gcolistnext;
 luaM_visitpage(curr, context, visitor);
 curr = next;
 }
}
#ifdef _MSC_VER
#endif
static const int kPow10TableMin = -292;
static const int kPow10TableMax = 324;
static const uint64_t kPow5Table[16] = {
 0x8000000000000000, 0xa000000000000000, 0xc800000000000000, 0xfa00000000000000, 0x9c40000000000000, 0xc350000000000000,
 0xf424000000000000, 0x9896800000000000, 0xbebc200000000000, 0xee6b280000000000, 0x9502f90000000000, 0xba43b74000000000,
 0xe8d4a51000000000, 0x9184e72a00000000, 0xb5e620f480000000, 0xe35fa931a0000000,
};
static const uint64_t kPow10Table[(kPow10TableMax - kPow10TableMin + 1 + 15) / 16][3] = {
 {0xff77b1fcbebcdc4f, 0x25e8e89c13bb0f7b, 0x333443443333443b}, {0x8dd01fad907ffc3b, 0xae3da7d97f6792e4, 0xbbb3ab3cb3ba3cbc},
 {0x9d71ac8fada6c9b5, 0x6f773fc3603db4aa, 0x4ba4bc4bb4bb4bcc}, {0xaecc49914078536d, 0x58fae9f773886e19, 0x3ba3bc33b43b43bb},
 {0xc21094364dfb5636, 0x985915fc12f542e5, 0x33b43b43a33b33cb}, {0xd77485cb25823ac7, 0x7d633293366b828c, 0x34b44c444343443c},
 {0xef340a98172aace4, 0x86fb897116c87c35, 0x333343333343334b}, {0x84c8d4dfd2c63f3b, 0x29ecd9f40041e074, 0xccaccbbcbcbb4bbc},
 {0x936b9fcebb25c995, 0xcab10dd900beec35, 0x3ab3ab3ab3bb3bbb}, {0xa3ab66580d5fdaf5, 0xc13e60d0d2e0ebbb, 0x4cc3dc4db4db4dbb},
 {0xb5b5ada8aaff80b8, 0x0d819992132456bb, 0x33b33a34c33b34ab}, {0xc9bcff6034c13052, 0xfc89b393dd02f0b6, 0x33c33b44b43c34bc},
 {0xdff9772470297ebd, 0x59787e2b93bc56f8, 0x43b444444443434c}, {0xf8a95fcf88747d94, 0x75a44c6397ce912b, 0x443334343443343b},
 {0x8a08f0f8bf0f156b, 0x1b8e9ecb641b5900, 0xbbabab3aa3ab4ccc}, {0x993fe2c6d07b7fab, 0xe546a8038efe402a, 0x4cb4bc4db4db4bcc},
 {0xaa242499697392d2, 0xdde50bd1d5d0b9ea, 0x3ba3ba3bb33b33bc}, {0xbce5086492111aea, 0x88f4bb1ca6bcf585, 0x44b44c44c44c43cb},
 {0xd1b71758e219652b, 0xd3c36113404ea4a9, 0x44c44c44c444443b}, {0xe8d4a51000000000, 0x0000000000000000, 0x444444444444444c},
 {0x813f3978f8940984, 0x4000000000000000, 0xcccccccccccccccc}, {0x8f7e32ce7bea5c6f, 0xe4820023a2000000, 0xbba3bc4cc4cc4ccc},
 {0x9f4f2726179a2245, 0x01d762422c946591, 0x4aa3bb3aa3ba3bab}, {0xb0de65388cc8ada8, 0x3b25a55f43294bcc, 0x3ca33b33b44b43bc},
 {0xc45d1df942711d9a, 0x3ba5d0bd324f8395, 0x44c44c34c44b44cb}, {0xda01ee641a708de9, 0xe80e6f4820cc9496, 0x33b33b343333333c},
 {0xf209787bb47d6b84, 0xc0678c5dbd23a49b, 0x443444444443443b}, {0x865b86925b9bc5c2, 0x0b8a2392ba45a9b3, 0xdbccbcccb4cb3bbb},
 {0x952ab45cfa97a0b2, 0xdd945a747bf26184, 0x3bc4bb4ab3ca3cbc}, {0xa59bc234db398c25, 0x43fab9837e699096, 0x3bb3ac3ab3bb33ac},
 {0xb7dcbf5354e9bece, 0x0c11ed6d538aeb30, 0x33b43b43b34c34dc}, {0xcc20ce9bd35c78a5, 0x31ec038df7b441f5, 0x34c44c43c44b44cb},
 {0xe2a0b5dc971f303a, 0x2e44ae64840fd61e, 0x333333333333333c}, {0xfb9b7cd9a4a7443c, 0x169840ef017da3b2, 0x433344443333344c},
 {0x8bab8eefb6409c1a, 0x1ad089b6c2f7548f, 0xdcbdcc3cc4cc4bcb}, {0x9b10a4e5e9913128, 0xca7cf2b4191c8327, 0x3ab3cb3bc3bb4bbb},
 {0xac2820d9623bf429, 0x546345fa9fbdcd45, 0x3bb3cc43c43c43cb}, {0xbf21e44003acdd2c, 0xe0470a63e6bd56c4, 0x44b34a43b44c44bc},
 {0xd433179d9c8cb841, 0x5fa60692a46151ec, 0x43a33a33a333333c},
};
static const char kDigitTable[] = "0001020304050607080910111213141516171819202122232425262728293031323334353637383940414243444546474849"
 "5051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899";
inline uint64_t mul128(uint64_t x, uint64_t y, uint64_t* hi)
{
#if defined(_MSC_VER) && defined(_M_X64)
 return _umul128(x, y, hi);
#elif defined(__SIZEOF_INT128__)
 unsigned __int128 r = x;
 r *= y;
 *hi = uint64_t(r >> 64);
 return uint64_t(r);
#else
 uint32_t x0 = uint32_t(x), x1 = uint32_t(x >> 32);
 uint32_t y0 = uint32_t(y), y1 = uint32_t(y >> 32);
 uint64_t p11 = uint64_t(x1) * y1, p01 = uint64_t(x0) * y1;
 uint64_t p10 = uint64_t(x1) * y0, p00 = uint64_t(x0) * y0;
 uint64_t mid = p10 + (p00 >> 32) + uint32_t(p01);
 uint64_t r0 = (mid << 32) | uint32_t(p00);
 uint64_t r1 = p11 + (mid >> 32) + (p01 >> 32);
 *hi = r1;
 return r0;
#endif
}
inline uint64_t mul192hi(uint64_t xhi, uint64_t xlo, uint64_t y, uint64_t* hi)
{
 uint64_t z2;
 uint64_t z1 = mul128(xhi, y, &z2);
 uint64_t z1c;
 uint64_t z0 = mul128(xlo, y, &z1c);
 (void)z0;
 z1 += z1c;
 z2 += (z1 < z1c);
 *hi = z2;
 return z1;
}
inline uint64_t roundodd(uint64_t ghi, uint64_t glo, uint64_t cp)
{
 uint64_t xhi;
 uint64_t xlo = mul128(glo, cp, &xhi);
 (void)xlo;
 uint64_t yhi;
 uint64_t ylo = mul128(ghi, cp, &yhi);
 uint64_t z = ylo + xhi;
 return (yhi + (z < xhi)) | (z > 1);
}
struct Decimal
{
 uint64_t s;
 int k;
};
static Decimal schubfach(int exponent, uint64_t fraction)
{
 uint64_t c = fraction;
 int q = exponent - 1023 - 51;
 if (exponent != 0)
 {
 c |= (1ull << 52);
 q--;
 }
 if (unsigned(-q) < 53 && (c & ((1ull << (-q)) - 1)) == 0)
 return {c >> (-q), 0};
 int irr = (c == (1ull << 52) && q != -1074); // Qmin
 int out = int(c & 1);
 uint64_t cbl = 4 * c - 2 + irr;
 uint64_t cb = 4 * c;
 uint64_t cbr = 4 * c + 2;
 const int Q = 20;
 const int C = 315652;
 const int A = -131008; // floor(2^Q * log10(3/4))
 const int C2 = 3483294;
 int k = (q * C + (irr ? A : 0)) >> Q;
 int h = q + ((-k * C2) >> Q) + 1;
 LUAU_ASSERT(-k >= kPow10TableMin && -k <= kPow10TableMax);
 int gtoff = -k - kPow10TableMin;
 const uint64_t* gt = kPow10Table[gtoff >> 4];
 uint64_t ghi;
 uint64_t glo = mul192hi(gt[0], gt[1], kPow5Table[gtoff & 15], &ghi);
 int gterr = (gt[2] >> ((gtoff & 15) * 4)) & 15;
 int gtscale = gterr >> 3;
 ghi <<= gtscale;
 ghi += (glo >> 63) & gtscale;
 glo <<= gtscale;
 glo -= (gterr & 7) - 4;
 uint64_t vbl = roundodd(ghi, glo, cbl << h);
 uint64_t vb = roundodd(ghi, glo, cb << h);
 uint64_t vbr = roundodd(ghi, glo, cbr << h);
 uint64_t s = vb / 4;
 if (s >= 10)
 {
 uint64_t sp = s / 10;
 bool upin = vbl + out <= 40 * sp;
 bool wpin = vbr >= 40 * sp + 40 + out;
 if (upin != wpin)
 return {sp + wpin, k + 1};
 }
 bool uin = vbl + out <= 4 * s;
 bool win = 4 * s + 4 + out <= vbr;
 bool rup = vb >= 4 * s + 2 + 1 - (s & 1);
 return {s + (uin != win ? win : rup), k};
}
static char* printspecial(char* buf, int sign, uint64_t fraction)
{
 if (fraction == 0)
 {
 memcpy(buf, ("-inf") + (1 - sign), 4);
 return buf + 3 + sign;
 }
 else
 {
 memcpy(buf, "nan", 4);
 return buf + 3;
 }
}
static char* printunsignedrev(char* end, uint64_t num)
{
 while (num >= 10000)
 {
 unsigned int tail = unsigned(num % 10000);
 memcpy(end - 4, &kDigitTable[int(tail / 100) * 2], 2);
 memcpy(end - 2, &kDigitTable[int(tail % 100) * 2], 2);
 num /= 10000;
 end -= 4;
 }
 unsigned int rest = unsigned(num);
 while (rest >= 10)
 {
 memcpy(end - 2, &kDigitTable[int(rest % 100) * 2], 2);
 rest /= 100;
 end -= 2;
 }
 if (rest)
 {
 end[-1] = '0' + int(rest);
 end -= 1;
 }
 return end;
}
static char* printexp(char* buf, int num)
{
 *buf++ = 'e';
 *buf++ = num < 0 ? '-' : '+';
 int v = num < 0 ? -num : num;
 if (v >= 100)
 {
 *buf++ = '0' + (v / 100);
 v %= 100;
 }
 memcpy(buf, &kDigitTable[v * 2], 2);
 return buf + 2;
}
inline char* trimzero(char* end)
{
 while (end[-1] == '0')
 end--;
 return end;
}
#define fastmemcpy(dst, src, size, sizefast) check_exp((size) <= sizefast, memcpy(dst, src, sizefast))
#define fastmemset(dst, val, size, sizefast) check_exp((size) <= sizefast, memset(dst, val, sizefast))
char* luai_num2str(char* buf, double n)
{
 union
 {
 double v;
 uint64_t bits;
 } v = {n};
 int sign = int(v.bits >> 63);
 int exponent = int(v.bits >> 52) & 2047;
 uint64_t fraction = v.bits & ((1ull << 52) - 1);
 if (LUAU_UNLIKELY(exponent == 0x7ff))
 return printspecial(buf, sign, fraction);
 *buf = '-';
 buf += sign;
 if (exponent == 0 && fraction == 0)
 {
 buf[0] = '0';
 return buf + 1;
 }
 Decimal d = schubfach(exponent, fraction);
 LUAU_ASSERT(d.s < uint64_t(1e17));
 char decbuf[40];
 char* decend = decbuf + 20;
 char* dec = printunsignedrev(decend, d.s);
 int declen = int(decend - dec);
 LUAU_ASSERT(declen <= 17);
 int dot = declen + d.k;
 if (dot >= -5 && dot <= 21)
 {
 if (dot <= 0)
 {
 buf[0] = '0';
 buf[1] = '.';
 fastmemset(buf + 2, '0', -dot, 5);
 fastmemcpy(buf + 2 + (-dot), dec, declen, 17);
 return trimzero(buf + 2 + (-dot) + declen);
 }
 else if (dot == declen)
 {
 fastmemcpy(buf, dec, dot, 17);
 return buf + dot;
 }
 else if (dot < declen)
 {
 fastmemcpy(buf, dec, dot, 16);
 buf[dot] = '.';
 fastmemcpy(buf + dot + 1, dec + dot, declen - dot, 16);
 return trimzero(buf + declen + 1);
 }
 else
 {
 fastmemcpy(buf, dec, declen, 17);
 fastmemset(buf + declen, '0', dot - declen, 8);
 return buf + dot;
 }
 }
 else
 {
 buf[0] = dec[0];
 buf[1] = '.';
 fastmemcpy(buf + 2, dec + 1, declen - 1, 16);
 char* exp = trimzero(buf + declen + 1);
 return printexp(exp, dot - 1);
 }
}
const TValue luaO_nilobject_ = {{NULL}, {0}, LUA_TNIL};
int luaO_log2(unsigned int x)
{
 static const uint8_t log_2[256] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6,
 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
 int l = -1;
 while (x >= 256)
 {
 l += 8;
 x >>= 8;
 }
 return l + log_2[x];
}
int luaO_rawequalObj(const TValue* t1, const TValue* t2)
{
 if (ttype(t1) != ttype(t2))
 return 0;
 else
 switch (ttype(t1))
 {
 case LUA_TNIL:
 return 1;
 case LUA_TNUMBER:
 return luai_numeq(nvalue(t1), nvalue(t2));
 case LUA_TVECTOR:
 return luai_veceq(vvalue(t1), vvalue(t2));
 case LUA_TBOOLEAN:
 return bvalue(t1) == bvalue(t2);
 case LUA_TLIGHTUSERDATA:
 return pvalue(t1) == pvalue(t2);
 default:
 LUAU_ASSERT(iscollectable(t1));
 return gcvalue(t1) == gcvalue(t2);
 }
}
int luaO_rawequalKey(const TKey* t1, const TValue* t2)
{
 if (ttype(t1) != ttype(t2))
 return 0;
 else
 switch (ttype(t1))
 {
 case LUA_TNIL:
 return 1;
 case LUA_TNUMBER:
 return luai_numeq(nvalue(t1), nvalue(t2));
 case LUA_TVECTOR:
 return luai_veceq(vvalue(t1), vvalue(t2));
 case LUA_TBOOLEAN:
 return bvalue(t1) == bvalue(t2);
 case LUA_TLIGHTUSERDATA:
 return pvalue(t1) == pvalue(t2);
 default:
 LUAU_ASSERT(iscollectable(t1));
 return gcvalue(t1) == gcvalue(t2);
 }
}
int luaO_str2d(const char* s, double* result)
{
 char* endptr;
 *result = luai_str2num(s, &endptr);
 if (endptr == s)
 return 0;
 if (*endptr == 'x' || *endptr == 'X') // maybe an hexadecimal constant?
 *result = cast_num(strtoul(s, &endptr, 16));
 if (*endptr == '\0')
 return 1;
 while (isspace(cast_to(unsigned char, *endptr)))
 endptr++;
 if (*endptr != '\0')
 return 0;
 return 1;
}
const char* luaO_pushvfstring(lua_State* L, const char* fmt, va_list argp)
{
 char result[LUA_BUFFERSIZE];
 vsnprintf(result, sizeof(result), fmt, argp);
 setsvalue(L, L->top, luaS_new(L, result));
 incr_top(L);
 return svalue(L->top - 1);
}
const char* luaO_pushfstring(lua_State* L, const char* fmt, ...)
{
 const char* msg;
 va_list argp;
 va_start(argp, fmt);
 msg = luaO_pushvfstring(L, fmt, argp);
 va_end(argp);
 return msg;
}
const char* luaO_chunkid(char* buf, size_t buflen, const char* source, size_t srclen)
{
 if (*source == '=')
 {
 if (srclen <= buflen)
 return source + 1;
 memcpy(buf, source + 1, buflen - 1);
 buf[buflen - 1] = '\0';
 }
 else if (*source == '@')
 {
 if (srclen <= buflen)
 return source + 1;
 memcpy(buf, "...", 3);
 memcpy(buf + 3, source + srclen - (buflen - 4), buflen - 4);
 buf[buflen - 1] = '\0';
 }
 else
 {
 size_t len = strcspn(source, "\n\r"); // stop at first newline
 buflen -= sizeof("[string \"...\"]");
 if (len > buflen)
 len = buflen;
 strcpy(buf, "[string \"");
 if (source[len] != '\0')
 {
 strncat(buf, source, len);
 strcat(buf, "...");
 }
 else
 strcat(buf, source);
 strcat(buf, "\"]");
 }
 return buf;
}
#define LUA_STRFTIMEOPTIONS "aAbBcdHIjmMpSUwWxXyYzZ%"
#if defined(_WIN32)
static tm* gmtime_r(const time_t* timep, tm* result)
{
 return gmtime_s(result, timep) == 0 ? result : NULL;
}
static tm* localtime_r(const time_t* timep, tm* result)
{
 return localtime_s(result, timep) == 0 ? result : NULL;
}
static time_t timegm(struct tm* timep)
{
 return _mkgmtime(timep);
}
#elif defined(__FreeBSD__)
static tm* gmtime_r(const time_t* timep, tm* result)
{
 return gmtime_s(timep, result) == 0 ? result : NULL;
}
static tm* localtime_r(const time_t* timep, tm* result)
{
 return localtime_s(timep, result) == 0 ? result : NULL;
}
static time_t timegm(struct tm* timep)
{
 return mktime(timep);
}
#endif
static int os_clock(lua_State* L)
{
 lua_pushnumber(L, lua_clock());
 return 1;
}
static void setfield(lua_State* L, const char* key, int value)
{
 lua_pushinteger(L, value);
 lua_setfield(L, -2, key);
}
static void setboolfield(lua_State* L, const char* key, int value)
{
 if (value < 0)
 return; // does not set field
 lua_pushboolean(L, value);
 lua_setfield(L, -2, key);
}
static int getboolfield(lua_State* L, const char* key)
{
 int res;
 lua_rawgetfield(L, -1, key);
 res = lua_isnil(L, -1) ? -1 : lua_toboolean(L, -1);
 lua_pop(L, 1);
 return res;
}
static int getfield(lua_State* L, const char* key, int d)
{
 int res;
 lua_rawgetfield(L, -1, key);
 if (lua_isnumber(L, -1))
 res = (int)lua_tointeger(L, -1);
 else
 {
 if (d < 0)
 luaL_error(L, "field '%s' missing in date table", key);
 res = d;
 }
 lua_pop(L, 1);
 return res;
}
static int os_date(lua_State* L)
{
 const char* s = luaL_optstring(L, 1, "%c");
 time_t t = luaL_opt(L, (time_t)luaL_checknumber, 2, time(NULL));
 struct tm tm;
 struct tm* stm;
 if (*s == '!')
 {
 stm = gmtime_r(&t, &tm);
 s++;
 }
 else
 {
 stm = t < 0 ? NULL : localtime_r(&t, &tm);
 }
 if (stm == NULL)
 {
 lua_pushnil(L);
 }
 else if (strcmp(s, "*t") == 0)
 {
 lua_createtable(L, 0, 9);
 setfield(L, "sec", stm->tm_sec);
 setfield(L, "min", stm->tm_min);
 setfield(L, "hour", stm->tm_hour);
 setfield(L, "day", stm->tm_mday);
 setfield(L, "month", stm->tm_mon + 1);
 setfield(L, "year", stm->tm_year + 1900);
 setfield(L, "wday", stm->tm_wday + 1);
 setfield(L, "yday", stm->tm_yday + 1);
 setboolfield(L, "isdst", stm->tm_isdst);
 }
 else
 {
 char cc[3];
 cc[0] = '%';
 cc[2] = '\0';
 luaL_Buffer b;
 luaL_buffinit(L, &b);
 for (; *s; s++)
 {
 if (*s != '%' || *(s + 1) == '\0')
 {
 luaL_addchar(&b, *s);
 }
 else if (strchr(LUA_STRFTIMEOPTIONS, *(s + 1)) == 0)
 {
 luaL_argerror(L, 1, "invalid conversion specifier");
 }
 else
 {
 size_t reslen;
 char buff[200];
 cc[1] = *(++s);
 reslen = strftime(buff, sizeof(buff), cc, stm);
 luaL_addlstring(&b, buff, reslen, -1);
 }
 }
 luaL_pushresult(&b);
 }
 return 1;
}
static int os_time(lua_State* L)
{
 time_t t;
 if (lua_isnoneornil(L, 1))
 t = time(NULL); // get current time
 else
 {
 struct tm ts;
 luaL_checktype(L, 1, LUA_TTABLE);
 lua_settop(L, 1);
 ts.tm_sec = getfield(L, "sec", 0);
 ts.tm_min = getfield(L, "min", 0);
 ts.tm_hour = getfield(L, "hour", 12);
 ts.tm_mday = getfield(L, "day", -1);
 ts.tm_mon = getfield(L, "month", -1) - 1;
 ts.tm_year = getfield(L, "year", -1) - 1900;
 ts.tm_isdst = getboolfield(L, "isdst");
 t = timegm(&ts);
 }
 if (t == (time_t)(-1))
 lua_pushnil(L);
 else
 lua_pushnumber(L, (double)t);
 return 1;
}
static int os_difftime(lua_State* L)
{
 lua_pushnumber(L, difftime((time_t)(luaL_checknumber(L, 1)), (time_t)(luaL_optnumber(L, 2, 0))));
 return 1;
}
static const luaL_Reg syslib[] = {
 {"clock", os_clock},
 {"date", os_date},
 {"difftime", os_difftime},
 {"time", os_time},
 {NULL, NULL},
};
int luaopen_os(lua_State* L)
{
 luaL_register(L, LUA_OSLIBNAME, syslib);
 return 1;
}
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif
#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif
static double clock_period()
{
#if defined(_WIN32)
 LARGE_INTEGER result = {};
 QueryPerformanceFrequency(&result);
 return 1.0 / double(result.QuadPart);
#elif defined(__APPLE__)
 mach_timebase_info_data_t result = {};
 mach_timebase_info(&result);
 return double(result.numer) / double(result.denom) * 1e-9;
#elif defined(__linux__)
 return 1e-9;
#else
 return 1.0 / double(CLOCKS_PER_SEC);
#endif
}
static double clock_timestamp()
{
#if defined(_WIN32)
 LARGE_INTEGER result = {};
 QueryPerformanceCounter(&result);
 return double(result.QuadPart);
#elif defined(__APPLE__)
 return double(mach_absolute_time());
#elif defined(__linux__)
 timespec now;
 clock_gettime(CLOCK_MONOTONIC, &now);
 return now.tv_sec * 1e9 + now.tv_nsec;
#else
 return double(clock());
#endif
}
double lua_clock()
{
 static double period = clock_period();
 return clock_timestamp() * period;
}
typedef struct LG
{
 lua_State l;
 global_State g;
} LG;
static void stack_init(lua_State* L1, lua_State* L)
{
 L1->base_ci = luaM_newarray(L, BASIC_CI_SIZE, CallInfo, L1->memcat);
 L1->ci = L1->base_ci;
 L1->size_ci = BASIC_CI_SIZE;
 L1->end_ci = L1->base_ci + L1->size_ci - 1;
 L1->stack = luaM_newarray(L, BASIC_STACK_SIZE + EXTRA_STACK, TValue, L1->memcat);
 L1->stacksize = BASIC_STACK_SIZE + EXTRA_STACK;
 TValue* stack = L1->stack;
 for (int i = 0; i < BASIC_STACK_SIZE + EXTRA_STACK; i++)
 setnilvalue(stack + i);
 L1->top = stack;
 L1->stack_last = stack + (L1->stacksize - EXTRA_STACK);
 L1->ci->func = L1->top;
 setnilvalue(L1->top++);
 L1->base = L1->ci->base = L1->top;
 L1->ci->top = L1->top + LUA_MINSTACK;
}
static void freestack(lua_State* L, lua_State* L1)
{
 luaM_freearray(L, L1->base_ci, L1->size_ci, CallInfo, L1->memcat);
 luaM_freearray(L, L1->stack, L1->stacksize, TValue, L1->memcat);
}
static void f_luaopen(lua_State* L, void* ud)
{
 global_State* g = L->global;
 stack_init(L, L);
 L->gt = luaH_new(L, 0, 2); // table of globals
 sethvalue(L, registry(L), luaH_new(L, 0, 2));
 luaS_resize(L, LUA_MINSTRTABSIZE); // initial size of string table
 luaT_init(L);
 luaS_fix(luaS_newliteral(L, LUA_MEMERRMSG));
 luaS_fix(luaS_newliteral(L, LUA_ERRERRMSG)); // pin to make sure we can always throw this error
 g->GCthreshold = 4 * g->totalbytes;
}
static void preinit_state(lua_State* L, global_State* g)
{
 L->global = g;
 L->stack = NULL;
 L->stacksize = 0;
 L->gt = NULL;
 L->openupval = NULL;
 L->size_ci = 0;
 L->nCcalls = L->baseCcalls = 0;
 L->status = 0;
 L->base_ci = L->ci = NULL;
 L->namecall = NULL;
 L->cachedslot = 0;
 L->singlestep = false;
 L->isactive = false;
 L->activememcat = 0;
 L->userdata = NULL;
}
static void close_state(lua_State* L)
{
 global_State* g = L->global;
 luaF_close(L, L->stack);
 luaC_freeall(L); // collect all objects
 LUAU_ASSERT(g->strt.nuse == 0);
 luaM_freearray(L, L->global->strt.hash, L->global->strt.size, TString*, 0);
 freestack(L, L);
 for (int i = 0; i < LUA_SIZECLASSES; i++)
 {
 LUAU_ASSERT(g->freepages[i] == NULL);
 LUAU_ASSERT(g->freegcopages[i] == NULL);
 }
 LUAU_ASSERT(g->allgcopages == NULL);
 LUAU_ASSERT(g->totalbytes == sizeof(LG));
 LUAU_ASSERT(g->memcatbytes[0] == sizeof(LG));
 for (int i = 1; i < LUA_MEMORY_CATEGORIES; i++)
 LUAU_ASSERT(g->memcatbytes[i] == 0);
#if LUA_CUSTOM_EXECUTION
 if (L->global->ecb.close)
 L->global->ecb.close(L);
#endif
 (*g->frealloc)(g->ud, L, sizeof(LG), 0);
}
lua_State* luaE_newthread(lua_State* L)
{
 lua_State* L1 = luaM_newgco(L, lua_State, sizeof(lua_State), L->activememcat);
 luaC_init(L, L1, LUA_TTHREAD);
 preinit_state(L1, L->global);
 L1->activememcat = L->activememcat;
 stack_init(L1, L); // init stack
 L1->gt = L->gt;
 L1->singlestep = L->singlestep;
 LUAU_ASSERT(iswhite(obj2gco(L1)));
 return L1;
}
void luaE_freethread(lua_State* L, lua_State* L1, lua_Page* page)
{
 global_State* g = L->global;
 if (g->cb.userthread)
 g->cb.userthread(NULL, L1);
 freestack(L, L1);
 luaM_freegco(L, L1, sizeof(lua_State), L1->memcat, page);
}
void lua_resetthread(lua_State* L)
{
 luaF_close(L, L->stack);
 CallInfo* ci = L->base_ci;
 ci->func = L->stack;
 ci->base = ci->func + 1;
 ci->top = ci->base + LUA_MINSTACK;
 setnilvalue(ci->func);
 L->ci = ci;
 if (L->size_ci != BASIC_CI_SIZE)
 luaD_reallocCI(L, BASIC_CI_SIZE);
 L->status = LUA_OK;
 L->base = L->ci->base;
 L->top = L->ci->base;
 L->nCcalls = L->baseCcalls = 0;
 if (L->stacksize != BASIC_STACK_SIZE + EXTRA_STACK)
 luaD_reallocstack(L, BASIC_STACK_SIZE);
 for (int i = 0; i < L->stacksize; i++)
 setnilvalue(L->stack + i);
}
int lua_isthreadreset(lua_State* L)
{
 return L->ci == L->base_ci && L->base == L->top && L->status == LUA_OK;
}
lua_State* lua_newstate(lua_Alloc f, void* ud)
{
 int i;
 lua_State* L;
 global_State* g;
 void* l = (*f)(ud, NULL, 0, sizeof(LG));
 if (l == NULL)
 return NULL;
 L = (lua_State*)l;
 g = &((LG*)L)->g;
 L->tt = LUA_TTHREAD;
 L->marked = g->currentwhite = bit2mask(WHITE0BIT, FIXEDBIT);
 L->memcat = 0;
 preinit_state(L, g);
 g->frealloc = f;
 g->ud = ud;
 g->mainthread = L;
 g->uvhead.u.open.prev = &g->uvhead;
 g->uvhead.u.open.next = &g->uvhead;
 g->GCthreshold = 0;
 g->registryfree = 0;
 g->errorjmp = NULL;
 g->rngstate = 0;
 g->ptrenckey[0] = 1;
 g->ptrenckey[1] = 0;
 g->ptrenckey[2] = 0;
 g->ptrenckey[3] = 0;
 g->strt.size = 0;
 g->strt.nuse = 0;
 g->strt.hash = NULL;
 setnilvalue(&g->pseudotemp);
 setnilvalue(registry(L));
 g->gcstate = GCSpause;
 g->gray = NULL;
 g->grayagain = NULL;
 g->weak = NULL;
 g->totalbytes = sizeof(LG);
 g->gcgoal = LUAI_GCGOAL;
 g->gcstepmul = LUAI_GCSTEPMUL;
 g->gcstepsize = LUAI_GCSTEPSIZE << 10;
 for (i = 0; i < LUA_SIZECLASSES; i++)
 {
 g->freepages[i] = NULL;
 g->freegcopages[i] = NULL;
 }
 g->allgcopages = NULL;
 g->sweepgcopage = NULL;
 for (i = 0; i < LUA_T_COUNT; i++)
 g->mt[i] = NULL;
 for (i = 0; i < LUA_UTAG_LIMIT; i++)
 g->udatagc[i] = NULL;
 for (i = 0; i < LUA_MEMORY_CATEGORIES; i++)
 g->memcatbytes[i] = 0;
 g->memcatbytes[0] = sizeof(LG);
 g->cb = lua_Callbacks();
 g->ecb = lua_ExecutionCallbacks();
 g->gcstats = GCStats();
#ifdef LUAI_GCMETRICS
 g->gcmetrics = GCMetrics();
#endif
 if (luaD_rawrunprotected(L, f_luaopen, NULL) != 0)
 {
 close_state(L);
 L = NULL;
 }
 return L;
}
void lua_close(lua_State* L)
{
 L = L->global->mainthread;
 luaF_close(L, L->stack); // close all upvalues for this thread
 close_state(L);
}
unsigned int luaS_hash(const char* str, size_t len)
{
 unsigned int a = 0, b = 0;
 unsigned int h = unsigned(len);
 while (len >= 32)
 {
#define rol(x, s) ((x >> s) | (x << (32 - s)))
#define mix(u, v, w) a ^= h, a -= rol(h, u), b ^= a, b -= rol(a, v), h ^= b, h -= rol(b, w)
 uint32_t block[3];
 memcpy(block, str, 12);
 a += block[0];
 b += block[1];
 h += block[2];
 mix(14, 11, 25);
 str += 12;
 len -= 12;
#undef mix
#undef rol
 }
 for (size_t i = len; i > 0; --i)
 h ^= (h << 5) + (h >> 2) + (uint8_t)str[i - 1];
 return h;
}
void luaS_resize(lua_State* L, int newsize)
{
 TString** newhash = luaM_newarray(L, newsize, TString*, 0);
 stringtable* tb = &L->global->strt;
 for (int i = 0; i < newsize; i++)
 newhash[i] = NULL;
 for (int i = 0; i < tb->size; i++)
 {
 TString* p = tb->hash[i];
 while (p)
 {
 TString* next = p->next; // save next
 unsigned int h = p->hash;
 int h1 = lmod(h, newsize);
 LUAU_ASSERT(cast_int(h % newsize) == lmod(h, newsize));
 p->next = newhash[h1];
 newhash[h1] = p;
 p = next;
 }
 }
 luaM_freearray(L, tb->hash, tb->size, TString*, 0);
 tb->size = newsize;
 tb->hash = newhash;
}
static TString* newlstr(lua_State* L, const char* str, size_t l, unsigned int h)
{
 if (l > MAXSSIZE)
 luaM_toobig(L);
 TString* ts = luaM_newgco(L, TString, sizestring(l), L->activememcat);
 luaC_init(L, ts, LUA_TSTRING);
 ts->atom = ATOM_UNDEF;
 ts->hash = h;
 ts->len = unsigned(l);
 memcpy(ts->data, str, l);
 ts->data[l] = '\0';
 stringtable* tb = &L->global->strt;
 h = lmod(h, tb->size);
 ts->next = tb->hash[h];
 tb->hash[h] = ts;
 tb->nuse++;
 if (tb->nuse > cast_to(uint32_t, tb->size) && tb->size <= INT_MAX / 2)
 luaS_resize(L, tb->size * 2);
 return ts;
}
TString* luaS_bufstart(lua_State* L, size_t size)
{
 if (size > MAXSSIZE)
 luaM_toobig(L);
 TString* ts = luaM_newgco(L, TString, sizestring(size), L->activememcat);
 luaC_init(L, ts, LUA_TSTRING);
 ts->atom = ATOM_UNDEF;
 ts->hash = 0;
 ts->len = unsigned(size);
 ts->next = NULL;
 return ts;
}
TString* luaS_buffinish(lua_State* L, TString* ts)
{
 unsigned int h = luaS_hash(ts->data, ts->len);
 stringtable* tb = &L->global->strt;
 int bucket = lmod(h, tb->size);
 for (TString* el = tb->hash[bucket]; el != NULL; el = el->next)
 {
 if (el->len == ts->len && memcmp(el->data, ts->data, ts->len) == 0)
 {
 if (isdead(L->global, obj2gco(el)))
 changewhite(obj2gco(el));
 return el;
 }
 }
 LUAU_ASSERT(ts->next == NULL);
 ts->hash = h;
 ts->data[ts->len] = '\0';
 ts->atom = ATOM_UNDEF;
 ts->next = tb->hash[bucket];
 tb->hash[bucket] = ts;
 tb->nuse++;
 if (tb->nuse > cast_to(uint32_t, tb->size) && tb->size <= INT_MAX / 2)
 luaS_resize(L, tb->size * 2);
 return ts;
}
TString* luaS_newlstr(lua_State* L, const char* str, size_t l)
{
 unsigned int h = luaS_hash(str, l);
 for (TString* el = L->global->strt.hash[lmod(h, L->global->strt.size)]; el != NULL; el = el->next)
 {
 if (el->len == l && (memcmp(str, getstr(el), l) == 0))
 {
 if (isdead(L->global, obj2gco(el)))
 changewhite(obj2gco(el));
 return el;
 }
 }
 return newlstr(L, str, l, h);
}
static bool unlinkstr(lua_State* L, TString* ts)
{
 global_State* g = L->global;
 TString** p = &g->strt.hash[lmod(ts->hash, g->strt.size)];
 while (TString* curr = *p)
 {
 if (curr == ts)
 {
 *p = curr->next;
 return true;
 }
 else
 {
 p = &curr->next;
 }
 }
 return false;
}
void luaS_free(lua_State* L, TString* ts, lua_Page* page)
{
 if (unlinkstr(L, ts))
 L->global->strt.nuse--;
 else
 LUAU_ASSERT(ts->next == NULL);
 luaM_freegco(L, ts, sizestring(ts->len), ts->memcat, page);
}
#define uchar(c) ((unsigned char)(c))
static int str_len(lua_State* L)
{
 size_t l;
 luaL_checklstring(L, 1, &l);
 lua_pushinteger(L, (int)l);
 return 1;
}
static int posrelat(int pos, size_t len)
{
 if (pos < 0)
 pos += (int)len + 1;
 return (pos >= 0) ? pos : 0;
}
static int str_sub(lua_State* L)
{
 size_t l;
 const char* s = luaL_checklstring(L, 1, &l);
 int start = posrelat(luaL_checkinteger(L, 2), l);
 int end = posrelat(luaL_optinteger(L, 3, -1), l);
 if (start < 1)
 start = 1;
 if (end > (int)l)
 end = (int)l;
 if (start <= end)
 lua_pushlstring(L, s + start - 1, end - start + 1);
 else
 lua_pushliteral(L, "");
 return 1;
}
static int str_reverse(lua_State* L)
{
 size_t l;
 const char* s = luaL_checklstring(L, 1, &l);
 luaL_Buffer b;
 char* ptr = luaL_buffinitsize(L, &b, l);
 while (l--)
 *ptr++ = s[l];
 luaL_pushresultsize(&b, ptr - b.p);
 return 1;
}
static int str_lower(lua_State* L)
{
 size_t l;
 const char* s = luaL_checklstring(L, 1, &l);
 luaL_Buffer b;
 char* ptr = luaL_buffinitsize(L, &b, l);
 for (size_t i = 0; i < l; i++)
 *ptr++ = tolower(uchar(s[i]));
 luaL_pushresultsize(&b, l);
 return 1;
}
static int str_upper(lua_State* L)
{
 size_t l;
 const char* s = luaL_checklstring(L, 1, &l);
 luaL_Buffer b;
 char* ptr = luaL_buffinitsize(L, &b, l);
 for (size_t i = 0; i < l; i++)
 *ptr++ = toupper(uchar(s[i]));
 luaL_pushresultsize(&b, l);
 return 1;
}
static int str_rep(lua_State* L)
{
 size_t l;
 const char* s = luaL_checklstring(L, 1, &l);
 int n = luaL_checkinteger(L, 2);
 if (n <= 0)
 {
 lua_pushliteral(L, "");
 return 1;
 }
 if (l > MAXSSIZE / (size_t)n) // may overflow?
 luaL_error(L, "resulting string too large");
 luaL_Buffer b;
 char* ptr = luaL_buffinitsize(L, &b, l * n);
 const char* start = ptr;
 size_t left = l * n;
 size_t step = l;
 memcpy(ptr, s, l);
 ptr += l;
 left -= l;
 while (step < left)
 {
 memcpy(ptr, start, step);
 ptr += step;
 left -= step;
 step <<= 1;
 }
 memcpy(ptr, start, left);
 ptr += left;
 luaL_pushresultsize(&b, l * n);
 return 1;
}
static int str_byte(lua_State* L)
{
 size_t l;
 const char* s = luaL_checklstring(L, 1, &l);
 int posi = posrelat(luaL_optinteger(L, 2, 1), l);
 int pose = posrelat(luaL_optinteger(L, 3, posi), l);
 int n, i;
 if (posi <= 0)
 posi = 1;
 if ((size_t)pose > l)
 pose = (int)l;
 if (posi > pose)
 return 0;
 n = (int)(pose - posi + 1);
 if (posi + n <= pose)
 luaL_error(L, "string slice too long");
 luaL_checkstack(L, n, "string slice too long");
 for (i = 0; i < n; i++)
 lua_pushinteger(L, uchar(s[posi + i - 1]));
 return n;
}
static int str_char(lua_State* L)
{
 int n = lua_gettop(L);
 luaL_Buffer b;
 char* ptr = luaL_buffinitsize(L, &b, n);
 for (int i = 1; i <= n; i++)
 {
 int c = luaL_checkinteger(L, i);
 luaL_argcheck(L, uchar(c) == c, i, "invalid value");
 *ptr++ = uchar(c);
 }
 luaL_pushresultsize(&b, n);
 return 1;
}
#define CAP_UNFINISHED (-1)
#define CAP_POSITION (-2)
typedef struct MatchState
{
 int matchdepth;
 const char* src_init; // init of source string
 const char* src_end;
 const char* p_end; // end ('\0') of pattern
 lua_State* L;
 int level;
 struct
 {
 const char* init;
 ptrdiff_t len;
 } capture[LUA_MAXCAPTURES];
} MatchState;
static const char* match(MatchState* ms, const char* s, const char* p);
#define L_ESC '%'
#define SPECIALS "^$*+?.([%-"
static int check_capture(MatchState* ms, int l)
{
 l -= '1';
 if (l < 0 || l >= ms->level || ms->capture[l].len == CAP_UNFINISHED)
 luaL_error(ms->L, "invalid capture index %%%d", l + 1);
 return l;
}
static int capture_to_close(MatchState* ms)
{
 int level = ms->level;
 for (level--; level >= 0; level--)
 if (ms->capture[level].len == CAP_UNFINISHED)
 return level;
 luaL_error(ms->L, "invalid pattern capture");
}
static const char* classend(MatchState* ms, const char* p)
{
 switch (*p++)
 {
 case L_ESC:
 {
 if (p == ms->p_end)
 luaL_error(ms->L, "malformed pattern (ends with '%%')");
 return p + 1;
 }
 case '[':
 {
 if (*p == '^')
 p++;
 do
 {
 if (p == ms->p_end)
 luaL_error(ms->L, "malformed pattern (missing ']')");
 if (*(p++) == L_ESC && p < ms->p_end)
 p++;
 } while (*p != ']');
 return p + 1;
 }
 default:
 {
 return p;
 }
 }
}
static int match_class(int c, int cl)
{
 int res;
 switch (tolower(cl))
 {
 case 'a':
 res = isalpha(c);
 break;
 case 'c':
 res = iscntrl(c);
 break;
 case 'd':
 res = isdigit(c);
 break;
 case 'g':
 res = isgraph(c);
 break;
 case 'l':
 res = islower(c);
 break;
 case 'p':
 res = ispunct(c);
 break;
 case 's':
 res = isspace(c);
 break;
 case 'u':
 res = isupper(c);
 break;
 case 'w':
 res = isalnum(c);
 break;
 case 'x':
 res = isxdigit(c);
 break;
 case 'z':
 res = (c == 0);
 break;
 default:
 return (cl == c);
 }
 return (islower(cl) ? res : !res);
}
static int matchbracketclass(int c, const char* p, const char* ec)
{
 int sig = 1;
 if (*(p + 1) == '^')
 {
 sig = 0;
 p++;
 }
 while (++p < ec)
 {
 if (*p == L_ESC)
 {
 p++;
 if (match_class(c, uchar(*p)))
 return sig;
 }
 else if ((*(p + 1) == '-') && (p + 2 < ec))
 {
 p += 2;
 if (uchar(*(p - 2)) <= c && c <= uchar(*p))
 return sig;
 }
 else if (uchar(*p) == c)
 return sig;
 }
 return !sig;
}
static int singlematch(MatchState* ms, const char* s, const char* p, const char* ep)
{
 if (s >= ms->src_end)
 return 0;
 else
 {
 int c = uchar(*s);
 switch (*p)
 {
 case '.':
 return 1;
 case L_ESC:
 return match_class(c, uchar(*(p + 1)));
 case '[':
 return matchbracketclass(c, p, ep - 1);
 default:
 return (uchar(*p) == c);
 }
 }
}
static const char* matchbalance(MatchState* ms, const char* s, const char* p)
{
 if (p >= ms->p_end - 1)
 luaL_error(ms->L, "malformed pattern (missing arguments to '%%b')");
 if (*s != *p)
 return NULL;
 else
 {
 int b = *p;
 int e = *(p + 1);
 int cont = 1;
 while (++s < ms->src_end)
 {
 if (*s == e)
 {
 if (--cont == 0)
 return s + 1;
 }
 else if (*s == b)
 cont++;
 }
 }
 return NULL;
}
static const char* max_expand(MatchState* ms, const char* s, const char* p, const char* ep)
{
 ptrdiff_t i = 0;
 while (singlematch(ms, s + i, p, ep))
 i++;
 while (i >= 0)
 {
 const char* res = match(ms, (s + i), ep + 1);
 if (res)
 return res;
 i--;
 }
 return NULL;
}
static const char* min_expand(MatchState* ms, const char* s, const char* p, const char* ep)
{
 for (;;)
 {
 const char* res = match(ms, s, ep + 1);
 if (res != NULL)
 return res;
 else if (singlematch(ms, s, p, ep))
 s++;
 else
 return NULL;
 }
}
static const char* start_capture(MatchState* ms, const char* s, const char* p, int what)
{
 const char* res;
 int level = ms->level;
 if (level >= LUA_MAXCAPTURES)
 luaL_error(ms->L, "too many captures");
 ms->capture[level].init = s;
 ms->capture[level].len = what;
 ms->level = level + 1;
 if ((res = match(ms, s, p)) == NULL)
 ms->level--; // undo capture
 return res;
}
static const char* end_capture(MatchState* ms, const char* s, const char* p)
{
 int l = capture_to_close(ms);
 const char* res;
 ms->capture[l].len = s - ms->capture[l].init;
 if ((res = match(ms, s, p)) == NULL) // match failed?
 ms->capture[l].len = CAP_UNFINISHED;
 return res;
}
static const char* match_capture(MatchState* ms, const char* s, int l)
{
 size_t len;
 l = check_capture(ms, l);
 len = ms->capture[l].len;
 if ((size_t)(ms->src_end - s) >= len && memcmp(ms->capture[l].init, s, len) == 0)
 return s + len;
 else
 return NULL;
}
static const char* match(MatchState* ms, const char* s, const char* p)
{
 if (ms->matchdepth-- == 0)
 luaL_error(ms->L, "pattern too complex");
init:
 if (p != ms->p_end)
 {
 switch (*p)
 {
 case '(':
 {
 if (*(p + 1) == ')') // position capture?
 s = start_capture(ms, s, p + 2, CAP_POSITION);
 else
 s = start_capture(ms, s, p + 1, CAP_UNFINISHED);
 break;
 }
 case ')':
 {
 s = end_capture(ms, s, p + 1);
 break;
 }
 case '$':
 {
 if ((p + 1) != ms->p_end)
 goto dflt; // no; go to default
 s = (s == ms->src_end) ? s : NULL;
 break;
 }
 case L_ESC:
 {
 switch (*(p + 1))
 {
 case 'b':
 {
 s = matchbalance(ms, s, p + 2);
 if (s != NULL)
 {
 p += 4;
 goto init;
 } // else fail (s == NULL)
 break;
 }
 case 'f':
 {
 const char* ep;
 char previous;
 p += 2;
 if (*p != '[')
 luaL_error(ms->L, "missing '[' after '%%f' in pattern");
 ep = classend(ms, p);
 previous = (s == ms->src_init) ? '\0' : *(s - 1);
 if (!matchbracketclass(uchar(previous), p, ep - 1) && matchbracketclass(uchar(*s), p, ep - 1))
 {
 p = ep;
 goto init;
 }
 s = NULL;
 break;
 }
 case '0':
 case '1':
 case '2':
 case '3':
 case '4':
 case '5':
 case '6':
 case '7':
 case '8':
 case '9':
 {
 s = match_capture(ms, s, uchar(*(p + 1)));
 if (s != NULL)
 {
 p += 2;
 goto init;
 }
 break;
 }
 default:
 goto dflt;
 }
 break;
 }
 default:
 dflt:
 {
 const char* ep = classend(ms, p); // points to optional suffix
 if (!singlematch(ms, s, p, ep))
 {
 if (*ep == '*' || *ep == '?' || *ep == '-')
 {
 p = ep + 1;
 goto init;
 }
 else
 s = NULL; // fail
 }
 else
 {
 switch (*ep)
 {
 case '?':
 {
 const char* res;
 if ((res = match(ms, s + 1, ep + 1)) != NULL)
 s = res;
 else
 {
 p = ep + 1;
 goto init;
 }
 break;
 }
 case '+':
 s++; // 1 match already done
 case '*': // 0 or more repetitions
 s = max_expand(ms, s, p, ep);
 break;
 case '-':
 s = min_expand(ms, s, p, ep);
 break;
 default:
 s++;
 p = ep;
 goto init;
 }
 }
 break;
 }
 }
 }
 ms->matchdepth++;
 return s;
}
static const char* lmemfind(const char* s1, size_t l1, const char* s2, size_t l2)
{
 if (l2 == 0)
 return s1;
 else if (l2 > l1)
 return NULL;
 else
 {
 const char* init;
 l2--; // 1st char will be checked by `memchr'
 l1 = l1 - l2;
 while (l1 > 0 && (init = (const char*)memchr(s1, *s2, l1)) != NULL)
 {
 init++;
 if (memcmp(init, s2 + 1, l2) == 0)
 return init - 1;
 else
 {
 l1 -= init - s1;
 s1 = init;
 }
 }
 return NULL;
 }
}
static void push_onecapture(MatchState* ms, int i, const char* s, const char* e)
{
 if (i >= ms->level)
 {
 if (i == 0)
 lua_pushlstring(ms->L, s, e - s); // add whole match
 else
 luaL_error(ms->L, "invalid capture index");
 }
 else
 {
 ptrdiff_t l = ms->capture[i].len;
 if (l == CAP_UNFINISHED)
 luaL_error(ms->L, "unfinished capture");
 if (l == CAP_POSITION)
 lua_pushinteger(ms->L, (int)(ms->capture[i].init - ms->src_init) + 1);
 else
 lua_pushlstring(ms->L, ms->capture[i].init, l);
 }
}
static int push_captures(MatchState* ms, const char* s, const char* e)
{
 int i;
 int nlevels = (ms->level == 0 && s) ? 1 : ms->level;
 luaL_checkstack(ms->L, nlevels, "too many captures");
 for (i = 0; i < nlevels; i++)
 push_onecapture(ms, i, s, e);
 return nlevels;
}
static int nospecials(const char* p, size_t l)
{
 size_t upto = 0;
 do
 {
 if (strpbrk(p + upto, SPECIALS))
 return 0;
 upto += strlen(p + upto) + 1; // may have more after \0
 } while (upto <= l);
 return 1;
}
static void prepstate(MatchState* ms, lua_State* L, const char* s, size_t ls, const char* p, size_t lp)
{
 ms->L = L;
 ms->matchdepth = LUAI_MAXCCALLS;
 ms->src_init = s;
 ms->src_end = s + ls;
 ms->p_end = p + lp;
}
static void reprepstate(MatchState* ms)
{
 ms->level = 0;
 LUAU_ASSERT(ms->matchdepth == LUAI_MAXCCALLS);
}
static int str_find_aux(lua_State* L, int find)
{
 size_t ls, lp;
 const char* s = luaL_checklstring(L, 1, &ls);
 const char* p = luaL_checklstring(L, 2, &lp);
 int init = posrelat(luaL_optinteger(L, 3, 1), ls);
 if (init < 1)
 init = 1;
 else if (init > (int)ls + 1)
 {
 lua_pushnil(L); // cannot find anything
 return 1;
 }
 if (find && (lua_toboolean(L, 4) || nospecials(p, lp)))
 {
 const char* s2 = lmemfind(s + init - 1, ls - init + 1, p, lp);
 if (s2)
 {
 lua_pushinteger(L, (int)(s2 - s + 1));
 lua_pushinteger(L, (int)(s2 - s + lp));
 return 2;
 }
 }
 else
 {
 MatchState ms;
 const char* s1 = s + init - 1;
 int anchor = (*p == '^');
 if (anchor)
 {
 p++;
 lp--;
 }
 prepstate(&ms, L, s, ls, p, lp);
 do
 {
 const char* res;
 reprepstate(&ms);
 if ((res = match(&ms, s1, p)) != NULL)
 {
 if (find)
 {
 lua_pushinteger(L, (int)(s1 - s + 1));
 lua_pushinteger(L, (int)(res - s)); // end
 return push_captures(&ms, NULL, 0) + 2;
 }
 else
 return push_captures(&ms, s1, res);
 }
 } while (s1++ < ms.src_end && !anchor);
 }
 lua_pushnil(L);
 return 1;
}
static int str_find(lua_State* L)
{
 return str_find_aux(L, 1);
}
static int str_match(lua_State* L)
{
 return str_find_aux(L, 0);
}
static int gmatch_aux(lua_State* L)
{
 MatchState ms;
 size_t ls, lp;
 const char* s = lua_tolstring(L, lua_upvalueindex(1), &ls);
 const char* p = lua_tolstring(L, lua_upvalueindex(2), &lp);
 const char* src;
 prepstate(&ms, L, s, ls, p, lp);
 for (src = s + (size_t)lua_tointeger(L, lua_upvalueindex(3)); src <= ms.src_end; src++)
 {
 const char* e;
 reprepstate(&ms);
 if ((e = match(&ms, src, p)) != NULL)
 {
 int newstart = (int)(e - s);
 if (e == src)
 newstart++;
 lua_pushinteger(L, newstart);
 lua_replace(L, lua_upvalueindex(3));
 return push_captures(&ms, src, e);
 }
 }
 return 0;
}
static int gmatch(lua_State* L)
{
 luaL_checkstring(L, 1);
 luaL_checkstring(L, 2);
 lua_settop(L, 2);
 lua_pushinteger(L, 0);
 lua_pushcclosure(L, gmatch_aux, NULL, 3);
 return 1;
}
static void add_s(MatchState* ms, luaL_Buffer* b, const char* s, const char* e)
{
 size_t l, i;
 const char* news = lua_tolstring(ms->L, 3, &l);
 luaL_reservebuffer(b, l, -1);
 for (i = 0; i < l; i++)
 {
 if (news[i] != L_ESC)
 luaL_addchar(b, news[i]);
 else
 {
 i++;
 if (!isdigit(uchar(news[i])))
 {
 if (news[i] != L_ESC)
 luaL_error(ms->L, "invalid use of '%c' in replacement string", L_ESC);
 luaL_addchar(b, news[i]);
 }
 else if (news[i] == '0')
 luaL_addlstring(b, s, e - s, -1);
 else
 {
 push_onecapture(ms, news[i] - '1', s, e);
 luaL_addvalue(b);
 }
 }
 }
}
static void add_value(MatchState* ms, luaL_Buffer* b, const char* s, const char* e, int tr)
{
 lua_State* L = ms->L;
 switch (tr)
 {
 case LUA_TFUNCTION:
 {
 int n;
 lua_pushvalue(L, 3);
 n = push_captures(ms, s, e);
 lua_call(L, n, 1);
 break;
 }
 case LUA_TTABLE:
 {
 push_onecapture(ms, 0, s, e);
 lua_gettable(L, 3);
 break;
 }
 default:
 {
 add_s(ms, b, s, e);
 return;
 }
 }
 if (!lua_toboolean(L, -1))
 {
 lua_pop(L, 1);
 lua_pushlstring(L, s, e - s);
 }
 else if (!lua_isstring(L, -1))
 luaL_error(L, "invalid replacement value (a %s)", luaL_typename(L, -1));
 luaL_addvalue(b);
}
static int str_gsub(lua_State* L)
{
 size_t srcl, lp;
 const char* src = luaL_checklstring(L, 1, &srcl);
 const char* p = luaL_checklstring(L, 2, &lp);
 int tr = lua_type(L, 3);
 int max_s = luaL_optinteger(L, 4, (int)srcl + 1);
 int anchor = (*p == '^');
 int n = 0;
 MatchState ms;
 luaL_Buffer b;
 luaL_argexpected(L, tr == LUA_TNUMBER || tr == LUA_TSTRING || tr == LUA_TFUNCTION || tr == LUA_TTABLE, 3, "string/function/table");
 luaL_buffinit(L, &b);
 if (anchor)
 {
 p++;
 lp--;
 }
 prepstate(&ms, L, src, srcl, p, lp);
 while (n < max_s)
 {
 const char* e;
 reprepstate(&ms);
 e = match(&ms, src, p);
 if (e)
 {
 n++;
 add_value(&ms, &b, src, e, tr);
 }
 if (e && e > src)
 src = e; // skip it
 else if (src < ms.src_end)
 luaL_addchar(&b, *src++);
 else
 break;
 if (anchor)
 break;
 }
 luaL_addlstring(&b, src, ms.src_end - src, -1);
 luaL_pushresult(&b);
 lua_pushinteger(L, n);
 return 2;
}
#define FLAGS "-+ #0"
#define MAX_ITEM 512
#define MAX_FORMAT 32
static void addquoted(lua_State* L, luaL_Buffer* b, int arg)
{
 size_t l;
 const char* s = luaL_checklstring(L, arg, &l);
 luaL_reservebuffer(b, l + 2, -1);
 luaL_addchar(b, '"');
 while (l--)
 {
 switch (*s)
 {
 case '"':
 case '\\':
 case '\n':
 {
 luaL_addchar(b, '\\');
 luaL_addchar(b, *s);
 break;
 }
 case '\r':
 {
 luaL_addlstring(b, "\\r", 2, -1);
 break;
 }
 case '\0':
 {
 luaL_addlstring(b, "\\000", 4, -1);
 break;
 }
 default:
 {
 luaL_addchar(b, *s);
 break;
 }
 }
 s++;
 }
 luaL_addchar(b, '"');
}
static const char* scanformat(lua_State* L, const char* strfrmt, char* form, size_t* size)
{
 const char* p = strfrmt;
 while (*p != '\0' && strchr(FLAGS, *p) != NULL)
 p++;
 if ((size_t)(p - strfrmt) >= sizeof(FLAGS))
 luaL_error(L, "invalid format (repeated flags)");
 if (isdigit(uchar(*p)))
 p++;
 if (isdigit(uchar(*p)))
 p++;
 if (*p == '.')
 {
 p++;
 if (isdigit(uchar(*p)))
 p++;
 if (isdigit(uchar(*p)))
 p++;
 }
 if (isdigit(uchar(*p)))
 luaL_error(L, "invalid format (width or precision too long)");
 *(form++) = '%';
 *size = p - strfrmt + 1;
 strncpy(form, strfrmt, *size);
 form += *size;
 *form = '\0';
 return p;
}
static void addInt64Format(char form[MAX_FORMAT], char formatIndicator, size_t formatItemSize)
{
 LUAU_ASSERT((formatItemSize + 3) <= MAX_FORMAT);
 LUAU_ASSERT(form[0] == '%');
 LUAU_ASSERT(form[formatItemSize] != 0);
 LUAU_ASSERT(form[formatItemSize + 1] == 0);
 form[formatItemSize + 0] = 'l';
 form[formatItemSize + 1] = 'l';
 form[formatItemSize + 2] = formatIndicator;
 form[formatItemSize + 3] = 0;
}
static int str_format(lua_State* L)
{
 int top = lua_gettop(L);
 int arg = 1;
 size_t sfl;
 const char* strfrmt = luaL_checklstring(L, arg, &sfl);
 const char* strfrmt_end = strfrmt + sfl;
 luaL_Buffer b;
 luaL_buffinit(L, &b);
 while (strfrmt < strfrmt_end)
 {
 if (*strfrmt != L_ESC)
 luaL_addchar(&b, *strfrmt++);
 else if (*++strfrmt == L_ESC)
 luaL_addchar(&b, *strfrmt++);
 else
 {
 char form[MAX_FORMAT]; // to store the format (`%...')
 char buff[MAX_ITEM];
 if (++arg > top)
 luaL_error(L, "missing argument #%d", arg);
 size_t formatItemSize = 0;
 strfrmt = scanformat(L, strfrmt, form, &formatItemSize);
 char formatIndicator = *strfrmt++;
 switch (formatIndicator)
 {
 case 'c':
 {
 snprintf(buff, sizeof(buff), form, (int)luaL_checknumber(L, arg));
 break;
 }
 case 'd':
 case 'i':
 {
 addInt64Format(form, formatIndicator, formatItemSize);
 snprintf(buff, sizeof(buff), form, (long long)luaL_checknumber(L, arg));
 break;
 }
 case 'o':
 case 'u':
 case 'x':
 case 'X':
 {
 double argValue = luaL_checknumber(L, arg);
 addInt64Format(form, formatIndicator, formatItemSize);
 unsigned long long v = (argValue < 0) ? (unsigned long long)(long long)argValue : (unsigned long long)argValue;
 snprintf(buff, sizeof(buff), form, v);
 break;
 }
 case 'e':
 case 'E':
 case 'f':
 case 'g':
 case 'G':
 {
 snprintf(buff, sizeof(buff), form, (double)luaL_checknumber(L, arg));
 break;
 }
 case 'q':
 {
 addquoted(L, &b, arg);
 continue;
 }
 case 's':
 {
 size_t l;
 const char* s = luaL_checklstring(L, arg, &l);
 if (!strchr(form, '.') && l >= 100)
 {
 lua_pushvalue(L, arg);
 luaL_addvalue(&b);
 continue;
 }
 else
 {
 snprintf(buff, sizeof(buff), form, s);
 break;
 }
 }
 case '*':
 {
 if (formatItemSize != 1)
 luaL_error(L, "'%%*' does not take a form");
 size_t length;
 const char* string = luaL_tolstring(L, arg, &length);
 luaL_addlstring(&b, string, length, -2);
 lua_pop(L, 1);
 continue;
 }
 default:
 {
 luaL_error(L, "invalid option '%%%c' to 'format'", *(strfrmt - 1));
 }
 }
 luaL_addlstring(&b, buff, strlen(buff), -1);
 }
 }
 luaL_pushresult(&b);
 return 1;
}
static int str_split(lua_State* L)
{
 size_t haystackLen;
 const char* haystack = luaL_checklstring(L, 1, &haystackLen);
 size_t needleLen;
 const char* needle = luaL_optlstring(L, 2, ",", &needleLen);
 const char* begin = haystack;
 const char* end = haystack + haystackLen;
 const char* spanStart = begin;
 int numMatches = 0;
 lua_createtable(L, 0, 0);
 if (needleLen == 0)
 begin++;
 for (const char* iter = begin; iter <= end - needleLen; iter++)
 {
 if (memcmp(iter, needle, needleLen) == 0)
 {
 lua_pushinteger(L, ++numMatches);
 lua_pushlstring(L, spanStart, iter - spanStart);
 lua_settable(L, -3);
 spanStart = iter + needleLen;
 if (needleLen > 0)
 iter += needleLen - 1;
 }
 }
 if (needleLen > 0)
 {
 lua_pushinteger(L, ++numMatches);
 lua_pushlstring(L, spanStart, end - spanStart);
 lua_settable(L, -3);
 }
 return 1;
}
#if !defined(LUAL_PACKPADBYTE)
#define LUAL_PACKPADBYTE 0x00
#endif
#define MAXINTSIZE 16
#define NB CHAR_BIT
#define MC ((1 << NB) - 1)
#define SZINT (int)sizeof(long long)
static const union
{
 int dummy;
 char little;
} nativeendian = {1};
#define MAXALIGN 8
typedef union Ftypes
{
 float f;
 double d;
 double n;
 char buff[5 * sizeof(double)];
} Ftypes;
typedef struct Header
{
 lua_State* L;
 int islittle;
 int maxalign;
} Header;
typedef enum KOption
{
 Kint,
 Kuint, // unsigned integers
 Kfloat,
 Kchar, // fixed-length strings
 Kstring,
 Kzstr, // zero-terminated strings
 Kpadding,
 Kpaddalign, // padding for alignment
 Knop
} KOption;
static int digit(int c)
{
 return '0' <= c && c <= '9';
}
static int getnum(Header* h, const char** fmt, int df)
{
 if (!digit(**fmt))
 return df; // return default value
 else
 {
 int a = 0;
 do
 {
 a = a * 10 + (*((*fmt)++) - '0');
 } while (digit(**fmt) && a <= (INT_MAX - 9) / 10);
 if (a > MAXSSIZE || digit(**fmt))
 luaL_error(h->L, "size specifier is too large");
 return a;
 }
}
static int getnumlimit(Header* h, const char** fmt, int df)
{
 int sz = getnum(h, fmt, df);
 if (sz > MAXINTSIZE || sz <= 0)
 luaL_error(h->L, "integral size (%d) out of limits [1,%d]", sz, MAXINTSIZE);
 return sz;
}
static void initheader(lua_State* L, Header* h)
{
 h->L = L;
 h->islittle = nativeendian.little;
 h->maxalign = 1;
}
static KOption getoption(Header* h, const char** fmt, int* size)
{
 int opt = *((*fmt)++);
 *size = 0;
 switch (opt)
 {
 case 'b':
 *size = 1;
 return Kint;
 case 'B':
 *size = 1;
 return Kuint;
 case 'h':
 *size = 2;
 return Kint;
 case 'H':
 *size = 2;
 return Kuint;
 case 'l':
 *size = 8;
 return Kint;
 case 'L':
 *size = 8;
 return Kuint;
 case 'j':
 *size = 4;
 return Kint;
 case 'J':
 *size = 4;
 return Kuint;
 case 'T':
 *size = 4;
 return Kuint;
 case 'f':
 *size = 4;
 return Kfloat;
 case 'd':
 *size = 8;
 return Kfloat;
 case 'n':
 *size = 8;
 return Kfloat;
 case 'i':
 *size = getnumlimit(h, fmt, 4);
 return Kint;
 case 'I':
 *size = getnumlimit(h, fmt, 4);
 return Kuint;
 case 's':
 *size = getnumlimit(h, fmt, 4);
 return Kstring;
 case 'c':
 *size = getnum(h, fmt, -1);
 if (*size == -1)
 luaL_error(h->L, "missing size for format option 'c'");
 return Kchar;
 case 'z':
 return Kzstr;
 case 'x':
 *size = 1;
 return Kpadding;
 case 'X':
 return Kpaddalign;
 case ' ':
 break;
 case '<':
 h->islittle = 1;
 break;
 case '>':
 h->islittle = 0;
 break;
 case '=':
 h->islittle = nativeendian.little;
 break;
 case '!':
 h->maxalign = getnumlimit(h, fmt, MAXALIGN);
 break;
 default:
 luaL_error(h->L, "invalid format option '%c'", opt);
 }
 return Knop;
}
static KOption getdetails(Header* h, size_t totalsize, const char** fmt, int* psize, int* ntoalign)
{
 KOption opt = getoption(h, fmt, psize);
 int align = *psize;
 if (opt == Kpaddalign)
 {
 if (**fmt == '\0' || getoption(h, fmt, &align) == Kchar || align == 0)
 luaL_argerror(h->L, 1, "invalid next option for option 'X'");
 }
 if (align <= 1 || opt == Kchar)
 *ntoalign = 0;
 else
 {
 if (align > h->maxalign)
 align = h->maxalign;
 if ((align & (align - 1)) != 0)
 luaL_argerror(h->L, 1, "format asks for alignment not power of 2");
 *ntoalign = (align - (int)(totalsize & (align - 1))) & (align - 1);
 }
 return opt;
}
static void packint(luaL_Buffer* b, unsigned long long n, int islittle, int size, int neg)
{
 LUAU_ASSERT(size <= MAXINTSIZE);
 char buff[MAXINTSIZE];
 int i;
 buff[islittle ? 0 : size - 1] = (char)(n & MC);
 for (i = 1; i < size; i++)
 {
 n >>= NB;
 buff[islittle ? i : size - 1 - i] = (char)(n & MC);
 }
 if (neg && size > SZINT)
 {
 for (i = SZINT; i < size; i++) // correct extra bytes
 buff[islittle ? i : size - 1 - i] = (char)MC;
 }
 luaL_addlstring(b, buff, size, -1);
}
static void copywithendian(volatile char* dest, volatile const char* src, int size, int islittle)
{
 if (islittle == nativeendian.little)
 {
 while (size-- != 0)
 *(dest++) = *(src++);
 }
 else
 {
 dest += size - 1;
 while (size-- != 0)
 *(dest--) = *(src++);
 }
}
static int str_pack(lua_State* L)
{
 luaL_Buffer b;
 Header h;
 const char* fmt = luaL_checkstring(L, 1);
 int arg = 1; // current argument to pack
 size_t totalsize = 0;
 initheader(L, &h);
 lua_pushnil(L);
 luaL_buffinit(L, &b);
 while (*fmt != '\0')
 {
 int size, ntoalign;
 KOption opt = getdetails(&h, totalsize, &fmt, &size, &ntoalign);
 totalsize += ntoalign + size;
 while (ntoalign-- > 0)
 luaL_addchar(&b, LUAL_PACKPADBYTE);
 arg++;
 switch (opt)
 {
 case Kint:
 {
 long long n = (long long)luaL_checknumber(L, arg);
 if (size < SZINT)
 {
 long long lim = (long long)1 << ((size * NB) - 1);
 luaL_argcheck(L, -lim <= n && n < lim, arg, "integer overflow");
 }
 packint(&b, n, h.islittle, size, (n < 0));
 break;
 }
 case Kuint:
 {
 long long n = (long long)luaL_checknumber(L, arg);
 if (size < SZINT)
 luaL_argcheck(L, (unsigned long long)n < ((unsigned long long)1 << (size * NB)), arg, "unsigned overflow");
 packint(&b, (unsigned long long)n, h.islittle, size, 0);
 break;
 }
 case Kfloat:
 {
 volatile Ftypes u;
 char buff[MAXINTSIZE];
 double n = luaL_checknumber(L, arg);
 if (size == sizeof(u.f))
 u.f = (float)n;
 else if (size == sizeof(u.d))
 u.d = (double)n;
 else
 u.n = n;
 copywithendian(buff, u.buff, size, h.islittle);
 luaL_addlstring(&b, buff, size, -1);
 break;
 }
 case Kchar:
 {
 size_t len;
 const char* s = luaL_checklstring(L, arg, &len);
 luaL_argcheck(L, len <= (size_t)size, arg, "string longer than given size");
 luaL_addlstring(&b, s, len, -1);
 while (len++ < (size_t)size) // pad extra space
 luaL_addchar(&b, LUAL_PACKPADBYTE);
 break;
 }
 case Kstring:
 {
 size_t len;
 const char* s = luaL_checklstring(L, arg, &len);
 luaL_argcheck(L, size >= (int)sizeof(size_t) || len < ((size_t)1 << (size * NB)), arg, "string length does not fit in given size");
 packint(&b, len, h.islittle, size, 0);
 luaL_addlstring(&b, s, len, -1);
 totalsize += len;
 break;
 }
 case Kzstr:
 {
 size_t len;
 const char* s = luaL_checklstring(L, arg, &len);
 luaL_argcheck(L, strlen(s) == len, arg, "string contains zeros");
 luaL_addlstring(&b, s, len, -1);
 luaL_addchar(&b, '\0');
 totalsize += len + 1;
 break;
 }
 case Kpadding:
 luaL_addchar(&b, LUAL_PACKPADBYTE);
 case Kpaddalign:
 case Knop:
 arg--;
 break;
 }
 }
 luaL_pushresult(&b);
 return 1;
}
static int str_packsize(lua_State* L)
{
 Header h;
 const char* fmt = luaL_checkstring(L, 1);
 int totalsize = 0; // accumulate total size of result
 initheader(L, &h);
 while (*fmt != '\0')
 {
 int size, ntoalign;
 KOption opt = getdetails(&h, totalsize, &fmt, &size, &ntoalign);
 luaL_argcheck(L, opt != Kstring && opt != Kzstr, 1, "variable-length format");
 size += ntoalign;
 luaL_argcheck(L, totalsize <= MAXSSIZE - size, 1, "format result too large");
 totalsize += size;
 }
 lua_pushinteger(L, totalsize);
 return 1;
}
static long long unpackint(lua_State* L, const char* str, int islittle, int size, int issigned)
{
 unsigned long long res = 0;
 int i;
 int limit = (size <= SZINT) ? size : SZINT;
 for (i = limit - 1; i >= 0; i--)
 {
 res <<= NB;
 res |= (unsigned char)str[islittle ? i : size - 1 - i];
 }
 if (size < SZINT)
 {
 if (issigned)
 {
 unsigned long long mask = (unsigned long long)1 << (size * NB - 1);
 res = ((res ^ mask) - mask);
 }
 }
 else if (size > SZINT)
 {
 int mask = (!issigned || (long long)res >= 0) ? 0 : MC;
 for (i = limit; i < size; i++)
 {
 if ((unsigned char)str[islittle ? i : size - 1 - i] != mask)
 luaL_error(L, "%d-byte integer does not fit into Lua Integer", size);
 }
 }
 return (long long)res;
}
static int str_unpack(lua_State* L)
{
 Header h;
 const char* fmt = luaL_checkstring(L, 1);
 size_t ld;
 const char* data = luaL_checklstring(L, 2, &ld);
 int pos = posrelat(luaL_optinteger(L, 3, 1), ld) - 1;
 if (pos < 0)
 pos = 0;
 int n = 0;
 luaL_argcheck(L, size_t(pos) <= ld, 3, "initial position out of string");
 initheader(L, &h);
 while (*fmt != '\0')
 {
 int size, ntoalign;
 KOption opt = getdetails(&h, pos, &fmt, &size, &ntoalign);
 luaL_argcheck(L, (size_t)ntoalign + size <= ld - pos, 2, "data string too short");
 pos += ntoalign;
 luaL_checkstack(L, 2, "too many results");
 n++;
 switch (opt)
 {
 case Kint:
 {
 long long res = unpackint(L, data + pos, h.islittle, size, true);
 lua_pushnumber(L, (double)res);
 break;
 }
 case Kuint:
 {
 unsigned long long res = unpackint(L, data + pos, h.islittle, size, false);
 lua_pushnumber(L, (double)res);
 break;
 }
 case Kfloat:
 {
 volatile Ftypes u;
 double num;
 copywithendian(u.buff, data + pos, size, h.islittle);
 if (size == sizeof(u.f))
 num = (double)u.f;
 else if (size == sizeof(u.d))
 num = (double)u.d;
 else
 num = u.n;
 lua_pushnumber(L, num);
 break;
 }
 case Kchar:
 {
 lua_pushlstring(L, data + pos, size);
 break;
 }
 case Kstring:
 {
 size_t len = (size_t)unpackint(L, data + pos, h.islittle, size, 0);
 luaL_argcheck(L, len <= ld - pos - size, 2, "data string too short");
 lua_pushlstring(L, data + pos + size, len);
 pos += (int)len;
 break;
 }
 case Kzstr:
 {
 size_t len = strlen(data + pos);
 luaL_argcheck(L, pos + len < ld, 2, "unfinished string for format 'z'");
 lua_pushlstring(L, data + pos, len);
 pos += (int)len + 1;
 break;
 }
 case Kpaddalign:
 case Kpadding:
 case Knop:
 n--;
 break;
 }
 pos += size;
 }
 lua_pushinteger(L, pos + 1);
 return n + 1;
}
static const luaL_Reg strlib[] = {
 {"byte", str_byte},
 {"char", str_char},
 {"find", str_find},
 {"format", str_format},
 {"gmatch", gmatch},
 {"gsub", str_gsub},
 {"len", str_len},
 {"lower", str_lower},
 {"match", str_match},
 {"rep", str_rep},
 {"reverse", str_reverse},
 {"sub", str_sub},
 {"upper", str_upper},
 {"split", str_split},
 {"pack", str_pack},
 {"packsize", str_packsize},
 {"unpack", str_unpack},
 {NULL, NULL},
};
static void createmetatable(lua_State* L)
{
 lua_createtable(L, 0, 1);
 lua_pushliteral(L, ""); // dummy string
 lua_pushvalue(L, -2);
 lua_setmetatable(L, -2);
 lua_pop(L, 1); // pop dummy string
 lua_pushvalue(L, -2);
 lua_setfield(L, -2, "__index"); // ...is the __index metamethod
 lua_pop(L, 1);
}
int luaopen_string(lua_State* L)
{
 luaL_register(L, LUA_STRLIBNAME, strlib);
 createmetatable(L);
 return 1;
}
#define MAXBITS 26
#define MAXSIZE (1 << MAXBITS)
static_assert(offsetof(LuaNode, val) == 0, "Unexpected Node memory layout, pointer cast in gval2slot is incorrect");
static_assert(TKey{{NULL}, {0}, LUA_TDEADKEY, 0}.tt == LUA_TDEADKEY, "not enough bits for tt");
static_assert(TKey{{NULL}, {0}, LUA_TNIL, MAXSIZE - 1}.next == MAXSIZE - 1, "not enough bits for next");
static_assert(TKey{{NULL}, {0}, LUA_TNIL, -(MAXSIZE - 1)}.next == -(MAXSIZE - 1), "not enough bits for next");
const LuaNode luaH_dummynode = {
 {{NULL}, {0}, LUA_TNIL},
 {{NULL}, {0}, LUA_TNIL, 0} // key
};
#define dummynode (&luaH_dummynode)
#define hashpow2(t, n) (gnode(t, lmod((n), sizenode(t))))
#define hashstr(t, str) hashpow2(t, (str)->hash)
#define hashboolean(t, p) hashpow2(t, p)
static LuaNode* hashpointer(const Table* t, const void* p)
{
 unsigned int h = unsigned(uintptr_t(p));
 h ^= h >> 16;
 h *= 0x85ebca6bu;
 h ^= h >> 13;
 h *= 0xc2b2ae35u;
 h ^= h >> 16;
 return hashpow2(t, h);
}
static LuaNode* hashnum(const Table* t, double n)
{
 static_assert(sizeof(double) == sizeof(unsigned int) * 2, "expected a 8-byte double");
 unsigned int i[2];
 memcpy(i, &n, sizeof(i));
 uint32_t h1 = i[0];
 uint32_t h2 = i[1] & 0x7fffffff;
 const uint32_t m = 0x5bd1e995;
 h1 ^= h2 >> 18;
 h1 *= m;
 h2 ^= h1 >> 22;
 h2 *= m;
 h1 ^= h2 >> 17;
 h1 *= m;
 h2 ^= h1 >> 19;
 h2 *= m;
 return hashpow2(t, h2);
}
static LuaNode* hashvec(const Table* t, const float* v)
{
 unsigned int i[LUA_VECTOR_SIZE];
 memcpy(i, v, sizeof(i));
 i[0] = (i[0] == 0x80000000) ? 0 : i[0];
 i[1] = (i[1] == 0x80000000) ? 0 : i[1];
 i[2] = (i[2] == 0x80000000) ? 0 : i[2];
 i[0] ^= i[0] >> 17;
 i[1] ^= i[1] >> 17;
 i[2] ^= i[2] >> 17;
 unsigned int h = (i[0] * 73856093) ^ (i[1] * 19349663) ^ (i[2] * 83492791);
#if LUA_VECTOR_SIZE == 4
 i[3] = (i[3] == 0x80000000) ? 0 : i[3];
 i[3] ^= i[3] >> 17;
 h ^= i[3] * 39916801;
#endif
 return hashpow2(t, h);
}
static LuaNode* mainposition(const Table* t, const TValue* key)
{
 switch (ttype(key))
 {
 case LUA_TNUMBER:
 return hashnum(t, nvalue(key));
 case LUA_TVECTOR:
 return hashvec(t, vvalue(key));
 case LUA_TSTRING:
 return hashstr(t, tsvalue(key));
 case LUA_TBOOLEAN:
 return hashboolean(t, bvalue(key));
 case LUA_TLIGHTUSERDATA:
 return hashpointer(t, pvalue(key));
 default:
 return hashpointer(t, gcvalue(key));
 }
}
static int arrayindex(double key)
{
 int i;
 luai_num2int(i, key);
 return luai_numeq(cast_num(i), key) ? i : -1;
}
static int findindex(lua_State* L, Table* t, StkId key)
{
 int i;
 if (ttisnil(key))
 return -1;
 i = ttisnumber(key) ? arrayindex(nvalue(key)) : -1;
 if (0 < i && i <= t->sizearray)
 return i - 1; // yes; that's the index (corrected to C)
 else
 {
 LuaNode* n = mainposition(t, key);
 for (;;)
 {
 if (luaO_rawequalKey(gkey(n), key) || (ttype(gkey(n)) == LUA_TDEADKEY && iscollectable(key) && gcvalue(gkey(n)) == gcvalue(key)))
 {
 i = cast_int(n - gnode(t, 0));
 return i + t->sizearray;
 }
 if (gnext(n) == 0)
 break;
 n += gnext(n);
 }
 luaG_runerror(L, "invalid key to 'next'"); // key not found
 }
}
int luaH_next(lua_State* L, Table* t, StkId key)
{
 int i = findindex(L, t, key);
 for (i++; i < t->sizearray; i++)
 {
 if (!ttisnil(&t->array[i]))
 {
 setnvalue(key, cast_num(i + 1));
 setobj2s(L, key + 1, &t->array[i]);
 return 1;
 }
 }
 for (i -= t->sizearray; i < sizenode(t); i++)
 {
 if (!ttisnil(gval(gnode(t, i))))
 {
 getnodekey(L, key, gnode(t, i));
 setobj2s(L, key + 1, gval(gnode(t, i)));
 return 1;
 }
 }
 return 0;
}
#define maybesetaboundary(t, boundary) { if (t->aboundary <= 0) t->aboundary = -int(boundary); }
#define getaboundary(t) (t->aboundary < 0 ? -t->aboundary : t->sizearray)
static int computesizes(int nums[], int* narray)
{
 int i;
 int twotoi;
 int a = 0; // number of elements smaller than 2^i
 int na = 0;
 int n = 0; // optimal size for array part
 for (i = 0, twotoi = 1; twotoi / 2 < *narray; i++, twotoi *= 2)
 {
 if (nums[i] > 0)
 {
 a += nums[i];
 if (a > twotoi / 2)
 {
 n = twotoi; // optimal size (till now)
 na = a;
 }
 }
 if (a == *narray)
 break;
 }
 *narray = n;
 LUAU_ASSERT(*narray / 2 <= na && na <= *narray);
 return na;
}
static int countint(double key, int* nums)
{
 int k = arrayindex(key);
 if (0 < k && k <= MAXSIZE)
 {
 nums[ceillog2(k)]++; // count as such
 return 1;
 }
 else
 return 0;
}
static int numusearray(const Table* t, int* nums)
{
 int lg;
 int ttlg;
 int ause = 0; // summation of `nums'
 int i = 1;
 for (lg = 0, ttlg = 1; lg <= MAXBITS; lg++, ttlg *= 2)
 {
 int lc = 0; // counter
 int lim = ttlg;
 if (lim > t->sizearray)
 {
 lim = t->sizearray;
 if (i > lim)
 break;
 }
 for (; i <= lim; i++)
 {
 if (!ttisnil(&t->array[i - 1]))
 lc++;
 }
 nums[lg] += lc;
 ause += lc;
 }
 return ause;
}
static int numusehash(const Table* t, int* nums, int* pnasize)
{
 int totaluse = 0;
 int ause = 0; // summation of `nums'
 int i = sizenode(t);
 while (i--)
 {
 LuaNode* n = &t->node[i];
 if (!ttisnil(gval(n)))
 {
 if (ttisnumber(gkey(n)))
 ause += countint(nvalue(gkey(n)), nums);
 totaluse++;
 }
 }
 *pnasize += ause;
 return totaluse;
}
static void setarrayvector(lua_State* L, Table* t, int size)
{
 if (size > MAXSIZE)
 luaG_runerror(L, "table overflow");
 luaM_reallocarray(L, t->array, t->sizearray, size, TValue, t->memcat);
 TValue* array = t->array;
 for (int i = t->sizearray; i < size; i++)
 setnilvalue(&array[i]);
 t->sizearray = size;
}
static void setnodevector(lua_State* L, Table* t, int size)
{
 int lsize;
 if (size == 0)
 {
 t->node = cast_to(LuaNode*, dummynode); // use common `dummynode'
 lsize = 0;
 }
 else
 {
 int i;
 lsize = ceillog2(size);
 if (lsize > MAXBITS)
 luaG_runerror(L, "table overflow");
 size = twoto(lsize);
 t->node = luaM_newarray(L, size, LuaNode, t->memcat);
 for (i = 0; i < size; i++)
 {
 LuaNode* n = gnode(t, i);
 gnext(n) = 0;
 setnilvalue(gkey(n));
 setnilvalue(gval(n));
 }
 }
 t->lsizenode = cast_byte(lsize);
 t->nodemask8 = cast_byte((1 << lsize) - 1);
 t->lastfree = size;
}
static TValue* newkey(lua_State* L, Table* t, const TValue* key);
static TValue* arrayornewkey(lua_State* L, Table* t, const TValue* key)
{
 if (ttisnumber(key))
 {
 int k;
 double n = nvalue(key);
 luai_num2int(k, n);
 if (luai_numeq(cast_num(k), n) && cast_to(unsigned int, k - 1) < cast_to(unsigned int, t->sizearray))
 return &t->array[k - 1];
 }
 return newkey(L, t, key);
}
static void resize(lua_State* L, Table* t, int nasize, int nhsize)
{
 if (nasize > MAXSIZE || nhsize > MAXSIZE)
 luaG_runerror(L, "table overflow");
 int oldasize = t->sizearray;
 int oldhsize = t->lsizenode;
 LuaNode* nold = t->node;
 if (nasize > oldasize) // array part must grow?
 setarrayvector(L, t, nasize);
 setnodevector(L, t, nhsize);
 LuaNode* nnew = t->node;
 if (nasize < oldasize)
 {
 t->sizearray = nasize;
 for (int i = nasize; i < oldasize; i++)
 {
 if (!ttisnil(&t->array[i]))
 {
 TValue ok;
 setnvalue(&ok, cast_num(i + 1));
 setobjt2t(L, newkey(L, t, &ok), &t->array[i]);
 }
 }
 luaM_reallocarray(L, t->array, oldasize, nasize, TValue, t->memcat);
 }
 TValue* anew = t->array;
 for (int i = twoto(oldhsize) - 1; i >= 0; i--)
 {
 LuaNode* old = nold + i;
 if (!ttisnil(gval(old)))
 {
 TValue ok;
 getnodekey(L, &ok, old);
 setobjt2t(L, arrayornewkey(L, t, &ok), gval(old));
 }
 }
 LUAU_ASSERT(nnew == t->node);
 LUAU_ASSERT(anew == t->array);
 if (nold != dummynode)
 luaM_freearray(L, nold, twoto(oldhsize), LuaNode, t->memcat);
}
static int adjustasize(Table* t, int size, const TValue* ek)
{
 bool tbound = t->node != dummynode || size < t->sizearray;
 int ekindex = ek && ttisnumber(ek) ? arrayindex(nvalue(ek)) : -1;
 while (size + 1 == ekindex || (tbound && !ttisnil(luaH_getnum(t, size + 1))))
 size++;
 return size;
}
void luaH_resizearray(lua_State* L, Table* t, int nasize)
{
 int nsize = (t->node == dummynode) ? 0 : sizenode(t);
 int asize = adjustasize(t, nasize, NULL);
 resize(L, t, asize, nsize);
}
void luaH_resizehash(lua_State* L, Table* t, int nhsize)
{
 resize(L, t, t->sizearray, nhsize);
}
static void rehash(lua_State* L, Table* t, const TValue* ek)
{
 int nums[MAXBITS + 1];
 for (int i = 0; i <= MAXBITS; i++)
 nums[i] = 0;
 int nasize = numusearray(t, nums); // count keys in array part
 int totaluse = nasize;
 totaluse += numusehash(t, nums, &nasize); // count keys in hash part
 if (ttisnumber(ek))
 nasize += countint(nvalue(ek), nums);
 totaluse++;
 int na = computesizes(nums, &nasize);
 int nh = totaluse - na;
 int nadjusted = adjustasize(t, nasize, ek);
 int aextra = nadjusted - nasize;
 if (aextra != 0)
 {
 nh -= aextra;
 nasize = nadjusted + aextra;
 nasize = adjustasize(t, nasize, ek);
 }
 resize(L, t, nasize, nh);
}
Table* luaH_new(lua_State* L, int narray, int nhash)
{
 Table* t = luaM_newgco(L, Table, sizeof(Table), L->activememcat);
 luaC_init(L, t, LUA_TTABLE);
 t->metatable = NULL;
 t->tmcache = cast_byte(~0);
 t->array = NULL;
 t->sizearray = 0;
 t->lastfree = 0;
 t->lsizenode = 0;
 t->readonly = 0;
 t->safeenv = 0;
 t->nodemask8 = 0;
 t->node = cast_to(LuaNode*, dummynode);
 if (narray > 0)
 setarrayvector(L, t, narray);
 if (nhash > 0)
 setnodevector(L, t, nhash);
 return t;
}
void luaH_free(lua_State* L, Table* t, lua_Page* page)
{
 if (t->node != dummynode)
 luaM_freearray(L, t->node, sizenode(t), LuaNode, t->memcat);
 if (t->array)
 luaM_freearray(L, t->array, t->sizearray, TValue, t->memcat);
 luaM_freegco(L, t, sizeof(Table), t->memcat, page);
}
static LuaNode* getfreepos(Table* t)
{
 while (t->lastfree > 0)
 {
 t->lastfree--;
 LuaNode* n = gnode(t, t->lastfree);
 if (ttisnil(gkey(n)))
 return n;
 }
 return NULL;
}
static TValue* newkey(lua_State* L, Table* t, const TValue* key)
{
 if (ttisnumber(key) && nvalue(key) == t->sizearray + 1)
 {
 rehash(L, t, key);
 return arrayornewkey(L, t, key);
 }
 LuaNode* mp = mainposition(t, key);
 if (!ttisnil(gval(mp)) || mp == dummynode)
 {
 LuaNode* n = getfreepos(t);
 if (n == NULL)
 {
 rehash(L, t, key); // grow table
 return arrayornewkey(L, t, key);
 }
 LUAU_ASSERT(n != dummynode);
 TValue mk;
 getnodekey(L, &mk, mp);
 LuaNode* othern = mainposition(t, &mk);
 if (othern != mp)
 {
 while (othern + gnext(othern) != mp)
 othern += gnext(othern);
 gnext(othern) = cast_int(n - othern); // redo the chain with `n' in place of `mp'
 *n = *mp;
 if (gnext(mp) != 0)
 {
 gnext(n) += cast_int(mp - n);
 gnext(mp) = 0; // now 'mp' is free
 }
 setnilvalue(gval(mp));
 }
 else
 {
 if (gnext(mp) != 0)
 gnext(n) = cast_int((mp + gnext(mp)) - n);
 else
 LUAU_ASSERT(gnext(n) == 0);
 gnext(mp) = cast_int(n - mp);
 mp = n;
 }
 }
 setnodekey(L, mp, key);
 luaC_barriert(L, t, key);
 LUAU_ASSERT(ttisnil(gval(mp)));
 return gval(mp);
}
const TValue* luaH_getnum(Table* t, int key)
{
 if (cast_to(unsigned int, key - 1) < cast_to(unsigned int, t->sizearray))
 return &t->array[key - 1];
 else if (t->node != dummynode)
 {
 double nk = cast_num(key);
 LuaNode* n = hashnum(t, nk);
 for (;;)
 {
 if (ttisnumber(gkey(n)) && luai_numeq(nvalue(gkey(n)), nk))
 return gval(n);
 if (gnext(n) == 0)
 break;
 n += gnext(n);
 }
 return luaO_nilobject;
 }
 else
 return luaO_nilobject;
}
const TValue* luaH_getstr(Table* t, TString* key)
{
 LuaNode* n = hashstr(t, key);
 for (;;)
 {
 if (ttisstring(gkey(n)) && tsvalue(gkey(n)) == key)
 return gval(n);
 if (gnext(n) == 0)
 break;
 n += gnext(n);
 }
 return luaO_nilobject;
}
const TValue* luaH_get(Table* t, const TValue* key)
{
 switch (ttype(key))
 {
 case LUA_TNIL:
 return luaO_nilobject;
 case LUA_TSTRING:
 return luaH_getstr(t, tsvalue(key));
 case LUA_TNUMBER:
 {
 int k;
 double n = nvalue(key);
 luai_num2int(k, n);
 if (luai_numeq(cast_num(k), nvalue(key)))
 return luaH_getnum(t, k); // use specialized version
 }
 default:
 {
 LuaNode* n = mainposition(t, key);
 for (;;)
 {
 if (luaO_rawequalKey(gkey(n), key))
 return gval(n);
 if (gnext(n) == 0)
 break;
 n += gnext(n);
 }
 return luaO_nilobject;
 }
 }
}
TValue* luaH_set(lua_State* L, Table* t, const TValue* key)
{
 const TValue* p = luaH_get(t, key);
 invalidateTMcache(t);
 if (p != luaO_nilobject)
 return cast_to(TValue*, p);
 else
 return luaH_newkey(L, t, key);
}
TValue* luaH_newkey(lua_State* L, Table* t, const TValue* key)
{
 if (ttisnil(key))
 luaG_runerror(L, "table index is nil");
 else if (ttisnumber(key) && luai_numisnan(nvalue(key)))
 luaG_runerror(L, "table index is NaN");
 else if (ttisvector(key) && luai_vecisnan(vvalue(key)))
 luaG_runerror(L, "table index contains NaN");
 return newkey(L, t, key);
}
TValue* luaH_setnum(lua_State* L, Table* t, int key)
{
 if (cast_to(unsigned int, key - 1) < cast_to(unsigned int, t->sizearray))
 return &t->array[key - 1];
 const TValue* p = luaH_getnum(t, key);
 if (p != luaO_nilobject)
 return cast_to(TValue*, p);
 else
 {
 TValue k;
 setnvalue(&k, cast_num(key));
 return newkey(L, t, &k);
 }
}
TValue* luaH_setstr(lua_State* L, Table* t, TString* key)
{
 const TValue* p = luaH_getstr(t, key);
 invalidateTMcache(t);
 if (p != luaO_nilobject)
 return cast_to(TValue*, p);
 else
 {
 TValue k;
 setsvalue(L, &k, key);
 return newkey(L, t, &k);
 }
}
static int updateaboundary(Table* t, int boundary)
{
 if (boundary < t->sizearray && ttisnil(&t->array[boundary - 1]))
 {
 if (boundary >= 2 && !ttisnil(&t->array[boundary - 2]))
 {
 maybesetaboundary(t, boundary - 1);
 return boundary - 1;
 }
 }
 else if (boundary + 1 < t->sizearray && !ttisnil(&t->array[boundary]) && ttisnil(&t->array[boundary + 1]))
 {
 maybesetaboundary(t, boundary + 1);
 return boundary + 1;
 }
 return 0;
}
int luaH_getn(Table* t)
{
 int boundary = getaboundary(t);
 if (boundary > 0)
 {
 if (!ttisnil(&t->array[t->sizearray - 1]) && t->node == dummynode)
 return t->sizearray;
 if (boundary < t->sizearray && !ttisnil(&t->array[boundary - 1]) && ttisnil(&t->array[boundary]))
 return boundary;
 int foundboundary = updateaboundary(t, boundary);
 if (foundboundary > 0)
 return foundboundary;
 }
 int j = t->sizearray;
 if (j > 0 && ttisnil(&t->array[j - 1]))
 {
 TValue* base = t->array;
 int rest = j;
 while (int half = rest >> 1)
 {
 base = ttisnil(&base[half]) ? base : base + half;
 rest -= half;
 }
 int boundary = !ttisnil(base) + int(base - t->array);
 maybesetaboundary(t, boundary);
 return boundary;
 }
 else
 {
 LUAU_ASSERT(t->node == dummynode || ttisnil(luaH_getnum(t, j + 1)));
 return j;
 }
}
Table* luaH_clone(lua_State* L, Table* tt)
{
 Table* t = luaM_newgco(L, Table, sizeof(Table), L->activememcat);
 luaC_init(L, t, LUA_TTABLE);
 t->metatable = tt->metatable;
 t->tmcache = tt->tmcache;
 t->array = NULL;
 t->sizearray = 0;
 t->lsizenode = 0;
 t->nodemask8 = 0;
 t->readonly = 0;
 t->safeenv = 0;
 t->node = cast_to(LuaNode*, dummynode);
 t->lastfree = 0;
 if (tt->sizearray)
 {
 t->array = luaM_newarray(L, tt->sizearray, TValue, t->memcat);
 maybesetaboundary(t, getaboundary(tt));
 t->sizearray = tt->sizearray;
 memcpy(t->array, tt->array, t->sizearray * sizeof(TValue));
 }
 if (tt->node != dummynode)
 {
 int size = 1 << tt->lsizenode;
 t->node = luaM_newarray(L, size, LuaNode, t->memcat);
 t->lsizenode = tt->lsizenode;
 t->nodemask8 = tt->nodemask8;
 memcpy(t->node, tt->node, size * sizeof(LuaNode));
 t->lastfree = tt->lastfree;
 }
 return t;
}
void luaH_clear(Table* tt)
{
 for (int i = 0; i < tt->sizearray; ++i)
 {
 setnilvalue(&tt->array[i]);
 }
 maybesetaboundary(tt, 0);
 if (tt->node != dummynode)
 {
 int size = sizenode(tt);
 tt->lastfree = size;
 for (int i = 0; i < size; ++i)
 {
 LuaNode* n = gnode(tt, i);
 setnilvalue(gkey(n));
 setnilvalue(gval(n));
 gnext(n) = 0;
 }
 }
 tt->tmcache = cast_byte(~0);
}
static int foreachi(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 luaL_checktype(L, 2, LUA_TFUNCTION);
 int i;
 int n = lua_objlen(L, 1);
 for (i = 1; i <= n; i++)
 {
 lua_pushvalue(L, 2);
 lua_pushinteger(L, i); // 1st argument
 lua_rawgeti(L, 1, i);
 lua_call(L, 2, 1);
 if (!lua_isnil(L, -1))
 return 1;
 lua_pop(L, 1);
 }
 return 0;
}
static int foreach (lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 luaL_checktype(L, 2, LUA_TFUNCTION);
 lua_pushnil(L);
 while (lua_next(L, 1))
 {
 lua_pushvalue(L, 2);
 lua_pushvalue(L, -3); // key
 lua_pushvalue(L, -3);
 lua_call(L, 2, 1);
 if (!lua_isnil(L, -1))
 return 1;
 lua_pop(L, 2);
 }
 return 0;
}
static int maxn(lua_State* L)
{
 double max = 0;
 luaL_checktype(L, 1, LUA_TTABLE);
 lua_pushnil(L);
 while (lua_next(L, 1))
 {
 lua_pop(L, 1);
 if (lua_type(L, -1) == LUA_TNUMBER)
 {
 double v = lua_tonumber(L, -1);
 if (v > max)
 max = v;
 }
 }
 lua_pushnumber(L, max);
 return 1;
}
static int getn(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 lua_pushinteger(L, lua_objlen(L, 1));
 return 1;
}
static void moveelements(lua_State* L, int srct, int dstt, int f, int e, int t)
{
 Table* src = hvalue(L->base + (srct - 1));
 Table* dst = hvalue(L->base + (dstt - 1));
 if (dst->readonly)
 luaG_readonlyerror(L);
 int n = e - f + 1;
 if (cast_to(unsigned int, f - 1) < cast_to(unsigned int, src->sizearray) &&
 cast_to(unsigned int, t - 1) < cast_to(unsigned int, dst->sizearray) &&
 cast_to(unsigned int, f - 1 + n) <= cast_to(unsigned int, src->sizearray) &&
 cast_to(unsigned int, t - 1 + n) <= cast_to(unsigned int, dst->sizearray))
 {
 TValue* srcarray = src->array;
 TValue* dstarray = dst->array;
 if (t > e || t <= f || (dstt != srct && dst != src))
 {
 for (int i = 0; i < n; ++i)
 {
 TValue* s = &srcarray[f + i - 1];
 TValue* d = &dstarray[t + i - 1];
 setobj2t(L, d, s);
 }
 }
 else
 {
 for (int i = n - 1; i >= 0; i--)
 {
 TValue* s = &srcarray[(f + i) - 1];
 TValue* d = &dstarray[(t + i) - 1];
 setobj2t(L, d, s);
 }
 }
 luaC_barrierfast(L, dst);
 }
 else
 {
 if (t > e || t <= f || dst != src)
 {
 for (int i = 0; i < n; ++i)
 {
 lua_rawgeti(L, srct, f + i);
 lua_rawseti(L, dstt, t + i);
 }
 }
 else
 {
 for (int i = n - 1; i >= 0; i--)
 {
 lua_rawgeti(L, srct, f + i);
 lua_rawseti(L, dstt, t + i);
 }
 }
 }
}
static int tinsert(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 int n = lua_objlen(L, 1);
 int pos;
 switch (lua_gettop(L))
 {
 case 2:
 {
 pos = n + 1; // insert new element at the end
 break;
 }
 case 3:
 {
 pos = luaL_checkinteger(L, 2);
 if (1 <= pos && pos <= n)
 moveelements(L, 1, 1, pos, n, pos + 1);
 break;
 }
 default:
 {
 luaL_error(L, "wrong number of arguments to 'insert'");
 }
 }
 lua_rawseti(L, 1, pos);
 return 0;
}
static int tremove(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 int n = lua_objlen(L, 1);
 int pos = luaL_optinteger(L, 2, n);
 if (!(1 <= pos && pos <= n))
 return 0; // nothing to remove
 lua_rawgeti(L, 1, pos);
 moveelements(L, 1, 1, pos + 1, n, pos);
 lua_pushnil(L);
 lua_rawseti(L, 1, n);
 return 1;
}
static int tmove(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 int f = luaL_checkinteger(L, 2);
 int e = luaL_checkinteger(L, 3);
 int t = luaL_checkinteger(L, 4);
 int tt = !lua_isnoneornil(L, 5) ? 5 : 1;
 luaL_checktype(L, tt, LUA_TTABLE);
 if (e >= f)
 {
 luaL_argcheck(L, f > 0 || e < INT_MAX + f, 3, "too many elements to move");
 int n = e - f + 1;
 luaL_argcheck(L, t <= INT_MAX - n + 1, 4, "destination wrap around");
 Table* dst = hvalue(L->base + (tt - 1));
 if (dst->readonly)
 luaG_readonlyerror(L);
 if (t > 0 && (t - 1) <= dst->sizearray && (t - 1 + n) > dst->sizearray)
 {
 luaH_resizearray(L, dst, t - 1 + n);
 }
 moveelements(L, 1, tt, f, e, t);
 }
 lua_pushvalue(L, tt);
 return 1;
}
static void addfield(lua_State* L, luaL_Buffer* b, int i)
{
 lua_rawgeti(L, 1, i);
 if (!lua_isstring(L, -1))
 luaL_error(L, "invalid value (%s) at index %d in table for 'concat'", luaL_typename(L, -1), i);
 luaL_addvalue(b);
}
static int tconcat(lua_State* L)
{
 luaL_Buffer b;
 size_t lsep;
 int i, last;
 const char* sep = luaL_optlstring(L, 2, "", &lsep);
 luaL_checktype(L, 1, LUA_TTABLE);
 i = luaL_optinteger(L, 3, 1);
 last = luaL_opt(L, luaL_checkinteger, 4, lua_objlen(L, 1));
 luaL_buffinit(L, &b);
 for (; i < last; i++)
 {
 addfield(L, &b, i);
 luaL_addlstring(&b, sep, lsep, -1);
 }
 if (i == last)
 addfield(L, &b, i);
 luaL_pushresult(&b);
 return 1;
}
static int tpack(lua_State* L)
{
 int n = lua_gettop(L);
 lua_createtable(L, n, 1); // create result table
 Table* t = hvalue(L->top - 1);
 for (int i = 0; i < n; ++i)
 {
 TValue* e = &t->array[i];
 setobj2t(L, e, L->base + i);
 }
 TValue* nv = luaH_setstr(L, t, luaS_newliteral(L, "n"));
 setnvalue(nv, n);
 return 1;
}
static int tunpack(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 Table* t = hvalue(L->base);
 int i = luaL_optinteger(L, 2, 1);
 int e = luaL_opt(L, luaL_checkinteger, 3, lua_objlen(L, 1));
 if (i > e)
 return 0;
 unsigned n = (unsigned)e - i; // number of elements minus 1 (avoid overflows)
 if (n >= (unsigned int)INT_MAX || !lua_checkstack(L, (int)(++n)))
 luaL_error(L, "too many results to unpack");
 if (i == 1 && int(n) <= t->sizearray)
 {
 for (i = 0; i < int(n); i++)
 setobj2s(L, L->top + i, &t->array[i]);
 L->top += n;
 }
 else
 {
 for (; i < e; i++)
 lua_rawgeti(L, 1, i);
 lua_rawgeti(L, 1, e);
 }
 return (int)n;
}
typedef int (*SortPredicate)(lua_State* L, const TValue* l, const TValue* r);
static int sort_func(lua_State* L, const TValue* l, const TValue* r)
{
 LUAU_ASSERT(L->top == L->base + 2);
 setobj2s(L, L->top, &L->base[1]);
 setobj2s(L, L->top + 1, l);
 setobj2s(L, L->top + 2, r);
 L->top += 3;
 luaD_call(L, L->top - 3, 1);
 L->top -= 1;
 return !l_isfalse(L->top);
}
inline void sort_swap(lua_State* L, Table* t, int i, int j)
{
 TValue* arr = t->array;
 int n = t->sizearray;
 LUAU_ASSERT(unsigned(i) < unsigned(n) && unsigned(j) < unsigned(n));
 TValue temp;
 setobj2s(L, &temp, &arr[i]);
 setobj2t(L, &arr[i], &arr[j]);
 setobj2t(L, &arr[j], &temp);
}
inline int sort_less(lua_State* L, Table* t, int i, int j, SortPredicate pred)
{
 TValue* arr = t->array;
 int n = t->sizearray;
 LUAU_ASSERT(unsigned(i) < unsigned(n) && unsigned(j) < unsigned(n));
 int res = pred(L, &arr[i], &arr[j]);
 if (t->sizearray != n)
 luaL_error(L, "table modified during sorting");
 return res;
}
static void sort_siftheap(lua_State* L, Table* t, int l, int u, SortPredicate pred, int root)
{
 LUAU_ASSERT(l <= u);
 int count = u - l + 1;
 while (root * 2 + 2 < count)
 {
 int left = root * 2 + 1, right = root * 2 + 2;
 int next = root;
 next = sort_less(L, t, l + next, l + left, pred) ? left : next;
 next = sort_less(L, t, l + next, l + right, pred) ? right : next;
 if (next == root)
 break;
 sort_swap(L, t, l + root, l + next);
 root = next;
 }
 int lastleft = root * 2 + 1;
 if (lastleft == count - 1 && sort_less(L, t, l + root, l + lastleft, pred))
 sort_swap(L, t, l + root, l + lastleft);
}
static void sort_heap(lua_State* L, Table* t, int l, int u, SortPredicate pred)
{
 LUAU_ASSERT(l <= u);
 int count = u - l + 1;
 for (int i = count / 2 - 1; i >= 0; --i)
 sort_siftheap(L, t, l, u, pred, i);
 for (int i = count - 1; i > 0; --i)
 {
 sort_swap(L, t, l, l + i);
 sort_siftheap(L, t, l, l + i - 1, pred, 0);
 }
}
static void sort_rec(lua_State* L, Table* t, int l, int u, int limit, SortPredicate pred)
{
 while (l < u)
 {
 if (limit == 0)
 return sort_heap(L, t, l, u, pred);
 if (sort_less(L, t, u, l, pred))
 sort_swap(L, t, u, l); // swap a[l] - a[u]
 if (u - l == 1)
 break;
 int m = l + ((u - l) >> 1); // midpoint
 if (sort_less(L, t, m, l, pred))
 sort_swap(L, t, m, l);
 else if (sort_less(L, t, u, m, pred))
 sort_swap(L, t, m, u);
 if (u - l == 2)
 break;
 int p = u - 1;
 sort_swap(L, t, m, u - 1);
 int i = l;
 int j = u - 1;
 for (;;)
 {
 while (sort_less(L, t, ++i, p, pred))
 {
 if (i >= u)
 luaL_error(L, "invalid order function for sorting");
 }
 while (sort_less(L, t, p, --j, pred))
 {
 if (j <= l)
 luaL_error(L, "invalid order function for sorting");
 }
 if (j < i)
 break;
 sort_swap(L, t, i, j);
 }
 sort_swap(L, t, p, i);
 limit = (limit >> 1) + (limit >> 2);
 if (i - l < u - i)
 {
 sort_rec(L, t, l, i - 1, limit, pred);
 l = i + 1;
 }
 else
 {
 sort_rec(L, t, i + 1, u, limit, pred);
 u = i - 1;
 }
 }
}
static int tsort(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 Table* t = hvalue(L->base);
 int n = luaH_getn(t);
 if (t->readonly)
 luaG_readonlyerror(L);
 SortPredicate pred = luaV_lessthan;
 if (!lua_isnoneornil(L, 2))
 {
 luaL_checktype(L, 2, LUA_TFUNCTION);
 pred = sort_func;
 }
 lua_settop(L, 2);
 if (n > 0)
 sort_rec(L, t, 0, n - 1, n, pred);
 return 0;
}
static int tcreate(lua_State* L)
{
 int size = luaL_checkinteger(L, 1);
 if (size < 0)
 luaL_argerror(L, 1, "size out of range");
 if (!lua_isnoneornil(L, 2))
 {
 lua_createtable(L, size, 0);
 Table* t = hvalue(L->top - 1);
 StkId v = L->base + 1;
 for (int i = 0; i < size; ++i)
 {
 TValue* e = &t->array[i];
 setobj2t(L, e, v);
 }
 }
 else
 {
 lua_createtable(L, size, 0);
 }
 return 1;
}
static int tfind(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 luaL_checkany(L, 2);
 int init = luaL_optinteger(L, 3, 1);
 if (init < 1)
 luaL_argerror(L, 3, "index out of range");
 Table* t = hvalue(L->base);
 StkId v = L->base + 1;
 for (int i = init;; ++i)
 {
 const TValue* e = luaH_getnum(t, i);
 if (ttisnil(e))
 break;
 if (equalobj(L, v, e))
 {
 lua_pushinteger(L, i);
 return 1;
 }
 }
 lua_pushnil(L);
 return 1;
}
static int tclear(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 Table* tt = hvalue(L->base);
 if (tt->readonly)
 luaG_readonlyerror(L);
 luaH_clear(tt);
 return 0;
}
static int tfreeze(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 luaL_argcheck(L, !lua_getreadonly(L, 1), 1, "table is already frozen");
 luaL_argcheck(L, !luaL_getmetafield(L, 1, "__metatable"), 1, "table has a protected metatable");
 lua_setreadonly(L, 1, true);
 lua_pushvalue(L, 1);
 return 1;
}
static int tisfrozen(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 lua_pushboolean(L, lua_getreadonly(L, 1));
 return 1;
}
static int tclone(lua_State* L)
{
 luaL_checktype(L, 1, LUA_TTABLE);
 luaL_argcheck(L, !luaL_getmetafield(L, 1, "__metatable"), 1, "table has a protected metatable");
 Table* tt = luaH_clone(L, hvalue(L->base));
 TValue v;
 sethvalue(L, &v, tt);
 luaA_pushobject(L, &v);
 return 1;
}
static const luaL_Reg tab_funcs[] = {
 {"concat", tconcat},
 {"foreach", foreach},
 {"foreachi", foreachi},
 {"getn", getn},
 {"maxn", maxn},
 {"insert", tinsert},
 {"remove", tremove},
 {"sort", tsort},
 {"pack", tpack},
 {"unpack", tunpack},
 {"move", tmove},
 {"create", tcreate},
 {"find", tfind},
 {"clear", tclear},
 {"freeze", tfreeze},
 {"isfrozen", tisfrozen},
 {"clone", tclone},
 {NULL, NULL},
};
int luaopen_table(lua_State* L)
{
 luaL_register(L, LUA_TABLIBNAME, tab_funcs);
 lua_pushcfunction(L, tunpack, "unpack");
 lua_setglobal(L, "unpack");
 return 1;
}
const char* const luaT_typenames[] = {
 "nil",
 "boolean",
 "userdata",
 "number",
 "vector",
 "string",
 "table",
 "function",
 "userdata",
 "thread",
};
const char* const luaT_eventname[] = {
 "__index",
 "__newindex",
 "__mode",
 "__namecall",
 "__call",
 "__iter",
 "__len",
 "__eq",
 "__add",
 "__sub",
 "__mul",
 "__div",
 "__mod",
 "__pow",
 "__unm",
 "__lt",
 "__le",
 "__concat",
 "__type",
 "__metatable",
};
static_assert(sizeof(luaT_typenames) / sizeof(luaT_typenames[0]) == LUA_T_COUNT, "luaT_typenames size mismatch");
static_assert(sizeof(luaT_eventname) / sizeof(luaT_eventname[0]) == TM_N, "luaT_eventname size mismatch");
static_assert(TM_EQ < 8, "fasttm optimization stores a bitfield with metamethods in a byte");
void luaT_init(lua_State* L)
{
 int i;
 for (i = 0; i < LUA_T_COUNT; i++)
 {
 L->global->ttname[i] = luaS_new(L, luaT_typenames[i]);
 luaS_fix(L->global->ttname[i]);
 }
 for (i = 0; i < TM_N; i++)
 {
 L->global->tmname[i] = luaS_new(L, luaT_eventname[i]);
 luaS_fix(L->global->tmname[i]);
 }
}
const TValue* luaT_gettm(Table* events, TMS event, TString* ename)
{
 const TValue* tm = luaH_getstr(events, ename);
 LUAU_ASSERT(event <= TM_EQ);
 if (ttisnil(tm))
 {
 events->tmcache |= cast_byte(1u << event); // cache this fact
 return NULL;
 }
 else
 return tm;
}
const TValue* luaT_gettmbyobj(lua_State* L, const TValue* o, TMS event)
{
 Table* mt;
 switch (ttype(o))
 {
 case LUA_TTABLE:
 mt = hvalue(o)->metatable;
 break;
 case LUA_TUSERDATA:
 mt = uvalue(o)->metatable;
 break;
 default:
 mt = L->global->mt[ttype(o)];
 }
 return (mt ? luaH_getstr(mt, L->global->tmname[event]) : luaO_nilobject);
}
const TString* luaT_objtypenamestr(lua_State* L, const TValue* o)
{
 if (ttisuserdata(o) && uvalue(o)->tag != UTAG_PROXY && uvalue(o)->metatable)
 {
 const TValue* type = luaH_getstr(uvalue(o)->metatable, L->global->tmname[TM_TYPE]);
 if (ttisstring(type))
 return tsvalue(type);
 }
 else if (Table* mt = L->global->mt[ttype(o)])
 {
 const TValue* type = luaH_getstr(mt, L->global->tmname[TM_TYPE]);
 if (ttisstring(type))
 return tsvalue(type);
 }
 return L->global->ttname[ttype(o)];
}
const char* luaT_objtypename(lua_State* L, const TValue* o)
{
 return getstr(luaT_objtypenamestr(L, o));
}
Udata* luaU_newudata(lua_State* L, size_t s, int tag)
{
 if (s > INT_MAX - sizeof(Udata))
 luaM_toobig(L);
 Udata* u = luaM_newgco(L, Udata, sizeudata(s), L->activememcat);
 luaC_init(L, u, LUA_TUSERDATA);
 u->len = int(s);
 u->metatable = NULL;
 LUAU_ASSERT(tag >= 0 && tag <= 255);
 u->tag = uint8_t(tag);
 return u;
}
void luaU_freeudata(lua_State* L, Udata* u, lua_Page* page)
{
 if (u->tag < LUA_UTAG_LIMIT)
 {
 lua_Destructor dtor = L->global->udatagc[u->tag];
 if (dtor)
 dtor(L, u->data);
 }
 else if (u->tag == UTAG_IDTOR)
 {
 void (*dtor)(void*) = nullptr;
 memcpy(&dtor, &u->data + u->len - sizeof(dtor), sizeof(dtor));
 if (dtor)
 dtor(u->data);
 }
 luaM_freegco(L, u, sizeudata(u->len), u->memcat, page);
}
#define MAXUNICODE 0x10FFFF
#define iscont(p) ((*(p)&0xC0) == 0x80)
static int u_posrelat(int pos, size_t len)
{
 if (pos >= 0)
 return pos;
 else if (0u - (size_t)pos > len)
 return 0;
 else
 return (int)len + pos + 1;
}
static const char* utf8_decode(const char* o, int* val)
{
 static const unsigned int limits[] = {0xFF, 0x7F, 0x7FF, 0xFFFF};
 const unsigned char* s = (const unsigned char*)o;
 unsigned int c = s[0];
 unsigned int res = 0;
 if (c < 0x80) // ascii?
 res = c;
 else
 {
 int count = 0;
 while (c & 0x40)
 {
 int cc = s[++count]; // read next byte
 if ((cc & 0xC0) != 0x80)
 return NULL; // invalid byte sequence
 res = (res << 6) | (cc & 0x3F);
 c <<= 1; // to test next bit
 }
 res |= ((c & 0x7F) << (count * 5));
 if (count > 3 || res > MAXUNICODE || res <= limits[count])
 return NULL;
 s += count; // skip continuation bytes read
 }
 if (val)
 *val = res;
 return (const char*)s + 1;
}
static int utflen(lua_State* L)
{
 int n = 0;
 size_t len;
 const char* s = luaL_checklstring(L, 1, &len);
 int posi = u_posrelat(luaL_optinteger(L, 2, 1), len);
 int posj = u_posrelat(luaL_optinteger(L, 3, -1), len);
 luaL_argcheck(L, 1 <= posi && --posi <= (int)len, 2, "initial position out of string");
 luaL_argcheck(L, --posj < (int)len, 3, "final position out of string");
 while (posi <= posj)
 {
 const char* s1 = utf8_decode(s + posi, NULL);
 if (s1 == NULL)
 {
 lua_pushnil(L); // return nil ...
 lua_pushinteger(L, posi + 1);
 return 2;
 }
 posi = (int)(s1 - s);
 n++;
 }
 lua_pushinteger(L, n);
 return 1;
}
static int codepoint(lua_State* L)
{
 size_t len;
 const char* s = luaL_checklstring(L, 1, &len);
 int posi = u_posrelat(luaL_optinteger(L, 2, 1), len);
 int pose = u_posrelat(luaL_optinteger(L, 3, posi), len);
 int n;
 const char* se;
 luaL_argcheck(L, posi >= 1, 2, "out of range");
 luaL_argcheck(L, pose <= (int)len, 3, "out of range");
 if (posi > pose)
 return 0;
 if (pose - posi >= INT_MAX) // (int -> int) overflow?
 luaL_error(L, "string slice too long");
 n = (int)(pose - posi) + 1;
 luaL_checkstack(L, n, "string slice too long");
 n = 0;
 se = s + pose;
 for (s += posi - 1; s < se;)
 {
 int code;
 s = utf8_decode(s, &code);
 if (s == NULL)
 luaL_error(L, "invalid UTF-8 code");
 lua_pushinteger(L, code);
 n++;
 }
 return n;
}
#define UTF8BUFFSZ 8
static int luaO_utf8esc(char* buff, unsigned long x)
{
 int n = 1;
 LUAU_ASSERT(x <= 0x10FFFF);
 if (x < 0x80)
 buff[UTF8BUFFSZ - 1] = cast_to(char, x);
 else
 {
 unsigned int mfb = 0x3f; // maximum that fits in first byte
 do
 {
 buff[UTF8BUFFSZ - (n++)] = cast_to(char, 0x80 | (x & 0x3f));
 x >>= 6;
 mfb >>= 1; // now there is one less bit available in first byte
 } while (x > mfb);
 buff[UTF8BUFFSZ - n] = cast_to(char, (~mfb << 1) | x); // add first byte
 }
 return n;
}
static int buffutfchar(lua_State* L, int arg, char* buff, const char** charstr)
{
 int code = luaL_checkinteger(L, arg);
 luaL_argcheck(L, 0 <= code && code <= MAXUNICODE, arg, "value out of range");
 int l = luaO_utf8esc(buff, cast_to(long, code));
 *charstr = buff + UTF8BUFFSZ - l;
 return l;
}
static int utfchar(lua_State* L)
{
 char buff[UTF8BUFFSZ];
 const char* charstr;
 int n = lua_gettop(L);
 if (n == 1)
 {
 int l = buffutfchar(L, 1, buff, &charstr);
 lua_pushlstring(L, charstr, l);
 }
 else
 {
 luaL_Buffer b;
 luaL_buffinit(L, &b);
 for (int i = 1; i <= n; i++)
 {
 int l = buffutfchar(L, i, buff, &charstr);
 luaL_addlstring(&b, charstr, l, -1);
 }
 luaL_pushresult(&b);
 }
 return 1;
}
static int byteoffset(lua_State* L)
{
 size_t len;
 const char* s = luaL_checklstring(L, 1, &len);
 int n = luaL_checkinteger(L, 2);
 int posi = (n >= 0) ? 1 : (int)len + 1;
 posi = u_posrelat(luaL_optinteger(L, 3, posi), len);
 luaL_argcheck(L, 1 <= posi && --posi <= (int)len, 3, "position out of range");
 if (n == 0)
 {
 while (posi > 0 && iscont(s + posi))
 posi--;
 }
 else
 {
 if (iscont(s + posi))
 luaL_error(L, "initial position is a continuation byte");
 if (n < 0)
 {
 while (n < 0 && posi > 0)
 {
 do
 {
 posi--;
 } while (posi > 0 && iscont(s + posi));
 n++;
 }
 }
 else
 {
 n--;
 while (n > 0 && posi < (int)len)
 {
 do
 {
 posi++;
 } while (iscont(s + posi));
 n--;
 }
 }
 }
 if (n == 0)
 lua_pushinteger(L, posi + 1);
 else
 lua_pushnil(L);
 return 1;
}
static int iter_aux(lua_State* L)
{
 size_t len;
 const char* s = luaL_checklstring(L, 1, &len);
 int n = lua_tointeger(L, 2) - 1;
 if (n < 0)
 n = 0; // start from here
 else if (n < (int)len)
 {
 n++;
 while (iscont(s + n))
 n++;
 }
 if (n >= (int)len)
 return 0;
 else
 {
 int code;
 const char* next = utf8_decode(s + n, &code);
 if (next == NULL || iscont(next))
 luaL_error(L, "invalid UTF-8 code");
 lua_pushinteger(L, n + 1);
 lua_pushinteger(L, code);
 return 2;
 }
}
static int iter_codes(lua_State* L)
{
 luaL_checkstring(L, 1);
 lua_pushcfunction(L, iter_aux, NULL);
 lua_pushvalue(L, 1);
 lua_pushinteger(L, 0);
 return 3;
}
#define UTF8PATT "[\0-\x7F\xC2-\xF4][\x80-\xBF]*"
static const luaL_Reg funcs[] = {
 {"offset", byteoffset},
 {"codepoint", codepoint},
 {"char", utfchar},
 {"len", utflen},
 {"codes", iter_codes},
 {NULL, NULL},
};
int luaopen_utf8(lua_State* L)
{
 luaL_register(L, LUA_UTF8LIBNAME, funcs);
 lua_pushlstring(L, UTF8PATT, sizeof(UTF8PATT) / sizeof(char) - 1);
 lua_setfield(L, -2, "charpattern");
 return 1;
}
LUAU_FASTFLAG(LuauGetImportDirect)
#ifdef __clang__
#if __has_warning("-Wc99-designator")
#pragma clang diagnostic ignored "-Wc99-designator"
#endif
#endif
#define VM_PROTECT(x) { L->ci->savedpc = pc; { x; }; base = L->base; }
#define VM_PROTECT_PC() L->ci->savedpc = pc
#define VM_REG(i) (LUAU_ASSERT(unsigned(i) < unsigned(L->top - base)), &base[i])
#define VM_KV(i) (LUAU_ASSERT(unsigned(i) < unsigned(cl->l.p->sizek)), &k[i])
#define VM_UV(i) (LUAU_ASSERT(unsigned(i) < unsigned(cl->nupvalues)), &cl->l.uprefs[i])
#define VM_PATCH_C(pc, slot) *const_cast<Instruction*>(pc) = ((uint8_t(slot) << 24) | (0x00ffffffu & *(pc)))
#define VM_PATCH_E(pc, slot) *const_cast<Instruction*>(pc) = ((uint32_t(slot) << 8) | (0x000000ffu & *(pc)))
#define VM_INTERRUPT() { void (*interrupt)(lua_State*, int) = L->global->cb.interrupt; if (LUAU_UNLIKELY(!!interrupt)) { VM_PROTECT(L->ci->savedpc++; interrupt(L, -1)); if (L->status != 0) { L->ci->savedpc--; goto exit; } } }
#define VM_DISPATCH_OP(op) &&CASE_##op
#define VM_DISPATCH_TABLE() VM_DISPATCH_OP(LOP_NOP), VM_DISPATCH_OP(LOP_BREAK), VM_DISPATCH_OP(LOP_LOADNIL), VM_DISPATCH_OP(LOP_LOADB), VM_DISPATCH_OP(LOP_LOADN), VM_DISPATCH_OP(LOP_LOADK), VM_DISPATCH_OP(LOP_MOVE), VM_DISPATCH_OP(LOP_GETGLOBAL), VM_DISPATCH_OP(LOP_SETGLOBAL), VM_DISPATCH_OP(LOP_GETUPVAL), VM_DISPATCH_OP(LOP_SETUPVAL), VM_DISPATCH_OP(LOP_CLOSEUPVALS), VM_DISPATCH_OP(LOP_GETIMPORT), VM_DISPATCH_OP(LOP_GETTABLE), VM_DISPATCH_OP(LOP_SETTABLE), VM_DISPATCH_OP(LOP_GETTABLEKS), VM_DISPATCH_OP(LOP_SETTABLEKS), VM_DISPATCH_OP(LOP_GETTABLEN), VM_DISPATCH_OP(LOP_SETTABLEN), VM_DISPATCH_OP(LOP_NEWCLOSURE), VM_DISPATCH_OP(LOP_NAMECALL), VM_DISPATCH_OP(LOP_CALL), VM_DISPATCH_OP(LOP_RETURN), VM_DISPATCH_OP(LOP_JUMP), VM_DISPATCH_OP(LOP_JUMPBACK), VM_DISPATCH_OP(LOP_JUMPIF), VM_DISPATCH_OP(LOP_JUMPIFNOT), VM_DISPATCH_OP(LOP_JUMPIFEQ), VM_DISPATCH_OP(LOP_JUMPIFLE), VM_DISPATCH_OP(LOP_JUMPIFLT), VM_DISPATCH_OP(LOP_JUMPIFNOTEQ), VM_DISPATCH_OP(LOP_JUMPIFNOTLE), VM_DISPATCH_OP(LOP_JUMPIFNOTLT), VM_DISPATCH_OP(LOP_ADD), VM_DISPATCH_OP(LOP_SUB), VM_DISPATCH_OP(LOP_MUL), VM_DISPATCH_OP(LOP_DIV), VM_DISPATCH_OP(LOP_MOD), VM_DISPATCH_OP(LOP_POW), VM_DISPATCH_OP(LOP_ADDK), VM_DISPATCH_OP(LOP_SUBK), VM_DISPATCH_OP(LOP_MULK), VM_DISPATCH_OP(LOP_DIVK), VM_DISPATCH_OP(LOP_MODK), VM_DISPATCH_OP(LOP_POWK), VM_DISPATCH_OP(LOP_AND), VM_DISPATCH_OP(LOP_OR), VM_DISPATCH_OP(LOP_ANDK), VM_DISPATCH_OP(LOP_ORK), VM_DISPATCH_OP(LOP_CONCAT), VM_DISPATCH_OP(LOP_NOT), VM_DISPATCH_OP(LOP_MINUS), VM_DISPATCH_OP(LOP_LENGTH), VM_DISPATCH_OP(LOP_NEWTABLE), VM_DISPATCH_OP(LOP_DUPTABLE), VM_DISPATCH_OP(LOP_SETLIST), VM_DISPATCH_OP(LOP_FORNPREP), VM_DISPATCH_OP(LOP_FORNLOOP), VM_DISPATCH_OP(LOP_FORGLOOP), VM_DISPATCH_OP(LOP_FORGPREP_INEXT), VM_DISPATCH_OP(LOP_DEP_FORGLOOP_INEXT), VM_DISPATCH_OP(LOP_FORGPREP_NEXT), VM_DISPATCH_OP(LOP_NATIVECALL), VM_DISPATCH_OP(LOP_GETVARARGS), VM_DISPATCH_OP(LOP_DUPCLOSURE), VM_DISPATCH_OP(LOP_PREPVARARGS), VM_DISPATCH_OP(LOP_LOADKX), VM_DISPATCH_OP(LOP_JUMPX), VM_DISPATCH_OP(LOP_FASTCALL), VM_DISPATCH_OP(LOP_COVERAGE), VM_DISPATCH_OP(LOP_CAPTURE), VM_DISPATCH_OP(LOP_DEP_JUMPIFEQK), VM_DISPATCH_OP(LOP_DEP_JUMPIFNOTEQK), VM_DISPATCH_OP(LOP_FASTCALL1), VM_DISPATCH_OP(LOP_FASTCALL2), VM_DISPATCH_OP(LOP_FASTCALL2K), VM_DISPATCH_OP(LOP_FORGPREP), VM_DISPATCH_OP(LOP_JUMPXEQKNIL), VM_DISPATCH_OP(LOP_JUMPXEQKB), VM_DISPATCH_OP(LOP_JUMPXEQKN), VM_DISPATCH_OP(LOP_JUMPXEQKS),
#if defined(__GNUC__) || defined(__clang__)
#define VM_USE_CGOTO 1
#else
#define VM_USE_CGOTO 0
#endif
#if VM_USE_CGOTO
#define VM_CASE(op) CASE_##op:
#define VM_NEXT() goto*(SingleStep ? &&dispatch : kDispatchTable[LUAU_INSN_OP(*pc)])
#define VM_CONTINUE(op) goto* kDispatchTable[uint8_t(op)]
#else
#define VM_CASE(op) case op:
#define VM_NEXT() goto dispatch
#define VM_CONTINUE(op) dispatchOp = uint8_t(op); goto dispatchContinue
#endif
LUAU_NOINLINE void luau_callhook(lua_State* L, lua_Hook hook, void* userdata)
{
 ptrdiff_t base = savestack(L, L->base);
 ptrdiff_t top = savestack(L, L->top);
 ptrdiff_t ci_top = savestack(L, L->ci->top);
 int status = L->status;
 if (status == LUA_YIELD || status == LUA_BREAK)
 {
 L->status = 0;
 L->base = L->ci->base;
 }
 if (L->ci->savedpc)
 L->ci->savedpc++;
 luaD_checkstack(L, LUA_MINSTACK);
 L->ci->top = L->top + LUA_MINSTACK;
 LUAU_ASSERT(L->ci->top <= L->stack_last);
 Closure* cl = clvalue(L->ci->func);
 lua_Debug ar;
 ar.currentline = cl->isC ? -1 : luaG_getline(cl->l.p, pcRel(L->ci->savedpc, cl->l.p));
 ar.userdata = userdata;
 hook(L, &ar);
 if (L->ci->savedpc)
 L->ci->savedpc--;
 L->ci->top = restorestack(L, ci_top);
 L->top = restorestack(L, top);
 if (status == LUA_YIELD && L->status != LUA_YIELD)
 {
 L->status = LUA_YIELD;
 L->base = restorestack(L, base);
 }
 else if (status == LUA_BREAK)
 {
 LUAU_ASSERT(L->status != LUA_BREAK);
 L->status = LUA_BREAK;
 L->base = restorestack(L, base);
 }
}
inline bool luau_skipstep(uint8_t op)
{
 return op == LOP_PREPVARARGS || op == LOP_BREAK;
}
template<bool SingleStep>
static void luau_execute(lua_State* L)
{
#if VM_USE_CGOTO
 static const void* kDispatchTable[256] = {VM_DISPATCH_TABLE()};
#endif
 Closure* cl;
 StkId base;
 TValue* k;
 const Instruction* pc;
 LUAU_ASSERT(isLua(L->ci));
 LUAU_ASSERT(L->isactive);
 LUAU_ASSERT(!isblack(obj2gco(L)));
#if LUA_CUSTOM_EXECUTION
 if ((L->ci->flags & LUA_CALLINFO_NATIVE) && !SingleStep)
 {
 Proto* p = clvalue(L->ci->func)->l.p;
 LUAU_ASSERT(p->execdata);
 if (L->global->ecb.enter(L, p) == 0)
 return;
 }
reentry:
#endif
 LUAU_ASSERT(isLua(L->ci));
 pc = L->ci->savedpc;
 cl = clvalue(L->ci->func);
 base = L->base;
 k = cl->l.p->k;
 VM_NEXT();
 {
 dispatch:
 LUAU_ASSERT(base == L->base && L->base == L->ci->base);
 LUAU_ASSERT(base <= L->top && L->top <= L->stack + L->stacksize);
 if (SingleStep)
 {
 if (L->global->cb.debugstep && !luau_skipstep(LUAU_INSN_OP(*pc)))
 {
 VM_PROTECT(luau_callhook(L, L->global->cb.debugstep, NULL));
 if (L->status != 0)
 goto exit;
 }
#if VM_USE_CGOTO
 VM_CONTINUE(LUAU_INSN_OP(*pc));
#endif
 }
#if !VM_USE_CGOTO
 size_t dispatchOp = LUAU_INSN_OP(*pc);
 dispatchContinue:
 switch (dispatchOp)
#endif
 {
 VM_CASE(LOP_NOP)
 {
 Instruction insn = *pc++;
 LUAU_ASSERT(insn == 0);
 VM_NEXT();
 }
 VM_CASE(LOP_LOADNIL)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 setnilvalue(ra);
 VM_NEXT();
 }
 VM_CASE(LOP_LOADB)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 setbvalue(ra, LUAU_INSN_B(insn));
 pc += LUAU_INSN_C(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_LOADN)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 setnvalue(ra, LUAU_INSN_D(insn));
 VM_NEXT();
 }
 VM_CASE(LOP_LOADK)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 TValue* kv = VM_KV(LUAU_INSN_D(insn));
 setobj2s(L, ra, kv);
 VM_NEXT();
 }
 VM_CASE(LOP_MOVE)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 setobj2s(L, ra, rb);
 VM_NEXT();
 }
 VM_CASE(LOP_GETGLOBAL)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 uint32_t aux = *pc++;
 TValue* kv = VM_KV(aux);
 LUAU_ASSERT(ttisstring(kv));
 Table* h = cl->env;
 int slot = LUAU_INSN_C(insn) & h->nodemask8;
 LuaNode* n = &h->node[slot];
 if (LUAU_LIKELY(ttisstring(gkey(n)) && tsvalue(gkey(n)) == tsvalue(kv)) && !ttisnil(gval(n)))
 {
 setobj2s(L, ra, gval(n));
 VM_NEXT();
 }
 else
 {
 TValue g;
 sethvalue(L, &g, h);
 L->cachedslot = slot;
 VM_PROTECT(luaV_gettable(L, &g, kv, ra));
 VM_PATCH_C(pc - 2, L->cachedslot);
 VM_NEXT();
 }
 }
 VM_CASE(LOP_SETGLOBAL)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 uint32_t aux = *pc++;
 TValue* kv = VM_KV(aux);
 LUAU_ASSERT(ttisstring(kv));
 Table* h = cl->env;
 int slot = LUAU_INSN_C(insn) & h->nodemask8;
 LuaNode* n = &h->node[slot];
 if (LUAU_LIKELY(ttisstring(gkey(n)) && tsvalue(gkey(n)) == tsvalue(kv) && !ttisnil(gval(n)) && !h->readonly))
 {
 setobj2t(L, gval(n), ra);
 luaC_barriert(L, h, ra);
 VM_NEXT();
 }
 else
 {
 TValue g;
 sethvalue(L, &g, h);
 L->cachedslot = slot;
 VM_PROTECT(luaV_settable(L, &g, kv, ra));
 VM_PATCH_C(pc - 2, L->cachedslot);
 VM_NEXT();
 }
 }
 VM_CASE(LOP_GETUPVAL)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 TValue* ur = VM_UV(LUAU_INSN_B(insn));
 TValue* v = ttisupval(ur) ? upvalue(ur)->v : ur;
 setobj2s(L, ra, v);
 VM_NEXT();
 }
 VM_CASE(LOP_SETUPVAL)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 TValue* ur = VM_UV(LUAU_INSN_B(insn));
 UpVal* uv = upvalue(ur);
 setobj(L, uv->v, ra);
 luaC_barrier(L, uv, ra);
 VM_NEXT();
 }
 VM_CASE(LOP_CLOSEUPVALS)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 if (L->openupval && L->openupval->v >= ra)
 luaF_close(L, ra);
 VM_NEXT();
 }
 VM_CASE(LOP_GETIMPORT)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 TValue* kv = VM_KV(LUAU_INSN_D(insn));
 if (!ttisnil(kv) && cl->env->safeenv)
 {
 setobj2s(L, ra, kv);
 pc++;
 VM_NEXT();
 }
 else
 {
 uint32_t aux = *pc++;
 if (FFlag::LuauGetImportDirect)
 {
 VM_PROTECT(luaV_getimport(L, cl->env, k, ra, aux, false));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_getimport_dep(L, cl->env, k, aux, false));
 ra = VM_REG(LUAU_INSN_A(insn));
 setobj2s(L, ra, L->top - 1);
 L->top--;
 VM_NEXT();
 }
 }
 }
 VM_CASE(LOP_GETTABLEKS)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 uint32_t aux = *pc++;
 TValue* kv = VM_KV(aux);
 LUAU_ASSERT(ttisstring(kv));
 if (LUAU_LIKELY(ttistable(rb)))
 {
 Table* h = hvalue(rb);
 int slot = LUAU_INSN_C(insn) & h->nodemask8;
 LuaNode* n = &h->node[slot];
 if (LUAU_LIKELY(ttisstring(gkey(n)) && tsvalue(gkey(n)) == tsvalue(kv) && !ttisnil(gval(n))))
 {
 setobj2s(L, ra, gval(n));
 VM_NEXT();
 }
 else if (!h->metatable)
 {
 const TValue* res = luaH_getstr(h, tsvalue(kv));
 if (res != luaO_nilobject)
 {
 int cachedslot = gval2slot(h, res);
 VM_PATCH_C(pc - 2, cachedslot);
 }
 setobj2s(L, ra, res);
 VM_NEXT();
 }
 else
 {
 L->cachedslot = slot;
 VM_PROTECT(luaV_gettable(L, rb, kv, ra));
 VM_PATCH_C(pc - 2, L->cachedslot);
 VM_NEXT();
 }
 }
 else
 {
 const TValue* fn = 0;
 if (ttisuserdata(rb) && (fn = fasttm(L, uvalue(rb)->metatable, TM_INDEX)) && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 setobj2s(L, top + 2, kv);
 L->top = top + 3;
 L->cachedslot = LUAU_INSN_C(insn);
 VM_PROTECT(luaV_callTM(L, 2, LUAU_INSN_A(insn)));
 VM_PATCH_C(pc - 2, L->cachedslot);
 VM_NEXT();
 }
 else if (ttisvector(rb))
 {
 const char* name = getstr(tsvalue(kv));
 int ic = (name[0] | ' ') - 'x';
#if LUA_VECTOR_SIZE == 4
 if (ic == -1)
 ic = 3;
#endif
 if (unsigned(ic) < LUA_VECTOR_SIZE && name[1] == '\0')
 {
 const float* v = rb->value.v;
 setnvalue(ra, v[ic]);
 VM_NEXT();
 }
 fn = fasttm(L, L->global->mt[LUA_TVECTOR], TM_INDEX);
 if (fn && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 setobj2s(L, top + 2, kv);
 L->top = top + 3;
 L->cachedslot = LUAU_INSN_C(insn);
 VM_PROTECT(luaV_callTM(L, 2, LUAU_INSN_A(insn)));
 VM_PATCH_C(pc - 2, L->cachedslot);
 VM_NEXT();
 }
 }
 }
 VM_PROTECT(luaV_gettable(L, rb, kv, ra));
 VM_NEXT();
 }
 VM_CASE(LOP_SETTABLEKS)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 uint32_t aux = *pc++;
 TValue* kv = VM_KV(aux);
 LUAU_ASSERT(ttisstring(kv));
 if (LUAU_LIKELY(ttistable(rb)))
 {
 Table* h = hvalue(rb);
 int slot = LUAU_INSN_C(insn) & h->nodemask8;
 LuaNode* n = &h->node[slot];
 if (LUAU_LIKELY(ttisstring(gkey(n)) && tsvalue(gkey(n)) == tsvalue(kv) && !ttisnil(gval(n)) && !h->readonly))
 {
 setobj2t(L, gval(n), ra);
 luaC_barriert(L, h, ra);
 VM_NEXT();
 }
 else if (fastnotm(h->metatable, TM_NEWINDEX) && !h->readonly)
 {
 VM_PROTECT_PC();
 TValue* res = luaH_setstr(L, h, tsvalue(kv));
 int cachedslot = gval2slot(h, res);
 VM_PATCH_C(pc - 2, cachedslot);
 setobj2t(L, res, ra);
 luaC_barriert(L, h, ra);
 VM_NEXT();
 }
 else
 {
 L->cachedslot = slot;
 VM_PROTECT(luaV_settable(L, rb, kv, ra));
 VM_PATCH_C(pc - 2, L->cachedslot);
 VM_NEXT();
 }
 }
 else
 {
 const TValue* fn = 0;
 if (ttisuserdata(rb) && (fn = fasttm(L, uvalue(rb)->metatable, TM_NEWINDEX)) && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 4 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 setobj2s(L, top + 2, kv);
 setobj2s(L, top + 3, ra);
 L->top = top + 4;
 L->cachedslot = LUAU_INSN_C(insn);
 VM_PROTECT(luaV_callTM(L, 3, -1));
 VM_PATCH_C(pc - 2, L->cachedslot);
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_settable(L, rb, kv, ra));
 VM_NEXT();
 }
 }
 }
 VM_CASE(LOP_GETTABLE)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 if (ttistable(rb) && ttisnumber(rc))
 {
 Table* h = hvalue(rb);
 double indexd = nvalue(rc);
 int index = int(indexd);
 if (LUAU_LIKELY(unsigned(index - 1) < unsigned(h->sizearray) && !h->metatable && double(index) == indexd))
 {
 setobj2s(L, ra, &h->array[unsigned(index - 1)]);
 VM_NEXT();
 }
 }
 VM_PROTECT(luaV_gettable(L, rb, rc, ra));
 VM_NEXT();
 }
 VM_CASE(LOP_SETTABLE)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 if (ttistable(rb) && ttisnumber(rc))
 {
 Table* h = hvalue(rb);
 double indexd = nvalue(rc);
 int index = int(indexd);
 if (LUAU_LIKELY(unsigned(index - 1) < unsigned(h->sizearray) && !h->metatable && !h->readonly && double(index) == indexd))
 {
 setobj2t(L, &h->array[unsigned(index - 1)], ra);
 luaC_barriert(L, h, ra);
 VM_NEXT();
 }
 }
 VM_PROTECT(luaV_settable(L, rb, rc, ra));
 VM_NEXT();
 }
 VM_CASE(LOP_GETTABLEN)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 int c = LUAU_INSN_C(insn);
 if (ttistable(rb))
 {
 Table* h = hvalue(rb);
 if (LUAU_LIKELY(unsigned(c) < unsigned(h->sizearray) && !h->metatable))
 {
 setobj2s(L, ra, &h->array[c]);
 VM_NEXT();
 }
 }
 TValue n;
 setnvalue(&n, c + 1);
 VM_PROTECT(luaV_gettable(L, rb, &n, ra));
 VM_NEXT();
 }
 VM_CASE(LOP_SETTABLEN)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 int c = LUAU_INSN_C(insn);
 if (ttistable(rb))
 {
 Table* h = hvalue(rb);
 if (LUAU_LIKELY(unsigned(c) < unsigned(h->sizearray) && !h->metatable && !h->readonly))
 {
 setobj2t(L, &h->array[c], ra);
 luaC_barriert(L, h, ra);
 VM_NEXT();
 }
 }
 TValue n;
 setnvalue(&n, c + 1);
 VM_PROTECT(luaV_settable(L, rb, &n, ra));
 VM_NEXT();
 }
 VM_CASE(LOP_NEWCLOSURE)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 Proto* pv = cl->l.p->p[LUAU_INSN_D(insn)];
 LUAU_ASSERT(unsigned(LUAU_INSN_D(insn)) < unsigned(cl->l.p->sizep));
 VM_PROTECT_PC();
 Closure* ncl = luaF_newLclosure(L, pv->nups, cl->env, pv);
 setclvalue(L, ra, ncl);
 for (int ui = 0; ui < pv->nups; ++ui)
 {
 Instruction uinsn = *pc++;
 LUAU_ASSERT(LUAU_INSN_OP(uinsn) == LOP_CAPTURE);
 switch (LUAU_INSN_A(uinsn))
 {
 case LCT_VAL:
 setobj(L, &ncl->l.uprefs[ui], VM_REG(LUAU_INSN_B(uinsn)));
 break;
 case LCT_REF:
 setupvalue(L, &ncl->l.uprefs[ui], luaF_findupval(L, VM_REG(LUAU_INSN_B(uinsn))));
 break;
 case LCT_UPVAL:
 setobj(L, &ncl->l.uprefs[ui], VM_UV(LUAU_INSN_B(uinsn)));
 break;
 default:
 LUAU_ASSERT(!"Unknown upvalue capture type");
 LUAU_UNREACHABLE();
 }
 }
 VM_PROTECT(luaC_checkGC(L));
 VM_NEXT();
 }
 VM_CASE(LOP_NAMECALL)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 uint32_t aux = *pc++;
 TValue* kv = VM_KV(aux);
 LUAU_ASSERT(ttisstring(kv));
 if (LUAU_LIKELY(ttistable(rb)))
 {
 Table* h = hvalue(rb);
 LuaNode* n = &h->node[tsvalue(kv)->hash & (sizenode(h) - 1)];
 const TValue* mt = 0;
 const LuaNode* mtn = 0;
 if (ttisstring(gkey(n)) && tsvalue(gkey(n)) == tsvalue(kv) && !ttisnil(gval(n)))
 {
 setobj2s(L, ra + 1, rb);
 setobj2s(L, ra, gval(n));
 }
 else if (gnext(n) == 0 && (mt = fasttm(L, hvalue(rb)->metatable, TM_INDEX)) && ttistable(mt) &&
 (mtn = &hvalue(mt)->node[LUAU_INSN_C(insn) & hvalue(mt)->nodemask8]) && ttisstring(gkey(mtn)) &&
 tsvalue(gkey(mtn)) == tsvalue(kv) && !ttisnil(gval(mtn)))
 {
 setobj2s(L, ra + 1, rb);
 setobj2s(L, ra, gval(mtn));
 }
 else
 {
 setobj2s(L, ra + 1, rb);
 L->cachedslot = LUAU_INSN_C(insn);
 VM_PROTECT(luaV_gettable(L, rb, kv, ra));
 VM_PATCH_C(pc - 2, L->cachedslot);
 ra = VM_REG(LUAU_INSN_A(insn));
 if (ttisnil(ra))
 luaG_methoderror(L, ra + 1, tsvalue(kv));
 }
 }
 else
 {
 Table* mt = ttisuserdata(rb) ? uvalue(rb)->metatable : L->global->mt[ttype(rb)];
 const TValue* tmi = 0;
 if (const TValue* fn = fasttm(L, mt, TM_NAMECALL))
 {
 setobj2s(L, ra + 1, rb);
 setobj2s(L, ra, fn);
 L->namecall = tsvalue(kv);
 }
 else if ((tmi = fasttm(L, mt, TM_INDEX)) && ttistable(tmi))
 {
 Table* h = hvalue(tmi);
 int slot = LUAU_INSN_C(insn) & h->nodemask8;
 LuaNode* n = &h->node[slot];
 if (LUAU_LIKELY(ttisstring(gkey(n)) && tsvalue(gkey(n)) == tsvalue(kv) && !ttisnil(gval(n))))
 {
 setobj2s(L, ra + 1, rb);
 setobj2s(L, ra, gval(n));
 }
 else
 {
 setobj2s(L, ra + 1, rb);
 L->cachedslot = slot;
 VM_PROTECT(luaV_gettable(L, rb, kv, ra));
 VM_PATCH_C(pc - 2, L->cachedslot);
 ra = VM_REG(LUAU_INSN_A(insn));
 if (ttisnil(ra))
 luaG_methoderror(L, ra + 1, tsvalue(kv));
 }
 }
 else
 {
 setobj2s(L, ra + 1, rb);
 VM_PROTECT(luaV_gettable(L, rb, kv, ra));
 ra = VM_REG(LUAU_INSN_A(insn));
 if (ttisnil(ra))
 luaG_methoderror(L, ra + 1, tsvalue(kv));
 }
 }
 LUAU_ASSERT(LUAU_INSN_OP(*pc) == LOP_CALL);
 }
 VM_CASE(LOP_CALL)
 {
 VM_INTERRUPT();
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 int nparams = LUAU_INSN_B(insn) - 1;
 int nresults = LUAU_INSN_C(insn) - 1;
 StkId argtop = L->top;
 argtop = (nparams == LUA_MULTRET) ? argtop : ra + 1 + nparams;
 if (LUAU_UNLIKELY(!ttisfunction(ra)))
 {
 VM_PROTECT_PC();
 luaV_tryfuncTM(L, ra);
 argtop++;
 }
 Closure* ccl = clvalue(ra);
 L->ci->savedpc = pc;
 CallInfo* ci = incr_ci(L);
 ci->func = ra;
 ci->base = ra + 1;
 ci->top = argtop + ccl->stacksize;
 ci->savedpc = NULL;
 ci->flags = 0;
 ci->nresults = nresults;
 L->base = ci->base;
 L->top = argtop;
 luaD_checkstack(L, ccl->stacksize);
 LUAU_ASSERT(ci->top <= L->stack_last);
 if (!ccl->isC)
 {
 Proto* p = ccl->l.p;
 StkId argi = L->top;
 StkId argend = L->base + p->numparams;
 while (argi < argend)
 setnilvalue(argi++);
 L->top = p->is_vararg ? argi : ci->top;
 pc = SingleStep ? p->code : p->codeentry;
 cl = ccl;
 base = L->base;
 k = p->k;
 VM_NEXT();
 }
 else
 {
 lua_CFunction func = ccl->c.f;
 int n = func(L);
 if (n < 0)
 goto exit;
 CallInfo* ci = L->ci;
 CallInfo* cip = ci - 1;
 StkId res = ci->func;
 StkId vali = L->top - n;
 StkId valend = L->top;
 int i;
 for (i = nresults; i != 0 && vali < valend; i--)
 setobj2s(L, res++, vali++);
 while (i-- > 0)
 setnilvalue(res++);
 L->ci = cip;
 L->base = cip->base;
 L->top = (nresults == LUA_MULTRET) ? res : cip->top;
 base = L->base;
 VM_NEXT();
 }
 }
 VM_CASE(LOP_RETURN)
 {
 VM_INTERRUPT();
 Instruction insn = *pc++;
 StkId ra = &base[LUAU_INSN_A(insn)];
 int b = LUAU_INSN_B(insn) - 1;
 CallInfo* ci = L->ci;
 CallInfo* cip = ci - 1;
 StkId res = ci->func;
 StkId vali = ra;
 StkId valend =
 (b == LUA_MULTRET) ? L->top : ra + b;
 int nresults = ci->nresults;
 int i;
 for (i = nresults; i != 0 && vali < valend; i--)
 setobj2s(L, res++, vali++);
 while (i-- > 0)
 setnilvalue(res++);
 L->ci = cip;
 L->base = cip->base;
 L->top = (nresults == LUA_MULTRET) ? res : cip->top;
 if (LUAU_UNLIKELY(ci->flags & LUA_CALLINFO_RETURN))
 {
 goto exit;
 }
 LUAU_ASSERT(isLua(L->ci));
 Closure* nextcl = clvalue(cip->func);
 Proto* nextproto = nextcl->l.p;
#if LUA_CUSTOM_EXECUTION
 if (LUAU_UNLIKELY((cip->flags & LUA_CALLINFO_NATIVE) && !SingleStep))
 {
 if (L->global->ecb.enter(L, nextproto) == 1)
 goto reentry;
 else
 goto exit;
 }
#endif
 pc = cip->savedpc;
 cl = nextcl;
 base = L->base;
 k = nextproto->k;
 VM_NEXT();
 }
 VM_CASE(LOP_JUMP)
 {
 Instruction insn = *pc++;
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_JUMPIF)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 pc += l_isfalse(ra) ? 0 : LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_JUMPIFNOT)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 pc += l_isfalse(ra) ? LUAU_INSN_D(insn) : 0;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_JUMPIFEQ)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(aux);
 if (ttype(ra) == ttype(rb))
 {
 switch (ttype(ra))
 {
 case LUA_TNIL:
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TBOOLEAN:
 pc += bvalue(ra) == bvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TLIGHTUSERDATA:
 pc += pvalue(ra) == pvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TNUMBER:
 pc += nvalue(ra) == nvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TVECTOR:
 pc += luai_veceq(vvalue(ra), vvalue(rb)) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TSTRING:
 case LUA_TFUNCTION:
 case LUA_TTHREAD:
 pc += gcvalue(ra) == gcvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TTABLE:
 if (hvalue(ra)->metatable == hvalue(rb)->metatable)
 {
 const TValue* fn = fasttm(L, hvalue(ra)->metatable, TM_EQ);
 if (!fn)
 {
 pc += hvalue(ra) == hvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 break;
 case LUA_TUSERDATA:
 if (uvalue(ra)->metatable == uvalue(rb)->metatable)
 {
 const TValue* fn = fasttm(L, uvalue(ra)->metatable, TM_EQ);
 if (!fn)
 {
 pc += uvalue(ra) == uvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else if (ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, ra);
 setobj2s(L, top + 2, rb);
 int res = int(top - base);
 L->top = top + 3;
 VM_PROTECT(luaV_callTM(L, 2, res));
 pc += !l_isfalse(&base[res]) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 break;
 default:
 LUAU_ASSERT(!"Unknown value type");
 LUAU_UNREACHABLE();
 }
 int res;
 VM_PROTECT(res = luaV_equalval(L, ra, rb));
 pc += (res == 1) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 pc += 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_JUMPIFNOTEQ)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(aux);
 if (ttype(ra) == ttype(rb))
 {
 switch (ttype(ra))
 {
 case LUA_TNIL:
 pc += 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TBOOLEAN:
 pc += bvalue(ra) != bvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TLIGHTUSERDATA:
 pc += pvalue(ra) != pvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TNUMBER:
 pc += nvalue(ra) != nvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TVECTOR:
 pc += !luai_veceq(vvalue(ra), vvalue(rb)) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TSTRING:
 case LUA_TFUNCTION:
 case LUA_TTHREAD:
 pc += gcvalue(ra) != gcvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 case LUA_TTABLE:
 if (hvalue(ra)->metatable == hvalue(rb)->metatable)
 {
 const TValue* fn = fasttm(L, hvalue(ra)->metatable, TM_EQ);
 if (!fn)
 {
 pc += hvalue(ra) != hvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 break;
 case LUA_TUSERDATA:
 if (uvalue(ra)->metatable == uvalue(rb)->metatable)
 {
 const TValue* fn = fasttm(L, uvalue(ra)->metatable, TM_EQ);
 if (!fn)
 {
 pc += uvalue(ra) != uvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else if (ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, ra);
 setobj2s(L, top + 2, rb);
 int res = int(top - base);
 L->top = top + 3;
 VM_PROTECT(luaV_callTM(L, 2, res));
 pc += l_isfalse(&base[res]) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 break;
 default:
 LUAU_ASSERT(!"Unknown value type");
 LUAU_UNREACHABLE();
 }
 int res;
 VM_PROTECT(res = luaV_equalval(L, ra, rb));
 pc += (res == 0) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_JUMPIFLE)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(aux);
 if (LUAU_LIKELY(ttisnumber(ra) && ttisnumber(rb)))
 {
 pc += nvalue(ra) <= nvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else if (ttisstring(ra) && ttisstring(rb))
 {
 pc += luaV_strcmp(tsvalue(ra), tsvalue(rb)) <= 0 ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 int res;
 VM_PROTECT(res = luaV_lessequal(L, ra, rb));
 pc += (res == 1) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_JUMPIFNOTLE)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(aux);
 if (LUAU_LIKELY(ttisnumber(ra) && ttisnumber(rb)))
 {
 pc += !(nvalue(ra) <= nvalue(rb)) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else if (ttisstring(ra) && ttisstring(rb))
 {
 pc += !(luaV_strcmp(tsvalue(ra), tsvalue(rb)) <= 0) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 int res;
 VM_PROTECT(res = luaV_lessequal(L, ra, rb));
 pc += (res == 0) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_JUMPIFLT)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(aux);
 if (LUAU_LIKELY(ttisnumber(ra) && ttisnumber(rb)))
 {
 pc += nvalue(ra) < nvalue(rb) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else if (ttisstring(ra) && ttisstring(rb))
 {
 pc += luaV_strcmp(tsvalue(ra), tsvalue(rb)) < 0 ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 int res;
 VM_PROTECT(res = luaV_lessthan(L, ra, rb));
 pc += (res == 1) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_JUMPIFNOTLT)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(aux);
 if (LUAU_LIKELY(ttisnumber(ra) && ttisnumber(rb)))
 {
 pc += !(nvalue(ra) < nvalue(rb)) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else if (ttisstring(ra) && ttisstring(rb))
 {
 pc += !(luaV_strcmp(tsvalue(ra), tsvalue(rb)) < 0) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 int res;
 VM_PROTECT(res = luaV_lessthan(L, ra, rb));
 pc += (res == 0) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_ADD)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 if (LUAU_LIKELY(ttisnumber(rb) && ttisnumber(rc)))
 {
 setnvalue(ra, nvalue(rb) + nvalue(rc));
 VM_NEXT();
 }
 else if (ttisvector(rb) && ttisvector(rc))
 {
 const float* vb = rb->value.v;
 const float* vc = rc->value.v;
 setvvalue(ra, vb[0] + vc[0], vb[1] + vc[1], vb[2] + vc[2], vb[3] + vc[3]);
 VM_NEXT();
 }
 else
 {
 const TValue* fn = 0;
 if (ttisuserdata(rb) && (fn = luaT_gettmbyobj(L, rb, TM_ADD)) && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 setobj2s(L, top + 2, rc);
 L->top = top + 3;
 VM_PROTECT(luaV_callTM(L, 2, LUAU_INSN_A(insn)));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, rc, TM_ADD));
 VM_NEXT();
 }
 }
 }
 VM_CASE(LOP_SUB)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 if (LUAU_LIKELY(ttisnumber(rb) && ttisnumber(rc)))
 {
 setnvalue(ra, nvalue(rb) - nvalue(rc));
 VM_NEXT();
 }
 else if (ttisvector(rb) && ttisvector(rc))
 {
 const float* vb = rb->value.v;
 const float* vc = rc->value.v;
 setvvalue(ra, vb[0] - vc[0], vb[1] - vc[1], vb[2] - vc[2], vb[3] - vc[3]);
 VM_NEXT();
 }
 else
 {
 const TValue* fn = 0;
 if (ttisuserdata(rb) && (fn = luaT_gettmbyobj(L, rb, TM_SUB)) && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 setobj2s(L, top + 2, rc);
 L->top = top + 3;
 VM_PROTECT(luaV_callTM(L, 2, LUAU_INSN_A(insn)));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, rc, TM_SUB));
 VM_NEXT();
 }
 }
 }
 VM_CASE(LOP_MUL)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 if (LUAU_LIKELY(ttisnumber(rb) && ttisnumber(rc)))
 {
 setnvalue(ra, nvalue(rb) * nvalue(rc));
 VM_NEXT();
 }
 else if (ttisvector(rb) && ttisnumber(rc))
 {
 const float* vb = rb->value.v;
 float vc = cast_to(float, nvalue(rc));
 setvvalue(ra, vb[0] * vc, vb[1] * vc, vb[2] * vc, vb[3] * vc);
 VM_NEXT();
 }
 else if (ttisvector(rb) && ttisvector(rc))
 {
 const float* vb = rb->value.v;
 const float* vc = rc->value.v;
 setvvalue(ra, vb[0] * vc[0], vb[1] * vc[1], vb[2] * vc[2], vb[3] * vc[3]);
 VM_NEXT();
 }
 else if (ttisnumber(rb) && ttisvector(rc))
 {
 float vb = cast_to(float, nvalue(rb));
 const float* vc = rc->value.v;
 setvvalue(ra, vb * vc[0], vb * vc[1], vb * vc[2], vb * vc[3]);
 VM_NEXT();
 }
 else
 {
 StkId rbc = ttisnumber(rb) ? rc : rb;
 const TValue* fn = 0;
 if (ttisuserdata(rbc) && (fn = luaT_gettmbyobj(L, rbc, TM_MUL)) && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 setobj2s(L, top + 2, rc);
 L->top = top + 3;
 VM_PROTECT(luaV_callTM(L, 2, LUAU_INSN_A(insn)));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, rc, TM_MUL));
 VM_NEXT();
 }
 }
 }
 VM_CASE(LOP_DIV)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 if (LUAU_LIKELY(ttisnumber(rb) && ttisnumber(rc)))
 {
 setnvalue(ra, nvalue(rb) / nvalue(rc));
 VM_NEXT();
 }
 else if (ttisvector(rb) && ttisnumber(rc))
 {
 const float* vb = rb->value.v;
 float vc = cast_to(float, nvalue(rc));
 setvvalue(ra, vb[0] / vc, vb[1] / vc, vb[2] / vc, vb[3] / vc);
 VM_NEXT();
 }
 else if (ttisvector(rb) && ttisvector(rc))
 {
 const float* vb = rb->value.v;
 const float* vc = rc->value.v;
 setvvalue(ra, vb[0] / vc[0], vb[1] / vc[1], vb[2] / vc[2], vb[3] / vc[3]);
 VM_NEXT();
 }
 else if (ttisnumber(rb) && ttisvector(rc))
 {
 float vb = cast_to(float, nvalue(rb));
 const float* vc = rc->value.v;
 setvvalue(ra, vb / vc[0], vb / vc[1], vb / vc[2], vb / vc[3]);
 VM_NEXT();
 }
 else
 {
 StkId rbc = ttisnumber(rb) ? rc : rb;
 const TValue* fn = 0;
 if (ttisuserdata(rbc) && (fn = luaT_gettmbyobj(L, rbc, TM_DIV)) && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 setobj2s(L, top + 2, rc);
 L->top = top + 3;
 VM_PROTECT(luaV_callTM(L, 2, LUAU_INSN_A(insn)));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, rc, TM_DIV));
 VM_NEXT();
 }
 }
 }
 VM_CASE(LOP_MOD)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 if (ttisnumber(rb) && ttisnumber(rc))
 {
 double nb = nvalue(rb);
 double nc = nvalue(rc);
 setnvalue(ra, luai_nummod(nb, nc));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, rc, TM_MOD));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_POW)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 if (ttisnumber(rb) && ttisnumber(rc))
 {
 setnvalue(ra, pow(nvalue(rb), nvalue(rc)));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, rc, TM_POW));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_ADDK)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 TValue* kv = VM_KV(LUAU_INSN_C(insn));
 if (ttisnumber(rb))
 {
 setnvalue(ra, nvalue(rb) + nvalue(kv));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, kv, TM_ADD));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_SUBK)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 TValue* kv = VM_KV(LUAU_INSN_C(insn));
 if (ttisnumber(rb))
 {
 setnvalue(ra, nvalue(rb) - nvalue(kv));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, kv, TM_SUB));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_MULK)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 TValue* kv = VM_KV(LUAU_INSN_C(insn));
 if (LUAU_LIKELY(ttisnumber(rb)))
 {
 setnvalue(ra, nvalue(rb) * nvalue(kv));
 VM_NEXT();
 }
 else if (ttisvector(rb))
 {
 const float* vb = rb->value.v;
 float vc = cast_to(float, nvalue(kv));
 setvvalue(ra, vb[0] * vc, vb[1] * vc, vb[2] * vc, vb[3] * vc);
 VM_NEXT();
 }
 else
 {
 const TValue* fn = 0;
 if (ttisuserdata(rb) && (fn = luaT_gettmbyobj(L, rb, TM_MUL)) && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 setobj2s(L, top + 2, kv);
 L->top = top + 3;
 VM_PROTECT(luaV_callTM(L, 2, LUAU_INSN_A(insn)));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, kv, TM_MUL));
 VM_NEXT();
 }
 }
 }
 VM_CASE(LOP_DIVK)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 TValue* kv = VM_KV(LUAU_INSN_C(insn));
 if (LUAU_LIKELY(ttisnumber(rb)))
 {
 setnvalue(ra, nvalue(rb) / nvalue(kv));
 VM_NEXT();
 }
 else if (ttisvector(rb))
 {
 const float* vb = rb->value.v;
 float vc = cast_to(float, nvalue(kv));
 setvvalue(ra, vb[0] / vc, vb[1] / vc, vb[2] / vc, vb[3] / vc);
 VM_NEXT();
 }
 else
 {
 const TValue* fn = 0;
 if (ttisuserdata(rb) && (fn = luaT_gettmbyobj(L, rb, TM_DIV)) && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 3 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 setobj2s(L, top + 2, kv);
 L->top = top + 3;
 VM_PROTECT(luaV_callTM(L, 2, LUAU_INSN_A(insn)));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, kv, TM_DIV));
 VM_NEXT();
 }
 }
 }
 VM_CASE(LOP_MODK)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 TValue* kv = VM_KV(LUAU_INSN_C(insn));
 if (ttisnumber(rb))
 {
 double nb = nvalue(rb);
 double nk = nvalue(kv);
 setnvalue(ra, luai_nummod(nb, nk));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, kv, TM_MOD));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_POWK)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 TValue* kv = VM_KV(LUAU_INSN_C(insn));
 if (ttisnumber(rb))
 {
 double nb = nvalue(rb);
 double nk = nvalue(kv);
 double r = (nk == 2.0) ? nb * nb : (nk == 0.5) ? sqrt(nb) : (nk == 3.0) ? nb * nb * nb : pow(nb, nk);
 setnvalue(ra, r);
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, kv, TM_POW));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_AND)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 setobj2s(L, ra, l_isfalse(rb) ? rb : rc);
 VM_NEXT();
 }
 VM_CASE(LOP_OR)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 StkId rc = VM_REG(LUAU_INSN_C(insn));
 setobj2s(L, ra, l_isfalse(rb) ? rc : rb);
 VM_NEXT();
 }
 VM_CASE(LOP_ANDK)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 TValue* kv = VM_KV(LUAU_INSN_C(insn));
 setobj2s(L, ra, l_isfalse(rb) ? rb : kv);
 VM_NEXT();
 }
 VM_CASE(LOP_ORK)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 TValue* kv = VM_KV(LUAU_INSN_C(insn));
 setobj2s(L, ra, l_isfalse(rb) ? kv : rb);
 VM_NEXT();
 }
 VM_CASE(LOP_CONCAT)
 {
 Instruction insn = *pc++;
 int b = LUAU_INSN_B(insn);
 int c = LUAU_INSN_C(insn);
 VM_PROTECT(luaV_concat(L, c - b + 1, c));
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 setobj2s(L, ra, base + b);
 VM_PROTECT(luaC_checkGC(L));
 VM_NEXT();
 }
 VM_CASE(LOP_NOT)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 int res = l_isfalse(rb);
 setbvalue(ra, res);
 VM_NEXT();
 }
 VM_CASE(LOP_MINUS)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 if (LUAU_LIKELY(ttisnumber(rb)))
 {
 setnvalue(ra, -nvalue(rb));
 VM_NEXT();
 }
 else if (ttisvector(rb))
 {
 const float* vb = rb->value.v;
 setvvalue(ra, -vb[0], -vb[1], -vb[2], -vb[3]);
 VM_NEXT();
 }
 else
 {
 const TValue* fn = 0;
 if (ttisuserdata(rb) && (fn = luaT_gettmbyobj(L, rb, TM_UNM)) && ttisfunction(fn) && clvalue(fn)->isC)
 {
 LUAU_ASSERT(L->top + 2 < L->stack + L->stacksize);
 StkId top = L->top;
 setobj2s(L, top + 0, fn);
 setobj2s(L, top + 1, rb);
 L->top = top + 2;
 VM_PROTECT(luaV_callTM(L, 1, LUAU_INSN_A(insn)));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_doarith(L, ra, rb, rb, TM_UNM));
 VM_NEXT();
 }
 }
 }
 VM_CASE(LOP_LENGTH)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = VM_REG(LUAU_INSN_B(insn));
 if (LUAU_LIKELY(ttistable(rb)))
 {
 Table* h = hvalue(rb);
 if (fastnotm(h->metatable, TM_LEN))
 {
 setnvalue(ra, cast_num(luaH_getn(h)));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_dolen(L, ra, rb));
 VM_NEXT();
 }
 }
 else if (ttisstring(rb))
 {
 TString* ts = tsvalue(rb);
 setnvalue(ra, cast_num(ts->len));
 VM_NEXT();
 }
 else
 {
 VM_PROTECT(luaV_dolen(L, ra, rb));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_NEWTABLE)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 int b = LUAU_INSN_B(insn);
 uint32_t aux = *pc++;
 VM_PROTECT_PC();
 sethvalue(L, ra, luaH_new(L, aux, b == 0 ? 0 : (1 << (b - 1))));
 VM_PROTECT(luaC_checkGC(L));
 VM_NEXT();
 }
 VM_CASE(LOP_DUPTABLE)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 TValue* kv = VM_KV(LUAU_INSN_D(insn));
 VM_PROTECT_PC();
 sethvalue(L, ra, luaH_clone(L, hvalue(kv)));
 VM_PROTECT(luaC_checkGC(L));
 VM_NEXT();
 }
 VM_CASE(LOP_SETLIST)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 StkId rb = &base[LUAU_INSN_B(insn)];
 int c = LUAU_INSN_C(insn) - 1;
 uint32_t index = *pc++;
 if (c == LUA_MULTRET)
 {
 c = int(L->top - rb);
 L->top = L->ci->top;
 }
 Table* h = hvalue(ra);
 if (!ttistable(ra))
 return;
 int last = index + c - 1;
 if (last > h->sizearray)
 {
 VM_PROTECT_PC();
 luaH_resizearray(L, h, last);
 }
 TValue* array = h->array;
 for (int i = 0; i < c; ++i)
 setobj2t(L, &array[index + i - 1], rb + i);
 luaC_barrierfast(L, h);
 VM_NEXT();
 }
 VM_CASE(LOP_FORNPREP)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 if (!ttisnumber(ra + 0) || !ttisnumber(ra + 1) || !ttisnumber(ra + 2))
 {
 VM_PROTECT_PC();
 luaV_prepareFORN(L, ra + 0, ra + 1, ra + 2);
 }
 double limit = nvalue(ra + 0);
 double step = nvalue(ra + 1);
 double idx = nvalue(ra + 2);
 pc += (step > 0 ? idx <= limit : limit <= idx) ? 0 : LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_FORNLOOP)
 {
 VM_INTERRUPT();
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 LUAU_ASSERT(ttisnumber(ra + 0) && ttisnumber(ra + 1) && ttisnumber(ra + 2));
 double limit = nvalue(ra + 0);
 double step = nvalue(ra + 1);
 double idx = nvalue(ra + 2) + step;
 setnvalue(ra + 2, idx);
 if (step > 0 ? idx <= limit : limit <= idx)
 {
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 VM_NEXT();
 }
 }
 VM_CASE(LOP_FORGPREP)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 if (ttisfunction(ra))
 {
 }
 else
 {
 Table* mt = ttistable(ra) ? hvalue(ra)->metatable : ttisuserdata(ra) ? uvalue(ra)->metatable : cast_to(Table*, NULL);
 if (const TValue* fn = fasttm(L, mt, TM_ITER))
 {
 setobj2s(L, ra + 1, ra);
 setobj2s(L, ra, fn);
 L->top = ra + 2;
 LUAU_ASSERT(L->top <= L->stack_last);
 VM_PROTECT(luaD_call(L, ra, 3));
 L->top = L->ci->top;
 ra = VM_REG(LUAU_INSN_A(insn));
 if (ttisnil(ra))
 {
 VM_PROTECT_PC();
 luaG_typeerror(L, ra, "call");
 }
 }
 else if (fasttm(L, mt, TM_CALL))
 {
 }
 else if (ttistable(ra))
 {
 setobj2s(L, ra + 1, ra);
 setpvalue(ra + 2, reinterpret_cast<void*>(uintptr_t(0)));
 setnilvalue(ra);
 }
 else
 {
 VM_PROTECT_PC();
 luaG_typeerror(L, ra, "iterate over");
 }
 }
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_FORGLOOP)
 {
 VM_INTERRUPT();
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 uint32_t aux = *pc;
 if (ttisnil(ra) && ttistable(ra + 1))
 {
 Table* h = hvalue(ra + 1);
 int index = int(reinterpret_cast<uintptr_t>(pvalue(ra + 2)));
 int sizearray = h->sizearray;
 if (LUAU_UNLIKELY(int(aux) > 2))
 for (int i = 2; i < int(aux); ++i)
 setnilvalue(ra + 3 + i);
 if (int(aux) < 0 && (unsigned(index) >= unsigned(sizearray) || ttisnil(&h->array[index])))
 {
 pc++;
 VM_NEXT();
 }
 while (unsigned(index) < unsigned(sizearray))
 {
 TValue* e = &h->array[index];
 if (!ttisnil(e))
 {
 setpvalue(ra + 2, reinterpret_cast<void*>(uintptr_t(index + 1)));
 setnvalue(ra + 3, double(index + 1));
 setobj2s(L, ra + 4, e);
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 index++;
 }
 int sizenode = 1 << h->lsizenode;
 while (unsigned(index - sizearray) < unsigned(sizenode))
 {
 LuaNode* n = &h->node[index - sizearray];
 if (!ttisnil(gval(n)))
 {
 setpvalue(ra + 2, reinterpret_cast<void*>(uintptr_t(index + 1)));
 getnodekey(L, ra + 3, n);
 setobj2s(L, ra + 4, gval(n));
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 index++;
 }
 pc++;
 VM_NEXT();
 }
 else
 {
 setobj2s(L, ra + 3 + 2, ra + 2);
 setobj2s(L, ra + 3 + 1, ra + 1);
 setobj2s(L, ra + 3, ra);
 L->top = ra + 3 + 3;
 LUAU_ASSERT(L->top <= L->stack_last);
 VM_PROTECT(luaD_call(L, ra + 3, uint8_t(aux)));
 L->top = L->ci->top;
 ra = VM_REG(LUAU_INSN_A(insn));
 setobj2s(L, ra + 2, ra + 3);
 pc += ttisnil(ra + 3) ? 1 : LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 }
 VM_CASE(LOP_FORGPREP_INEXT)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 if (cl->env->safeenv && ttistable(ra + 1) && ttisnumber(ra + 2) && nvalue(ra + 2) == 0.0)
 {
 setnilvalue(ra);
 setpvalue(ra + 2, reinterpret_cast<void*>(uintptr_t(0)));
 }
 else if (!ttisfunction(ra))
 {
 VM_PROTECT_PC();
 luaG_typeerror(L, ra, "iterate over");
 }
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_DEP_FORGLOOP_INEXT)
 {
 LUAU_ASSERT(!"Unsupported deprecated opcode");
 LUAU_UNREACHABLE();
 }
 VM_CASE(LOP_FORGPREP_NEXT)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 if (cl->env->safeenv && ttistable(ra + 1) && ttisnil(ra + 2))
 {
 setnilvalue(ra);
 setpvalue(ra + 2, reinterpret_cast<void*>(uintptr_t(0)));
 }
 else if (!ttisfunction(ra))
 {
 VM_PROTECT_PC();
 luaG_typeerror(L, ra, "iterate over");
 }
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_NATIVECALL)
 {
 Proto* p = cl->l.p;
 LUAU_ASSERT(p->execdata);
 CallInfo* ci = L->ci;
 ci->flags = LUA_CALLINFO_NATIVE;
 ci->savedpc = p->code;
#if LUA_CUSTOM_EXECUTION
 if (L->global->ecb.enter(L, p) == 1)
 goto reentry;
 else
 goto exit;
#else
 LUAU_ASSERT(!"Opcode is only valid when LUA_CUSTOM_EXECUTION is defined");
 LUAU_UNREACHABLE();
#endif
 }
 VM_CASE(LOP_GETVARARGS)
 {
 Instruction insn = *pc++;
 int b = LUAU_INSN_B(insn) - 1;
 int n = cast_int(base - L->ci->func) - cl->l.p->numparams - 1;
 if (b == LUA_MULTRET)
 {
 VM_PROTECT(luaD_checkstack(L, n));
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 for (int j = 0; j < n; j++)
 setobj2s(L, ra + j, base - n + j);
 L->top = ra + n;
 VM_NEXT();
 }
 else
 {
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 for (int j = 0; j < b && j < n; j++)
 setobj2s(L, ra + j, base - n + j);
 for (int j = n; j < b; j++)
 setnilvalue(ra + j);
 VM_NEXT();
 }
 }
 VM_CASE(LOP_DUPCLOSURE)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 TValue* kv = VM_KV(LUAU_INSN_D(insn));
 Closure* kcl = clvalue(kv);
 VM_PROTECT_PC();
 Closure* ncl = (kcl->env == cl->env) ? kcl : luaF_newLclosure(L, kcl->nupvalues, cl->env, kcl->l.p);
 setclvalue(L, ra, ncl);
 for (int ui = 0; ui < kcl->nupvalues; ++ui)
 {
 Instruction uinsn = pc[ui];
 LUAU_ASSERT(LUAU_INSN_OP(uinsn) == LOP_CAPTURE);
 LUAU_ASSERT(LUAU_INSN_A(uinsn) == LCT_VAL || LUAU_INSN_A(uinsn) == LCT_UPVAL);
 TValue* uv = (LUAU_INSN_A(uinsn) == LCT_VAL) ? VM_REG(LUAU_INSN_B(uinsn)) : VM_UV(LUAU_INSN_B(uinsn));
 if (ncl == kcl && luaO_rawequalObj(&ncl->l.uprefs[ui], uv))
 continue;
 if (ncl == kcl && kcl->preload == 0)
 {
 ncl = luaF_newLclosure(L, kcl->nupvalues, cl->env, kcl->l.p);
 setclvalue(L, ra, ncl);
 ui = -1;
 continue;
 }
 setobj(L, &ncl->l.uprefs[ui], uv);
 luaC_barrier(L, ncl, uv);
 }
 ncl->preload = 0;
 if (kcl != ncl)
 VM_PROTECT(luaC_checkGC(L));
 pc += kcl->nupvalues;
 VM_NEXT();
 }
 VM_CASE(LOP_PREPVARARGS)
 {
 Instruction insn = *pc++;
 int numparams = LUAU_INSN_A(insn);
 VM_PROTECT(luaD_checkstack(L, cl->stacksize + numparams));
 LUAU_ASSERT(cast_int(L->top - base) >= numparams);
 StkId fixed = base; // first fixed argument
 base = L->top;
 for (int i = 0; i < numparams; ++i)
 {
 setobj2s(L, base + i, fixed + i);
 setnilvalue(fixed + i);
 }
 L->ci->base = base;
 L->ci->top = base + cl->stacksize;
 L->base = base;
 L->top = L->ci->top;
 VM_NEXT();
 }
 VM_CASE(LOP_JUMPBACK)
 {
 VM_INTERRUPT();
 Instruction insn = *pc++;
 pc += LUAU_INSN_D(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_LOADKX)
 {
 Instruction insn = *pc++;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 uint32_t aux = *pc++;
 TValue* kv = VM_KV(aux);
 setobj2s(L, ra, kv);
 VM_NEXT();
 }
 VM_CASE(LOP_JUMPX)
 {
 VM_INTERRUPT();
 Instruction insn = *pc++;
 pc += LUAU_INSN_E(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_FASTCALL)
 {
 Instruction insn = *pc++;
 int bfid = LUAU_INSN_A(insn);
 int skip = LUAU_INSN_C(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code + skip) < unsigned(cl->l.p->sizecode));
 Instruction call = pc[skip];
 LUAU_ASSERT(LUAU_INSN_OP(call) == LOP_CALL);
 StkId ra = VM_REG(LUAU_INSN_A(call));
 int nparams = LUAU_INSN_B(call) - 1;
 int nresults = LUAU_INSN_C(call) - 1;
 nparams = (nparams == LUA_MULTRET) ? int(L->top - ra - 1) : nparams;
 luau_FastFunction f = luauF_table[bfid];
 LUAU_ASSERT(f);
 if (cl->env->safeenv)
 {
 VM_PROTECT_PC();
 int n = f(L, ra, ra + 1, nresults, ra + 2, nparams);
 if (n >= 0)
 {
 L->top = (nresults == LUA_MULTRET) ? ra + n : L->ci->top;
 pc += skip + 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 VM_NEXT();
 }
 }
 else
 {
 VM_NEXT();
 }
 }
 VM_CASE(LOP_COVERAGE)
 {
 Instruction insn = *pc++;
 int hits = LUAU_INSN_E(insn);
 hits = (hits < (1 << 23) - 1) ? hits + 1 : hits;
 VM_PATCH_E(pc - 1, hits);
 VM_NEXT();
 }
 VM_CASE(LOP_CAPTURE)
 {
 LUAU_ASSERT(!"CAPTURE is a pseudo-opcode and must be executed as part of NEWCLOSURE");
 LUAU_UNREACHABLE();
 }
 VM_CASE(LOP_DEP_JUMPIFEQK)
 {
 LUAU_ASSERT(!"Unsupported deprecated opcode");
 LUAU_UNREACHABLE();
 }
 VM_CASE(LOP_DEP_JUMPIFNOTEQK)
 {
 LUAU_ASSERT(!"Unsupported deprecated opcode");
 LUAU_UNREACHABLE();
 }
 VM_CASE(LOP_FASTCALL1)
 {
 Instruction insn = *pc++;
 int bfid = LUAU_INSN_A(insn);
 TValue* arg = VM_REG(LUAU_INSN_B(insn));
 int skip = LUAU_INSN_C(insn);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code + skip) < unsigned(cl->l.p->sizecode));
 Instruction call = pc[skip];
 LUAU_ASSERT(LUAU_INSN_OP(call) == LOP_CALL);
 StkId ra = VM_REG(LUAU_INSN_A(call));
 int nparams = 1;
 int nresults = LUAU_INSN_C(call) - 1;
 luau_FastFunction f = luauF_table[bfid];
 LUAU_ASSERT(f);
 if (cl->env->safeenv)
 {
 VM_PROTECT_PC();
 int n = f(L, ra, arg, nresults, NULL, nparams);
 if (n >= 0)
 {
 if (nresults == LUA_MULTRET)
 L->top = ra + n;
 pc += skip + 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 VM_NEXT();
 }
 }
 else
 {
 VM_NEXT();
 }
 }
 VM_CASE(LOP_FASTCALL2)
 {
 Instruction insn = *pc++;
 int bfid = LUAU_INSN_A(insn);
 int skip = LUAU_INSN_C(insn) - 1;
 uint32_t aux = *pc++;
 TValue* arg1 = VM_REG(LUAU_INSN_B(insn));
 TValue* arg2 = VM_REG(aux);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code + skip) < unsigned(cl->l.p->sizecode));
 Instruction call = pc[skip];
 LUAU_ASSERT(LUAU_INSN_OP(call) == LOP_CALL);
 StkId ra = VM_REG(LUAU_INSN_A(call));
 int nparams = 2;
 int nresults = LUAU_INSN_C(call) - 1;
 luau_FastFunction f = luauF_table[bfid];
 LUAU_ASSERT(f);
 if (cl->env->safeenv)
 {
 VM_PROTECT_PC();
 int n = f(L, ra, arg1, nresults, arg2, nparams);
 if (n >= 0)
 {
 if (nresults == LUA_MULTRET)
 L->top = ra + n;
 pc += skip + 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 VM_NEXT();
 }
 }
 else
 {
 VM_NEXT();
 }
 }
 VM_CASE(LOP_FASTCALL2K)
 {
 Instruction insn = *pc++;
 int bfid = LUAU_INSN_A(insn);
 int skip = LUAU_INSN_C(insn) - 1;
 uint32_t aux = *pc++;
 TValue* arg1 = VM_REG(LUAU_INSN_B(insn));
 TValue* arg2 = VM_KV(aux);
 LUAU_ASSERT(unsigned(pc - cl->l.p->code + skip) < unsigned(cl->l.p->sizecode));
 Instruction call = pc[skip];
 LUAU_ASSERT(LUAU_INSN_OP(call) == LOP_CALL);
 StkId ra = VM_REG(LUAU_INSN_A(call));
 int nparams = 2;
 int nresults = LUAU_INSN_C(call) - 1;
 luau_FastFunction f = luauF_table[bfid];
 LUAU_ASSERT(f);
 if (cl->env->safeenv)
 {
 VM_PROTECT_PC();
 int n = f(L, ra, arg1, nresults, arg2, nparams);
 if (n >= 0)
 {
 if (nresults == LUA_MULTRET)
 L->top = ra + n;
 pc += skip + 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 else
 {
 VM_NEXT();
 }
 }
 else
 {
 VM_NEXT();
 }
 }
 VM_CASE(LOP_BREAK)
 {
 LUAU_ASSERT(cl->l.p->debuginsn);
 uint8_t op = cl->l.p->debuginsn[unsigned(pc - cl->l.p->code)];
 LUAU_ASSERT(op != LOP_BREAK);
 if (L->global->cb.debugbreak)
 {
 VM_PROTECT(luau_callhook(L, L->global->cb.debugbreak, NULL));
 if (L->status != 0)
 goto exit;
 }
 VM_CONTINUE(op);
 }
 VM_CASE(LOP_JUMPXEQKNIL)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 static_assert(LUA_TNIL == 0, "we expect type-1 to be negative iff type is nil");
 pc += int((ttype(ra) - 1) ^ aux) < 0 ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_JUMPXEQKB)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 pc += int(ttisboolean(ra) && bvalue(ra) == int(aux & 1)) != (aux >> 31) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_JUMPXEQKN)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 TValue* kv = VM_KV(aux & 0xffffff);
 LUAU_ASSERT(ttisnumber(kv));
#if defined(__aarch64__)
 if (aux >> 31)
 pc += !(ttisnumber(ra) && nvalue(ra) == nvalue(kv)) ? LUAU_INSN_D(insn) : 1;
 else
 pc += (ttisnumber(ra) && nvalue(ra) == nvalue(kv)) ? LUAU_INSN_D(insn) : 1;
#else
 pc += int(ttisnumber(ra) && nvalue(ra) == nvalue(kv)) != (aux >> 31) ? LUAU_INSN_D(insn) : 1;
#endif
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
 VM_CASE(LOP_JUMPXEQKS)
 {
 Instruction insn = *pc++;
 uint32_t aux = *pc;
 StkId ra = VM_REG(LUAU_INSN_A(insn));
 TValue* kv = VM_KV(aux & 0xffffff);
 LUAU_ASSERT(ttisstring(kv));
 pc += int(ttisstring(ra) && gcvalue(ra) == gcvalue(kv)) != (aux >> 31) ? LUAU_INSN_D(insn) : 1;
 LUAU_ASSERT(unsigned(pc - cl->l.p->code) < unsigned(cl->l.p->sizecode));
 VM_NEXT();
 }
#if !VM_USE_CGOTO
 default:
 LUAU_ASSERT(!"Unknown opcode");
 LUAU_UNREACHABLE();
#endif
 }
 }
exit:;
}
void luau_execute(lua_State* L)
{
 if (L->singlestep)
 luau_execute<true>(L);
 else
 luau_execute<false>(L);
}
int luau_precall(lua_State* L, StkId func, int nresults)
{
 if (!ttisfunction(func))
 {
 luaV_tryfuncTM(L, func);
 }
 Closure* ccl = clvalue(func);
 CallInfo* ci = incr_ci(L);
 ci->func = func;
 ci->base = func + 1;
 ci->top = L->top + ccl->stacksize;
 ci->savedpc = NULL;
 ci->flags = 0;
 ci->nresults = nresults;
 L->base = ci->base;
 luaD_checkstack(L, ccl->stacksize);
 LUAU_ASSERT(ci->top <= L->stack_last);
 if (!ccl->isC)
 {
 Proto* p = ccl->l.p;
 StkId argi = L->top;
 StkId argend = L->base + p->numparams;
 while (argi < argend)
 setnilvalue(argi++);
 L->top = p->is_vararg ? argi : ci->top;
 ci->savedpc = p->code;
#if LUA_CUSTOM_EXECUTION
 if (p->execdata)
 ci->flags = LUA_CALLINFO_NATIVE;
#endif
 return PCRLUA;
 }
 else
 {
 lua_CFunction func = ccl->c.f;
 int n = func(L);
 if (n < 0)
 return PCRYIELD;
 CallInfo* ci = L->ci;
 CallInfo* cip = ci - 1;
 StkId res = ci->func;
 StkId vali = L->top - n;
 StkId valend = L->top;
 int i;
 for (i = nresults; i != 0 && vali < valend; i--)
 setobj2s(L, res++, vali++);
 while (i-- > 0)
 setnilvalue(res++);
 L->ci = cip;
 L->base = cip->base;
 L->top = res;
 return PCRC;
 }
}
void luau_poscall(lua_State* L, StkId first)
{
 CallInfo* ci = L->ci;
 CallInfo* cip = ci - 1;
 StkId res = ci->func;
 StkId vali = first;
 StkId valend = L->top;
 int i;
 for (i = ci->nresults; i != 0 && vali < valend; i--)
 setobj2s(L, res++, vali++);
 while (i-- > 0)
 setnilvalue(res++);
 L->ci = cip;
 L->base = cip->base;
 L->top = (ci->nresults == LUA_MULTRET) ? res : cip->top;
}
LUAU_FASTFLAGVARIABLE(LuauGetImportDirect, false)
template<typename T>
struct TempBuffer
{
 lua_State* L;
 T* data;
 size_t count;
 TempBuffer(lua_State* L, size_t count)
 : L(L)
 , data(luaM_newarray(L, count, T, 0))
 , count(count)
 {
 }
 ~TempBuffer()
 {
 luaM_freearray(L, data, count, T, 0);
 }
 T& operator[](size_t index)
 {
 LUAU_ASSERT(index < count);
 return data[index];
 }
};
void luaV_getimport(lua_State* L, Table* env, TValue* k, StkId res, uint32_t id, bool propagatenil)
{
 int count = id >> 30;
 LUAU_ASSERT(count > 0);
 int id0 = int(id >> 20) & 1023;
 int id1 = int(id >> 10) & 1023;
 int id2 = int(id) & 1023;
 ptrdiff_t resp = savestack(L, res);
 TValue g;
 sethvalue(L, &g, env);
 luaV_gettable(L, &g, &k[id0], res);
 if (count < 2)
 return;
 res = restorestack(L, resp);
 if (!propagatenil || !ttisnil(res))
 luaV_gettable(L, res, &k[id1], res);
 if (count < 3)
 return;
 res = restorestack(L, resp);
 if (!propagatenil || !ttisnil(res))
 luaV_gettable(L, res, &k[id2], res);
}
void luaV_getimport_dep(lua_State* L, Table* env, TValue* k, uint32_t id, bool propagatenil)
{
 LUAU_ASSERT(!FFlag::LuauGetImportDirect);
 int count = id >> 30;
 int id0 = count > 0 ? int(id >> 20) & 1023 : -1;
 int id1 = count > 1 ? int(id >> 10) & 1023 : -1;
 int id2 = count > 2 ? int(id) & 1023 : -1;
 luaD_checkstack(L, 1);
 setnilvalue(L->top);
 L->top++;
 TValue g;
 sethvalue(L, &g, env);
 luaV_gettable(L, &g, &k[id0], L->top - 1);
 if (id1 >= 0 && (!propagatenil || !ttisnil(L->top - 1)))
 luaV_gettable(L, L->top - 1, &k[id1], L->top - 1);
 if (id2 >= 0 && (!propagatenil || !ttisnil(L->top - 1)))
 luaV_gettable(L, L->top - 1, &k[id2], L->top - 1);
}
template<typename T>
static T read(const char* data, size_t size, size_t& offset)
{
 T result;
 memcpy(&result, data + offset, sizeof(T));
 offset += sizeof(T);
 return result;
}
static unsigned int readVarInt(const char* data, size_t size, size_t& offset)
{
 unsigned int result = 0;
 unsigned int shift = 0;
 uint8_t byte;
 do
 {
 byte = read<uint8_t>(data, size, offset);
 result |= (byte & 127) << shift;
 shift += 7;
 } while (byte & 128);
 return result;
}
static TString* readString(TempBuffer<TString*>& strings, const char* data, size_t size, size_t& offset)
{
 unsigned int id = readVarInt(data, size, offset);
 return id == 0 ? NULL : strings[id - 1];
}
static void resolveImportSafe(lua_State* L, Table* env, TValue* k, uint32_t id)
{
 struct ResolveImport
 {
 TValue* k;
 uint32_t id;
 static void run(lua_State* L, void* ud)
 {
 ResolveImport* self = static_cast<ResolveImport*>(ud);
 if (FFlag::LuauGetImportDirect)
 {
 luaD_checkstack(L, 1);
 setnilvalue(L->top);
 L->top++;
 luaV_getimport(L, L->gt, self->k, L->top - 1, self->id, true);
 }
 else
 luaV_getimport_dep(L, L->gt, self->k, self->id, true);
 }
 };
 ResolveImport ri = {k, id};
 if (L->gt->safeenv)
 {
 int oldTop = lua_gettop(L);
 int status = luaD_pcall(L, &ResolveImport::run, &ri, savestack(L, L->top), 0);
 LUAU_ASSERT(oldTop + 1 == lua_gettop(L));
 if (status != 0)
 {
 setnilvalue(L->top - 1);
 }
 }
 else
 {
 setnilvalue(L->top);
 L->top++;
 }
}
int luau_load(lua_State* L, const char* chunkname, const char* data, size_t size, int env)
{
 size_t offset = 0;
 uint8_t version = read<uint8_t>(data, size, offset);
 if (version == 0)
 {
 char chunkbuf[LUA_IDSIZE];
 const char* chunkid = luaO_chunkid(chunkbuf, sizeof(chunkbuf), chunkname, strlen(chunkname));
 lua_pushfstring(L, "%s%.*s", chunkid, int(size - offset), data + offset);
 return 1;
 }
 if (version < LBC_VERSION_MIN || version > LBC_VERSION_MAX)
 {
 char chunkbuf[LUA_IDSIZE];
 const char* chunkid = luaO_chunkid(chunkbuf, sizeof(chunkbuf), chunkname, strlen(chunkname));
 lua_pushfstring(L, "%s: bytecode version mismatch (expected [%d..%d], got %d)", chunkid, LBC_VERSION_MIN, LBC_VERSION_MAX, version);
 return 1;
 }
 size_t GCthreshold = L->global->GCthreshold;
 L->global->GCthreshold = SIZE_MAX;
 Table* envt = (env == 0) ? L->gt : hvalue(luaA_toobject(L, env));
 TString* source = luaS_new(L, chunkname);
 unsigned int stringCount = readVarInt(data, size, offset);
 TempBuffer<TString*> strings(L, stringCount);
 for (unsigned int i = 0; i < stringCount; ++i)
 {
 unsigned int length = readVarInt(data, size, offset);
 strings[i] = luaS_newlstr(L, data + offset, length);
 offset += length;
 }
 unsigned int protoCount = readVarInt(data, size, offset);
 TempBuffer<Proto*> protos(L, protoCount);
 for (unsigned int i = 0; i < protoCount; ++i)
 {
 Proto* p = luaF_newproto(L);
 p->source = source;
 p->bytecodeid = int(i);
 p->maxstacksize = read<uint8_t>(data, size, offset);
 p->numparams = read<uint8_t>(data, size, offset);
 p->nups = read<uint8_t>(data, size, offset);
 p->is_vararg = read<uint8_t>(data, size, offset);
 p->sizecode = readVarInt(data, size, offset);
 p->code = luaM_newarray(L, p->sizecode, Instruction, p->memcat);
 for (int j = 0; j < p->sizecode; ++j)
 p->code[j] = read<uint32_t>(data, size, offset);
 p->codeentry = p->code;
 p->sizek = readVarInt(data, size, offset);
 p->k = luaM_newarray(L, p->sizek, TValue, p->memcat);
#ifdef HARDMEMTESTS
 for (int j = 0; j < p->sizek; ++j)
 {
 setnilvalue(&p->k[j]);
 }
#endif
 for (int j = 0; j < p->sizek; ++j)
 {
 switch (read<uint8_t>(data, size, offset))
 {
 case LBC_CONSTANT_NIL:
 setnilvalue(&p->k[j]);
 break;
 case LBC_CONSTANT_BOOLEAN:
 {
 uint8_t v = read<uint8_t>(data, size, offset);
 setbvalue(&p->k[j], v);
 break;
 }
 case LBC_CONSTANT_NUMBER:
 {
 double v = read<double>(data, size, offset);
 setnvalue(&p->k[j], v);
 break;
 }
 case LBC_CONSTANT_STRING:
 {
 TString* v = readString(strings, data, size, offset);
 setsvalue(L, &p->k[j], v);
 break;
 }
 case LBC_CONSTANT_IMPORT:
 {
 uint32_t iid = read<uint32_t>(data, size, offset);
 resolveImportSafe(L, envt, p->k, iid);
 setobj(L, &p->k[j], L->top - 1);
 L->top--;
 break;
 }
 case LBC_CONSTANT_TABLE:
 {
 int keys = readVarInt(data, size, offset);
 Table* h = luaH_new(L, 0, keys);
 for (int i = 0; i < keys; ++i)
 {
 int key = readVarInt(data, size, offset);
 TValue* val = luaH_set(L, h, &p->k[key]);
 setnvalue(val, 0.0);
 }
 sethvalue(L, &p->k[j], h);
 break;
 }
 case LBC_CONSTANT_CLOSURE:
 {
 uint32_t fid = readVarInt(data, size, offset);
 Closure* cl = luaF_newLclosure(L, protos[fid]->nups, envt, protos[fid]);
 cl->preload = (cl->nupvalues > 0);
 setclvalue(L, &p->k[j], cl);
 break;
 }
 default:
 LUAU_ASSERT(!"Unexpected constant kind");
 }
 }
 p->sizep = readVarInt(data, size, offset);
 p->p = luaM_newarray(L, p->sizep, Proto*, p->memcat);
 for (int j = 0; j < p->sizep; ++j)
 {
 uint32_t fid = readVarInt(data, size, offset);
 p->p[j] = protos[fid];
 }
 p->linedefined = readVarInt(data, size, offset);
 p->debugname = readString(strings, data, size, offset);
 uint8_t lineinfo = read<uint8_t>(data, size, offset);
 if (lineinfo)
 {
 p->linegaplog2 = read<uint8_t>(data, size, offset);
 int intervals = ((p->sizecode - 1) >> p->linegaplog2) + 1;
 int absoffset = (p->sizecode + 3) & ~3;
 p->sizelineinfo = absoffset + intervals * sizeof(int);
 p->lineinfo = luaM_newarray(L, p->sizelineinfo, uint8_t, p->memcat);
 p->abslineinfo = (int*)(p->lineinfo + absoffset);
 uint8_t lastoffset = 0;
 for (int j = 0; j < p->sizecode; ++j)
 {
 lastoffset += read<uint8_t>(data, size, offset);
 p->lineinfo[j] = lastoffset;
 }
 int lastline = 0;
 for (int j = 0; j < intervals; ++j)
 {
 lastline += read<int32_t>(data, size, offset);
 p->abslineinfo[j] = lastline;
 }
 }
 uint8_t debuginfo = read<uint8_t>(data, size, offset);
 if (debuginfo)
 {
 p->sizelocvars = readVarInt(data, size, offset);
 p->locvars = luaM_newarray(L, p->sizelocvars, LocVar, p->memcat);
 for (int j = 0; j < p->sizelocvars; ++j)
 {
 p->locvars[j].varname = readString(strings, data, size, offset);
 p->locvars[j].startpc = readVarInt(data, size, offset);
 p->locvars[j].endpc = readVarInt(data, size, offset);
 p->locvars[j].reg = read<uint8_t>(data, size, offset);
 }
 p->sizeupvalues = readVarInt(data, size, offset);
 p->upvalues = luaM_newarray(L, p->sizeupvalues, TString*, p->memcat);
 for (int j = 0; j < p->sizeupvalues; ++j)
 {
 p->upvalues[j] = readString(strings, data, size, offset);
 }
 }
 protos[i] = p;
 }
 uint32_t mainid = readVarInt(data, size, offset);
 Proto* main = protos[mainid];
 luaC_threadbarrier(L);
 Closure* cl = luaF_newLclosure(L, 0, envt, main);
 setclvalue(L, L->top, cl);
 incr_top(L);
 L->global->GCthreshold = GCthreshold;
 return 0;
}
#define MAXTAGLOOP 100
const TValue* luaV_tonumber(const TValue* obj, TValue* n)
{
 double num;
 if (ttisnumber(obj))
 return obj;
 if (ttisstring(obj) && luaO_str2d(svalue(obj), &num))
 {
 setnvalue(n, num);
 return n;
 }
 else
 return NULL;
}
int luaV_tostring(lua_State* L, StkId obj)
{
 if (!ttisnumber(obj))
 return 0;
 else
 {
 char s[LUAI_MAXNUM2STR];
 double n = nvalue(obj);
 char* e = luai_num2str(s, n);
 LUAU_ASSERT(e < s + sizeof(s));
 setsvalue(L, obj, luaS_newlstr(L, s, e - s));
 return 1;
 }
}
const float* luaV_tovector(const TValue* obj)
{
 if (ttisvector(obj))
 return obj->value.v;
 return nullptr;
}
static StkId callTMres(lua_State* L, StkId res, const TValue* f, const TValue* p1, const TValue* p2)
{
 ptrdiff_t result = savestack(L, res);
 LUAU_ASSERT((L->top + 3) < (L->stack + L->stacksize));
 setobj2s(L, L->top, f);
 setobj2s(L, L->top + 1, p1); // 1st argument
 setobj2s(L, L->top + 2, p2);
 luaD_checkstack(L, 3);
 L->top += 3;
 luaD_call(L, L->top - 3, 1);
 res = restorestack(L, result);
 L->top--;
 setobj2s(L, res, L->top);
 return res;
}
static void callTM(lua_State* L, const TValue* f, const TValue* p1, const TValue* p2, const TValue* p3)
{
 LUAU_ASSERT((L->top + 4) < (L->stack + L->stacksize));
 setobj2s(L, L->top, f);
 setobj2s(L, L->top + 1, p1); // 1st argument
 setobj2s(L, L->top + 2, p2);
 setobj2s(L, L->top + 3, p3); // 3th argument
 luaD_checkstack(L, 4);
 L->top += 4;
 luaD_call(L, L->top - 4, 0);
}
void luaV_gettable(lua_State* L, const TValue* t, TValue* key, StkId val)
{
 int loop;
 for (loop = 0; loop < MAXTAGLOOP; loop++)
 {
 const TValue* tm;
 if (ttistable(t))
 {
 Table* h = hvalue(t);
 const TValue* res = luaH_get(h, key);
 if (res != luaO_nilobject)
 L->cachedslot = gval2slot(h, res);
 if (!ttisnil(res)
 || (tm = fasttm(L, h->metatable, TM_INDEX)) == NULL)
 {
 setobj2s(L, val, res);
 return;
 }
 }
 else if (ttisnil(tm = luaT_gettmbyobj(L, t, TM_INDEX)))
 luaG_indexerror(L, t, key);
 if (ttisfunction(tm))
 {
 callTMres(L, val, tm, t, key);
 return;
 }
 t = tm;
 }
 luaG_runerror(L, "'__index' chain too long; possible loop");
}
void luaV_settable(lua_State* L, const TValue* t, TValue* key, StkId val)
{
 int loop;
 TValue temp;
 for (loop = 0; loop < MAXTAGLOOP; loop++)
 {
 const TValue* tm;
 if (ttistable(t))
 {
 Table* h = hvalue(t);
 const TValue* oldval = luaH_get(h, key);
 if (!ttisnil(oldval) || (tm = fasttm(L, h->metatable, TM_NEWINDEX)) == NULL)
 {
 if (h->readonly)
 luaG_readonlyerror(L);
 TValue* newval = luaH_setslot(L, h, oldval, key);
 L->cachedslot = gval2slot(h, newval);
 setobj2t(L, newval, val);
 luaC_barriert(L, h, val);
 return;
 }
 }
 else if (ttisnil(tm = luaT_gettmbyobj(L, t, TM_NEWINDEX)))
 luaG_indexerror(L, t, key);
 if (ttisfunction(tm))
 {
 callTM(L, tm, t, key, val);
 return;
 }
 setobj(L, &temp, tm); // avoid pointing inside table (may rehash)
 t = &temp;
 }
 luaG_runerror(L, "'__newindex' chain too long; possible loop");
}
static int call_binTM(lua_State* L, const TValue* p1, const TValue* p2, StkId res, TMS event)
{
 const TValue* tm = luaT_gettmbyobj(L, p1, event);
 if (ttisnil(tm))
 tm = luaT_gettmbyobj(L, p2, event);
 if (ttisnil(tm))
 return 0;
 callTMres(L, res, tm, p1, p2);
 return 1;
}
static const TValue* get_compTM(lua_State* L, Table* mt1, Table* mt2, TMS event)
{
 const TValue* tm1 = fasttm(L, mt1, event);
 const TValue* tm2;
 if (tm1 == NULL)
 return NULL;
 if (mt1 == mt2)
 return tm1;
 tm2 = fasttm(L, mt2, event);
 if (tm2 == NULL)
 return NULL;
 if (luaO_rawequalObj(tm1, tm2)) // same metamethods?
 return tm1;
 return NULL;
}
static int call_orderTM(lua_State* L, const TValue* p1, const TValue* p2, TMS event, bool error = false)
{
 const TValue* tm1 = luaT_gettmbyobj(L, p1, event);
 const TValue* tm2;
 if (ttisnil(tm1))
 {
 if (error)
 luaG_ordererror(L, p1, p2, event);
 return -1;
 }
 tm2 = luaT_gettmbyobj(L, p2, event);
 if (!luaO_rawequalObj(tm1, tm2))
 {
 if (error)
 luaG_ordererror(L, p1, p2, event);
 return -1;
 }
 callTMres(L, L->top, tm1, p1, p2);
 return !l_isfalse(L->top);
}
int luaV_strcmp(const TString* ls, const TString* rs)
{
 if (ls == rs)
 return 0;
 const char* l = getstr(ls);
 const char* r = getstr(rs);
 if (*l != *r)
 return uint8_t(*l) - uint8_t(*r);
 size_t ll = ls->len;
 size_t lr = rs->len;
 size_t lmin = ll < lr ? ll : lr;
 int res = memcmp(l, r, lmin);
 if (res != 0)
 return res;
 return ll == lr ? 0 : ll < lr ? -1 : 1;
}
int luaV_lessthan(lua_State* L, const TValue* l, const TValue* r)
{
 if (LUAU_UNLIKELY(ttype(l) != ttype(r)))
 luaG_ordererror(L, l, r, TM_LT);
 else if (LUAU_LIKELY(ttisnumber(l)))
 return luai_numlt(nvalue(l), nvalue(r));
 else if (ttisstring(l))
 return luaV_strcmp(tsvalue(l), tsvalue(r)) < 0;
 else
 return call_orderTM(L, l, r, TM_LT, true);
}
int luaV_lessequal(lua_State* L, const TValue* l, const TValue* r)
{
 int res;
 if (ttype(l) != ttype(r))
 luaG_ordererror(L, l, r, TM_LE);
 else if (ttisnumber(l))
 return luai_numle(nvalue(l), nvalue(r));
 else if (ttisstring(l))
 return luaV_strcmp(tsvalue(l), tsvalue(r)) <= 0;
 else if ((res = call_orderTM(L, l, r, TM_LE)) != -1)
 return res;
 else if ((res = call_orderTM(L, r, l, TM_LT)) == -1)
 luaG_ordererror(L, l, r, TM_LE);
 return !res;
}
int luaV_equalval(lua_State* L, const TValue* t1, const TValue* t2)
{
 const TValue* tm;
 LUAU_ASSERT(ttype(t1) == ttype(t2));
 switch (ttype(t1))
 {
 case LUA_TNIL:
 return 1;
 case LUA_TNUMBER:
 return luai_numeq(nvalue(t1), nvalue(t2));
 case LUA_TVECTOR:
 return luai_veceq(vvalue(t1), vvalue(t2));
 case LUA_TBOOLEAN:
 return bvalue(t1) == bvalue(t2);
 case LUA_TLIGHTUSERDATA:
 return pvalue(t1) == pvalue(t2);
 case LUA_TUSERDATA:
 {
 tm = get_compTM(L, uvalue(t1)->metatable, uvalue(t2)->metatable, TM_EQ);
 if (!tm)
 return uvalue(t1) == uvalue(t2);
 break;
 }
 case LUA_TTABLE:
 {
 tm = get_compTM(L, hvalue(t1)->metatable, hvalue(t2)->metatable, TM_EQ);
 if (!tm)
 return hvalue(t1) == hvalue(t2);
 break;
 }
 default:
 return gcvalue(t1) == gcvalue(t2);
 }
 callTMres(L, L->top, tm, t1, t2);
 return !l_isfalse(L->top);
}
void luaV_concat(lua_State* L, int total, int last)
{
 do
 {
 StkId top = L->base + last + 1;
 int n = 2;
 if (!(ttisstring(top - 2) || ttisnumber(top - 2)) || !tostring(L, top - 1))
 {
 if (!call_binTM(L, top - 2, top - 1, top - 2, TM_CONCAT))
 luaG_concaterror(L, top - 2, top - 1);
 }
 else if (tsvalue(top - 1)->len == 0)
 (void)tostring(L, top - 2); // result is first op (as string)
 else
 {
 size_t tl = tsvalue(top - 1)->len;
 char* buffer;
 int i;
 for (n = 1; n < total && tostring(L, top - n - 1); n++)
 {
 size_t l = tsvalue(top - n - 1)->len;
 if (l > MAXSSIZE - tl)
 luaG_runerror(L, "string length overflow");
 tl += l;
 }
 char buf[LUA_BUFFERSIZE];
 TString* ts = nullptr;
 if (tl < LUA_BUFFERSIZE)
 {
 buffer = buf;
 }
 else
 {
 ts = luaS_bufstart(L, tl);
 buffer = ts->data;
 }
 tl = 0;
 for (i = n; i > 0; i--)
 {
 size_t l = tsvalue(top - i)->len;
 memcpy(buffer + tl, svalue(top - i), l);
 tl += l;
 }
 if (tl < LUA_BUFFERSIZE)
 {
 setsvalue(L, top - n, luaS_newlstr(L, buffer, tl));
 }
 else
 {
 setsvalue(L, top - n, luaS_buffinish(L, ts));
 }
 }
 total -= n - 1;
 last -= n - 1;
 } while (total > 1);
}
void luaV_doarith(lua_State* L, StkId ra, const TValue* rb, const TValue* rc, TMS op)
{
 TValue tempb, tempc;
 const TValue *b, *c;
 if ((b = luaV_tonumber(rb, &tempb)) != NULL && (c = luaV_tonumber(rc, &tempc)) != NULL)
 {
 double nb = nvalue(b), nc = nvalue(c);
 switch (op)
 {
 case TM_ADD:
 setnvalue(ra, luai_numadd(nb, nc));
 break;
 case TM_SUB:
 setnvalue(ra, luai_numsub(nb, nc));
 break;
 case TM_MUL:
 setnvalue(ra, luai_nummul(nb, nc));
 break;
 case TM_DIV:
 setnvalue(ra, luai_numdiv(nb, nc));
 break;
 case TM_MOD:
 setnvalue(ra, luai_nummod(nb, nc));
 break;
 case TM_POW:
 setnvalue(ra, luai_numpow(nb, nc));
 break;
 case TM_UNM:
 setnvalue(ra, luai_numunm(nb));
 break;
 default:
 LUAU_ASSERT(0);
 break;
 }
 }
 else
 {
 const float* vb = luaV_tovector(rb);
 const float* vc = luaV_tovector(rc);
 if (vb && vc)
 {
 switch (op)
 {
 case TM_ADD:
 setvvalue(ra, vb[0] + vc[0], vb[1] + vc[1], vb[2] + vc[2], vb[3] + vc[3]);
 return;
 case TM_SUB:
 setvvalue(ra, vb[0] - vc[0], vb[1] - vc[1], vb[2] - vc[2], vb[3] - vc[3]);
 return;
 case TM_MUL:
 setvvalue(ra, vb[0] * vc[0], vb[1] * vc[1], vb[2] * vc[2], vb[3] * vc[3]);
 return;
 case TM_DIV:
 setvvalue(ra, vb[0] / vc[0], vb[1] / vc[1], vb[2] / vc[2], vb[3] / vc[3]);
 return;
 case TM_UNM:
 setvvalue(ra, -vb[0], -vb[1], -vb[2], -vb[3]);
 return;
 default:
 break;
 }
 }
 else if (vb)
 {
 c = luaV_tonumber(rc, &tempc);
 if (c)
 {
 float nc = cast_to(float, nvalue(c));
 switch (op)
 {
 case TM_MUL:
 setvvalue(ra, vb[0] * nc, vb[1] * nc, vb[2] * nc, vb[3] * nc);
 return;
 case TM_DIV:
 setvvalue(ra, vb[0] / nc, vb[1] / nc, vb[2] / nc, vb[3] / nc);
 return;
 default:
 break;
 }
 }
 }
 else if (vc)
 {
 b = luaV_tonumber(rb, &tempb);
 if (b)
 {
 float nb = cast_to(float, nvalue(b));
 switch (op)
 {
 case TM_MUL:
 setvvalue(ra, nb * vc[0], nb * vc[1], nb * vc[2], nb * vc[3]);
 return;
 case TM_DIV:
 setvvalue(ra, nb / vc[0], nb / vc[1], nb / vc[2], nb / vc[3]);
 return;
 default:
 break;
 }
 }
 }
 if (!call_binTM(L, rb, rc, ra, op))
 {
 luaG_aritherror(L, rb, rc, op);
 }
 }
}
void luaV_dolen(lua_State* L, StkId ra, const TValue* rb)
{
 const TValue* tm = NULL;
 switch (ttype(rb))
 {
 case LUA_TTABLE:
 {
 Table* h = hvalue(rb);
 if ((tm = fasttm(L, h->metatable, TM_LEN)) == NULL)
 {
 setnvalue(ra, cast_num(luaH_getn(h)));
 return;
 }
 break;
 }
 case LUA_TSTRING:
 {
 TString* ts = tsvalue(rb);
 setnvalue(ra, cast_num(ts->len));
 return;
 }
 default:
 tm = luaT_gettmbyobj(L, rb, TM_LEN);
 }
 if (ttisnil(tm))
 luaG_typeerror(L, rb, "get length of");
 StkId res = callTMres(L, ra, tm, rb, luaO_nilobject);
 if (!ttisnumber(res))
 luaG_runerror(L, "'__len' must return a number"); // note, we can't access rb since stack may have been reallocated
}
LUAU_NOINLINE void luaV_prepareFORN(lua_State* L, StkId plimit, StkId pstep, StkId pinit)
{
 if (!ttisnumber(pinit) && !luaV_tonumber(pinit, pinit))
 luaG_forerror(L, pinit, "initial value");
 if (!ttisnumber(plimit) && !luaV_tonumber(plimit, plimit))
 luaG_forerror(L, plimit, "limit");
 if (!ttisnumber(pstep) && !luaV_tonumber(pstep, pstep))
 luaG_forerror(L, pstep, "step");
}
LUAU_NOINLINE void luaV_callTM(lua_State* L, int nparams, int res)
{
 ++L->nCcalls;
 if (L->nCcalls >= LUAI_MAXCCALLS)
 luaD_checkCstack(L);
 luaD_checkstack(L, LUA_MINSTACK);
 StkId top = L->top;
 StkId fun = top - nparams - 1;
 CallInfo* ci = incr_ci(L);
 ci->func = fun;
 ci->base = fun + 1;
 ci->top = top + LUA_MINSTACK;
 ci->savedpc = NULL;
 ci->flags = 0;
 ci->nresults = (res >= 0);
 LUAU_ASSERT(ci->top <= L->stack_last);
 LUAU_ASSERT(ttisfunction(ci->func));
 LUAU_ASSERT(clvalue(ci->func)->isC);
 L->base = fun + 1;
 LUAU_ASSERT(L->top == L->base + nparams);
 lua_CFunction func = clvalue(fun)->c.f;
 int n = func(L);
 LUAU_ASSERT(n >= 0);
 CallInfo* cip = L->ci - 1;
 if (res >= 0)
 {
 if (n > 0)
 {
 setobj2s(L, &cip->base[res], L->top - n);
 }
 else
 {
 setnilvalue(&cip->base[res]);
 }
 }
 L->ci = cip;
 L->base = cip->base;
 L->top = cip->top;
 --L->nCcalls;
}
LUAU_NOINLINE void luaV_tryfuncTM(lua_State* L, StkId func)
{
 const TValue* tm = luaT_gettmbyobj(L, func, TM_CALL);
 if (!ttisfunction(tm))
 luaG_typeerror(L, func, "call");
 for (StkId p = L->top; p > func; p--)
 setobj2s(L, p, p - 1);
 L->top++;
 setobj2s(L, func, tm); // tag method is the new function to be called
}
#ifdef LUAU_ENABLE_COMPILER
#undef upvalue
namespace Luau
{
struct Position
{
 unsigned int line, column;
 Position(unsigned int line, unsigned int column);
 bool operator==(const Position& rhs) const;
 bool operator!=(const Position& rhs) const;
 bool operator<(const Position& rhs) const;
 bool operator>(const Position& rhs) const;
 bool operator<=(const Position& rhs) const;
 bool operator>=(const Position& rhs) const;
 void shift(const Position& start, const Position& oldEnd, const Position& newEnd);
};
struct Location
{
 Position begin, end;
 Location();
 Location(const Position& begin, const Position& end);
 Location(const Position& begin, unsigned int length);
 Location(const Location& begin, const Location& end);
 bool operator==(const Location& rhs) const;
 bool operator!=(const Location& rhs) const;
 bool encloses(const Location& l) const;
 bool overlaps(const Location& l) const;
 bool contains(const Position& p) const;
 bool containsClosed(const Position& p) const;
 void extend(const Location& other);
 void shift(const Position& start, const Position& oldEnd, const Position& newEnd);
};
}
#include <optional>
#include <string>
namespace Luau
{
struct AstName
{
 const char* value;
 AstName()
 : value(nullptr)
 {
 }
 explicit AstName(const char* value)
 : value(value)
 {
 }
 bool operator==(const AstName& rhs) const
 {
 return value == rhs.value;
 }
 bool operator!=(const AstName& rhs) const
 {
 return value != rhs.value;
 }
 bool operator==(const char* rhs) const
 {
 return value && strcmp(value, rhs) == 0;
 }
 bool operator!=(const char* rhs) const
 {
 return !value || strcmp(value, rhs) != 0;
 }
 bool operator<(const AstName& rhs) const
 {
 return (value && rhs.value) ? strcmp(value, rhs.value) < 0 : value < rhs.value;
 }
};
class AstType;
class AstVisitor;
class AstStat;
class AstStatBlock;
class AstExpr;
class AstTypePack;
struct AstLocal
{
 AstName name;
 Location location;
 AstLocal* shadow;
 size_t functionDepth;
 size_t loopDepth;
 AstType* annotation;
 AstLocal(const AstName& name, const Location& location, AstLocal* shadow, size_t functionDepth, size_t loopDepth, AstType* annotation)
 : name(name)
 , location(location)
 , shadow(shadow)
 , functionDepth(functionDepth)
 , loopDepth(loopDepth)
 , annotation(annotation)
 {
 }
};
template<typename T>
struct AstArray
{
 T* data;
 size_t size;
 const T* begin() const
 {
 return data;
 }
 const T* end() const
 {
 return data + size;
 }
};
struct AstTypeList
{
 AstArray<AstType*> types;
 AstTypePack* tailType = nullptr;
};
using AstArgumentName = std::pair<AstName, Location>;
struct AstGenericType
{
 AstName name;
 Location location;
 AstType* defaultValue = nullptr;
};
struct AstGenericTypePack
{
 AstName name;
 Location location;
 AstTypePack* defaultValue = nullptr;
};
extern int gAstRttiIndex;
template<typename T>
struct AstRtti
{
 static const int value;
};
template<typename T>
const int AstRtti<T>::value = ++gAstRttiIndex;
#define LUAU_RTTI(Class) static int ClassIndex() { return AstRtti<Class>::value; }
class AstNode
{
public:
 explicit AstNode(int classIndex, const Location& location)
 : classIndex(classIndex)
 , location(location)
 {
 }
 virtual void visit(AstVisitor* visitor) = 0;
 virtual AstExpr* asExpr()
 {
 return nullptr;
 }
 virtual AstStat* asStat()
 {
 return nullptr;
 }
 virtual AstType* asType()
 {
 return nullptr;
 }
 template<typename T>
 bool is() const
 {
 return classIndex == T::ClassIndex();
 }
 template<typename T>
 T* as()
 {
 return classIndex == T::ClassIndex() ? static_cast<T*>(this) : nullptr;
 }
 template<typename T>
 const T* as() const
 {
 return classIndex == T::ClassIndex() ? static_cast<const T*>(this) : nullptr;
 }
 const int classIndex;
 Location location;
};
class AstExpr : public AstNode
{
public:
 explicit AstExpr(int classIndex, const Location& location)
 : AstNode(classIndex, location)
 {
 }
 AstExpr* asExpr() override
 {
 return this;
 }
};
class AstStat : public AstNode
{
public:
 explicit AstStat(int classIndex, const Location& location)
 : AstNode(classIndex, location)
 , hasSemicolon(false)
 {
 }
 AstStat* asStat() override
 {
 return this;
 }
 bool hasSemicolon;
};
class AstExprGroup : public AstExpr
{
public:
 LUAU_RTTI(AstExprGroup)
 explicit AstExprGroup(const Location& location, AstExpr* expr);
 void visit(AstVisitor* visitor) override;
 AstExpr* expr;
};
class AstExprConstantNil : public AstExpr
{
public:
 LUAU_RTTI(AstExprConstantNil)
 explicit AstExprConstantNil(const Location& location);
 void visit(AstVisitor* visitor) override;
};
class AstExprConstantBool : public AstExpr
{
public:
 LUAU_RTTI(AstExprConstantBool)
 AstExprConstantBool(const Location& location, bool value);
 void visit(AstVisitor* visitor) override;
 bool value;
};
enum class ConstantNumberParseResult
{
 Ok,
 Malformed,
 BinOverflow,
 HexOverflow,
};
class AstExprConstantNumber : public AstExpr
{
public:
 LUAU_RTTI(AstExprConstantNumber)
 AstExprConstantNumber(const Location& location, double value, ConstantNumberParseResult parseResult = ConstantNumberParseResult::Ok);
 void visit(AstVisitor* visitor) override;
 double value;
 ConstantNumberParseResult parseResult;
};
class AstExprConstantString : public AstExpr
{
public:
 LUAU_RTTI(AstExprConstantString)
 AstExprConstantString(const Location& location, const AstArray<char>& value);
 void visit(AstVisitor* visitor) override;
 AstArray<char> value;
};
class AstExprLocal : public AstExpr
{
public:
 LUAU_RTTI(AstExprLocal)
 AstExprLocal(const Location& location, AstLocal* local, bool upvalue);
 void visit(AstVisitor* visitor) override;
 AstLocal* local;
 bool upvalue;
};
class AstExprGlobal : public AstExpr
{
public:
 LUAU_RTTI(AstExprGlobal)
 AstExprGlobal(const Location& location, const AstName& name);
 void visit(AstVisitor* visitor) override;
 AstName name;
};
class AstExprVarargs : public AstExpr
{
public:
 LUAU_RTTI(AstExprVarargs)
 AstExprVarargs(const Location& location);
 void visit(AstVisitor* visitor) override;
};
class AstExprCall : public AstExpr
{
public:
 LUAU_RTTI(AstExprCall)
 AstExprCall(const Location& location, AstExpr* func, const AstArray<AstExpr*>& args, bool self, const Location& argLocation);
 void visit(AstVisitor* visitor) override;
 AstExpr* func;
 AstArray<AstExpr*> args;
 bool self;
 Location argLocation;
};
class AstExprIndexName : public AstExpr
{
public:
 LUAU_RTTI(AstExprIndexName)
 AstExprIndexName(
 const Location& location, AstExpr* expr, const AstName& index, const Location& indexLocation, const Position& opPosition, char op);
 void visit(AstVisitor* visitor) override;
 AstExpr* expr;
 AstName index;
 Location indexLocation;
 Position opPosition;
 char op = '.';
};
class AstExprIndexExpr : public AstExpr
{
public:
 LUAU_RTTI(AstExprIndexExpr)
 AstExprIndexExpr(const Location& location, AstExpr* expr, AstExpr* index);
 void visit(AstVisitor* visitor) override;
 AstExpr* expr;
 AstExpr* index;
};
class AstExprFunction : public AstExpr
{
public:
 LUAU_RTTI(AstExprFunction)
 AstExprFunction(const Location& location, const AstArray<AstGenericType>& generics, const AstArray<AstGenericTypePack>& genericPacks,
 AstLocal* self, const AstArray<AstLocal*>& args, bool vararg, const Location& varargLocation, AstStatBlock* body, size_t functionDepth,
 const AstName& debugname, const std::optional<AstTypeList>& returnAnnotation = {}, AstTypePack* varargAnnotation = nullptr,
 bool hasEnd = false, const std::optional<Location>& argLocation = std::nullopt);
 void visit(AstVisitor* visitor) override;
 AstArray<AstGenericType> generics;
 AstArray<AstGenericTypePack> genericPacks;
 AstLocal* self;
 AstArray<AstLocal*> args;
 std::optional<AstTypeList> returnAnnotation;
 bool vararg = false;
 Location varargLocation;
 AstTypePack* varargAnnotation;
 AstStatBlock* body;
 size_t functionDepth;
 AstName debugname;
 bool hasEnd = false;
 std::optional<Location> argLocation;
};
class AstExprTable : public AstExpr
{
public:
 LUAU_RTTI(AstExprTable)
 struct Item
 {
 enum Kind
 {
 List,
 Record, // foo=bar, in which case key is a AstExprConstantString
 General,
 };
 Kind kind;
 AstExpr* key;
 AstExpr* value;
 };
 AstExprTable(const Location& location, const AstArray<Item>& items);
 void visit(AstVisitor* visitor) override;
 AstArray<Item> items;
};
class AstExprUnary : public AstExpr
{
public:
 LUAU_RTTI(AstExprUnary)
 enum Op
 {
 Not,
 Minus,
 Len
 };
 AstExprUnary(const Location& location, Op op, AstExpr* expr);
 void visit(AstVisitor* visitor) override;
 Op op;
 AstExpr* expr;
};
std::string toString(AstExprUnary::Op op);
class AstExprBinary : public AstExpr
{
public:
 LUAU_RTTI(AstExprBinary)
 enum Op
 {
 Add,
 Sub,
 Mul,
 Div,
 Mod,
 Pow,
 Concat,
 CompareNe,
 CompareEq,
 CompareLt,
 CompareLe,
 CompareGt,
 CompareGe,
 And,
 Or
 };
 AstExprBinary(const Location& location, Op op, AstExpr* left, AstExpr* right);
 void visit(AstVisitor* visitor) override;
 Op op;
 AstExpr* left;
 AstExpr* right;
};
std::string toString(AstExprBinary::Op op);
class AstExprTypeAssertion : public AstExpr
{
public:
 LUAU_RTTI(AstExprTypeAssertion)
 AstExprTypeAssertion(const Location& location, AstExpr* expr, AstType* annotation);
 void visit(AstVisitor* visitor) override;
 AstExpr* expr;
 AstType* annotation;
};
class AstExprIfElse : public AstExpr
{
public:
 LUAU_RTTI(AstExprIfElse)
 AstExprIfElse(const Location& location, AstExpr* condition, bool hasThen, AstExpr* trueExpr, bool hasElse, AstExpr* falseExpr);
 void visit(AstVisitor* visitor) override;
 AstExpr* condition;
 bool hasThen;
 AstExpr* trueExpr;
 bool hasElse;
 AstExpr* falseExpr;
};
class AstExprInterpString : public AstExpr
{
public:
 LUAU_RTTI(AstExprInterpString)
 AstExprInterpString(const Location& location, const AstArray<AstArray<char>>& strings, const AstArray<AstExpr*>& expressions);
 void visit(AstVisitor* visitor) override;
 AstArray<AstArray<char>> strings;
 AstArray<AstExpr*> expressions;
};
class AstStatBlock : public AstStat
{
public:
 LUAU_RTTI(AstStatBlock)
 AstStatBlock(const Location& location, const AstArray<AstStat*>& body);
 void visit(AstVisitor* visitor) override;
 AstArray<AstStat*> body;
};
class AstStatIf : public AstStat
{
public:
 LUAU_RTTI(AstStatIf)
 AstStatIf(const Location& location, AstExpr* condition, AstStatBlock* thenbody, AstStat* elsebody, const std::optional<Location>& thenLocation,
 const std::optional<Location>& elseLocation, bool hasEnd);
 void visit(AstVisitor* visitor) override;
 AstExpr* condition;
 AstStatBlock* thenbody;
 AstStat* elsebody;
 std::optional<Location> thenLocation;
 std::optional<Location> elseLocation;
 bool hasEnd = false;
};
class AstStatWhile : public AstStat
{
public:
 LUAU_RTTI(AstStatWhile)
 AstStatWhile(const Location& location, AstExpr* condition, AstStatBlock* body, bool hasDo, const Location& doLocation, bool hasEnd);
 void visit(AstVisitor* visitor) override;
 AstExpr* condition;
 AstStatBlock* body;
 bool hasDo = false;
 Location doLocation;
 bool hasEnd = false;
};
class AstStatRepeat : public AstStat
{
public:
 LUAU_RTTI(AstStatRepeat)
 AstStatRepeat(const Location& location, AstExpr* condition, AstStatBlock* body, bool hasUntil);
 void visit(AstVisitor* visitor) override;
 AstExpr* condition;
 AstStatBlock* body;
 bool hasUntil = false;
};
class AstStatBreak : public AstStat
{
public:
 LUAU_RTTI(AstStatBreak)
 AstStatBreak(const Location& location);
 void visit(AstVisitor* visitor) override;
};
class AstStatContinue : public AstStat
{
public:
 LUAU_RTTI(AstStatContinue)
 AstStatContinue(const Location& location);
 void visit(AstVisitor* visitor) override;
};
class AstStatReturn : public AstStat
{
public:
 LUAU_RTTI(AstStatReturn)
 AstStatReturn(const Location& location, const AstArray<AstExpr*>& list);
 void visit(AstVisitor* visitor) override;
 AstArray<AstExpr*> list;
};
class AstStatExpr : public AstStat
{
public:
 LUAU_RTTI(AstStatExpr)
 AstStatExpr(const Location& location, AstExpr* expr);
 void visit(AstVisitor* visitor) override;
 AstExpr* expr;
};
class AstStatLocal : public AstStat
{
public:
 LUAU_RTTI(AstStatLocal)
 AstStatLocal(const Location& location, const AstArray<AstLocal*>& vars, const AstArray<AstExpr*>& values,
 const std::optional<Location>& equalsSignLocation);
 void visit(AstVisitor* visitor) override;
 AstArray<AstLocal*> vars;
 AstArray<AstExpr*> values;
 std::optional<Location> equalsSignLocation;
};
class AstStatFor : public AstStat
{
public:
 LUAU_RTTI(AstStatFor)
 AstStatFor(const Location& location, AstLocal* var, AstExpr* from, AstExpr* to, AstExpr* step, AstStatBlock* body, bool hasDo,
 const Location& doLocation, bool hasEnd);
 void visit(AstVisitor* visitor) override;
 AstLocal* var;
 AstExpr* from;
 AstExpr* to;
 AstExpr* step;
 AstStatBlock* body;
 bool hasDo = false;
 Location doLocation;
 bool hasEnd = false;
};
class AstStatForIn : public AstStat
{
public:
 LUAU_RTTI(AstStatForIn)
 AstStatForIn(const Location& location, const AstArray<AstLocal*>& vars, const AstArray<AstExpr*>& values, AstStatBlock* body, bool hasIn,
 const Location& inLocation, bool hasDo, const Location& doLocation, bool hasEnd);
 void visit(AstVisitor* visitor) override;
 AstArray<AstLocal*> vars;
 AstArray<AstExpr*> values;
 AstStatBlock* body;
 bool hasIn = false;
 Location inLocation;
 bool hasDo = false;
 Location doLocation;
 bool hasEnd = false;
};
class AstStatAssign : public AstStat
{
public:
 LUAU_RTTI(AstStatAssign)
 AstStatAssign(const Location& location, const AstArray<AstExpr*>& vars, const AstArray<AstExpr*>& values);
 void visit(AstVisitor* visitor) override;
 AstArray<AstExpr*> vars;
 AstArray<AstExpr*> values;
};
class AstStatCompoundAssign : public AstStat
{
public:
 LUAU_RTTI(AstStatCompoundAssign)
 AstStatCompoundAssign(const Location& location, AstExprBinary::Op op, AstExpr* var, AstExpr* value);
 void visit(AstVisitor* visitor) override;
 AstExprBinary::Op op;
 AstExpr* var;
 AstExpr* value;
};
class AstStatFunction : public AstStat
{
public:
 LUAU_RTTI(AstStatFunction)
 AstStatFunction(const Location& location, AstExpr* name, AstExprFunction* func);
 void visit(AstVisitor* visitor) override;
 AstExpr* name;
 AstExprFunction* func;
};
class AstStatLocalFunction : public AstStat
{
public:
 LUAU_RTTI(AstStatLocalFunction)
 AstStatLocalFunction(const Location& location, AstLocal* name, AstExprFunction* func);
 void visit(AstVisitor* visitor) override;
 AstLocal* name;
 AstExprFunction* func;
};
class AstStatTypeAlias : public AstStat
{
public:
 LUAU_RTTI(AstStatTypeAlias)
 AstStatTypeAlias(const Location& location, const AstName& name, const Location& nameLocation, const AstArray<AstGenericType>& generics,
 const AstArray<AstGenericTypePack>& genericPacks, AstType* type, bool exported);
 void visit(AstVisitor* visitor) override;
 AstName name;
 Location nameLocation;
 AstArray<AstGenericType> generics;
 AstArray<AstGenericTypePack> genericPacks;
 AstType* type;
 bool exported;
};
class AstStatDeclareGlobal : public AstStat
{
public:
 LUAU_RTTI(AstStatDeclareGlobal)
 AstStatDeclareGlobal(const Location& location, const AstName& name, AstType* type);
 void visit(AstVisitor* visitor) override;
 AstName name;
 AstType* type;
};
class AstStatDeclareFunction : public AstStat
{
public:
 LUAU_RTTI(AstStatDeclareFunction)
 AstStatDeclareFunction(const Location& location, const AstName& name, const AstArray<AstGenericType>& generics,
 const AstArray<AstGenericTypePack>& genericPacks, const AstTypeList& params, const AstArray<AstArgumentName>& paramNames,
 const AstTypeList& retTypes);
 void visit(AstVisitor* visitor) override;
 AstName name;
 AstArray<AstGenericType> generics;
 AstArray<AstGenericTypePack> genericPacks;
 AstTypeList params;
 AstArray<AstArgumentName> paramNames;
 AstTypeList retTypes;
};
struct AstDeclaredClassProp
{
 AstName name;
 AstType* ty = nullptr;
 bool isMethod = false;
};
class AstStatDeclareClass : public AstStat
{
public:
 LUAU_RTTI(AstStatDeclareClass)
 AstStatDeclareClass(const Location& location, const AstName& name, std::optional<AstName> superName, const AstArray<AstDeclaredClassProp>& props);
 void visit(AstVisitor* visitor) override;
 AstName name;
 std::optional<AstName> superName;
 AstArray<AstDeclaredClassProp> props;
};
class AstType : public AstNode
{
public:
 AstType(int classIndex, const Location& location)
 : AstNode(classIndex, location)
 {
 }
 AstType* asType() override
 {
 return this;
 }
};
struct AstTypeOrPack
{
 AstType* type = nullptr;
 AstTypePack* typePack = nullptr;
};
class AstTypeReference : public AstType
{
public:
 LUAU_RTTI(AstTypeReference)
 AstTypeReference(const Location& location, std::optional<AstName> prefix, AstName name, std::optional<Location> prefixLocation,
 const Location& nameLocation, bool hasParameterList = false, const AstArray<AstTypeOrPack>& parameters = {});
 void visit(AstVisitor* visitor) override;
 bool hasParameterList;
 std::optional<AstName> prefix;
 std::optional<Location> prefixLocation;
 AstName name;
 Location nameLocation;
 AstArray<AstTypeOrPack> parameters;
};
struct AstTableProp
{
 AstName name;
 Location location;
 AstType* type;
};
struct AstTableIndexer
{
 AstType* indexType;
 AstType* resultType;
 Location location;
};
class AstTypeTable : public AstType
{
public:
 LUAU_RTTI(AstTypeTable)
 AstTypeTable(const Location& location, const AstArray<AstTableProp>& props, AstTableIndexer* indexer = nullptr);
 void visit(AstVisitor* visitor) override;
 AstArray<AstTableProp> props;
 AstTableIndexer* indexer;
};
class AstTypeFunction : public AstType
{
public:
 LUAU_RTTI(AstTypeFunction)
 AstTypeFunction(const Location& location, const AstArray<AstGenericType>& generics, const AstArray<AstGenericTypePack>& genericPacks,
 const AstTypeList& argTypes, const AstArray<std::optional<AstArgumentName>>& argNames, const AstTypeList& returnTypes);
 void visit(AstVisitor* visitor) override;
 AstArray<AstGenericType> generics;
 AstArray<AstGenericTypePack> genericPacks;
 AstTypeList argTypes;
 AstArray<std::optional<AstArgumentName>> argNames;
 AstTypeList returnTypes;
};
class AstTypeTypeof : public AstType
{
public:
 LUAU_RTTI(AstTypeTypeof)
 AstTypeTypeof(const Location& location, AstExpr* expr);
 void visit(AstVisitor* visitor) override;
 AstExpr* expr;
};
class AstTypeUnion : public AstType
{
public:
 LUAU_RTTI(AstTypeUnion)
 AstTypeUnion(const Location& location, const AstArray<AstType*>& types);
 void visit(AstVisitor* visitor) override;
 AstArray<AstType*> types;
};
class AstTypeIntersection : public AstType
{
public:
 LUAU_RTTI(AstTypeIntersection)
 AstTypeIntersection(const Location& location, const AstArray<AstType*>& types);
 void visit(AstVisitor* visitor) override;
 AstArray<AstType*> types;
};
class AstExprError : public AstExpr
{
public:
 LUAU_RTTI(AstExprError)
 AstExprError(const Location& location, const AstArray<AstExpr*>& expressions, unsigned messageIndex);
 void visit(AstVisitor* visitor) override;
 AstArray<AstExpr*> expressions;
 unsigned messageIndex;
};
class AstStatError : public AstStat
{
public:
 LUAU_RTTI(AstStatError)
 AstStatError(const Location& location, const AstArray<AstExpr*>& expressions, const AstArray<AstStat*>& statements, unsigned messageIndex);
 void visit(AstVisitor* visitor) override;
 AstArray<AstExpr*> expressions;
 AstArray<AstStat*> statements;
 unsigned messageIndex;
};
class AstTypeError : public AstType
{
public:
 LUAU_RTTI(AstTypeError)
 AstTypeError(const Location& location, const AstArray<AstType*>& types, bool isMissing, unsigned messageIndex);
 void visit(AstVisitor* visitor) override;
 AstArray<AstType*> types;
 bool isMissing;
 unsigned messageIndex;
};
class AstTypeSingletonBool : public AstType
{
public:
 LUAU_RTTI(AstTypeSingletonBool)
 AstTypeSingletonBool(const Location& location, bool value);
 void visit(AstVisitor* visitor) override;
 bool value;
};
class AstTypeSingletonString : public AstType
{
public:
 LUAU_RTTI(AstTypeSingletonString)
 AstTypeSingletonString(const Location& location, const AstArray<char>& value);
 void visit(AstVisitor* visitor) override;
 const AstArray<char> value;
};
class AstTypePack : public AstNode
{
public:
 AstTypePack(int classIndex, const Location& location)
 : AstNode(classIndex, location)
 {
 }
};
class AstTypePackExplicit : public AstTypePack
{
public:
 LUAU_RTTI(AstTypePackExplicit)
 AstTypePackExplicit(const Location& location, AstTypeList typeList);
 void visit(AstVisitor* visitor) override;
 AstTypeList typeList;
};
class AstTypePackVariadic : public AstTypePack
{
public:
 LUAU_RTTI(AstTypePackVariadic)
 AstTypePackVariadic(const Location& location, AstType* variadicType);
 void visit(AstVisitor* visitor) override;
 AstType* variadicType;
};
class AstTypePackGeneric : public AstTypePack
{
public:
 LUAU_RTTI(AstTypePackGeneric)
 AstTypePackGeneric(const Location& location, AstName name);
 void visit(AstVisitor* visitor) override;
 AstName genericName;
};
class AstVisitor
{
public:
 virtual ~AstVisitor() {}
 virtual bool visit(class AstNode*)
 {
 return true;
 }
 virtual bool visit(class AstExpr* node)
 {
 return visit(static_cast<AstNode*>(node));
 }
 virtual bool visit(class AstExprGroup* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprConstantNil* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprConstantBool* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprConstantNumber* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprConstantString* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprLocal* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprGlobal* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprVarargs* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprCall* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprIndexName* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprIndexExpr* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprFunction* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprTable* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprUnary* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprBinary* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprTypeAssertion* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprIfElse* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprInterpString* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstExprError* node)
 {
 return visit(static_cast<AstExpr*>(node));
 }
 virtual bool visit(class AstStat* node)
 {
 return visit(static_cast<AstNode*>(node));
 }
 virtual bool visit(class AstStatBlock* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatIf* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatWhile* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatRepeat* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatBreak* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatContinue* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatReturn* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatExpr* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatLocal* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatFor* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatForIn* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatAssign* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatCompoundAssign* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatFunction* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatLocalFunction* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatTypeAlias* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatDeclareFunction* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatDeclareGlobal* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatDeclareClass* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstStatError* node)
 {
 return visit(static_cast<AstStat*>(node));
 }
 virtual bool visit(class AstType* node)
 {
 return false;
 }
 virtual bool visit(class AstTypeReference* node)
 {
 return visit(static_cast<AstType*>(node));
 }
 virtual bool visit(class AstTypeTable* node)
 {
 return visit(static_cast<AstType*>(node));
 }
 virtual bool visit(class AstTypeFunction* node)
 {
 return visit(static_cast<AstType*>(node));
 }
 virtual bool visit(class AstTypeTypeof* node)
 {
 return visit(static_cast<AstType*>(node));
 }
 virtual bool visit(class AstTypeUnion* node)
 {
 return visit(static_cast<AstType*>(node));
 }
 virtual bool visit(class AstTypeIntersection* node)
 {
 return visit(static_cast<AstType*>(node));
 }
 virtual bool visit(class AstTypeSingletonBool* node)
 {
 return visit(static_cast<AstType*>(node));
 }
 virtual bool visit(class AstTypeSingletonString* node)
 {
 return visit(static_cast<AstType*>(node));
 }
 virtual bool visit(class AstTypeError* node)
 {
 return visit(static_cast<AstType*>(node));
 }
 virtual bool visit(class AstTypePack* node)
 {
 return false;
 }
 virtual bool visit(class AstTypePackExplicit* node)
 {
 return visit(static_cast<AstTypePack*>(node));
 }
 virtual bool visit(class AstTypePackVariadic* node)
 {
 return visit(static_cast<AstTypePack*>(node));
 }
 virtual bool visit(class AstTypePackGeneric* node)
 {
 return visit(static_cast<AstTypePack*>(node));
 }
};
AstName getIdentifier(AstExpr*);
Location getLocation(const AstTypeList& typeList);
template<typename T>
Location getLocation(AstArray<T*> array)
{
 if (0 == array.size)
 return {};
 return Location{array.data[0]->location.begin, array.data[array.size - 1]->location.end};
}
#undef LUAU_RTTI
}
namespace std
{
template<>
struct hash<Luau::AstName>
{
 size_t operator()(const Luau::AstName& value) const
 {
 return (uintptr_t(value.value) >> 4) ^ (uintptr_t(value.value) >> 9);
 }
};
}
namespace Luau
{
static void visitTypeList(AstVisitor* visitor, const AstTypeList& list)
{
 for (AstType* ty : list.types)
 ty->visit(visitor);
 if (list.tailType)
 list.tailType->visit(visitor);
}
int gAstRttiIndex = 0;
AstExprGroup::AstExprGroup(const Location& location, AstExpr* expr)
 : AstExpr(ClassIndex(), location)
 , expr(expr)
{
}
void AstExprGroup::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 expr->visit(visitor);
}
AstExprConstantNil::AstExprConstantNil(const Location& location)
 : AstExpr(ClassIndex(), location)
{
}
void AstExprConstantNil::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstExprConstantBool::AstExprConstantBool(const Location& location, bool value)
 : AstExpr(ClassIndex(), location)
 , value(value)
{
}
void AstExprConstantBool::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstExprConstantNumber::AstExprConstantNumber(const Location& location, double value, ConstantNumberParseResult parseResult)
 : AstExpr(ClassIndex(), location)
 , value(value)
 , parseResult(parseResult)
{
}
void AstExprConstantNumber::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstExprConstantString::AstExprConstantString(const Location& location, const AstArray<char>& value)
 : AstExpr(ClassIndex(), location)
 , value(value)
{
}
void AstExprConstantString::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstExprLocal::AstExprLocal(const Location& location, AstLocal* local, bool upvalue)
 : AstExpr(ClassIndex(), location)
 , local(local)
 , upvalue(upvalue)
{
}
void AstExprLocal::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstExprGlobal::AstExprGlobal(const Location& location, const AstName& name)
 : AstExpr(ClassIndex(), location)
 , name(name)
{
}
void AstExprGlobal::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstExprVarargs::AstExprVarargs(const Location& location)
 : AstExpr(ClassIndex(), location)
{
}
void AstExprVarargs::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstExprCall::AstExprCall(const Location& location, AstExpr* func, const AstArray<AstExpr*>& args, bool self, const Location& argLocation)
 : AstExpr(ClassIndex(), location)
 , func(func)
 , args(args)
 , self(self)
 , argLocation(argLocation)
{
}
void AstExprCall::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 func->visit(visitor);
 for (AstExpr* arg : args)
 arg->visit(visitor);
 }
}
AstExprIndexName::AstExprIndexName(
 const Location& location, AstExpr* expr, const AstName& index, const Location& indexLocation, const Position& opPosition, char op)
 : AstExpr(ClassIndex(), location)
 , expr(expr)
 , index(index)
 , indexLocation(indexLocation)
 , opPosition(opPosition)
 , op(op)
{
}
void AstExprIndexName::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 expr->visit(visitor);
}
AstExprIndexExpr::AstExprIndexExpr(const Location& location, AstExpr* expr, AstExpr* index)
 : AstExpr(ClassIndex(), location)
 , expr(expr)
 , index(index)
{
}
void AstExprIndexExpr::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 expr->visit(visitor);
 index->visit(visitor);
 }
}
AstExprFunction::AstExprFunction(const Location& location, const AstArray<AstGenericType>& generics, const AstArray<AstGenericTypePack>& genericPacks,
 AstLocal* self, const AstArray<AstLocal*>& args, bool vararg, const Location& varargLocation, AstStatBlock* body, size_t functionDepth,
 const AstName& debugname, const std::optional<AstTypeList>& returnAnnotation, AstTypePack* varargAnnotation, bool hasEnd,
 const std::optional<Location>& argLocation)
 : AstExpr(ClassIndex(), location)
 , generics(generics)
 , genericPacks(genericPacks)
 , self(self)
 , args(args)
 , returnAnnotation(returnAnnotation)
 , vararg(vararg)
 , varargLocation(varargLocation)
 , varargAnnotation(varargAnnotation)
 , body(body)
 , functionDepth(functionDepth)
 , debugname(debugname)
 , hasEnd(hasEnd)
 , argLocation(argLocation)
{
}
void AstExprFunction::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstLocal* arg : args)
 {
 if (arg->annotation)
 arg->annotation->visit(visitor);
 }
 if (varargAnnotation)
 varargAnnotation->visit(visitor);
 if (returnAnnotation)
 visitTypeList(visitor, *returnAnnotation);
 body->visit(visitor);
 }
}
AstExprTable::AstExprTable(const Location& location, const AstArray<Item>& items)
 : AstExpr(ClassIndex(), location)
 , items(items)
{
}
void AstExprTable::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (const Item& item : items)
 {
 if (item.key)
 item.key->visit(visitor);
 item.value->visit(visitor);
 }
 }
}
AstExprUnary::AstExprUnary(const Location& location, Op op, AstExpr* expr)
 : AstExpr(ClassIndex(), location)
 , op(op)
 , expr(expr)
{
}
void AstExprUnary::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 expr->visit(visitor);
}
std::string toString(AstExprUnary::Op op)
{
 switch (op)
 {
 case AstExprUnary::Minus:
 return "-";
 case AstExprUnary::Not:
 return "not";
 case AstExprUnary::Len:
 return "#";
 default:
 LUAU_ASSERT(false);
 return ""; // MSVC requires this even though the switch/case is exhaustive
 }
}
AstExprBinary::AstExprBinary(const Location& location, Op op, AstExpr* left, AstExpr* right)
 : AstExpr(ClassIndex(), location)
 , op(op)
 , left(left)
 , right(right)
{
}
void AstExprBinary::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 left->visit(visitor);
 right->visit(visitor);
 }
}
std::string toString(AstExprBinary::Op op)
{
 switch (op)
 {
 case AstExprBinary::Add:
 return "+";
 case AstExprBinary::Sub:
 return "-";
 case AstExprBinary::Mul:
 return "*";
 case AstExprBinary::Div:
 return "/";
 case AstExprBinary::Mod:
 return "%";
 case AstExprBinary::Pow:
 return "^";
 case AstExprBinary::Concat:
 return "..";
 case AstExprBinary::CompareNe:
 return "~=";
 case AstExprBinary::CompareEq:
 return "==";
 case AstExprBinary::CompareLt:
 return "<";
 case AstExprBinary::CompareLe:
 return "<=";
 case AstExprBinary::CompareGt:
 return ">";
 case AstExprBinary::CompareGe:
 return ">=";
 case AstExprBinary::And:
 return "and";
 case AstExprBinary::Or:
 return "or";
 default:
 LUAU_ASSERT(false);
 return ""; // MSVC requires this even though the switch/case is exhaustive
 }
}
AstExprTypeAssertion::AstExprTypeAssertion(const Location& location, AstExpr* expr, AstType* annotation)
 : AstExpr(ClassIndex(), location)
 , expr(expr)
 , annotation(annotation)
{
}
void AstExprTypeAssertion::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 expr->visit(visitor);
 annotation->visit(visitor);
 }
}
AstExprIfElse::AstExprIfElse(const Location& location, AstExpr* condition, bool hasThen, AstExpr* trueExpr, bool hasElse, AstExpr* falseExpr)
 : AstExpr(ClassIndex(), location)
 , condition(condition)
 , hasThen(hasThen)
 , trueExpr(trueExpr)
 , hasElse(hasElse)
 , falseExpr(falseExpr)
{
}
void AstExprIfElse::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 condition->visit(visitor);
 trueExpr->visit(visitor);
 falseExpr->visit(visitor);
 }
}
AstExprError::AstExprError(const Location& location, const AstArray<AstExpr*>& expressions, unsigned messageIndex)
 : AstExpr(ClassIndex(), location)
 , expressions(expressions)
 , messageIndex(messageIndex)
{
}
AstExprInterpString::AstExprInterpString(const Location& location, const AstArray<AstArray<char>>& strings, const AstArray<AstExpr*>& expressions)
 : AstExpr(ClassIndex(), location)
 , strings(strings)
 , expressions(expressions)
{
}
void AstExprInterpString::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstExpr* expr : expressions)
 expr->visit(visitor);
 }
}
void AstExprError::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstExpr* expression : expressions)
 expression->visit(visitor);
 }
}
AstStatBlock::AstStatBlock(const Location& location, const AstArray<AstStat*>& body)
 : AstStat(ClassIndex(), location)
 , body(body)
{
}
void AstStatBlock::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstStat* stat : body)
 stat->visit(visitor);
 }
}
AstStatIf::AstStatIf(const Location& location, AstExpr* condition, AstStatBlock* thenbody, AstStat* elsebody,
 const std::optional<Location>& thenLocation, const std::optional<Location>& elseLocation, bool hasEnd)
 : AstStat(ClassIndex(), location)
 , condition(condition)
 , thenbody(thenbody)
 , elsebody(elsebody)
 , thenLocation(thenLocation)
 , elseLocation(elseLocation)
 , hasEnd(hasEnd)
{
}
void AstStatIf::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 condition->visit(visitor);
 thenbody->visit(visitor);
 if (elsebody)
 elsebody->visit(visitor);
 }
}
AstStatWhile::AstStatWhile(const Location& location, AstExpr* condition, AstStatBlock* body, bool hasDo, const Location& doLocation, bool hasEnd)
 : AstStat(ClassIndex(), location)
 , condition(condition)
 , body(body)
 , hasDo(hasDo)
 , doLocation(doLocation)
 , hasEnd(hasEnd)
{
}
void AstStatWhile::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 condition->visit(visitor);
 body->visit(visitor);
 }
}
AstStatRepeat::AstStatRepeat(const Location& location, AstExpr* condition, AstStatBlock* body, bool hasUntil)
 : AstStat(ClassIndex(), location)
 , condition(condition)
 , body(body)
 , hasUntil(hasUntil)
{
}
void AstStatRepeat::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 body->visit(visitor);
 condition->visit(visitor);
 }
}
AstStatBreak::AstStatBreak(const Location& location)
 : AstStat(ClassIndex(), location)
{
}
void AstStatBreak::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstStatContinue::AstStatContinue(const Location& location)
 : AstStat(ClassIndex(), location)
{
}
void AstStatContinue::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstStatReturn::AstStatReturn(const Location& location, const AstArray<AstExpr*>& list)
 : AstStat(ClassIndex(), location)
 , list(list)
{
}
void AstStatReturn::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstExpr* expr : list)
 expr->visit(visitor);
 }
}
AstStatExpr::AstStatExpr(const Location& location, AstExpr* expr)
 : AstStat(ClassIndex(), location)
 , expr(expr)
{
}
void AstStatExpr::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 expr->visit(visitor);
}
AstStatLocal::AstStatLocal(
 const Location& location, const AstArray<AstLocal*>& vars, const AstArray<AstExpr*>& values, const std::optional<Location>& equalsSignLocation)
 : AstStat(ClassIndex(), location)
 , vars(vars)
 , values(values)
 , equalsSignLocation(equalsSignLocation)
{
}
void AstStatLocal::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstLocal* var : vars)
 {
 if (var->annotation)
 var->annotation->visit(visitor);
 }
 for (AstExpr* expr : values)
 expr->visit(visitor);
 }
}
AstStatFor::AstStatFor(const Location& location, AstLocal* var, AstExpr* from, AstExpr* to, AstExpr* step, AstStatBlock* body, bool hasDo,
 const Location& doLocation, bool hasEnd)
 : AstStat(ClassIndex(), location)
 , var(var)
 , from(from)
 , to(to)
 , step(step)
 , body(body)
 , hasDo(hasDo)
 , doLocation(doLocation)
 , hasEnd(hasEnd)
{
}
void AstStatFor::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 if (var->annotation)
 var->annotation->visit(visitor);
 from->visit(visitor);
 to->visit(visitor);
 if (step)
 step->visit(visitor);
 body->visit(visitor);
 }
}
AstStatForIn::AstStatForIn(const Location& location, const AstArray<AstLocal*>& vars, const AstArray<AstExpr*>& values, AstStatBlock* body,
 bool hasIn, const Location& inLocation, bool hasDo, const Location& doLocation, bool hasEnd)
 : AstStat(ClassIndex(), location)
 , vars(vars)
 , values(values)
 , body(body)
 , hasIn(hasIn)
 , inLocation(inLocation)
 , hasDo(hasDo)
 , doLocation(doLocation)
 , hasEnd(hasEnd)
{
}
void AstStatForIn::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstLocal* var : vars)
 {
 if (var->annotation)
 var->annotation->visit(visitor);
 }
 for (AstExpr* expr : values)
 expr->visit(visitor);
 body->visit(visitor);
 }
}
AstStatAssign::AstStatAssign(const Location& location, const AstArray<AstExpr*>& vars, const AstArray<AstExpr*>& values)
 : AstStat(ClassIndex(), location)
 , vars(vars)
 , values(values)
{
}
void AstStatAssign::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstExpr* lvalue : vars)
 lvalue->visit(visitor);
 for (AstExpr* expr : values)
 expr->visit(visitor);
 }
}
AstStatCompoundAssign::AstStatCompoundAssign(const Location& location, AstExprBinary::Op op, AstExpr* var, AstExpr* value)
 : AstStat(ClassIndex(), location)
 , op(op)
 , var(var)
 , value(value)
{
}
void AstStatCompoundAssign::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 var->visit(visitor);
 value->visit(visitor);
 }
}
AstStatFunction::AstStatFunction(const Location& location, AstExpr* name, AstExprFunction* func)
 : AstStat(ClassIndex(), location)
 , name(name)
 , func(func)
{
}
void AstStatFunction::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 name->visit(visitor);
 func->visit(visitor);
 }
}
AstStatLocalFunction::AstStatLocalFunction(const Location& location, AstLocal* name, AstExprFunction* func)
 : AstStat(ClassIndex(), location)
 , name(name)
 , func(func)
{
}
void AstStatLocalFunction::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 func->visit(visitor);
}
AstStatTypeAlias::AstStatTypeAlias(const Location& location, const AstName& name, const Location& nameLocation,
 const AstArray<AstGenericType>& generics, const AstArray<AstGenericTypePack>& genericPacks, AstType* type, bool exported)
 : AstStat(ClassIndex(), location)
 , name(name)
 , nameLocation(nameLocation)
 , generics(generics)
 , genericPacks(genericPacks)
 , type(type)
 , exported(exported)
{
}
void AstStatTypeAlias::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (const AstGenericType& el : generics)
 {
 if (el.defaultValue)
 el.defaultValue->visit(visitor);
 }
 for (const AstGenericTypePack& el : genericPacks)
 {
 if (el.defaultValue)
 el.defaultValue->visit(visitor);
 }
 type->visit(visitor);
 }
}
AstStatDeclareGlobal::AstStatDeclareGlobal(const Location& location, const AstName& name, AstType* type)
 : AstStat(ClassIndex(), location)
 , name(name)
 , type(type)
{
}
void AstStatDeclareGlobal::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 type->visit(visitor);
}
AstStatDeclareFunction::AstStatDeclareFunction(const Location& location, const AstName& name, const AstArray<AstGenericType>& generics,
 const AstArray<AstGenericTypePack>& genericPacks, const AstTypeList& params, const AstArray<AstArgumentName>& paramNames,
 const AstTypeList& retTypes)
 : AstStat(ClassIndex(), location)
 , name(name)
 , generics(generics)
 , genericPacks(genericPacks)
 , params(params)
 , paramNames(paramNames)
 , retTypes(retTypes)
{
}
void AstStatDeclareFunction::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 visitTypeList(visitor, params);
 visitTypeList(visitor, retTypes);
 }
}
AstStatDeclareClass::AstStatDeclareClass(
 const Location& location, const AstName& name, std::optional<AstName> superName, const AstArray<AstDeclaredClassProp>& props)
 : AstStat(ClassIndex(), location)
 , name(name)
 , superName(superName)
 , props(props)
{
}
void AstStatDeclareClass::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (const AstDeclaredClassProp& prop : props)
 prop.ty->visit(visitor);
 }
}
AstStatError::AstStatError(
 const Location& location, const AstArray<AstExpr*>& expressions, const AstArray<AstStat*>& statements, unsigned messageIndex)
 : AstStat(ClassIndex(), location)
 , expressions(expressions)
 , statements(statements)
 , messageIndex(messageIndex)
{
}
void AstStatError::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstNode* expression : expressions)
 expression->visit(visitor);
 for (AstNode* statement : statements)
 statement->visit(visitor);
 }
}
AstTypeReference::AstTypeReference(const Location& location, std::optional<AstName> prefix, AstName name, std::optional<Location> prefixLocation,
 const Location& nameLocation, bool hasParameterList, const AstArray<AstTypeOrPack>& parameters)
 : AstType(ClassIndex(), location)
 , hasParameterList(hasParameterList)
 , prefix(prefix)
 , prefixLocation(prefixLocation)
 , name(name)
 , nameLocation(nameLocation)
 , parameters(parameters)
{
}
void AstTypeReference::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (const AstTypeOrPack& param : parameters)
 {
 if (param.type)
 param.type->visit(visitor);
 else
 param.typePack->visit(visitor);
 }
 }
}
AstTypeTable::AstTypeTable(const Location& location, const AstArray<AstTableProp>& props, AstTableIndexer* indexer)
 : AstType(ClassIndex(), location)
 , props(props)
 , indexer(indexer)
{
}
void AstTypeTable::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (const AstTableProp& prop : props)
 prop.type->visit(visitor);
 if (indexer)
 {
 indexer->indexType->visit(visitor);
 indexer->resultType->visit(visitor);
 }
 }
}
AstTypeFunction::AstTypeFunction(const Location& location, const AstArray<AstGenericType>& generics, const AstArray<AstGenericTypePack>& genericPacks,
 const AstTypeList& argTypes, const AstArray<std::optional<AstArgumentName>>& argNames, const AstTypeList& returnTypes)
 : AstType(ClassIndex(), location)
 , generics(generics)
 , genericPacks(genericPacks)
 , argTypes(argTypes)
 , argNames(argNames)
 , returnTypes(returnTypes)
{
 LUAU_ASSERT(argNames.size == 0 || argNames.size == argTypes.types.size);
}
void AstTypeFunction::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 visitTypeList(visitor, argTypes);
 visitTypeList(visitor, returnTypes);
 }
}
AstTypeTypeof::AstTypeTypeof(const Location& location, AstExpr* expr)
 : AstType(ClassIndex(), location)
 , expr(expr)
{
}
void AstTypeTypeof::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 expr->visit(visitor);
}
AstTypeUnion::AstTypeUnion(const Location& location, const AstArray<AstType*>& types)
 : AstType(ClassIndex(), location)
 , types(types)
{
}
void AstTypeUnion::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstType* type : types)
 type->visit(visitor);
 }
}
AstTypeIntersection::AstTypeIntersection(const Location& location, const AstArray<AstType*>& types)
 : AstType(ClassIndex(), location)
 , types(types)
{
}
void AstTypeIntersection::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstType* type : types)
 type->visit(visitor);
 }
}
AstTypeSingletonBool::AstTypeSingletonBool(const Location& location, bool value)
 : AstType(ClassIndex(), location)
 , value(value)
{
}
void AstTypeSingletonBool::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstTypeSingletonString::AstTypeSingletonString(const Location& location, const AstArray<char>& value)
 : AstType(ClassIndex(), location)
 , value(value)
{
}
void AstTypeSingletonString::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstTypeError::AstTypeError(const Location& location, const AstArray<AstType*>& types, bool isMissing, unsigned messageIndex)
 : AstType(ClassIndex(), location)
 , types(types)
 , isMissing(isMissing)
 , messageIndex(messageIndex)
{
}
void AstTypeError::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstType* type : types)
 type->visit(visitor);
 }
}
AstTypePackExplicit::AstTypePackExplicit(const Location& location, AstTypeList typeList)
 : AstTypePack(ClassIndex(), location)
 , typeList(typeList)
{
}
void AstTypePackExplicit::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 {
 for (AstType* type : typeList.types)
 type->visit(visitor);
 if (typeList.tailType)
 typeList.tailType->visit(visitor);
 }
}
AstTypePackVariadic::AstTypePackVariadic(const Location& location, AstType* variadicType)
 : AstTypePack(ClassIndex(), location)
 , variadicType(variadicType)
{
}
void AstTypePackVariadic::visit(AstVisitor* visitor)
{
 if (visitor->visit(this))
 variadicType->visit(visitor);
}
AstTypePackGeneric::AstTypePackGeneric(const Location& location, AstName name)
 : AstTypePack(ClassIndex(), location)
 , genericName(name)
{
}
void AstTypePackGeneric::visit(AstVisitor* visitor)
{
 visitor->visit(this);
}
AstName getIdentifier(AstExpr* node)
{
 if (AstExprGlobal* expr = node->as<AstExprGlobal>())
 return expr->name;
 if (AstExprLocal* expr = node->as<AstExprLocal>())
 return expr->local->name;
 return AstName();
}
Location getLocation(const AstTypeList& typeList)
{
 Location result;
 if (typeList.types.size)
 {
 result = Location{typeList.types.data[0]->location, typeList.types.data[typeList.types.size - 1]->location};
 }
 if (typeList.tailType)
 result.end = typeList.tailType->location.end;
 return result;
}
}
namespace Luau
{
const char* findConfusable(uint32_t codepoint);
}
#include <algorithm>
#include <array>
namespace Luau
{
struct Confusable
{
 unsigned codepoint : 24;
 char text[5];
};
static const Confusable kConfusables[] =
{
 {34, "\""}, // MA#* ( " → '' ) QUOTATION MARK → APOSTROPHE, APOSTROPHE# # Converted to a quote.
 {48, "O"}, // MA# ( 0 → O ) DIGIT ZERO → LATIN CAPITAL LETTER O#
 {49, "l"}, // MA# ( 1 → l ) DIGIT ONE → LATIN SMALL LETTER L#
 {73, "l"}, // MA# ( I → l ) LATIN CAPITAL LETTER I → LATIN SMALL LETTER L#
 {96, "'"}, // MA#* ( ` → ' ) GRAVE ACCENT → APOSTROPHE# →ˋ→→｀→→‘→
 {109, "rn"}, // MA# ( m → rn ) LATIN SMALL LETTER M → LATIN SMALL LETTER R, LATIN SMALL LETTER N#
 {124, "l"}, // MA#* ( | → l ) VERTICAL LINE → LATIN SMALL LETTER L#
 {160, " "}, // MA#* (   → ) NO-BREAK SPACE → SPACE#
 {180, "'"}, // MA#* ( ´ → ' ) ACUTE ACCENT → APOSTROPHE# →΄→→ʹ→
 {184, ","}, // MA#* ( ¸ → , ) CEDILLA → COMMA#
 {198, "AE"}, // MA# ( Æ → AE ) LATIN CAPITAL LETTER AE → LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER E#
 {215, "x"}, // MA#* ( × → x ) MULTIPLICATION SIGN → LATIN SMALL LETTER X#
 {230, "ae"}, // MA# ( æ → ae ) LATIN SMALL LETTER AE → LATIN SMALL LETTER A, LATIN SMALL LETTER E#
 {305, "i"}, // MA# ( ı → i ) LATIN SMALL LETTER DOTLESS I → LATIN SMALL LETTER I#
 {306, "lJ"}, // MA# ( Ĳ → lJ ) LATIN CAPITAL LIGATURE IJ → LATIN SMALL LETTER L, LATIN CAPITAL LETTER J# →IJ→
 {307, "ij"}, // MA# ( ĳ → ij ) LATIN SMALL LIGATURE IJ → LATIN SMALL LETTER I, LATIN SMALL LETTER J#
 {329, "'n"}, // MA# ( ŉ → 'n ) LATIN SMALL LETTER N PRECEDED BY APOSTROPHE → APOSTROPHE, LATIN SMALL LETTER N# →ʼn→
 {338, "OE"}, // MA# ( Œ → OE ) LATIN CAPITAL LIGATURE OE → LATIN CAPITAL LETTER O, LATIN CAPITAL LETTER E#
 {339, "oe"}, // MA# ( œ → oe ) LATIN SMALL LIGATURE OE → LATIN SMALL LETTER O, LATIN SMALL LETTER E#
 {383, "f"}, // MA# ( ſ → f ) LATIN SMALL LETTER LONG S → LATIN SMALL LETTER F#
 {385, "'B"}, // MA# ( Ɓ → 'B ) LATIN CAPITAL LETTER B WITH HOOK → APOSTROPHE, LATIN CAPITAL LETTER B# →ʽB→
 {388, "b"}, // MA# ( Ƅ → b ) LATIN CAPITAL LETTER TONE SIX → LATIN SMALL LETTER B#
 {391, "C'"}, // MA# ( Ƈ → C' ) LATIN CAPITAL LETTER C WITH HOOK → LATIN CAPITAL LETTER C, APOSTROPHE# →Cʽ→
 {394, "'D"}, // MA# ( Ɗ → 'D ) LATIN CAPITAL LETTER D WITH HOOK → APOSTROPHE, LATIN CAPITAL LETTER D# →ʽD→
 {397, "g"}, // MA# ( ƍ → g ) LATIN SMALL LETTER TURNED DELTA → LATIN SMALL LETTER G#
 {403, "G'"}, // MA# ( Ɠ → G' ) LATIN CAPITAL LETTER G WITH HOOK → LATIN CAPITAL LETTER G, APOSTROPHE# →Gʽ→
 {406, "l"}, // MA# ( Ɩ → l ) LATIN CAPITAL LETTER IOTA → LATIN SMALL LETTER L#
 {408, "K'"}, // MA# ( Ƙ → K' ) LATIN CAPITAL LETTER K WITH HOOK → LATIN CAPITAL LETTER K, APOSTROPHE# →Kʽ→
 {416, "O'"}, // MA# ( Ơ → O' ) LATIN CAPITAL LETTER O WITH HORN → LATIN CAPITAL LETTER O, APOSTROPHE# →Oʼ→
 {417, "o'"}, // MA# ( ơ → o' ) LATIN SMALL LETTER O WITH HORN → LATIN SMALL LETTER O, APOSTROPHE# →oʼ→
 {420, "'P"}, // MA# ( Ƥ → 'P ) LATIN CAPITAL LETTER P WITH HOOK → APOSTROPHE, LATIN CAPITAL LETTER P# →ʽP→
 {422, "R"}, // MA# ( Ʀ → R ) LATIN LETTER YR → LATIN CAPITAL LETTER R#
 {423, "2"}, // MA# ( Ƨ → 2 ) LATIN CAPITAL LETTER TONE TWO → DIGIT TWO#
 {428, "'T"}, // MA# ( Ƭ → 'T ) LATIN CAPITAL LETTER T WITH HOOK → APOSTROPHE, LATIN CAPITAL LETTER T# →ʽT→
 {435, "'Y"}, // MA# ( Ƴ → 'Y ) LATIN CAPITAL LETTER Y WITH HOOK → APOSTROPHE, LATIN CAPITAL LETTER Y# →ʽY→
 {439, "3"}, // MA# ( Ʒ → 3 ) LATIN CAPITAL LETTER EZH → DIGIT THREE#
 {444, "5"}, // MA# ( Ƽ → 5 ) LATIN CAPITAL LETTER TONE FIVE → DIGIT FIVE#
 {445, "s"}, // MA# ( ƽ → s ) LATIN SMALL LETTER TONE FIVE → LATIN SMALL LETTER S#
 {448, "l"}, // MA# ( ǀ → l ) LATIN LETTER DENTAL CLICK → LATIN SMALL LETTER L#
 {449, "ll"}, // MA# ( ǁ → ll ) LATIN LETTER LATERAL CLICK → LATIN SMALL LETTER L, LATIN SMALL LETTER L# →‖→→∥→→||→
 {451, "!"}, // MA# ( ǃ → ! ) LATIN LETTER RETROFLEX CLICK → EXCLAMATION MARK#
 {455, "LJ"}, // MA# ( Ǉ → LJ ) LATIN CAPITAL LETTER LJ → LATIN CAPITAL LETTER L, LATIN CAPITAL LETTER J#
 {456, "Lj"}, // MA# ( ǈ → Lj ) LATIN CAPITAL LETTER L WITH SMALL LETTER J → LATIN CAPITAL LETTER L, LATIN SMALL LETTER J#
 {457, "lj"}, // MA# ( ǉ → lj ) LATIN SMALL LETTER LJ → LATIN SMALL LETTER L, LATIN SMALL LETTER J#
 {458, "NJ"}, // MA# ( Ǌ → NJ ) LATIN CAPITAL LETTER NJ → LATIN CAPITAL LETTER N, LATIN CAPITAL LETTER J#
 {459, "Nj"}, // MA# ( ǋ → Nj ) LATIN CAPITAL LETTER N WITH SMALL LETTER J → LATIN CAPITAL LETTER N, LATIN SMALL LETTER J#
 {460, "nj"}, // MA# ( ǌ → nj ) LATIN SMALL LETTER NJ → LATIN SMALL LETTER N, LATIN SMALL LETTER J#
 {497, "DZ"}, // MA# ( Ǳ → DZ ) LATIN CAPITAL LETTER DZ → LATIN CAPITAL LETTER D, LATIN CAPITAL LETTER Z#
 {498, "Dz"}, // MA# ( ǲ → Dz ) LATIN CAPITAL LETTER D WITH SMALL LETTER Z → LATIN CAPITAL LETTER D, LATIN SMALL LETTER Z#
 {499, "dz"}, // MA# ( ǳ → dz ) LATIN SMALL LETTER DZ → LATIN SMALL LETTER D, LATIN SMALL LETTER Z#
 {540, "3"}, // MA# ( Ȝ → 3 ) LATIN CAPITAL LETTER YOGH → DIGIT THREE# →Ʒ→
 {546, "8"}, // MA# ( Ȣ → 8 ) LATIN CAPITAL LETTER OU → DIGIT EIGHT#
 {547, "8"}, // MA# ( ȣ → 8 ) LATIN SMALL LETTER OU → DIGIT EIGHT#
 {577, "?"}, // MA# ( Ɂ → ? ) LATIN CAPITAL LETTER GLOTTAL STOP → QUESTION MARK# →ʔ→
 {593, "a"}, // MA# ( ɑ → a ) LATIN SMALL LETTER ALPHA → LATIN SMALL LETTER A#
 {609, "g"}, // MA# ( ɡ → g ) LATIN SMALL LETTER SCRIPT G → LATIN SMALL LETTER G#
 {611, "y"}, // MA# ( ɣ → y ) LATIN SMALL LETTER GAMMA → LATIN SMALL LETTER Y# →γ→
 {617, "i"}, // MA# ( ɩ → i ) LATIN SMALL LETTER IOTA → LATIN SMALL LETTER I#
 {618, "i"}, // MA# ( ɪ → i ) LATIN LETTER SMALL CAPITAL I → LATIN SMALL LETTER I# →ı→
 {623, "w"}, // MA# ( ɯ → w ) LATIN SMALL LETTER TURNED M → LATIN SMALL LETTER W#
 {651, "u"}, // MA# ( ʋ → u ) LATIN SMALL LETTER V WITH HOOK → LATIN SMALL LETTER U#
 {655, "y"}, // MA# ( ʏ → y ) LATIN LETTER SMALL CAPITAL Y → LATIN SMALL LETTER Y# →ү→→γ→
 {660, "?"}, // MA# ( ʔ → ? ) LATIN LETTER GLOTTAL STOP → QUESTION MARK#
 {675, "dz"}, // MA# ( ʣ → dz ) LATIN SMALL LETTER DZ DIGRAPH → LATIN SMALL LETTER D, LATIN SMALL LETTER Z#
 {678, "ts"}, // MA# ( ʦ → ts ) LATIN SMALL LETTER TS DIGRAPH → LATIN SMALL LETTER T, LATIN SMALL LETTER S#
 {682, "ls"}, // MA# ( ʪ → ls ) LATIN SMALL LETTER LS DIGRAPH → LATIN SMALL LETTER L, LATIN SMALL LETTER S#
 {683, "lz"}, // MA# ( ʫ → lz ) LATIN SMALL LETTER LZ DIGRAPH → LATIN SMALL LETTER L, LATIN SMALL LETTER Z#
 {697, "'"}, // MA# ( ʹ → ' ) MODIFIER LETTER PRIME → APOSTROPHE#
 {698, "\""}, // MA# ( ʺ → '' ) MODIFIER LETTER DOUBLE PRIME → APOSTROPHE, APOSTROPHE# →"→# Converted to a quote.
 {699, "'"}, // MA# ( ʻ → ' ) MODIFIER LETTER TURNED COMMA → APOSTROPHE# →‘→
 {700, "'"}, // MA# ( ʼ → ' ) MODIFIER LETTER APOSTROPHE → APOSTROPHE# →′→
 {701, "'"}, // MA# ( ʽ → ' ) MODIFIER LETTER REVERSED COMMA → APOSTROPHE# →‘→
 {702, "'"}, // MA# ( ʾ → ' ) MODIFIER LETTER RIGHT HALF RING → APOSTROPHE# →ʼ→→′→
 {706, "<"}, // MA#* ( ˂ → < ) MODIFIER LETTER LEFT ARROWHEAD → LESS-THAN SIGN#
 {707, ">"}, // MA#* ( ˃ → > ) MODIFIER LETTER RIGHT ARROWHEAD → GREATER-THAN SIGN#
 {708, "^"}, // MA#* ( ˄ → ^ ) MODIFIER LETTER UP ARROWHEAD → CIRCUMFLEX ACCENT#
 {710, "^"}, // MA# ( ˆ → ^ ) MODIFIER LETTER CIRCUMFLEX ACCENT → CIRCUMFLEX ACCENT#
 {712, "'"}, // MA# ( ˈ → ' ) MODIFIER LETTER VERTICAL LINE → APOSTROPHE#
 {714, "'"}, // MA# ( ˊ → ' ) MODIFIER LETTER ACUTE ACCENT → APOSTROPHE# →ʹ→→′→
 {715, "'"}, // MA# ( ˋ → ' ) MODIFIER LETTER GRAVE ACCENT → APOSTROPHE# →｀→→‘→
 {720, ":"}, // MA# ( ː → : ) MODIFIER LETTER TRIANGULAR COLON → COLON#
 {727, "-"}, // MA#* ( ˗ → - ) MODIFIER LETTER MINUS SIGN → HYPHEN-MINUS#
 {731, "i"}, // MA#* ( ˛ → i ) OGONEK → LATIN SMALL LETTER I# →ͺ→→ι→→ι→
 {732, "~"}, // MA#* ( ˜ → ~ ) SMALL TILDE → TILDE#
 {733, "\""}, // MA#* ( ˝ → '' ) DOUBLE ACUTE ACCENT → APOSTROPHE, APOSTROPHE# →"→# Converted to a quote.
 {750, "\""}, // MA# ( ˮ → '' ) MODIFIER LETTER DOUBLE APOSTROPHE → APOSTROPHE, APOSTROPHE# →″→→"→# Converted to a quote.
 {756, "'"}, // MA#* ( ˴ → ' ) MODIFIER LETTER MIDDLE GRAVE ACCENT → APOSTROPHE# →ˋ→→｀→→‘→
 {758, "\""}, // MA#* ( ˶ → '' ) MODIFIER LETTER MIDDLE DOUBLE ACUTE ACCENT → APOSTROPHE, APOSTROPHE# →˝→→"→# Converted to a quote.
 {760, ":"}, // MA#* ( ˸ → : ) MODIFIER LETTER RAISED COLON → COLON#
 {884, "'"}, // MA# ( ʹ → ' ) GREEK NUMERAL SIGN → APOSTROPHE# →′→
 {890, "i"}, // MA#* ( ͺ → i ) GREEK YPOGEGRAMMENI → LATIN SMALL LETTER I# →ι→→ι→
 {894, ";"}, // MA#* ( ; → ; ) GREEK QUESTION MARK → SEMICOLON#
 {895, "J"}, // MA# ( Ϳ → J ) GREEK CAPITAL LETTER YOT → LATIN CAPITAL LETTER J#
 {900, "'"}, // MA#* ( ΄ → ' ) GREEK TONOS → APOSTROPHE# →ʹ→
 {913, "A"}, // MA# ( Α → A ) GREEK CAPITAL LETTER ALPHA → LATIN CAPITAL LETTER A#
 {914, "B"}, // MA# ( Β → B ) GREEK CAPITAL LETTER BETA → LATIN CAPITAL LETTER B#
 {917, "E"}, // MA# ( Ε → E ) GREEK CAPITAL LETTER EPSILON → LATIN CAPITAL LETTER E#
 {918, "Z"}, // MA# ( Ζ → Z ) GREEK CAPITAL LETTER ZETA → LATIN CAPITAL LETTER Z#
 {919, "H"}, // MA# ( Η → H ) GREEK CAPITAL LETTER ETA → LATIN CAPITAL LETTER H#
 {921, "l"}, // MA# ( Ι → l ) GREEK CAPITAL LETTER IOTA → LATIN SMALL LETTER L#
 {922, "K"}, // MA# ( Κ → K ) GREEK CAPITAL LETTER KAPPA → LATIN CAPITAL LETTER K#
 {924, "M"}, // MA# ( Μ → M ) GREEK CAPITAL LETTER MU → LATIN CAPITAL LETTER M#
 {925, "N"}, // MA# ( Ν → N ) GREEK CAPITAL LETTER NU → LATIN CAPITAL LETTER N#
 {927, "O"}, // MA# ( Ο → O ) GREEK CAPITAL LETTER OMICRON → LATIN CAPITAL LETTER O#
 {929, "P"}, // MA# ( Ρ → P ) GREEK CAPITAL LETTER RHO → LATIN CAPITAL LETTER P#
 {932, "T"}, // MA# ( Τ → T ) GREEK CAPITAL LETTER TAU → LATIN CAPITAL LETTER T#
 {933, "Y"}, // MA# ( Υ → Y ) GREEK CAPITAL LETTER UPSILON → LATIN CAPITAL LETTER Y#
 {935, "X"}, // MA# ( Χ → X ) GREEK CAPITAL LETTER CHI → LATIN CAPITAL LETTER X#
 {945, "a"}, // MA# ( α → a ) GREEK SMALL LETTER ALPHA → LATIN SMALL LETTER A#
 {947, "y"}, // MA# ( γ → y ) GREEK SMALL LETTER GAMMA → LATIN SMALL LETTER Y#
 {953, "i"}, // MA# ( ι → i ) GREEK SMALL LETTER IOTA → LATIN SMALL LETTER I#
 {957, "v"}, // MA# ( ν → v ) GREEK SMALL LETTER NU → LATIN SMALL LETTER V#
 {959, "o"}, // MA# ( ο → o ) GREEK SMALL LETTER OMICRON → LATIN SMALL LETTER O#
 {961, "p"}, // MA# ( ρ → p ) GREEK SMALL LETTER RHO → LATIN SMALL LETTER P#
 {963, "o"}, // MA# ( σ → o ) GREEK SMALL LETTER SIGMA → LATIN SMALL LETTER O#
 {965, "u"}, // MA# ( υ → u ) GREEK SMALL LETTER UPSILON → LATIN SMALL LETTER U# →ʋ→
 {978, "Y"}, // MA# ( ϒ → Y ) GREEK UPSILON WITH HOOK SYMBOL → LATIN CAPITAL LETTER Y#
 {988, "F"}, // MA# ( Ϝ → F ) GREEK LETTER DIGAMMA → LATIN CAPITAL LETTER F#
 {1000, "2"}, // MA# ( Ϩ → 2 ) COPTIC CAPITAL LETTER HORI → DIGIT TWO# →Ƨ→
 {1009, "p"}, // MA# ( ϱ → p ) GREEK RHO SYMBOL → LATIN SMALL LETTER P# →ρ→
 {1010, "c"}, // MA# ( ϲ → c ) GREEK LUNATE SIGMA SYMBOL → LATIN SMALL LETTER C#
 {1011, "j"}, // MA# ( ϳ → j ) GREEK LETTER YOT → LATIN SMALL LETTER J#
 {1017, "C"}, // MA# ( Ϲ → C ) GREEK CAPITAL LUNATE SIGMA SYMBOL → LATIN CAPITAL LETTER C#
 {1018, "M"}, // MA# ( Ϻ → M ) GREEK CAPITAL LETTER SAN → LATIN CAPITAL LETTER M#
 {1029, "S"}, // MA# ( Ѕ → S ) CYRILLIC CAPITAL LETTER DZE → LATIN CAPITAL LETTER S#
 {1030, "l"}, // MA# ( І → l ) CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I → LATIN SMALL LETTER L#
 {1032, "J"}, // MA# ( Ј → J ) CYRILLIC CAPITAL LETTER JE → LATIN CAPITAL LETTER J#
 {1040, "A"}, // MA# ( А → A ) CYRILLIC CAPITAL LETTER A → LATIN CAPITAL LETTER A#
 {1042, "B"}, // MA# ( В → B ) CYRILLIC CAPITAL LETTER VE → LATIN CAPITAL LETTER B#
 {1045, "E"}, // MA# ( Е → E ) CYRILLIC CAPITAL LETTER IE → LATIN CAPITAL LETTER E#
 {1047, "3"}, // MA# ( З → 3 ) CYRILLIC CAPITAL LETTER ZE → DIGIT THREE#
 {1050, "K"}, // MA# ( К → K ) CYRILLIC CAPITAL LETTER KA → LATIN CAPITAL LETTER K#
 {1052, "M"}, // MA# ( М → M ) CYRILLIC CAPITAL LETTER EM → LATIN CAPITAL LETTER M#
 {1053, "H"}, // MA# ( Н → H ) CYRILLIC CAPITAL LETTER EN → LATIN CAPITAL LETTER H#
 {1054, "O"}, // MA# ( О → O ) CYRILLIC CAPITAL LETTER O → LATIN CAPITAL LETTER O#
 {1056, "P"}, // MA# ( Р → P ) CYRILLIC CAPITAL LETTER ER → LATIN CAPITAL LETTER P#
 {1057, "C"}, // MA# ( С → C ) CYRILLIC CAPITAL LETTER ES → LATIN CAPITAL LETTER C#
 {1058, "T"}, // MA# ( Т → T ) CYRILLIC CAPITAL LETTER TE → LATIN CAPITAL LETTER T#
 {1059, "Y"}, // MA# ( У → Y ) CYRILLIC CAPITAL LETTER U → LATIN CAPITAL LETTER Y#
 {1061, "X"}, // MA# ( Х → X ) CYRILLIC CAPITAL LETTER HA → LATIN CAPITAL LETTER X#
 {1067, "bl"}, // MA# ( Ы → bl ) CYRILLIC CAPITAL LETTER YERU → LATIN SMALL LETTER B, LATIN SMALL LETTER L# →ЬІ→→Ь1→
 {1068, "b"}, // MA# ( Ь → b ) CYRILLIC CAPITAL LETTER SOFT SIGN → LATIN SMALL LETTER B# →Ƅ→
 {1070, "lO"}, // MA# ( Ю → lO ) CYRILLIC CAPITAL LETTER YU → LATIN SMALL LETTER L, LATIN CAPITAL LETTER O# →IO→
 {1072, "a"}, // MA# ( а → a ) CYRILLIC SMALL LETTER A → LATIN SMALL LETTER A#
 {1073, "6"}, // MA# ( б → 6 ) CYRILLIC SMALL LETTER BE → DIGIT SIX#
 {1075, "r"}, // MA# ( г → r ) CYRILLIC SMALL LETTER GHE → LATIN SMALL LETTER R#
 {1077, "e"}, // MA# ( е → e ) CYRILLIC SMALL LETTER IE → LATIN SMALL LETTER E#
 {1086, "o"}, // MA# ( о → o ) CYRILLIC SMALL LETTER O → LATIN SMALL LETTER O#
 {1088, "p"}, // MA# ( р → p ) CYRILLIC SMALL LETTER ER → LATIN SMALL LETTER P#
 {1089, "c"}, // MA# ( с → c ) CYRILLIC SMALL LETTER ES → LATIN SMALL LETTER C#
 {1091, "y"}, // MA# ( у → y ) CYRILLIC SMALL LETTER U → LATIN SMALL LETTER Y#
 {1093, "x"}, // MA# ( х → x ) CYRILLIC SMALL LETTER HA → LATIN SMALL LETTER X#
 {1109, "s"}, // MA# ( ѕ → s ) CYRILLIC SMALL LETTER DZE → LATIN SMALL LETTER S#
 {1110, "i"}, // MA# ( і → i ) CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I → LATIN SMALL LETTER I#
 {1112, "j"}, // MA# ( ј → j ) CYRILLIC SMALL LETTER JE → LATIN SMALL LETTER J#
 {1121, "w"}, // MA# ( ѡ → w ) CYRILLIC SMALL LETTER OMEGA → LATIN SMALL LETTER W#
 {1140, "V"}, // MA# ( Ѵ → V ) CYRILLIC CAPITAL LETTER IZHITSA → LATIN CAPITAL LETTER V#
 {1141, "v"}, // MA# ( ѵ → v ) CYRILLIC SMALL LETTER IZHITSA → LATIN SMALL LETTER V#
 {1169, "r'"}, // MA# ( ґ → r' ) CYRILLIC SMALL LETTER GHE WITH UPTURN → LATIN SMALL LETTER R, APOSTROPHE# →гˈ→
 {1198, "Y"}, // MA# ( Ү → Y ) CYRILLIC CAPITAL LETTER STRAIGHT U → LATIN CAPITAL LETTER Y#
 {1199, "y"}, // MA# ( ү → y ) CYRILLIC SMALL LETTER STRAIGHT U → LATIN SMALL LETTER Y# →γ→
 {1211, "h"}, // MA# ( һ → h ) CYRILLIC SMALL LETTER SHHA → LATIN SMALL LETTER H#
 {1213, "e"}, // MA# ( ҽ → e ) CYRILLIC SMALL LETTER ABKHASIAN CHE → LATIN SMALL LETTER E#
 {1216, "l"}, // MA# ( Ӏ → l ) CYRILLIC LETTER PALOCHKA → LATIN SMALL LETTER L#
 {1231, "i"}, // MA# ( ӏ → i ) CYRILLIC SMALL LETTER PALOCHKA → LATIN SMALL LETTER I# →ı→
 {1236, "AE"}, // MA# ( Ӕ → AE ) CYRILLIC CAPITAL LIGATURE A IE → LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER E# →Æ→
 {1237, "ae"}, // MA# ( ӕ → ae ) CYRILLIC SMALL LIGATURE A IE → LATIN SMALL LETTER A, LATIN SMALL LETTER E# →ае→
 {1248, "3"}, // MA# ( Ӡ → 3 ) CYRILLIC CAPITAL LETTER ABKHASIAN DZE → DIGIT THREE# →Ʒ→
 {1281, "d"}, // MA# ( ԁ → d ) CYRILLIC SMALL LETTER KOMI DE → LATIN SMALL LETTER D#
 {1292, "G"}, // MA# ( Ԍ → G ) CYRILLIC CAPITAL LETTER KOMI SJE → LATIN CAPITAL LETTER G#
 {1307, "q"}, // MA# ( ԛ → q ) CYRILLIC SMALL LETTER QA → LATIN SMALL LETTER Q#
 {1308, "W"}, // MA# ( Ԝ → W ) CYRILLIC CAPITAL LETTER WE → LATIN CAPITAL LETTER W#
 {1309, "w"}, // MA# ( ԝ → w ) CYRILLIC SMALL LETTER WE → LATIN SMALL LETTER W#
 {1357, "U"}, // MA# ( Ս → U ) ARMENIAN CAPITAL LETTER SEH → LATIN CAPITAL LETTER U#
 {1359, "S"}, // MA# ( Տ → S ) ARMENIAN CAPITAL LETTER TIWN → LATIN CAPITAL LETTER S#
 {1365, "O"}, // MA# ( Օ → O ) ARMENIAN CAPITAL LETTER OH → LATIN CAPITAL LETTER O#
 {1370, "'"}, // MA#* ( ՚ → ' ) ARMENIAN APOSTROPHE → APOSTROPHE# →’→
 {1373, "'"}, // MA#* ( ՝ → ' ) ARMENIAN COMMA → APOSTROPHE# →ˋ→→｀→→‘→
 {1377, "w"}, // MA# ( ա → w ) ARMENIAN SMALL LETTER AYB → LATIN SMALL LETTER W# →ɯ→
 {1379, "q"}, // MA# ( գ → q ) ARMENIAN SMALL LETTER GIM → LATIN SMALL LETTER Q#
 {1382, "q"}, // MA# ( զ → q ) ARMENIAN SMALL LETTER ZA → LATIN SMALL LETTER Q#
 {1392, "h"}, // MA# ( հ → h ) ARMENIAN SMALL LETTER HO → LATIN SMALL LETTER H#
 {1400, "n"}, // MA# ( ո → n ) ARMENIAN SMALL LETTER VO → LATIN SMALL LETTER N#
 {1404, "n"}, // MA# ( ռ → n ) ARMENIAN SMALL LETTER RA → LATIN SMALL LETTER N#
 {1405, "u"}, // MA# ( ս → u ) ARMENIAN SMALL LETTER SEH → LATIN SMALL LETTER U#
 {1409, "g"}, // MA# ( ց → g ) ARMENIAN SMALL LETTER CO → LATIN SMALL LETTER G#
 {1412, "f"}, // MA# ( ք → f ) ARMENIAN SMALL LETTER KEH → LATIN SMALL LETTER F#
 {1413, "o"}, // MA# ( օ → o ) ARMENIAN SMALL LETTER OH → LATIN SMALL LETTER O#
 {1417, ":"}, // MA#* ( ։ → : ) ARMENIAN FULL STOP → COLON#
 {1472, "l"}, // MA#* ( ‎׀‎ → l ) HEBREW PUNCTUATION PASEQ → LATIN SMALL LETTER L# →|→
 {1475, ":"}, // MA#* ( ‎׃‎ → : ) HEBREW PUNCTUATION SOF PASUQ → COLON#
 {1493, "l"}, // MA# ( ‎ו‎ → l ) HEBREW LETTER VAV → LATIN SMALL LETTER L#
 {1496, "v"}, // MA# ( ‎ט‎ → v ) HEBREW LETTER TET → LATIN SMALL LETTER V#
 {1497, "'"}, // MA# ( ‎י‎ → ' ) HEBREW LETTER YOD → APOSTROPHE#
 {1503, "l"}, // MA# ( ‎ן‎ → l ) HEBREW LETTER FINAL NUN → LATIN SMALL LETTER L#
 {1505, "o"}, // MA# ( ‎ס‎ → o ) HEBREW LETTER SAMEKH → LATIN SMALL LETTER O#
 {1520, "ll"}, // MA# ( ‎װ‎ → ll ) HEBREW LIGATURE YIDDISH DOUBLE VAV → LATIN SMALL LETTER L, LATIN SMALL LETTER L# →‎וו‎→
 {1521, "l'"}, // MA# ( ‎ױ‎ → l' ) HEBREW LIGATURE YIDDISH VAV YOD → LATIN SMALL LETTER L, APOSTROPHE# →‎וי‎→
 {1522, "\""}, // MA# ( ‎ײ‎ → '' ) HEBREW LIGATURE YIDDISH DOUBLE YOD → APOSTROPHE, APOSTROPHE# →‎יי‎→# Converted to a quote.
 {1523, "'"}, // MA#* ( ‎׳‎ → ' ) HEBREW PUNCTUATION GERESH → APOSTROPHE#
 {1524, "\""}, // MA#* ( ‎״‎ → '' ) HEBREW PUNCTUATION GERSHAYIM → APOSTROPHE, APOSTROPHE# →"→# Converted to a quote.
 {1549, ","}, // MA#* ( ‎؍‎ → , ) ARABIC DATE SEPARATOR → COMMA# →‎٫‎→
 {1575, "l"}, // MA# ( ‎ا‎ → l ) ARABIC LETTER ALEF → LATIN SMALL LETTER L# →1→
 {1607, "o"}, // MA# ( ‎ه‎ → o ) ARABIC LETTER HEH → LATIN SMALL LETTER O#
 {1632, "."}, // MA# ( ‎٠‎ → . ) ARABIC-INDIC DIGIT ZERO → FULL STOP#
 {1633, "l"}, // MA# ( ‎١‎ → l ) ARABIC-INDIC DIGIT ONE → LATIN SMALL LETTER L# →1→
 {1637, "o"}, // MA# ( ‎٥‎ → o ) ARABIC-INDIC DIGIT FIVE → LATIN SMALL LETTER O#
 {1639, "V"}, // MA# ( ‎٧‎ → V ) ARABIC-INDIC DIGIT SEVEN → LATIN CAPITAL LETTER V#
 {1643, ","}, // MA#* ( ‎٫‎ → , ) ARABIC DECIMAL SEPARATOR → COMMA#
 {1645, "*"}, // MA#* ( ‎٭‎ → * ) ARABIC FIVE POINTED STAR → ASTERISK#
 {1726, "o"}, // MA# ( ‎ھ‎ → o ) ARABIC LETTER HEH DOACHASHMEE → LATIN SMALL LETTER O# →‎ه‎→
 {1729, "o"}, // MA# ( ‎ہ‎ → o ) ARABIC LETTER HEH GOAL → LATIN SMALL LETTER O# →‎ه‎→
 {1748, "-"}, // MA#* ( ‎۔‎ → - ) ARABIC FULL STOP → HYPHEN-MINUS# →‐→
 {1749, "o"}, // MA# ( ‎ە‎ → o ) ARABIC LETTER AE → LATIN SMALL LETTER O# →‎ه‎→
 {1776, "."}, // MA# ( ۰ → . ) EXTENDED ARABIC-INDIC DIGIT ZERO → FULL STOP# →‎٠‎→
 {1777, "l"}, // MA# ( ۱ → l ) EXTENDED ARABIC-INDIC DIGIT ONE → LATIN SMALL LETTER L# →1→
 {1781, "o"}, // MA# ( ۵ → o ) EXTENDED ARABIC-INDIC DIGIT FIVE → LATIN SMALL LETTER O# →‎٥‎→
 {1783, "V"}, // MA# ( ۷ → V ) EXTENDED ARABIC-INDIC DIGIT SEVEN → LATIN CAPITAL LETTER V# →‎٧‎→
 {1793, "."}, // MA#* ( ‎܁‎ → . ) SYRIAC SUPRALINEAR FULL STOP → FULL STOP#
 {1794, "."}, // MA#* ( ‎܂‎ → . ) SYRIAC SUBLINEAR FULL STOP → FULL STOP#
 {1795, ":"}, // MA#* ( ‎܃‎ → : ) SYRIAC SUPRALINEAR COLON → COLON#
 {1796, ":"}, // MA#* ( ‎܄‎ → : ) SYRIAC SUBLINEAR COLON → COLON#
 {1984, "O"}, // MA# ( ‎߀‎ → O ) NKO DIGIT ZERO → LATIN CAPITAL LETTER O# →0→
 {1994, "l"}, // MA# ( ‎ߊ‎ → l ) NKO LETTER A → LATIN SMALL LETTER L# →∣→→ǀ→
 {2036, "'"}, // MA# ( ‎ߴ‎ → ' ) NKO HIGH TONE APOSTROPHE → APOSTROPHE# →’→
 {2037, "'"}, // MA# ( ‎ߵ‎ → ' ) NKO LOW TONE APOSTROPHE → APOSTROPHE# →‘→
 {2042, "_"}, // MA# ( ‎ߺ‎ → _ ) NKO LAJANYALAN → LOW LINE#
 {2307, ":"}, // MA# ( ः → : ) DEVANAGARI SIGN VISARGA → COLON#
 {2406, "o"}, // MA# ( ० → o ) DEVANAGARI DIGIT ZERO → LATIN SMALL LETTER O#
 {2429, "?"}, // MA# ( ॽ → ? ) DEVANAGARI LETTER GLOTTAL STOP → QUESTION MARK#
 {2534, "O"}, // MA# ( ০ → O ) BENGALI DIGIT ZERO → LATIN CAPITAL LETTER O# →0→
 {2538, "8"}, // MA# ( ৪ → 8 ) BENGALI DIGIT FOUR → DIGIT EIGHT#
 {2541, "9"}, // MA# ( ৭ → 9 ) BENGALI DIGIT SEVEN → DIGIT NINE#
 {2662, "o"}, // MA# ( ੦ → o ) GURMUKHI DIGIT ZERO → LATIN SMALL LETTER O#
 {2663, "9"}, // MA# ( ੧ → 9 ) GURMUKHI DIGIT ONE → DIGIT NINE#
 {2666, "8"}, // MA# ( ੪ → 8 ) GURMUKHI DIGIT FOUR → DIGIT EIGHT#
 {2691, ":"}, // MA# ( ઃ → : ) GUJARATI SIGN VISARGA → COLON#
 {2790, "o"}, // MA# ( ૦ → o ) GUJARATI DIGIT ZERO → LATIN SMALL LETTER O#
 {2819, "8"}, // MA# ( ଃ → 8 ) ORIYA SIGN VISARGA → DIGIT EIGHT#
 {2848, "O"}, // MA# ( ଠ → O ) ORIYA LETTER TTHA → LATIN CAPITAL LETTER O# →୦→→0→
 {2918, "O"}, // MA# ( ୦ → O ) ORIYA DIGIT ZERO → LATIN CAPITAL LETTER O# →0→
 {2920, "9"}, // MA# ( ୨ → 9 ) ORIYA DIGIT TWO → DIGIT NINE#
 {3046, "o"}, // MA# ( ௦ → o ) TAMIL DIGIT ZERO → LATIN SMALL LETTER O#
 {3074, "o"}, // MA# ( ం → o ) TELUGU SIGN ANUSVARA → LATIN SMALL LETTER O#
 {3174, "o"}, // MA# ( ౦ → o ) TELUGU DIGIT ZERO → LATIN SMALL LETTER O#
 {3202, "o"}, // MA# ( ಂ → o ) KANNADA SIGN ANUSVARA → LATIN SMALL LETTER O#
 {3302, "o"}, // MA# ( ೦ → o ) KANNADA DIGIT ZERO → LATIN SMALL LETTER O# →౦→
 {3330, "o"}, // MA# ( ം → o ) MALAYALAM SIGN ANUSVARA → LATIN SMALL LETTER O#
 {3360, "o"}, // MA# ( ഠ → o ) MALAYALAM LETTER TTHA → LATIN SMALL LETTER O#
 {3430, "o"}, // MA# ( ൦ → o ) MALAYALAM DIGIT ZERO → LATIN SMALL LETTER O#
 {3437, "9"}, // MA# ( ൭ → 9 ) MALAYALAM DIGIT SEVEN → DIGIT NINE#
 {3458, "o"}, // MA# ( ං → o ) SINHALA SIGN ANUSVARAYA → LATIN SMALL LETTER O#
 {3664, "o"}, // MA# ( ๐ → o ) THAI DIGIT ZERO → LATIN SMALL LETTER O#
 {3792, "o"}, // MA# ( ໐ → o ) LAO DIGIT ZERO → LATIN SMALL LETTER O#
 {4125, "o"}, // MA# ( ဝ → o ) MYANMAR LETTER WA → LATIN SMALL LETTER O#
 {4160, "o"}, // MA# ( ၀ → o ) MYANMAR DIGIT ZERO → LATIN SMALL LETTER O#
 {4327, "y"}, // MA# ( ყ → y ) GEORGIAN LETTER QAR → LATIN SMALL LETTER Y#
 {4351, "o"}, // MA# ( ჿ → o ) GEORGIAN LETTER LABIAL SIGN → LATIN SMALL LETTER O#
 {4608, "U"}, // MA# ( ሀ → U ) ETHIOPIC SYLLABLE HA → LATIN CAPITAL LETTER U# →Ս→
 {4816, "O"}, // MA# ( ዐ → O ) ETHIOPIC SYLLABLE PHARYNGEAL A → LATIN CAPITAL LETTER O# →Օ→
 {5024, "D"}, // MA# ( Ꭰ → D ) CHEROKEE LETTER A → LATIN CAPITAL LETTER D#
 {5025, "R"}, // MA# ( Ꭱ → R ) CHEROKEE LETTER E → LATIN CAPITAL LETTER R#
 {5026, "T"}, // MA# ( Ꭲ → T ) CHEROKEE LETTER I → LATIN CAPITAL LETTER T#
 {5028, "O'"}, // MA# ( Ꭴ → O' ) CHEROKEE LETTER U → LATIN CAPITAL LETTER O, APOSTROPHE# →Ơ→→Oʼ→
 {5029, "i"}, // MA# ( Ꭵ → i ) CHEROKEE LETTER V → LATIN SMALL LETTER I#
 {5033, "Y"}, // MA# ( Ꭹ → Y ) CHEROKEE LETTER GI → LATIN CAPITAL LETTER Y#
 {5034, "A"}, // MA# ( Ꭺ → A ) CHEROKEE LETTER GO → LATIN CAPITAL LETTER A#
 {5035, "J"}, // MA# ( Ꭻ → J ) CHEROKEE LETTER GU → LATIN CAPITAL LETTER J#
 {5036, "E"}, // MA# ( Ꭼ → E ) CHEROKEE LETTER GV → LATIN CAPITAL LETTER E#
 {5038, "?"}, // MA# ( Ꭾ → ? ) CHEROKEE LETTER HE → QUESTION MARK# →Ɂ→→ʔ→
 {5043, "W"}, // MA# ( Ꮃ → W ) CHEROKEE LETTER LA → LATIN CAPITAL LETTER W#
 {5047, "M"}, // MA# ( Ꮇ → M ) CHEROKEE LETTER LU → LATIN CAPITAL LETTER M#
 {5051, "H"}, // MA# ( Ꮋ → H ) CHEROKEE LETTER MI → LATIN CAPITAL LETTER H#
 {5053, "Y"}, // MA# ( Ꮍ → Y ) CHEROKEE LETTER MU → LATIN CAPITAL LETTER Y# →Ꭹ→
 {5056, "G"}, // MA# ( Ꮐ → G ) CHEROKEE LETTER NAH → LATIN CAPITAL LETTER G#
 {5058, "h"}, // MA# ( Ꮒ → h ) CHEROKEE LETTER NI → LATIN SMALL LETTER H#
 {5059, "Z"}, // MA# ( Ꮓ → Z ) CHEROKEE LETTER NO → LATIN CAPITAL LETTER Z#
 {5070, "4"}, // MA# ( Ꮞ → 4 ) CHEROKEE LETTER SE → DIGIT FOUR#
 {5071, "b"}, // MA# ( Ꮟ → b ) CHEROKEE LETTER SI → LATIN SMALL LETTER B#
 {5074, "R"}, // MA# ( Ꮢ → R ) CHEROKEE LETTER SV → LATIN CAPITAL LETTER R#
 {5076, "W"}, // MA# ( Ꮤ → W ) CHEROKEE LETTER TA → LATIN CAPITAL LETTER W#
 {5077, "S"}, // MA# ( Ꮥ → S ) CHEROKEE LETTER DE → LATIN CAPITAL LETTER S#
 {5081, "V"}, // MA# ( Ꮩ → V ) CHEROKEE LETTER DO → LATIN CAPITAL LETTER V#
 {5082, "S"}, // MA# ( Ꮪ → S ) CHEROKEE LETTER DU → LATIN CAPITAL LETTER S#
 {5086, "L"}, // MA# ( Ꮮ → L ) CHEROKEE LETTER TLE → LATIN CAPITAL LETTER L#
 {5087, "C"}, // MA# ( Ꮯ → C ) CHEROKEE LETTER TLI → LATIN CAPITAL LETTER C#
 {5090, "P"}, // MA# ( Ꮲ → P ) CHEROKEE LETTER TLV → LATIN CAPITAL LETTER P#
 {5094, "K"}, // MA# ( Ꮶ → K ) CHEROKEE LETTER TSO → LATIN CAPITAL LETTER K#
 {5095, "d"}, // MA# ( Ꮷ → d ) CHEROKEE LETTER TSU → LATIN SMALL LETTER D#
 {5102, "6"}, // MA# ( Ꮾ → 6 ) CHEROKEE LETTER WV → DIGIT SIX#
 {5107, "G"}, // MA# ( Ᏻ → G ) CHEROKEE LETTER YU → LATIN CAPITAL LETTER G#
 {5108, "B"}, // MA# ( Ᏼ → B ) CHEROKEE LETTER YV → LATIN CAPITAL LETTER B#
 {5120, "="}, // MA#* ( ᐀ → = ) CANADIAN SYLLABICS HYPHEN → EQUALS SIGN#
 {5167, "V"}, // MA# ( ᐯ → V ) CANADIAN SYLLABICS PE → LATIN CAPITAL LETTER V#
 {5171, ">"}, // MA# ( ᐳ → > ) CANADIAN SYLLABICS PO → GREATER-THAN SIGN#
 {5176, "<"}, // MA# ( ᐸ → < ) CANADIAN SYLLABICS PA → LESS-THAN SIGN#
 {5194, "'"}, // MA# ( ᑊ → ' ) CANADIAN SYLLABICS WEST-CREE P → APOSTROPHE# →ˈ→
 {5196, "U"}, // MA# ( ᑌ → U ) CANADIAN SYLLABICS TE → LATIN CAPITAL LETTER U#
 {5223, "U'"}, // MA# ( ᑧ → U' ) CANADIAN SYLLABICS TTE → LATIN CAPITAL LETTER U, APOSTROPHE# →ᑌᑊ→→ᑌ'→
 {5229, "P"}, // MA# ( ᑭ → P ) CANADIAN SYLLABICS KI → LATIN CAPITAL LETTER P#
 {5231, "d"}, // MA# ( ᑯ → d ) CANADIAN SYLLABICS KO → LATIN SMALL LETTER D#
 {5254, "P'"}, // MA# ( ᒆ → P' ) CANADIAN SYLLABICS SOUTH-SLAVEY KIH → LATIN CAPITAL LETTER P, APOSTROPHE# →ᑭᑊ→
 {5255, "d'"}, // MA# ( ᒇ → d' ) CANADIAN SYLLABICS SOUTH-SLAVEY KOH → LATIN SMALL LETTER D, APOSTROPHE# →ᑯᑊ→
 {5261, "J"}, // MA# ( ᒍ → J ) CANADIAN SYLLABICS CO → LATIN CAPITAL LETTER J#
 {5290, "L"}, // MA# ( ᒪ → L ) CANADIAN SYLLABICS MA → LATIN CAPITAL LETTER L#
 {5311, "2"}, // MA# ( ᒿ → 2 ) CANADIAN SYLLABICS SAYISI M → DIGIT TWO#
 {5441, "x"}, // MA# ( ᕁ → x ) CANADIAN SYLLABICS SAYISI YI → LATIN SMALL LETTER X# →᙮→
 {5500, "H"}, // MA# ( ᕼ → H ) CANADIAN SYLLABICS NUNAVUT H → LATIN CAPITAL LETTER H#
 {5501, "x"}, // MA# ( ᕽ → x ) CANADIAN SYLLABICS HK → LATIN SMALL LETTER X# →ᕁ→→᙮→
 {5511, "R"}, // MA# ( ᖇ → R ) CANADIAN SYLLABICS TLHI → LATIN CAPITAL LETTER R#
 {5551, "b"}, // MA# ( ᖯ → b ) CANADIAN SYLLABICS AIVILIK B → LATIN SMALL LETTER B#
 {5556, "F"}, // MA# ( ᖴ → F ) CANADIAN SYLLABICS BLACKFOOT WE → LATIN CAPITAL LETTER F#
 {5573, "A"}, // MA# ( ᗅ → A ) CANADIAN SYLLABICS CARRIER GHO → LATIN CAPITAL LETTER A#
 {5598, "D"}, // MA# ( ᗞ → D ) CANADIAN SYLLABICS CARRIER THE → LATIN CAPITAL LETTER D#
 {5610, "D"}, // MA# ( ᗪ → D ) CANADIAN SYLLABICS CARRIER PE → LATIN CAPITAL LETTER D# →ᗞ→
 {5616, "M"}, // MA# ( ᗰ → M ) CANADIAN SYLLABICS CARRIER GO → LATIN CAPITAL LETTER M#
 {5623, "B"}, // MA# ( ᗷ → B ) CANADIAN SYLLABICS CARRIER KHE → LATIN CAPITAL LETTER B#
 {5741, "X"}, // MA#* ( ᙭ → X ) CANADIAN SYLLABICS CHI SIGN → LATIN CAPITAL LETTER X#
 {5742, "x"}, // MA#* ( ᙮ → x ) CANADIAN SYLLABICS FULL STOP → LATIN SMALL LETTER X#
 {5760, " "}, // MA#* (   → ) OGHAM SPACE MARK → SPACE#
 {5810, "<"}, // MA# ( ᚲ → < ) RUNIC LETTER KAUNA → LESS-THAN SIGN#
 {5815, "X"}, // MA# ( ᚷ → X ) RUNIC LETTER GEBO GYFU G → LATIN CAPITAL LETTER X#
 {5825, "l"}, // MA# ( ᛁ → l ) RUNIC LETTER ISAZ IS ISS I → LATIN SMALL LETTER L# →I→
 {5836, "'"}, // MA# ( ᛌ → ' ) RUNIC LETTER SHORT-TWIG-SOL S → APOSTROPHE#
 {5845, "K"}, // MA# ( ᛕ → K ) RUNIC LETTER OPEN-P → LATIN CAPITAL LETTER K#
 {5846, "M"}, // MA# ( ᛖ → M ) RUNIC LETTER EHWAZ EH E → LATIN CAPITAL LETTER M#
 {5868, ":"}, // MA#* ( ᛬ → : ) RUNIC MULTIPLE PUNCTUATION → COLON#
 {5869, "+"}, // MA#* ( ᛭ → + ) RUNIC CROSS PUNCTUATION → PLUS SIGN#
 {5941, "/"}, // MA#* ( ᜵ → / ) PHILIPPINE SINGLE PUNCTUATION → SOLIDUS#
 {6147, ":"}, // MA#* ( ᠃ → : ) MONGOLIAN FULL STOP → COLON#
 {6153, ":"}, // MA#* ( ᠉ → : ) MONGOLIAN MANCHU FULL STOP → COLON#
 {7379, "\""}, // MA#* ( ᳓ → '' ) VEDIC SIGN NIHSHVASA → APOSTROPHE, APOSTROPHE# →″→→"→# Converted to a quote.
 {7428, "c"}, // MA# ( ᴄ → c ) LATIN LETTER SMALL CAPITAL C → LATIN SMALL LETTER C#
 {7439, "o"}, // MA# ( ᴏ → o ) LATIN LETTER SMALL CAPITAL O → LATIN SMALL LETTER O#
 {7441, "o"}, // MA# ( ᴑ → o ) LATIN SMALL LETTER SIDEWAYS O → LATIN SMALL LETTER O#
 {7452, "u"}, // MA# ( ᴜ → u ) LATIN LETTER SMALL CAPITAL U → LATIN SMALL LETTER U#
 {7456, "v"}, // MA# ( ᴠ → v ) LATIN LETTER SMALL CAPITAL V → LATIN SMALL LETTER V#
 {7457, "w"}, // MA# ( ᴡ → w ) LATIN LETTER SMALL CAPITAL W → LATIN SMALL LETTER W#
 {7458, "z"}, // MA# ( ᴢ → z ) LATIN LETTER SMALL CAPITAL Z → LATIN SMALL LETTER Z#
 {7462, "r"}, // MA# ( ᴦ → r ) GREEK LETTER SMALL CAPITAL GAMMA → LATIN SMALL LETTER R# →г→
 {7531, "ue"}, // MA# ( ᵫ → ue ) LATIN SMALL LETTER UE → LATIN SMALL LETTER U, LATIN SMALL LETTER E#
 {7555, "g"}, // MA# ( ᶃ → g ) LATIN SMALL LETTER G WITH PALATAL HOOK → LATIN SMALL LETTER G#
 {7564, "y"}, // MA# ( ᶌ → y ) LATIN SMALL LETTER V WITH PALATAL HOOK → LATIN SMALL LETTER Y#
 {7837, "f"}, // MA# ( ẝ → f ) LATIN SMALL LETTER LONG S WITH HIGH STROKE → LATIN SMALL LETTER F#
 {7935, "y"}, // MA# ( ỿ → y ) LATIN SMALL LETTER Y WITH LOOP → LATIN SMALL LETTER Y#
 {8125, "'"}, // MA#* ( ᾽ → ' ) GREEK KORONIS → APOSTROPHE# →’→
 {8126, "i"}, // MA# ( ι → i ) GREEK PROSGEGRAMMENI → LATIN SMALL LETTER I# →ι→
 {8127, "'"}, // MA#* ( ᾿ → ' ) GREEK PSILI → APOSTROPHE# →’→
 {8128, "~"}, // MA#* ( ῀ → ~ ) GREEK PERISPOMENI → TILDE# →˜→
 {8175, "'"}, // MA#* ( ` → ' ) GREEK VARIA → APOSTROPHE# →ˋ→→｀→→‘→
 {8189, "'"}, // MA#* ( ´ → ' ) GREEK OXIA → APOSTROPHE# →´→→΄→→ʹ→
 {8190, "'"}, // MA#* ( ῾ → ' ) GREEK DASIA → APOSTROPHE# →‛→→′→
 {8192, " "}, // MA#* (   → ) EN QUAD → SPACE#
 {8193, " "}, // MA#* (   → ) EM QUAD → SPACE#
 {8194, " "}, // MA#* (   → ) EN SPACE → SPACE#
 {8195, " "}, // MA#* (   → ) EM SPACE → SPACE#
 {8196, " "}, // MA#* (   → ) THREE-PER-EM SPACE → SPACE#
 {8197, " "}, // MA#* (   → ) FOUR-PER-EM SPACE → SPACE#
 {8198, " "}, // MA#* (   → ) SIX-PER-EM SPACE → SPACE#
 {8199, " "}, // MA#* (   → ) FIGURE SPACE → SPACE#
 {8200, " "}, // MA#* (   → ) PUNCTUATION SPACE → SPACE#
 {8201, " "}, // MA#* (   → ) THIN SPACE → SPACE#
 {8202, " "}, // MA#* (   → ) HAIR SPACE → SPACE#
 {8208, "-"}, // MA#* ( ‐ → - ) HYPHEN → HYPHEN-MINUS#
 {8209, "-"}, // MA#* ( ‑ → - ) NON-BREAKING HYPHEN → HYPHEN-MINUS#
 {8210, "-"}, // MA#* ( ‒ → - ) FIGURE DASH → HYPHEN-MINUS#
 {8211, "-"}, // MA#* ( – → - ) EN DASH → HYPHEN-MINUS#
 {8214, "ll"}, // MA#* ( ‖ → ll ) DOUBLE VERTICAL LINE → LATIN SMALL LETTER L, LATIN SMALL LETTER L# →∥→→||→
 {8216, "'"}, // MA#* ( ‘ → ' ) LEFT SINGLE QUOTATION MARK → APOSTROPHE#
 {8217, "'"}, // MA#* ( ’ → ' ) RIGHT SINGLE QUOTATION MARK → APOSTROPHE#
 {8218, ","}, // MA#* ( ‚ → , ) SINGLE LOW-9 QUOTATION MARK → COMMA#
 {8219, "'"}, // MA#* ( ‛ → ' ) SINGLE HIGH-REVERSED-9 QUOTATION MARK → APOSTROPHE# →′→
 {8220, "\""}, // MA#* ( “ → '' ) LEFT DOUBLE QUOTATION MARK → APOSTROPHE, APOSTROPHE# →"→# Converted to a quote.
 {8221, "\""}, // MA#* ( ” → '' ) RIGHT DOUBLE QUOTATION MARK → APOSTROPHE, APOSTROPHE# →"→# Converted to a quote.
 {8223, "\""}, // MA#* ( ‟ → '' ) DOUBLE HIGH-REVERSED-9 QUOTATION MARK → APOSTROPHE, APOSTROPHE# →“→→"→# Converted to a quote.
 {8228, "."}, // MA#* ( ․ → . ) ONE DOT LEADER → FULL STOP#
 {8229, ".."}, // MA#* ( ‥ → .. ) TWO DOT LEADER → FULL STOP, FULL STOP#
 {8230, "..."}, // MA#* ( … → ... ) HORIZONTAL ELLIPSIS → FULL STOP, FULL STOP, FULL STOP#
 {8232, " "}, // MA#* ( → ) LINE SEPARATOR → SPACE#
 {8233, " "}, // MA#* ( → ) PARAGRAPH SEPARATOR → SPACE#
 {8239, " "}, // MA#* (   → ) NARROW NO-BREAK SPACE → SPACE#
 {8242, "'"}, // MA#* ( ′ → ' ) PRIME → APOSTROPHE#
 {8243, "\""}, // MA#* ( ″ → '' ) DOUBLE PRIME → APOSTROPHE, APOSTROPHE# →"→# Converted to a quote.
 {8244, "'''"}, // MA#* ( ‴ → ''' ) TRIPLE PRIME → APOSTROPHE, APOSTROPHE, APOSTROPHE# →′′′→
 {8245, "'"}, // MA#* ( ‵ → ' ) REVERSED PRIME → APOSTROPHE# →ʽ→→‘→
 {8246, "\""}, // MA#* ( ‶ → '' ) REVERSED DOUBLE PRIME → APOSTROPHE, APOSTROPHE# →‵‵→# Converted to a quote.
 {8247, "'''"}, // MA#* ( ‷ → ''' ) REVERSED TRIPLE PRIME → APOSTROPHE, APOSTROPHE, APOSTROPHE# →‵‵‵→
 {8249, "<"}, // MA#* ( ‹ → < ) SINGLE LEFT-POINTING ANGLE QUOTATION MARK → LESS-THAN SIGN#
 {8250, ">"}, // MA#* ( › → > ) SINGLE RIGHT-POINTING ANGLE QUOTATION MARK → GREATER-THAN SIGN#
 {8252, "!!"}, // MA#* ( ‼ → !! ) DOUBLE EXCLAMATION MARK → EXCLAMATION MARK, EXCLAMATION MARK#
 {8257, "/"}, // MA#* ( ⁁ → / ) CARET INSERTION POINT → SOLIDUS#
 {8259, "-"}, // MA#* ( ⁃ → - ) HYPHEN BULLET → HYPHEN-MINUS# →‐→
 {8260, "/"}, // MA#* ( ⁄ → / ) FRACTION SLASH → SOLIDUS#
 {8263, "??"}, // MA#* ( ⁇ → ?? ) DOUBLE QUESTION MARK → QUESTION MARK, QUESTION MARK#
 {8264, "?!"}, // MA#* ( ⁈ → ?! ) QUESTION EXCLAMATION MARK → QUESTION MARK, EXCLAMATION MARK#
 {8265, "!?"}, // MA#* ( ⁉ → !? ) EXCLAMATION QUESTION MARK → EXCLAMATION MARK, QUESTION MARK#
 {8270, "*"}, // MA#* ( ⁎ → * ) LOW ASTERISK → ASTERISK#
 {8275, "~"}, // MA#* ( ⁓ → ~ ) SWUNG DASH → TILDE#
 {8279, "''''"}, // MA#* ( ⁗ → '''' ) QUADRUPLE PRIME → APOSTROPHE, APOSTROPHE, APOSTROPHE, APOSTROPHE# →′′′′→
 {8282, ":"}, // MA#* ( ⁚ → : ) TWO DOT PUNCTUATION → COLON#
 {8287, " "}, // MA#* (   → ) MEDIUM MATHEMATICAL SPACE → SPACE#
 {8360, "Rs"}, // MA#* ( ₨ → Rs ) RUPEE SIGN → LATIN CAPITAL LETTER R, LATIN SMALL LETTER S#
 {8374, "lt"}, // MA#* ( ₶ → lt ) LIVRE TOURNOIS SIGN → LATIN SMALL LETTER L, LATIN SMALL LETTER T#
 {8448, "a/c"}, // MA#* ( ℀ → a/c ) ACCOUNT OF → LATIN SMALL LETTER A, SOLIDUS, LATIN SMALL LETTER C#
 {8449, "a/s"}, // MA#* ( ℁ → a/s ) ADDRESSED TO THE SUBJECT → LATIN SMALL LETTER A, SOLIDUS, LATIN SMALL LETTER S#
 {8450, "C"}, // MA# ( ℂ → C ) DOUBLE-STRUCK CAPITAL C → LATIN CAPITAL LETTER C#
 {8453, "c/o"}, // MA#* ( ℅ → c/o ) CARE OF → LATIN SMALL LETTER C, SOLIDUS, LATIN SMALL LETTER O#
 {8454, "c/u"}, // MA#* ( ℆ → c/u ) CADA UNA → LATIN SMALL LETTER C, SOLIDUS, LATIN SMALL LETTER U#
 {8458, "g"}, // MA# ( ℊ → g ) SCRIPT SMALL G → LATIN SMALL LETTER G#
 {8459, "H"}, // MA# ( ℋ → H ) SCRIPT CAPITAL H → LATIN CAPITAL LETTER H#
 {8460, "H"}, // MA# ( ℌ → H ) BLACK-LETTER CAPITAL H → LATIN CAPITAL LETTER H#
 {8461, "H"}, // MA# ( ℍ → H ) DOUBLE-STRUCK CAPITAL H → LATIN CAPITAL LETTER H#
 {8462, "h"}, // MA# ( ℎ → h ) PLANCK CONSTANT → LATIN SMALL LETTER H#
 {8464, "l"}, // MA# ( ℐ → l ) SCRIPT CAPITAL I → LATIN SMALL LETTER L# →I→
 {8465, "l"}, // MA# ( ℑ → l ) BLACK-LETTER CAPITAL I → LATIN SMALL LETTER L# →I→
 {8466, "L"}, // MA# ( ℒ → L ) SCRIPT CAPITAL L → LATIN CAPITAL LETTER L#
 {8467, "l"}, // MA# ( ℓ → l ) SCRIPT SMALL L → LATIN SMALL LETTER L#
 {8469, "N"}, // MA# ( ℕ → N ) DOUBLE-STRUCK CAPITAL N → LATIN CAPITAL LETTER N#
 {8470, "No"}, // MA#* ( № → No ) NUMERO SIGN → LATIN CAPITAL LETTER N, LATIN SMALL LETTER O#
 {8473, "P"}, // MA# ( ℙ → P ) DOUBLE-STRUCK CAPITAL P → LATIN CAPITAL LETTER P#
 {8474, "Q"}, // MA# ( ℚ → Q ) DOUBLE-STRUCK CAPITAL Q → LATIN CAPITAL LETTER Q#
 {8475, "R"}, // MA# ( ℛ → R ) SCRIPT CAPITAL R → LATIN CAPITAL LETTER R#
 {8476, "R"}, // MA# ( ℜ → R ) BLACK-LETTER CAPITAL R → LATIN CAPITAL LETTER R#
 {8477, "R"}, // MA# ( ℝ → R ) DOUBLE-STRUCK CAPITAL R → LATIN CAPITAL LETTER R#
 {8481, "TEL"}, // MA#* ( ℡ → TEL ) TELEPHONE SIGN → LATIN CAPITAL LETTER T, LATIN CAPITAL LETTER E, LATIN CAPITAL LETTER L#
 {8484, "Z"}, // MA# ( ℤ → Z ) DOUBLE-STRUCK CAPITAL Z → LATIN CAPITAL LETTER Z#
 {8488, "Z"}, // MA# ( ℨ → Z ) BLACK-LETTER CAPITAL Z → LATIN CAPITAL LETTER Z#
 {8490, "K"}, // MA# ( K → K ) KELVIN SIGN → LATIN CAPITAL LETTER K#
 {8492, "B"}, // MA# ( ℬ → B ) SCRIPT CAPITAL B → LATIN CAPITAL LETTER B#
 {8493, "C"}, // MA# ( ℭ → C ) BLACK-LETTER CAPITAL C → LATIN CAPITAL LETTER C#
 {8494, "e"}, // MA# ( ℮ → e ) ESTIMATED SYMBOL → LATIN SMALL LETTER E#
 {8495, "e"}, // MA# ( ℯ → e ) SCRIPT SMALL E → LATIN SMALL LETTER E#
 {8496, "E"}, // MA# ( ℰ → E ) SCRIPT CAPITAL E → LATIN CAPITAL LETTER E#
 {8497, "F"}, // MA# ( ℱ → F ) SCRIPT CAPITAL F → LATIN CAPITAL LETTER F#
 {8499, "M"}, // MA# ( ℳ → M ) SCRIPT CAPITAL M → LATIN CAPITAL LETTER M#
 {8500, "o"}, // MA# ( ℴ → o ) SCRIPT SMALL O → LATIN SMALL LETTER O#
 {8505, "i"}, // MA# ( ℹ → i ) INFORMATION SOURCE → LATIN SMALL LETTER I#
 {8507, "FAX"}, // MA#* ( ℻ → FAX ) FACSIMILE SIGN → LATIN CAPITAL LETTER F, LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER X#
 {8509, "y"}, // MA# ( ℽ → y ) DOUBLE-STRUCK SMALL GAMMA → LATIN SMALL LETTER Y# →γ→
 {8517, "D"}, // MA# ( ⅅ → D ) DOUBLE-STRUCK ITALIC CAPITAL D → LATIN CAPITAL LETTER D#
 {8518, "d"}, // MA# ( ⅆ → d ) DOUBLE-STRUCK ITALIC SMALL D → LATIN SMALL LETTER D#
 {8519, "e"}, // MA# ( ⅇ → e ) DOUBLE-STRUCK ITALIC SMALL E → LATIN SMALL LETTER E#
 {8520, "i"}, // MA# ( ⅈ → i ) DOUBLE-STRUCK ITALIC SMALL I → LATIN SMALL LETTER I#
 {8521, "j"}, // MA# ( ⅉ → j ) DOUBLE-STRUCK ITALIC SMALL J → LATIN SMALL LETTER J#
 {8544, "l"}, // MA# ( Ⅰ → l ) ROMAN NUMERAL ONE → LATIN SMALL LETTER L# →Ӏ→
 {8545, "ll"}, // MA# ( Ⅱ → ll ) ROMAN NUMERAL TWO → LATIN SMALL LETTER L, LATIN SMALL LETTER L# →II→
 {8546, "lll"}, // MA# ( Ⅲ → lll ) ROMAN NUMERAL THREE → LATIN SMALL LETTER L, LATIN SMALL LETTER L, LATIN SMALL LETTER L# →III→
 {8547, "lV"}, // MA# ( Ⅳ → lV ) ROMAN NUMERAL FOUR → LATIN SMALL LETTER L, LATIN CAPITAL LETTER V# →IV→
 {8548, "V"}, // MA# ( Ⅴ → V ) ROMAN NUMERAL FIVE → LATIN CAPITAL LETTER V#
 {8549, "Vl"}, // MA# ( Ⅵ → Vl ) ROMAN NUMERAL SIX → LATIN CAPITAL LETTER V, LATIN SMALL LETTER L# →VI→
 {8550, "Vll"}, // MA# ( Ⅶ → Vll ) ROMAN NUMERAL SEVEN → LATIN CAPITAL LETTER V, LATIN SMALL LETTER L, LATIN SMALL LETTER L# →VII→
 {8551, "Vlll"}, // MA# ( Ⅷ → Vlll ) ROMAN NUMERAL EIGHT → LATIN CAPITAL LETTER V, LATIN SMALL LETTER L, LATIN SMALL LETTER L, LATIN SMALL LETTER L# →VIII→
 {8552, "lX"}, // MA# ( Ⅸ → lX ) ROMAN NUMERAL NINE → LATIN SMALL LETTER L, LATIN CAPITAL LETTER X# →IX→
 {8553, "X"}, // MA# ( Ⅹ → X ) ROMAN NUMERAL TEN → LATIN CAPITAL LETTER X#
 {8554, "Xl"}, // MA# ( Ⅺ → Xl ) ROMAN NUMERAL ELEVEN → LATIN CAPITAL LETTER X, LATIN SMALL LETTER L# →XI→
 {8555, "Xll"}, // MA# ( Ⅻ → Xll ) ROMAN NUMERAL TWELVE → LATIN CAPITAL LETTER X, LATIN SMALL LETTER L, LATIN SMALL LETTER L# →XII→
 {8556, "L"}, // MA# ( Ⅼ → L ) ROMAN NUMERAL FIFTY → LATIN CAPITAL LETTER L#
 {8557, "C"}, // MA# ( Ⅽ → C ) ROMAN NUMERAL ONE HUNDRED → LATIN CAPITAL LETTER C#
 {8558, "D"}, // MA# ( Ⅾ → D ) ROMAN NUMERAL FIVE HUNDRED → LATIN CAPITAL LETTER D#
 {8559, "M"}, // MA# ( Ⅿ → M ) ROMAN NUMERAL ONE THOUSAND → LATIN CAPITAL LETTER M#
 {8560, "i"}, // MA# ( ⅰ → i ) SMALL ROMAN NUMERAL ONE → LATIN SMALL LETTER I#
 {8561, "ii"}, // MA# ( ⅱ → ii ) SMALL ROMAN NUMERAL TWO → LATIN SMALL LETTER I, LATIN SMALL LETTER I#
 {8562, "iii"}, // MA# ( ⅲ → iii ) SMALL ROMAN NUMERAL THREE → LATIN SMALL LETTER I, LATIN SMALL LETTER I, LATIN SMALL LETTER I#
 {8563, "iv"}, // MA# ( ⅳ → iv ) SMALL ROMAN NUMERAL FOUR → LATIN SMALL LETTER I, LATIN SMALL LETTER V#
 {8564, "v"}, // MA# ( ⅴ → v ) SMALL ROMAN NUMERAL FIVE → LATIN SMALL LETTER V#
 {8565, "vi"}, // MA# ( ⅵ → vi ) SMALL ROMAN NUMERAL SIX → LATIN SMALL LETTER V, LATIN SMALL LETTER I#
 {8566, "vii"}, // MA# ( ⅶ → vii ) SMALL ROMAN NUMERAL SEVEN → LATIN SMALL LETTER V, LATIN SMALL LETTER I, LATIN SMALL LETTER I#
 {8567, "viii"}, // MA# ( ⅷ → viii ) SMALL ROMAN NUMERAL EIGHT → LATIN SMALL LETTER V, LATIN SMALL LETTER I, LATIN SMALL LETTER I, LATIN SMALL LETTER I#
 {8568, "ix"}, // MA# ( ⅸ → ix ) SMALL ROMAN NUMERAL NINE → LATIN SMALL LETTER I, LATIN SMALL LETTER X#
 {8569, "x"}, // MA# ( ⅹ → x ) SMALL ROMAN NUMERAL TEN → LATIN SMALL LETTER X#
 {8570, "xi"}, // MA# ( ⅺ → xi ) SMALL ROMAN NUMERAL ELEVEN → LATIN SMALL LETTER X, LATIN SMALL LETTER I#
 {8571, "xii"}, // MA# ( ⅻ → xii ) SMALL ROMAN NUMERAL TWELVE → LATIN SMALL LETTER X, LATIN SMALL LETTER I, LATIN SMALL LETTER I#
 {8572, "l"}, // MA# ( ⅼ → l ) SMALL ROMAN NUMERAL FIFTY → LATIN SMALL LETTER L#
 {8573, "c"}, // MA# ( ⅽ → c ) SMALL ROMAN NUMERAL ONE HUNDRED → LATIN SMALL LETTER C#
 {8574, "d"}, // MA# ( ⅾ → d ) SMALL ROMAN NUMERAL FIVE HUNDRED → LATIN SMALL LETTER D#
 {8575, "rn"}, // MA# ( ⅿ → rn ) SMALL ROMAN NUMERAL ONE THOUSAND → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {8722, "-"}, // MA#* ( − → - ) MINUS SIGN → HYPHEN-MINUS#
 {8725, "/"}, // MA#* ( ∕ → / ) DIVISION SLASH → SOLIDUS#
 {8726, "\\"}, // MA#* ( ∖ → \ ) SET MINUS → REVERSE SOLIDUS#
 {8727, "*"}, // MA#* ( ∗ → * ) ASTERISK OPERATOR → ASTERISK#
 {8734, "oo"}, // MA#* ( ∞ → oo ) INFINITY → LATIN SMALL LETTER O, LATIN SMALL LETTER O# →ꝏ→
 {8739, "l"}, // MA#* ( ∣ → l ) DIVIDES → LATIN SMALL LETTER L# →ǀ→
 {8741, "ll"}, // MA#* ( ∥ → ll ) PARALLEL TO → LATIN SMALL LETTER L, LATIN SMALL LETTER L# →||→
 {8744, "v"}, // MA#* ( ∨ → v ) LOGICAL OR → LATIN SMALL LETTER V#
 {8746, "U"}, // MA#* ( ∪ → U ) UNION → LATIN CAPITAL LETTER U# →ᑌ→
 {8758, ":"}, // MA#* ( ∶ → : ) RATIO → COLON#
 {8764, "~"}, // MA#* ( ∼ → ~ ) TILDE OPERATOR → TILDE#
 {8810, "<<"}, // MA#* ( ≪ → << ) MUCH LESS-THAN → LESS-THAN SIGN, LESS-THAN SIGN#
 {8811, ">>"}, // MA#* ( ≫ → >> ) MUCH GREATER-THAN → GREATER-THAN SIGN, GREATER-THAN SIGN#
 {8868, "T"}, // MA#* ( ⊤ → T ) DOWN TACK → LATIN CAPITAL LETTER T#
 {8897, "v"}, // MA#* ( ⋁ → v ) N-ARY LOGICAL OR → LATIN SMALL LETTER V# →∨→
 {8899, "U"}, // MA#* ( ⋃ → U ) N-ARY UNION → LATIN CAPITAL LETTER U# →∪→→ᑌ→
 {8920, "<<<"}, // MA#* ( ⋘ → <<< ) VERY MUCH LESS-THAN → LESS-THAN SIGN, LESS-THAN SIGN, LESS-THAN SIGN#
 {8921, ">>>"}, // MA#* ( ⋙ → >>> ) VERY MUCH GREATER-THAN → GREATER-THAN SIGN, GREATER-THAN SIGN, GREATER-THAN SIGN#
 {8959, "E"}, // MA#* ( ⋿ → E ) Z NOTATION BAG MEMBERSHIP → LATIN CAPITAL LETTER E#
 {9075, "i"}, // MA#* ( ⍳ → i ) APL FUNCTIONAL SYMBOL IOTA → LATIN SMALL LETTER I# →ι→
 {9076, "p"}, // MA#* ( ⍴ → p ) APL FUNCTIONAL SYMBOL RHO → LATIN SMALL LETTER P# →ρ→
 {9082, "a"}, // MA#* ( ⍺ → a ) APL FUNCTIONAL SYMBOL ALPHA → LATIN SMALL LETTER A# →α→
 {9213, "l"}, // MA#* ( ⏽ → l ) POWER ON SYMBOL → LATIN SMALL LETTER L# →I→
 {9290, "\\\\"}, // MA#* ( ⑊ → \\ ) OCR DOUBLE BACKSLASH → REVERSE SOLIDUS, REVERSE SOLIDUS#
 {9332, "(l)"}, // MA#* ( ⑴ → (l) ) PARENTHESIZED DIGIT ONE → LEFT PARENTHESIS, LATIN SMALL LETTER L, RIGHT PARENTHESIS# →(1)→
 {9333, "(2)"}, // MA#* ( ⑵ → (2) ) PARENTHESIZED DIGIT TWO → LEFT PARENTHESIS, DIGIT TWO, RIGHT PARENTHESIS#
 {9334, "(3)"}, // MA#* ( ⑶ → (3) ) PARENTHESIZED DIGIT THREE → LEFT PARENTHESIS, DIGIT THREE, RIGHT PARENTHESIS#
 {9335, "(4)"}, // MA#* ( ⑷ → (4) ) PARENTHESIZED DIGIT FOUR → LEFT PARENTHESIS, DIGIT FOUR, RIGHT PARENTHESIS#
 {9336, "(5)"}, // MA#* ( ⑸ → (5) ) PARENTHESIZED DIGIT FIVE → LEFT PARENTHESIS, DIGIT FIVE, RIGHT PARENTHESIS#
 {9337, "(6)"}, // MA#* ( ⑹ → (6) ) PARENTHESIZED DIGIT SIX → LEFT PARENTHESIS, DIGIT SIX, RIGHT PARENTHESIS#
 {9338, "(7)"}, // MA#* ( ⑺ → (7) ) PARENTHESIZED DIGIT SEVEN → LEFT PARENTHESIS, DIGIT SEVEN, RIGHT PARENTHESIS#
 {9339, "(8)"}, // MA#* ( ⑻ → (8) ) PARENTHESIZED DIGIT EIGHT → LEFT PARENTHESIS, DIGIT EIGHT, RIGHT PARENTHESIS#
 {9340, "(9)"}, // MA#* ( ⑼ → (9) ) PARENTHESIZED DIGIT NINE → LEFT PARENTHESIS, DIGIT NINE, RIGHT PARENTHESIS#
 {9341, "(lO)"}, // MA#* ( ⑽ → (lO) ) PARENTHESIZED NUMBER TEN → LEFT PARENTHESIS, LATIN SMALL LETTER L, LATIN CAPITAL LETTER O, RIGHT PARENTHESIS# →(10)→
 {9342, "(ll)"}, // MA#* ( ⑾ → (ll) ) PARENTHESIZED NUMBER ELEVEN → LEFT PARENTHESIS, LATIN SMALL LETTER L, LATIN SMALL LETTER L, RIGHT PARENTHESIS# →(11)→
 {9343, "(l2)"}, // MA#* ( ⑿ → (l2) ) PARENTHESIZED NUMBER TWELVE → LEFT PARENTHESIS, LATIN SMALL LETTER L, DIGIT TWO, RIGHT PARENTHESIS# →(12)→
 {9344, "(l3)"}, // MA#* ( ⒀ → (l3) ) PARENTHESIZED NUMBER THIRTEEN → LEFT PARENTHESIS, LATIN SMALL LETTER L, DIGIT THREE, RIGHT PARENTHESIS# →(13)→
 {9345, "(l4)"}, // MA#* ( ⒁ → (l4) ) PARENTHESIZED NUMBER FOURTEEN → LEFT PARENTHESIS, LATIN SMALL LETTER L, DIGIT FOUR, RIGHT PARENTHESIS# →(14)→
 {9346, "(l5)"}, // MA#* ( ⒂ → (l5) ) PARENTHESIZED NUMBER FIFTEEN → LEFT PARENTHESIS, LATIN SMALL LETTER L, DIGIT FIVE, RIGHT PARENTHESIS# →(15)→
 {9347, "(l6)"}, // MA#* ( ⒃ → (l6) ) PARENTHESIZED NUMBER SIXTEEN → LEFT PARENTHESIS, LATIN SMALL LETTER L, DIGIT SIX, RIGHT PARENTHESIS# →(16)→
 {9348, "(l7)"}, // MA#* ( ⒄ → (l7) ) PARENTHESIZED NUMBER SEVENTEEN → LEFT PARENTHESIS, LATIN SMALL LETTER L, DIGIT SEVEN, RIGHT PARENTHESIS# →(17)→
 {9349, "(l8)"}, // MA#* ( ⒅ → (l8) ) PARENTHESIZED NUMBER EIGHTEEN → LEFT PARENTHESIS, LATIN SMALL LETTER L, DIGIT EIGHT, RIGHT PARENTHESIS# →(18)→
 {9350, "(l9)"}, // MA#* ( ⒆ → (l9) ) PARENTHESIZED NUMBER NINETEEN → LEFT PARENTHESIS, LATIN SMALL LETTER L, DIGIT NINE, RIGHT PARENTHESIS# →(19)→
 {9351, "(2O)"}, // MA#* ( ⒇ → (2O) ) PARENTHESIZED NUMBER TWENTY → LEFT PARENTHESIS, DIGIT TWO, LATIN CAPITAL LETTER O, RIGHT PARENTHESIS# →(20)→
 {9352, "l."}, // MA#* ( ⒈ → l. ) DIGIT ONE FULL STOP → LATIN SMALL LETTER L, FULL STOP# →1.→
 {9353, "2."}, // MA#* ( ⒉ → 2. ) DIGIT TWO FULL STOP → DIGIT TWO, FULL STOP#
 {9354, "3."}, // MA#* ( ⒊ → 3. ) DIGIT THREE FULL STOP → DIGIT THREE, FULL STOP#
 {9355, "4."}, // MA#* ( ⒋ → 4. ) DIGIT FOUR FULL STOP → DIGIT FOUR, FULL STOP#
 {9356, "5."}, // MA#* ( ⒌ → 5. ) DIGIT FIVE FULL STOP → DIGIT FIVE, FULL STOP#
 {9357, "6."}, // MA#* ( ⒍ → 6. ) DIGIT SIX FULL STOP → DIGIT SIX, FULL STOP#
 {9358, "7."}, // MA#* ( ⒎ → 7. ) DIGIT SEVEN FULL STOP → DIGIT SEVEN, FULL STOP#
 {9359, "8."}, // MA#* ( ⒏ → 8. ) DIGIT EIGHT FULL STOP → DIGIT EIGHT, FULL STOP#
 {9360, "9."}, // MA#* ( ⒐ → 9. ) DIGIT NINE FULL STOP → DIGIT NINE, FULL STOP#
 {9361, "lO."}, // MA#* ( ⒑ → lO. ) NUMBER TEN FULL STOP → LATIN SMALL LETTER L, LATIN CAPITAL LETTER O, FULL STOP# →10.→
 {9362, "ll."}, // MA#* ( ⒒ → ll. ) NUMBER ELEVEN FULL STOP → LATIN SMALL LETTER L, LATIN SMALL LETTER L, FULL STOP# →11.→
 {9363, "l2."}, // MA#* ( ⒓ → l2. ) NUMBER TWELVE FULL STOP → LATIN SMALL LETTER L, DIGIT TWO, FULL STOP# →12.→
 {9364, "l3."}, // MA#* ( ⒔ → l3. ) NUMBER THIRTEEN FULL STOP → LATIN SMALL LETTER L, DIGIT THREE, FULL STOP# →13.→
 {9365, "l4."}, // MA#* ( ⒕ → l4. ) NUMBER FOURTEEN FULL STOP → LATIN SMALL LETTER L, DIGIT FOUR, FULL STOP# →14.→
 {9366, "l5."}, // MA#* ( ⒖ → l5. ) NUMBER FIFTEEN FULL STOP → LATIN SMALL LETTER L, DIGIT FIVE, FULL STOP# →15.→
 {9367, "l6."}, // MA#* ( ⒗ → l6. ) NUMBER SIXTEEN FULL STOP → LATIN SMALL LETTER L, DIGIT SIX, FULL STOP# →16.→
 {9368, "l7."}, // MA#* ( ⒘ → l7. ) NUMBER SEVENTEEN FULL STOP → LATIN SMALL LETTER L, DIGIT SEVEN, FULL STOP# →17.→
 {9369, "l8."}, // MA#* ( ⒙ → l8. ) NUMBER EIGHTEEN FULL STOP → LATIN SMALL LETTER L, DIGIT EIGHT, FULL STOP# →18.→
 {9370, "l9."}, // MA#* ( ⒚ → l9. ) NUMBER NINETEEN FULL STOP → LATIN SMALL LETTER L, DIGIT NINE, FULL STOP# →19.→
 {9371, "2O."}, // MA#* ( ⒛ → 2O. ) NUMBER TWENTY FULL STOP → DIGIT TWO, LATIN CAPITAL LETTER O, FULL STOP# →20.→
 {9372, "(a)"}, // MA#* ( ⒜ → (a) ) PARENTHESIZED LATIN SMALL LETTER A → LEFT PARENTHESIS, LATIN SMALL LETTER A, RIGHT PARENTHESIS#
 {9373, "(b)"}, // MA#* ( ⒝ → (b) ) PARENTHESIZED LATIN SMALL LETTER B → LEFT PARENTHESIS, LATIN SMALL LETTER B, RIGHT PARENTHESIS#
 {9374, "(c)"}, // MA#* ( ⒞ → (c) ) PARENTHESIZED LATIN SMALL LETTER C → LEFT PARENTHESIS, LATIN SMALL LETTER C, RIGHT PARENTHESIS#
 {9375, "(d)"}, // MA#* ( ⒟ → (d) ) PARENTHESIZED LATIN SMALL LETTER D → LEFT PARENTHESIS, LATIN SMALL LETTER D, RIGHT PARENTHESIS#
 {9376, "(e)"}, // MA#* ( ⒠ → (e) ) PARENTHESIZED LATIN SMALL LETTER E → LEFT PARENTHESIS, LATIN SMALL LETTER E, RIGHT PARENTHESIS#
 {9377, "(f)"}, // MA#* ( ⒡ → (f) ) PARENTHESIZED LATIN SMALL LETTER F → LEFT PARENTHESIS, LATIN SMALL LETTER F, RIGHT PARENTHESIS#
 {9378, "(g)"}, // MA#* ( ⒢ → (g) ) PARENTHESIZED LATIN SMALL LETTER G → LEFT PARENTHESIS, LATIN SMALL LETTER G, RIGHT PARENTHESIS#
 {9379, "(h)"}, // MA#* ( ⒣ → (h) ) PARENTHESIZED LATIN SMALL LETTER H → LEFT PARENTHESIS, LATIN SMALL LETTER H, RIGHT PARENTHESIS#
 {9380, "(i)"}, // MA#* ( ⒤ → (i) ) PARENTHESIZED LATIN SMALL LETTER I → LEFT PARENTHESIS, LATIN SMALL LETTER I, RIGHT PARENTHESIS#
 {9381, "(j)"}, // MA#* ( ⒥ → (j) ) PARENTHESIZED LATIN SMALL LETTER J → LEFT PARENTHESIS, LATIN SMALL LETTER J, RIGHT PARENTHESIS#
 {9382, "(k)"}, // MA#* ( ⒦ → (k) ) PARENTHESIZED LATIN SMALL LETTER K → LEFT PARENTHESIS, LATIN SMALL LETTER K, RIGHT PARENTHESIS#
 {9383, "(l)"}, // MA#* ( ⒧ → (l) ) PARENTHESIZED LATIN SMALL LETTER L → LEFT PARENTHESIS, LATIN SMALL LETTER L, RIGHT PARENTHESIS#
 {9384, "(rn)"}, // MA#* ( ⒨ → (rn) ) PARENTHESIZED LATIN SMALL LETTER M → LEFT PARENTHESIS, LATIN SMALL LETTER R, LATIN SMALL LETTER N, RIGHT PARENTHESIS# →(m)→
 {9385, "(n)"}, // MA#* ( ⒩ → (n) ) PARENTHESIZED LATIN SMALL LETTER N → LEFT PARENTHESIS, LATIN SMALL LETTER N, RIGHT PARENTHESIS#
 {9386, "(o)"}, // MA#* ( ⒪ → (o) ) PARENTHESIZED LATIN SMALL LETTER O → LEFT PARENTHESIS, LATIN SMALL LETTER O, RIGHT PARENTHESIS#
 {9387, "(p)"}, // MA#* ( ⒫ → (p) ) PARENTHESIZED LATIN SMALL LETTER P → LEFT PARENTHESIS, LATIN SMALL LETTER P, RIGHT PARENTHESIS#
 {9388, "(q)"}, // MA#* ( ⒬ → (q) ) PARENTHESIZED LATIN SMALL LETTER Q → LEFT PARENTHESIS, LATIN SMALL LETTER Q, RIGHT PARENTHESIS#
 {9389, "(r)"}, // MA#* ( ⒭ → (r) ) PARENTHESIZED LATIN SMALL LETTER R → LEFT PARENTHESIS, LATIN SMALL LETTER R, RIGHT PARENTHESIS#
 {9390, "(s)"}, // MA#* ( ⒮ → (s) ) PARENTHESIZED LATIN SMALL LETTER S → LEFT PARENTHESIS, LATIN SMALL LETTER S, RIGHT PARENTHESIS#
 {9391, "(t)"}, // MA#* ( ⒯ → (t) ) PARENTHESIZED LATIN SMALL LETTER T → LEFT PARENTHESIS, LATIN SMALL LETTER T, RIGHT PARENTHESIS#
 {9392, "(u)"}, // MA#* ( ⒰ → (u) ) PARENTHESIZED LATIN SMALL LETTER U → LEFT PARENTHESIS, LATIN SMALL LETTER U, RIGHT PARENTHESIS#
 {9393, "(v)"}, // MA#* ( ⒱ → (v) ) PARENTHESIZED LATIN SMALL LETTER V → LEFT PARENTHESIS, LATIN SMALL LETTER V, RIGHT PARENTHESIS#
 {9394, "(w)"}, // MA#* ( ⒲ → (w) ) PARENTHESIZED LATIN SMALL LETTER W → LEFT PARENTHESIS, LATIN SMALL LETTER W, RIGHT PARENTHESIS#
 {9395, "(x)"}, // MA#* ( ⒳ → (x) ) PARENTHESIZED LATIN SMALL LETTER X → LEFT PARENTHESIS, LATIN SMALL LETTER X, RIGHT PARENTHESIS#
 {9396, "(y)"}, // MA#* ( ⒴ → (y) ) PARENTHESIZED LATIN SMALL LETTER Y → LEFT PARENTHESIS, LATIN SMALL LETTER Y, RIGHT PARENTHESIS#
 {9397, "(z)"}, // MA#* ( ⒵ → (z) ) PARENTHESIZED LATIN SMALL LETTER Z → LEFT PARENTHESIS, LATIN SMALL LETTER Z, RIGHT PARENTHESIS#
 {9585, "/"}, // MA#* ( ╱ → / ) BOX DRAWINGS LIGHT DIAGONAL UPPER RIGHT TO LOWER LEFT → SOLIDUS#
 {9587, "X"}, // MA#* ( ╳ → X ) BOX DRAWINGS LIGHT DIAGONAL CROSS → LATIN CAPITAL LETTER X#
 {10088, "("}, // MA#* ( ❨ → ( ) MEDIUM LEFT PARENTHESIS ORNAMENT → LEFT PARENTHESIS#
 {10089, ")"}, // MA#* ( ❩ → ) ) MEDIUM RIGHT PARENTHESIS ORNAMENT → RIGHT PARENTHESIS#
 {10094, "<"}, // MA#* ( ❮ → < ) HEAVY LEFT-POINTING ANGLE QUOTATION MARK ORNAMENT → LESS-THAN SIGN# →‹→
 {10095, ">"}, // MA#* ( ❯ → > ) HEAVY RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT → GREATER-THAN SIGN# →›→
 {10098, "("}, // MA#* ( ❲ → ( ) LIGHT LEFT TORTOISE SHELL BRACKET ORNAMENT → LEFT PARENTHESIS# →〔→
 {10099, ")"}, // MA#* ( ❳ → ) ) LIGHT RIGHT TORTOISE SHELL BRACKET ORNAMENT → RIGHT PARENTHESIS# →〕→
 {10100, "{"}, // MA#* ( ❴ → { ) MEDIUM LEFT CURLY BRACKET ORNAMENT → LEFT CURLY BRACKET#
 {10101, "}"}, // MA#* ( ❵ → } ) MEDIUM RIGHT CURLY BRACKET ORNAMENT → RIGHT CURLY BRACKET#
 {10133, "+"}, // MA#* ( ➕ → + ) HEAVY PLUS SIGN → PLUS SIGN#
 {10134, "-"}, // MA#* ( ➖ → - ) HEAVY MINUS SIGN → HYPHEN-MINUS# →−→
 {10187, "/"}, // MA#* ( ⟋ → / ) MATHEMATICAL RISING DIAGONAL → SOLIDUS#
 {10189, "\\"}, // MA#* ( ⟍ → \ ) MATHEMATICAL FALLING DIAGONAL → REVERSE SOLIDUS#
 {10201, "T"}, // MA#* ( ⟙ → T ) LARGE DOWN TACK → LATIN CAPITAL LETTER T#
 {10539, "x"}, // MA#* ( ⤫ → x ) RISING DIAGONAL CROSSING FALLING DIAGONAL → LATIN SMALL LETTER X#
 {10540, "x"}, // MA#* ( ⤬ → x ) FALLING DIAGONAL CROSSING RISING DIAGONAL → LATIN SMALL LETTER X#
 {10741, "\\"}, // MA#* ( ⧵ → \ ) REVERSE SOLIDUS OPERATOR → REVERSE SOLIDUS#
 {10744, "/"}, // MA#* ( ⧸ → / ) BIG SOLIDUS → SOLIDUS#
 {10745, "\\"}, // MA#* ( ⧹ → \ ) BIG REVERSE SOLIDUS → REVERSE SOLIDUS#
 {10784, ">>"}, // MA#* ( ⨠ → >> ) Z NOTATION SCHEMA PIPING → GREATER-THAN SIGN, GREATER-THAN SIGN# →≫→
 {10799, "x"}, // MA#* ( ⨯ → x ) VECTOR OR CROSS PRODUCT → LATIN SMALL LETTER X# →×→
 {10868, "::="}, // MA#* ( ⩴ → ::= ) DOUBLE COLON EQUAL → COLON, COLON, EQUALS SIGN#
 {10869, "=="}, // MA#* ( ⩵ → == ) TWO CONSECUTIVE EQUALS SIGNS → EQUALS SIGN, EQUALS SIGN#
 {10870, "==="}, // MA#* ( ⩶ → === ) THREE CONSECUTIVE EQUALS SIGNS → EQUALS SIGN, EQUALS SIGN, EQUALS SIGN#
 {10917, "><"}, // MA#* ( ⪥ → >< ) GREATER-THAN BESIDE LESS-THAN → GREATER-THAN SIGN, LESS-THAN SIGN#
 {11003, "///"}, // MA#* ( ⫻ → /// ) TRIPLE SOLIDUS BINARY RELATION → SOLIDUS, SOLIDUS, SOLIDUS#
 {11005, "//"}, // MA#* ( ⫽ → // ) DOUBLE SOLIDUS OPERATOR → SOLIDUS, SOLIDUS#
 {11397, "r"}, // MA# ( ⲅ → r ) COPTIC SMALL LETTER GAMMA → LATIN SMALL LETTER R# →г→
 {11406, "H"}, // MA# ( Ⲏ → H ) COPTIC CAPITAL LETTER HATE → LATIN CAPITAL LETTER H# →Η→
 {11410, "l"}, // MA# ( Ⲓ → l ) COPTIC CAPITAL LETTER IAUDA → LATIN SMALL LETTER L# →Ӏ→
 {11412, "K"}, // MA# ( Ⲕ → K ) COPTIC CAPITAL LETTER KAPA → LATIN CAPITAL LETTER K# →Κ→
 {11416, "M"}, // MA# ( Ⲙ → M ) COPTIC CAPITAL LETTER MI → LATIN CAPITAL LETTER M#
 {11418, "N"}, // MA# ( Ⲛ → N ) COPTIC CAPITAL LETTER NI → LATIN CAPITAL LETTER N#
 {11422, "O"}, // MA# ( Ⲟ → O ) COPTIC CAPITAL LETTER O → LATIN CAPITAL LETTER O#
 {11423, "o"}, // MA# ( ⲟ → o ) COPTIC SMALL LETTER O → LATIN SMALL LETTER O#
 {11426, "P"}, // MA# ( Ⲣ → P ) COPTIC CAPITAL LETTER RO → LATIN CAPITAL LETTER P#
 {11427, "p"}, // MA# ( ⲣ → p ) COPTIC SMALL LETTER RO → LATIN SMALL LETTER P# →ρ→
 {11428, "C"}, // MA# ( Ⲥ → C ) COPTIC CAPITAL LETTER SIMA → LATIN CAPITAL LETTER C# →Ϲ→
 {11429, "c"}, // MA# ( ⲥ → c ) COPTIC SMALL LETTER SIMA → LATIN SMALL LETTER C# →ϲ→
 {11430, "T"}, // MA# ( Ⲧ → T ) COPTIC CAPITAL LETTER TAU → LATIN CAPITAL LETTER T#
 {11432, "Y"}, // MA# ( Ⲩ → Y ) COPTIC CAPITAL LETTER UA → LATIN CAPITAL LETTER Y#
 {11436, "X"}, // MA# ( Ⲭ → X ) COPTIC CAPITAL LETTER KHI → LATIN CAPITAL LETTER X# →Х→
 {11450, "-"}, // MA# ( Ⲻ → - ) COPTIC CAPITAL LETTER DIALECT-P NI → HYPHEN-MINUS# →‒→
 {11462, "/"}, // MA# ( Ⳇ → / ) COPTIC CAPITAL LETTER OLD COPTIC ESH → SOLIDUS#
 {11466, "9"}, // MA# ( Ⳋ → 9 ) COPTIC CAPITAL LETTER DIALECT-P HORI → DIGIT NINE#
 {11468, "3"}, // MA# ( Ⳍ → 3 ) COPTIC CAPITAL LETTER OLD COPTIC HORI → DIGIT THREE# →Ȝ→→Ʒ→
 {11472, "L"}, // MA# ( Ⳑ → L ) COPTIC CAPITAL LETTER L-SHAPED HA → LATIN CAPITAL LETTER L#
 {11474, "6"}, // MA# ( Ⳓ → 6 ) COPTIC CAPITAL LETTER OLD COPTIC HEI → DIGIT SIX#
 {11513, "\\\\"}, // MA#* ( ⳹ → \\ ) COPTIC OLD NUBIAN FULL STOP → REVERSE SOLIDUS, REVERSE SOLIDUS#
 {11576, "V"}, // MA# ( ⴸ → V ) TIFINAGH LETTER YADH → LATIN CAPITAL LETTER V#
 {11577, "E"}, // MA# ( ⴹ → E ) TIFINAGH LETTER YADD → LATIN CAPITAL LETTER E#
 {11599, "l"}, // MA# ( ⵏ → l ) TIFINAGH LETTER YAN → LATIN SMALL LETTER L# →Ӏ→
 {11601, "!"}, // MA# ( ⵑ → ! ) TIFINAGH LETTER TUAREG YANG → EXCLAMATION MARK#
 {11604, "O"}, // MA# ( ⵔ → O ) TIFINAGH LETTER YAR → LATIN CAPITAL LETTER O#
 {11605, "Q"}, // MA# ( ⵕ → Q ) TIFINAGH LETTER YARR → LATIN CAPITAL LETTER Q#
 {11613, "X"}, // MA# ( ⵝ → X ) TIFINAGH LETTER YATH → LATIN CAPITAL LETTER X#
 {11816, "(("}, // MA#* ( ⸨ → (( ) LEFT DOUBLE PARENTHESIS → LEFT PARENTHESIS, LEFT PARENTHESIS#
 {11817, "))"}, // MA#* ( ⸩ → )) ) RIGHT DOUBLE PARENTHESIS → RIGHT PARENTHESIS, RIGHT PARENTHESIS#
 {11840, "="}, // MA#* ( ⹀ → = ) DOUBLE HYPHEN → EQUALS SIGN#
 {12034, "\\"}, // MA#* ( ⼂ → \ ) KANGXI RADICAL DOT → REVERSE SOLIDUS#
 {12035, "/"}, // MA#* ( ⼃ → / ) KANGXI RADICAL SLASH → SOLIDUS#
 {12291, "\""}, // MA#* ( 〃 → '' ) DITTO MARK → APOSTROPHE, APOSTROPHE# →″→→"→# Converted to a quote.
 {12295, "O"}, // MA# ( 〇 → O ) IDEOGRAPHIC NUMBER ZERO → LATIN CAPITAL LETTER O#
 {12308, "("}, // MA#* ( 〔 → ( ) LEFT TORTOISE SHELL BRACKET → LEFT PARENTHESIS#
 {12309, ")"}, // MA#* ( 〕 → ) ) RIGHT TORTOISE SHELL BRACKET → RIGHT PARENTHESIS#
 {12339, "/"}, // MA# ( 〳 → / ) VERTICAL KANA REPEAT MARK UPPER HALF → SOLIDUS#
 {12448, "="}, // MA#* ( ゠ → = ) KATAKANA-HIRAGANA DOUBLE HYPHEN → EQUALS SIGN#
 {12494, "/"}, // MA# ( ノ → / ) KATAKANA LETTER NO → SOLIDUS# →⼃→
 {12755, "/"}, // MA#* ( ㇓ → / ) CJK STROKE SP → SOLIDUS# →⼃→
 {12756, "\\"}, // MA#* ( ㇔ → \ ) CJK STROKE D → REVERSE SOLIDUS# →⼂→
 {20022, "\\"}, // MA# ( 丶 → \ ) CJK UNIFIED IDEOGRAPH-4E36 → REVERSE SOLIDUS# →⼂→
 {20031, "/"}, // MA# ( 丿 → / ) CJK UNIFIED IDEOGRAPH-4E3F → SOLIDUS# →⼃→
 {42192, "B"}, // MA# ( ꓐ → B ) LISU LETTER BA → LATIN CAPITAL LETTER B#
 {42193, "P"}, // MA# ( ꓑ → P ) LISU LETTER PA → LATIN CAPITAL LETTER P#
 {42194, "d"}, // MA# ( ꓒ → d ) LISU LETTER PHA → LATIN SMALL LETTER D#
 {42195, "D"}, // MA# ( ꓓ → D ) LISU LETTER DA → LATIN CAPITAL LETTER D#
 {42196, "T"}, // MA# ( ꓔ → T ) LISU LETTER TA → LATIN CAPITAL LETTER T#
 {42198, "G"}, // MA# ( ꓖ → G ) LISU LETTER GA → LATIN CAPITAL LETTER G#
 {42199, "K"}, // MA# ( ꓗ → K ) LISU LETTER KA → LATIN CAPITAL LETTER K#
 {42201, "J"}, // MA# ( ꓙ → J ) LISU LETTER JA → LATIN CAPITAL LETTER J#
 {42202, "C"}, // MA# ( ꓚ → C ) LISU LETTER CA → LATIN CAPITAL LETTER C#
 {42204, "Z"}, // MA# ( ꓜ → Z ) LISU LETTER DZA → LATIN CAPITAL LETTER Z#
 {42205, "F"}, // MA# ( ꓝ → F ) LISU LETTER TSA → LATIN CAPITAL LETTER F#
 {42207, "M"}, // MA# ( ꓟ → M ) LISU LETTER MA → LATIN CAPITAL LETTER M#
 {42208, "N"}, // MA# ( ꓠ → N ) LISU LETTER NA → LATIN CAPITAL LETTER N#
 {42209, "L"}, // MA# ( ꓡ → L ) LISU LETTER LA → LATIN CAPITAL LETTER L#
 {42210, "S"}, // MA# ( ꓢ → S ) LISU LETTER SA → LATIN CAPITAL LETTER S#
 {42211, "R"}, // MA# ( ꓣ → R ) LISU LETTER ZHA → LATIN CAPITAL LETTER R#
 {42214, "V"}, // MA# ( ꓦ → V ) LISU LETTER HA → LATIN CAPITAL LETTER V#
 {42215, "H"}, // MA# ( ꓧ → H ) LISU LETTER XA → LATIN CAPITAL LETTER H#
 {42218, "W"}, // MA# ( ꓪ → W ) LISU LETTER WA → LATIN CAPITAL LETTER W#
 {42219, "X"}, // MA# ( ꓫ → X ) LISU LETTER SHA → LATIN CAPITAL LETTER X#
 {42220, "Y"}, // MA# ( ꓬ → Y ) LISU LETTER YA → LATIN CAPITAL LETTER Y#
 {42222, "A"}, // MA# ( ꓮ → A ) LISU LETTER A → LATIN CAPITAL LETTER A#
 {42224, "E"}, // MA# ( ꓰ → E ) LISU LETTER E → LATIN CAPITAL LETTER E#
 {42226, "l"}, // MA# ( ꓲ → l ) LISU LETTER I → LATIN SMALL LETTER L# →I→
 {42227, "O"}, // MA# ( ꓳ → O ) LISU LETTER O → LATIN CAPITAL LETTER O#
 {42228, "U"}, // MA# ( ꓴ → U ) LISU LETTER U → LATIN CAPITAL LETTER U#
 {42232, "."}, // MA# ( ꓸ → . ) LISU LETTER TONE MYA TI → FULL STOP#
 {42233, ","}, // MA# ( ꓹ → , ) LISU LETTER TONE NA PO → COMMA#
 {42234, ".."}, // MA# ( ꓺ → .. ) LISU LETTER TONE MYA CYA → FULL STOP, FULL STOP#
 {42235, ".,"}, // MA# ( ꓻ → ., ) LISU LETTER TONE MYA BO → FULL STOP, COMMA#
 {42237, ":"}, // MA# ( ꓽ → : ) LISU LETTER TONE MYA JEU → COLON#
 {42238, "-."}, // MA#* ( ꓾ → -. ) LISU PUNCTUATION COMMA → HYPHEN-MINUS, FULL STOP#
 {42239, "="}, // MA#* ( ꓿ → = ) LISU PUNCTUATION FULL STOP → EQUALS SIGN#
 {42510, "."}, // MA#* ( ꘎ → . ) VAI FULL STOP → FULL STOP#
 {42564, "2"}, // MA# ( Ꙅ → 2 ) CYRILLIC CAPITAL LETTER REVERSED DZE → DIGIT TWO# →Ƨ→
 {42567, "i"}, // MA# ( ꙇ → i ) CYRILLIC SMALL LETTER IOTA → LATIN SMALL LETTER I# →ι→
 {42648, "OO"}, // MA# ( Ꚙ → OO ) CYRILLIC CAPITAL LETTER DOUBLE O → LATIN CAPITAL LETTER O, LATIN CAPITAL LETTER O#
 {42649, "oo"}, // MA# ( ꚙ → oo ) CYRILLIC SMALL LETTER DOUBLE O → LATIN SMALL LETTER O, LATIN SMALL LETTER O#
 {42719, "V"}, // MA# ( ꛟ → V ) BAMUM LETTER KO → LATIN CAPITAL LETTER V#
 {42731, "?"}, // MA# ( ꛫ → ? ) BAMUM LETTER NTUU → QUESTION MARK# →ʔ→
 {42735, "2"}, // MA# ( ꛯ → 2 ) BAMUM LETTER KOGHOM → DIGIT TWO# →Ƨ→
 {42792, "T3"}, // MA# ( Ꜩ → T3 ) LATIN CAPITAL LETTER TZ → LATIN CAPITAL LETTER T, DIGIT THREE# →TƷ→
 {42801, "s"}, // MA# ( ꜱ → s ) LATIN LETTER SMALL CAPITAL S → LATIN SMALL LETTER S#
 {42802, "AA"}, // MA# ( Ꜳ → AA ) LATIN CAPITAL LETTER AA → LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER A#
 {42803, "aa"}, // MA# ( ꜳ → aa ) LATIN SMALL LETTER AA → LATIN SMALL LETTER A, LATIN SMALL LETTER A#
 {42804, "AO"}, // MA# ( Ꜵ → AO ) LATIN CAPITAL LETTER AO → LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER O#
 {42805, "ao"}, // MA# ( ꜵ → ao ) LATIN SMALL LETTER AO → LATIN SMALL LETTER A, LATIN SMALL LETTER O#
 {42806, "AU"}, // MA# ( Ꜷ → AU ) LATIN CAPITAL LETTER AU → LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER U#
 {42807, "au"}, // MA# ( ꜷ → au ) LATIN SMALL LETTER AU → LATIN SMALL LETTER A, LATIN SMALL LETTER U#
 {42808, "AV"}, // MA# ( Ꜹ → AV ) LATIN CAPITAL LETTER AV → LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER V#
 {42809, "av"}, // MA# ( ꜹ → av ) LATIN SMALL LETTER AV → LATIN SMALL LETTER A, LATIN SMALL LETTER V#
 {42810, "AV"}, // MA# ( Ꜻ → AV ) LATIN CAPITAL LETTER AV WITH HORIZONTAL BAR → LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER V#
 {42811, "av"}, // MA# ( ꜻ → av ) LATIN SMALL LETTER AV WITH HORIZONTAL BAR → LATIN SMALL LETTER A, LATIN SMALL LETTER V#
 {42812, "AY"}, // MA# ( Ꜽ → AY ) LATIN CAPITAL LETTER AY → LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER Y#
 {42813, "ay"}, // MA# ( ꜽ → ay ) LATIN SMALL LETTER AY → LATIN SMALL LETTER A, LATIN SMALL LETTER Y#
 {42830, "OO"}, // MA# ( Ꝏ → OO ) LATIN CAPITAL LETTER OO → LATIN CAPITAL LETTER O, LATIN CAPITAL LETTER O#
 {42831, "oo"}, // MA# ( ꝏ → oo ) LATIN SMALL LETTER OO → LATIN SMALL LETTER O, LATIN SMALL LETTER O#
 {42842, "2"}, // MA# ( Ꝛ → 2 ) LATIN CAPITAL LETTER R ROTUNDA → DIGIT TWO#
 {42858, "3"}, // MA# ( Ꝫ → 3 ) LATIN CAPITAL LETTER ET → DIGIT THREE#
 {42862, "9"}, // MA# ( Ꝯ → 9 ) LATIN CAPITAL LETTER CON → DIGIT NINE#
 {42871, "tf"}, // MA# ( ꝷ → tf ) LATIN SMALL LETTER TUM → LATIN SMALL LETTER T, LATIN SMALL LETTER F#
 {42872, "&"}, // MA# ( ꝸ → & ) LATIN SMALL LETTER UM → AMPERSAND#
 {42889, ":"}, // MA#* ( ꞉ → : ) MODIFIER LETTER COLON → COLON#
 {42892, "'"}, // MA# ( ꞌ → ' ) LATIN SMALL LETTER SALTILLO → APOSTROPHE#
 {42904, "F"}, // MA# ( Ꞙ → F ) LATIN CAPITAL LETTER F WITH STROKE → LATIN CAPITAL LETTER F#
 {42905, "f"}, // MA# ( ꞙ → f ) LATIN SMALL LETTER F WITH STROKE → LATIN SMALL LETTER F#
 {42911, "u"}, // MA# ( ꞟ → u ) LATIN SMALL LETTER VOLAPUK UE → LATIN SMALL LETTER U#
 {42923, "3"}, // MA# ( Ɜ → 3 ) LATIN CAPITAL LETTER REVERSED OPEN E → DIGIT THREE#
 {42930, "J"}, // MA# ( Ʝ → J ) LATIN CAPITAL LETTER J WITH CROSSED-TAIL → LATIN CAPITAL LETTER J#
 {42931, "X"}, // MA# ( Ꭓ → X ) LATIN CAPITAL LETTER CHI → LATIN CAPITAL LETTER X#
 {42932, "B"}, // MA# ( Ꞵ → B ) LATIN CAPITAL LETTER BETA → LATIN CAPITAL LETTER B#
 {43826, "e"}, // MA# ( ꬲ → e ) LATIN SMALL LETTER BLACKLETTER E → LATIN SMALL LETTER E#
 {43829, "f"}, // MA# ( ꬵ → f ) LATIN SMALL LETTER LENIS F → LATIN SMALL LETTER F#
 {43837, "o"}, // MA# ( ꬽ → o ) LATIN SMALL LETTER BLACKLETTER O → LATIN SMALL LETTER O#
 {43847, "r"}, // MA# ( ꭇ → r ) LATIN SMALL LETTER R WITHOUT HANDLE → LATIN SMALL LETTER R#
 {43848, "r"}, // MA# ( ꭈ → r ) LATIN SMALL LETTER DOUBLE R → LATIN SMALL LETTER R#
 {43854, "u"}, // MA# ( ꭎ → u ) LATIN SMALL LETTER U WITH SHORT RIGHT LEG → LATIN SMALL LETTER U#
 {43858, "u"}, // MA# ( ꭒ → u ) LATIN SMALL LETTER U WITH LEFT HOOK → LATIN SMALL LETTER U#
 {43866, "y"}, // MA# ( ꭚ → y ) LATIN SMALL LETTER Y WITH SHORT RIGHT LEG → LATIN SMALL LETTER Y#
 {43875, "uo"}, // MA# ( ꭣ → uo ) LATIN SMALL LETTER UO → LATIN SMALL LETTER U, LATIN SMALL LETTER O#
 {43893, "i"}, // MA# ( ꭵ → i ) CHEROKEE SMALL LETTER V → LATIN SMALL LETTER I#
 {43905, "r"}, // MA# ( ꮁ → r ) CHEROKEE SMALL LETTER HU → LATIN SMALL LETTER R# →ᴦ→→г→
 {43907, "w"}, // MA# ( ꮃ → w ) CHEROKEE SMALL LETTER LA → LATIN SMALL LETTER W# →ᴡ→
 {43923, "z"}, // MA# ( ꮓ → z ) CHEROKEE SMALL LETTER NO → LATIN SMALL LETTER Z# →ᴢ→
 {43945, "v"}, // MA# ( ꮩ → v ) CHEROKEE SMALL LETTER DO → LATIN SMALL LETTER V# →ᴠ→
 {43946, "s"}, // MA# ( ꮪ → s ) CHEROKEE SMALL LETTER DU → LATIN SMALL LETTER S# →ꜱ→
 {43951, "c"}, // MA# ( ꮯ → c ) CHEROKEE SMALL LETTER TLI → LATIN SMALL LETTER C# →ᴄ→
 {64256, "ff"}, // MA# ( ﬀ → ff ) LATIN SMALL LIGATURE FF → LATIN SMALL LETTER F, LATIN SMALL LETTER F#
 {64257, "fi"}, // MA# ( ﬁ → fi ) LATIN SMALL LIGATURE FI → LATIN SMALL LETTER F, LATIN SMALL LETTER I#
 {64258, "fl"}, // MA# ( ﬂ → fl ) LATIN SMALL LIGATURE FL → LATIN SMALL LETTER F, LATIN SMALL LETTER L#
 {64259, "ffi"}, // MA# ( ﬃ → ffi ) LATIN SMALL LIGATURE FFI → LATIN SMALL LETTER F, LATIN SMALL LETTER F, LATIN SMALL LETTER I#
 {64260, "ffl"}, // MA# ( ﬄ → ffl ) LATIN SMALL LIGATURE FFL → LATIN SMALL LETTER F, LATIN SMALL LETTER F, LATIN SMALL LETTER L#
 {64262, "st"}, // MA# ( ﬆ → st ) LATIN SMALL LIGATURE ST → LATIN SMALL LETTER S, LATIN SMALL LETTER T#
 {64422, "o"}, // MA# ( ‎ﮦ‎ → o ) ARABIC LETTER HEH GOAL ISOLATED FORM → LATIN SMALL LETTER O# →‎ه‎→
 {64423, "o"}, // MA# ( ‎ﮧ‎ → o ) ARABIC LETTER HEH GOAL FINAL FORM → LATIN SMALL LETTER O# →‎ہ‎→→‎ه‎→
 {64424, "o"}, // MA# ( ‎ﮨ‎ → o ) ARABIC LETTER HEH GOAL INITIAL FORM → LATIN SMALL LETTER O# →‎ہ‎→→‎ه‎→
 {64425, "o"}, // MA# ( ‎ﮩ‎ → o ) ARABIC LETTER HEH GOAL MEDIAL FORM → LATIN SMALL LETTER O# →‎ہ‎→→‎ه‎→
 {64426, "o"}, // MA# ( ‎ﮪ‎ → o ) ARABIC LETTER HEH DOACHASHMEE ISOLATED FORM → LATIN SMALL LETTER O# →‎ه‎→
 {64427, "o"}, // MA# ( ‎ﮫ‎ → o ) ARABIC LETTER HEH DOACHASHMEE FINAL FORM → LATIN SMALL LETTER O# →‎ﻪ‎→→‎ه‎→
 {64428, "o"}, // MA# ( ‎ﮬ‎ → o ) ARABIC LETTER HEH DOACHASHMEE INITIAL FORM → LATIN SMALL LETTER O# →‎ﻫ‎→→‎ه‎→
 {64429, "o"}, // MA# ( ‎ﮭ‎ → o ) ARABIC LETTER HEH DOACHASHMEE MEDIAL FORM → LATIN SMALL LETTER O# →‎ﻬ‎→→‎ه‎→
 {64830, "("}, // MA#* ( ﴾ → ( ) ORNATE LEFT PARENTHESIS → LEFT PARENTHESIS#
 {64831, ")"}, // MA#* ( ﴿ → ) ) ORNATE RIGHT PARENTHESIS → RIGHT PARENTHESIS#
 {65072, ":"}, // MA#* ( ︰ → : ) PRESENTATION FORM FOR VERTICAL TWO DOT LEADER → COLON#
 {65101, "_"}, // MA# ( ﹍ → _ ) DASHED LOW LINE → LOW LINE#
 {65102, "_"}, // MA# ( ﹎ → _ ) CENTRELINE LOW LINE → LOW LINE#
 {65103, "_"}, // MA# ( ﹏ → _ ) WAVY LOW LINE → LOW LINE#
 {65112, "-"}, // MA#* ( ﹘ → - ) SMALL EM DASH → HYPHEN-MINUS#
 {65128, "\\"}, // MA#* ( ﹨ → \ ) SMALL REVERSE SOLIDUS → REVERSE SOLIDUS# →∖→
 {65165, "l"}, // MA# ( ‎ﺍ‎ → l ) ARABIC LETTER ALEF ISOLATED FORM → LATIN SMALL LETTER L# →‎ا‎→→1→
 {65166, "l"}, // MA# ( ‎ﺎ‎ → l ) ARABIC LETTER ALEF FINAL FORM → LATIN SMALL LETTER L# →‎ا‎→→1→
 {65257, "o"}, // MA# ( ‎ﻩ‎ → o ) ARABIC LETTER HEH ISOLATED FORM → LATIN SMALL LETTER O# →‎ه‎→
 {65258, "o"}, // MA# ( ‎ﻪ‎ → o ) ARABIC LETTER HEH FINAL FORM → LATIN SMALL LETTER O# →‎ه‎→
 {65259, "o"}, // MA# ( ‎ﻫ‎ → o ) ARABIC LETTER HEH INITIAL FORM → LATIN SMALL LETTER O# →‎ه‎→
 {65260, "o"}, // MA# ( ‎ﻬ‎ → o ) ARABIC LETTER HEH MEDIAL FORM → LATIN SMALL LETTER O# →‎ه‎→
 {65281, "!"}, // MA#* ( ！ → ! ) FULLWIDTH EXCLAMATION MARK → EXCLAMATION MARK# →ǃ→
 {65282, "\""}, // MA#* ( ＂ → '' ) FULLWIDTH QUOTATION MARK → APOSTROPHE, APOSTROPHE# →”→→"→# Converted to a quote.
 {65287, "'"}, // MA#* ( ＇ → ' ) FULLWIDTH APOSTROPHE → APOSTROPHE# →’→
 {65306, ":"}, // MA#* ( ： → : ) FULLWIDTH COLON → COLON# →︰→
 {65313, "A"}, // MA# ( Ａ → A ) FULLWIDTH LATIN CAPITAL LETTER A → LATIN CAPITAL LETTER A# →А→
 {65314, "B"}, // MA# ( Ｂ → B ) FULLWIDTH LATIN CAPITAL LETTER B → LATIN CAPITAL LETTER B# →Β→
 {65315, "C"}, // MA# ( Ｃ → C ) FULLWIDTH LATIN CAPITAL LETTER C → LATIN CAPITAL LETTER C# →С→
 {65317, "E"}, // MA# ( Ｅ → E ) FULLWIDTH LATIN CAPITAL LETTER E → LATIN CAPITAL LETTER E# →Ε→
 {65320, "H"}, // MA# ( Ｈ → H ) FULLWIDTH LATIN CAPITAL LETTER H → LATIN CAPITAL LETTER H# →Η→
 {65321, "l"}, // MA# ( Ｉ → l ) FULLWIDTH LATIN CAPITAL LETTER I → LATIN SMALL LETTER L# →Ӏ→
 {65322, "J"}, // MA# ( Ｊ → J ) FULLWIDTH LATIN CAPITAL LETTER J → LATIN CAPITAL LETTER J# →Ј→
 {65323, "K"}, // MA# ( Ｋ → K ) FULLWIDTH LATIN CAPITAL LETTER K → LATIN CAPITAL LETTER K# →Κ→
 {65325, "M"}, // MA# ( Ｍ → M ) FULLWIDTH LATIN CAPITAL LETTER M → LATIN CAPITAL LETTER M# →Μ→
 {65326, "N"}, // MA# ( Ｎ → N ) FULLWIDTH LATIN CAPITAL LETTER N → LATIN CAPITAL LETTER N# →Ν→
 {65327, "O"}, // MA# ( Ｏ → O ) FULLWIDTH LATIN CAPITAL LETTER O → LATIN CAPITAL LETTER O# →О→
 {65328, "P"}, // MA# ( Ｐ → P ) FULLWIDTH LATIN CAPITAL LETTER P → LATIN CAPITAL LETTER P# →Р→
 {65331, "S"}, // MA# ( Ｓ → S ) FULLWIDTH LATIN CAPITAL LETTER S → LATIN CAPITAL LETTER S# →Ѕ→
 {65332, "T"}, // MA# ( Ｔ → T ) FULLWIDTH LATIN CAPITAL LETTER T → LATIN CAPITAL LETTER T# →Т→
 {65336, "X"}, // MA# ( Ｘ → X ) FULLWIDTH LATIN CAPITAL LETTER X → LATIN CAPITAL LETTER X# →Х→
 {65337, "Y"}, // MA# ( Ｙ → Y ) FULLWIDTH LATIN CAPITAL LETTER Y → LATIN CAPITAL LETTER Y# →Υ→
 {65338, "Z"}, // MA# ( Ｚ → Z ) FULLWIDTH LATIN CAPITAL LETTER Z → LATIN CAPITAL LETTER Z# →Ζ→
 {65339, "("}, // MA#* ( ［ → ( ) FULLWIDTH LEFT SQUARE BRACKET → LEFT PARENTHESIS# →〔→
 {65340, "\\"}, // MA#* ( ＼ → \ ) FULLWIDTH REVERSE SOLIDUS → REVERSE SOLIDUS# →∖→
 {65341, ")"}, // MA#* ( ］ → ) ) FULLWIDTH RIGHT SQUARE BRACKET → RIGHT PARENTHESIS# →〕→
 {65344, "'"}, // MA#* ( ｀ → ' ) FULLWIDTH GRAVE ACCENT → APOSTROPHE# →‘→
 {65345, "a"}, // MA# ( ａ → a ) FULLWIDTH LATIN SMALL LETTER A → LATIN SMALL LETTER A# →а→
 {65347, "c"}, // MA# ( ｃ → c ) FULLWIDTH LATIN SMALL LETTER C → LATIN SMALL LETTER C# →с→
 {65349, "e"}, // MA# ( ｅ → e ) FULLWIDTH LATIN SMALL LETTER E → LATIN SMALL LETTER E# →е→
 {65351, "g"}, // MA# ( ｇ → g ) FULLWIDTH LATIN SMALL LETTER G → LATIN SMALL LETTER G# →ɡ→
 {65352, "h"}, // MA# ( ｈ → h ) FULLWIDTH LATIN SMALL LETTER H → LATIN SMALL LETTER H# →һ→
 {65353, "i"}, // MA# ( ｉ → i ) FULLWIDTH LATIN SMALL LETTER I → LATIN SMALL LETTER I# →і→
 {65354, "j"}, // MA# ( ｊ → j ) FULLWIDTH LATIN SMALL LETTER J → LATIN SMALL LETTER J# →ϳ→
 {65356, "l"}, // MA# ( ｌ → l ) FULLWIDTH LATIN SMALL LETTER L → LATIN SMALL LETTER L# →Ⅰ→→Ӏ→
 {65359, "o"}, // MA# ( ｏ → o ) FULLWIDTH LATIN SMALL LETTER O → LATIN SMALL LETTER O# →о→
 {65360, "p"}, // MA# ( ｐ → p ) FULLWIDTH LATIN SMALL LETTER P → LATIN SMALL LETTER P# →р→
 {65363, "s"}, // MA# ( ｓ → s ) FULLWIDTH LATIN SMALL LETTER S → LATIN SMALL LETTER S# →ѕ→
 {65366, "v"}, // MA# ( ｖ → v ) FULLWIDTH LATIN SMALL LETTER V → LATIN SMALL LETTER V# →ν→
 {65368, "x"}, // MA# ( ｘ → x ) FULLWIDTH LATIN SMALL LETTER X → LATIN SMALL LETTER X# →х→
 {65369, "y"}, // MA# ( ｙ → y ) FULLWIDTH LATIN SMALL LETTER Y → LATIN SMALL LETTER Y# →у→
 {65512, "l"}, // MA#* ( ￨ → l ) HALFWIDTH FORMS LIGHT VERTICAL → LATIN SMALL LETTER L# →|→
 {66178, "B"}, // MA# ( 𐊂 → B ) LYCIAN LETTER B → LATIN CAPITAL LETTER B#
 {66182, "E"}, // MA# ( 𐊆 → E ) LYCIAN LETTER I → LATIN CAPITAL LETTER E#
 {66183, "F"}, // MA# ( 𐊇 → F ) LYCIAN LETTER W → LATIN CAPITAL LETTER F#
 {66186, "l"}, // MA# ( 𐊊 → l ) LYCIAN LETTER J → LATIN SMALL LETTER L# →I→
 {66192, "X"}, // MA# ( 𐊐 → X ) LYCIAN LETTER MM → LATIN CAPITAL LETTER X#
 {66194, "O"}, // MA# ( 𐊒 → O ) LYCIAN LETTER U → LATIN CAPITAL LETTER O#
 {66197, "P"}, // MA# ( 𐊕 → P ) LYCIAN LETTER R → LATIN CAPITAL LETTER P#
 {66198, "S"}, // MA# ( 𐊖 → S ) LYCIAN LETTER S → LATIN CAPITAL LETTER S#
 {66199, "T"}, // MA# ( 𐊗 → T ) LYCIAN LETTER T → LATIN CAPITAL LETTER T#
 {66203, "+"}, // MA# ( 𐊛 → + ) LYCIAN LETTER H → PLUS SIGN#
 {66208, "A"}, // MA# ( 𐊠 → A ) CARIAN LETTER A → LATIN CAPITAL LETTER A#
 {66209, "B"}, // MA# ( 𐊡 → B ) CARIAN LETTER P2 → LATIN CAPITAL LETTER B#
 {66210, "C"}, // MA# ( 𐊢 → C ) CARIAN LETTER D → LATIN CAPITAL LETTER C#
 {66213, "F"}, // MA# ( 𐊥 → F ) CARIAN LETTER R → LATIN CAPITAL LETTER F#
 {66219, "O"}, // MA# ( 𐊫 → O ) CARIAN LETTER O → LATIN CAPITAL LETTER O#
 {66224, "M"}, // MA# ( 𐊰 → M ) CARIAN LETTER S → LATIN CAPITAL LETTER M#
 {66225, "T"}, // MA# ( 𐊱 → T ) CARIAN LETTER C-18 → LATIN CAPITAL LETTER T#
 {66226, "Y"}, // MA# ( 𐊲 → Y ) CARIAN LETTER U → LATIN CAPITAL LETTER Y#
 {66228, "X"}, // MA# ( 𐊴 → X ) CARIAN LETTER X → LATIN CAPITAL LETTER X#
 {66255, "H"}, // MA# ( 𐋏 → H ) CARIAN LETTER E2 → LATIN CAPITAL LETTER H#
 {66293, "Z"}, // MA#* ( 𐋵 → Z ) COPTIC EPACT NUMBER THREE HUNDRED → LATIN CAPITAL LETTER Z#
 {66305, "B"}, // MA# ( 𐌁 → B ) OLD ITALIC LETTER BE → LATIN CAPITAL LETTER B#
 {66306, "C"}, // MA# ( 𐌂 → C ) OLD ITALIC LETTER KE → LATIN CAPITAL LETTER C#
 {66313, "l"}, // MA# ( 𐌉 → l ) OLD ITALIC LETTER I → LATIN SMALL LETTER L# →I→
 {66321, "M"}, // MA# ( 𐌑 → M ) OLD ITALIC LETTER SHE → LATIN CAPITAL LETTER M#
 {66325, "T"}, // MA# ( 𐌕 → T ) OLD ITALIC LETTER TE → LATIN CAPITAL LETTER T#
 {66327, "X"}, // MA# ( 𐌗 → X ) OLD ITALIC LETTER EKS → LATIN CAPITAL LETTER X#
 {66330, "8"}, // MA# ( 𐌚 → 8 ) OLD ITALIC LETTER EF → DIGIT EIGHT#
 {66335, "*"}, // MA# ( 𐌟 → * ) OLD ITALIC LETTER ESS → ASTERISK#
 {66336, "l"}, // MA#* ( 𐌠 → l ) OLD ITALIC NUMERAL ONE → LATIN SMALL LETTER L# →𐌉→→I→
 {66338, "X"}, // MA#* ( 𐌢 → X ) OLD ITALIC NUMERAL TEN → LATIN CAPITAL LETTER X# →𐌗→
 {66564, "O"}, // MA# ( 𐐄 → O ) DESERET CAPITAL LETTER LONG O → LATIN CAPITAL LETTER O#
 {66581, "C"}, // MA# ( 𐐕 → C ) DESERET CAPITAL LETTER CHEE → LATIN CAPITAL LETTER C#
 {66587, "L"}, // MA# ( 𐐛 → L ) DESERET CAPITAL LETTER ETH → LATIN CAPITAL LETTER L#
 {66592, "S"}, // MA# ( 𐐠 → S ) DESERET CAPITAL LETTER ZHEE → LATIN CAPITAL LETTER S#
 {66604, "o"}, // MA# ( 𐐬 → o ) DESERET SMALL LETTER LONG O → LATIN SMALL LETTER O#
 {66621, "c"}, // MA# ( 𐐽 → c ) DESERET SMALL LETTER CHEE → LATIN SMALL LETTER C#
 {66632, "s"}, // MA# ( 𐑈 → s ) DESERET SMALL LETTER ZHEE → LATIN SMALL LETTER S#
 {66740, "R"}, // MA# ( 𐒴 → R ) OSAGE CAPITAL LETTER BRA → LATIN CAPITAL LETTER R# →Ʀ→
 {66754, "O"}, // MA# ( 𐓂 → O ) OSAGE CAPITAL LETTER O → LATIN CAPITAL LETTER O#
 {66766, "U"}, // MA# ( 𐓎 → U ) OSAGE CAPITAL LETTER U → LATIN CAPITAL LETTER U#
 {66770, "7"}, // MA# ( 𐓒 → 7 ) OSAGE CAPITAL LETTER ZA → DIGIT SEVEN#
 {66794, "o"}, // MA# ( 𐓪 → o ) OSAGE SMALL LETTER O → LATIN SMALL LETTER O#
 {66806, "u"}, // MA# ( 𐓶 → u ) OSAGE SMALL LETTER U → LATIN SMALL LETTER U# →ᴜ→
 {66835, "N"}, // MA# ( 𐔓 → N ) ELBASAN LETTER NE → LATIN CAPITAL LETTER N#
 {66838, "O"}, // MA# ( 𐔖 → O ) ELBASAN LETTER O → LATIN CAPITAL LETTER O#
 {66840, "K"}, // MA# ( 𐔘 → K ) ELBASAN LETTER QE → LATIN CAPITAL LETTER K#
 {66844, "C"}, // MA# ( 𐔜 → C ) ELBASAN LETTER SHE → LATIN CAPITAL LETTER C#
 {66845, "V"}, // MA# ( 𐔝 → V ) ELBASAN LETTER TE → LATIN CAPITAL LETTER V#
 {66853, "F"}, // MA# ( 𐔥 → F ) ELBASAN LETTER GHE → LATIN CAPITAL LETTER F#
 {66854, "L"}, // MA# ( 𐔦 → L ) ELBASAN LETTER GHAMMA → LATIN CAPITAL LETTER L#
 {66855, "X"}, // MA# ( 𐔧 → X ) ELBASAN LETTER KHE → LATIN CAPITAL LETTER X#
 {68176, "."}, // MA#* ( ‎𐩐‎ → . ) KHAROSHTHI PUNCTUATION DOT → FULL STOP#
 {70864, "O"}, // MA# ( 𑓐 → O ) TIRHUTA DIGIT ZERO → LATIN CAPITAL LETTER O# →০→→0→
 {71424, "rn"}, // MA# ( 𑜀 → rn ) AHOM LETTER KA → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {71430, "v"}, // MA# ( 𑜆 → v ) AHOM LETTER PA → LATIN SMALL LETTER V#
 {71434, "w"}, // MA# ( 𑜊 → w ) AHOM LETTER JA → LATIN SMALL LETTER W#
 {71438, "w"}, // MA# ( 𑜎 → w ) AHOM LETTER LA → LATIN SMALL LETTER W#
 {71439, "w"}, // MA# ( 𑜏 → w ) AHOM LETTER SA → LATIN SMALL LETTER W#
 {71840, "V"}, // MA# ( 𑢠 → V ) WARANG CITI CAPITAL LETTER NGAA → LATIN CAPITAL LETTER V#
 {71842, "F"}, // MA# ( 𑢢 → F ) WARANG CITI CAPITAL LETTER WI → LATIN CAPITAL LETTER F#
 {71843, "L"}, // MA# ( 𑢣 → L ) WARANG CITI CAPITAL LETTER YU → LATIN CAPITAL LETTER L#
 {71844, "Y"}, // MA# ( 𑢤 → Y ) WARANG CITI CAPITAL LETTER YA → LATIN CAPITAL LETTER Y#
 {71846, "E"}, // MA# ( 𑢦 → E ) WARANG CITI CAPITAL LETTER II → LATIN CAPITAL LETTER E#
 {71849, "Z"}, // MA# ( 𑢩 → Z ) WARANG CITI CAPITAL LETTER O → LATIN CAPITAL LETTER Z#
 {71852, "9"}, // MA# ( 𑢬 → 9 ) WARANG CITI CAPITAL LETTER KO → DIGIT NINE#
 {71854, "E"}, // MA# ( 𑢮 → E ) WARANG CITI CAPITAL LETTER YUJ → LATIN CAPITAL LETTER E#
 {71855, "4"}, // MA# ( 𑢯 → 4 ) WARANG CITI CAPITAL LETTER UC → DIGIT FOUR#
 {71858, "L"}, // MA# ( 𑢲 → L ) WARANG CITI CAPITAL LETTER TTE → LATIN CAPITAL LETTER L#
 {71861, "O"}, // MA# ( 𑢵 → O ) WARANG CITI CAPITAL LETTER AT → LATIN CAPITAL LETTER O#
 {71864, "U"}, // MA# ( 𑢸 → U ) WARANG CITI CAPITAL LETTER PU → LATIN CAPITAL LETTER U#
 {71867, "5"}, // MA# ( 𑢻 → 5 ) WARANG CITI CAPITAL LETTER HORR → DIGIT FIVE#
 {71868, "T"}, // MA# ( 𑢼 → T ) WARANG CITI CAPITAL LETTER HAR → LATIN CAPITAL LETTER T#
 {71872, "v"}, // MA# ( 𑣀 → v ) WARANG CITI SMALL LETTER NGAA → LATIN SMALL LETTER V#
 {71873, "s"}, // MA# ( 𑣁 → s ) WARANG CITI SMALL LETTER A → LATIN SMALL LETTER S#
 {71874, "F"}, // MA# ( 𑣂 → F ) WARANG CITI SMALL LETTER WI → LATIN CAPITAL LETTER F#
 {71875, "i"}, // MA# ( 𑣃 → i ) WARANG CITI SMALL LETTER YU → LATIN SMALL LETTER I# →ι→
 {71876, "z"}, // MA# ( 𑣄 → z ) WARANG CITI SMALL LETTER YA → LATIN SMALL LETTER Z#
 {71878, "7"}, // MA# ( 𑣆 → 7 ) WARANG CITI SMALL LETTER II → DIGIT SEVEN#
 {71880, "o"}, // MA# ( 𑣈 → o ) WARANG CITI SMALL LETTER E → LATIN SMALL LETTER O#
 {71882, "3"}, // MA# ( 𑣊 → 3 ) WARANG CITI SMALL LETTER ANG → DIGIT THREE#
 {71884, "9"}, // MA# ( 𑣌 → 9 ) WARANG CITI SMALL LETTER KO → DIGIT NINE#
 {71893, "6"}, // MA# ( 𑣕 → 6 ) WARANG CITI SMALL LETTER AT → DIGIT SIX#
 {71894, "9"}, // MA# ( 𑣖 → 9 ) WARANG CITI SMALL LETTER AM → DIGIT NINE#
 {71895, "o"}, // MA# ( 𑣗 → o ) WARANG CITI SMALL LETTER BU → LATIN SMALL LETTER O#
 {71896, "u"}, // MA# ( 𑣘 → u ) WARANG CITI SMALL LETTER PU → LATIN SMALL LETTER U# →υ→→ʋ→
 {71900, "y"}, // MA# ( 𑣜 → y ) WARANG CITI SMALL LETTER HAR → LATIN SMALL LETTER Y# →ɣ→→γ→
 {71904, "O"}, // MA# ( 𑣠 → O ) WARANG CITI DIGIT ZERO → LATIN CAPITAL LETTER O# →0→
 {71907, "rn"}, // MA# ( 𑣣 → rn ) WARANG CITI DIGIT THREE → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {71909, "Z"}, // MA# ( 𑣥 → Z ) WARANG CITI DIGIT FIVE → LATIN CAPITAL LETTER Z#
 {71910, "W"}, // MA# ( 𑣦 → W ) WARANG CITI DIGIT SIX → LATIN CAPITAL LETTER W#
 {71913, "C"}, // MA# ( 𑣩 → C ) WARANG CITI DIGIT NINE → LATIN CAPITAL LETTER C#
 {71916, "X"}, // MA#* ( 𑣬 → X ) WARANG CITI NUMBER THIRTY → LATIN CAPITAL LETTER X#
 {71919, "W"}, // MA#* ( 𑣯 → W ) WARANG CITI NUMBER SIXTY → LATIN CAPITAL LETTER W#
 {71922, "C"}, // MA#* ( 𑣲 → C ) WARANG CITI NUMBER NINETY → LATIN CAPITAL LETTER C#
 {93960, "V"}, // MA# ( 𖼈 → V ) MIAO LETTER VA → LATIN CAPITAL LETTER V#
 {93962, "T"}, // MA# ( 𖼊 → T ) MIAO LETTER TA → LATIN CAPITAL LETTER T#
 {93974, "L"}, // MA# ( 𖼖 → L ) MIAO LETTER LA → LATIN CAPITAL LETTER L#
 {93992, "l"}, // MA# ( 𖼨 → l ) MIAO LETTER GHA → LATIN SMALL LETTER L# →I→
 {94005, "R"}, // MA# ( 𖼵 → R ) MIAO LETTER ZHA → LATIN CAPITAL LETTER R#
 {94010, "S"}, // MA# ( 𖼺 → S ) MIAO LETTER SA → LATIN CAPITAL LETTER S#
 {94011, "3"}, // MA# ( 𖼻 → 3 ) MIAO LETTER ZA → DIGIT THREE# →Ʒ→
 {94015, ">"}, // MA# ( 𖼿 → > ) MIAO LETTER ARCHAIC ZZA → GREATER-THAN SIGN#
 {94016, "A"}, // MA# ( 𖽀 → A ) MIAO LETTER ZZYA → LATIN CAPITAL LETTER A#
 {94018, "U"}, // MA# ( 𖽂 → U ) MIAO LETTER WA → LATIN CAPITAL LETTER U#
 {94019, "Y"}, // MA# ( 𖽃 → Y ) MIAO LETTER AH → LATIN CAPITAL LETTER Y#
 {94033, "'"}, // MA# ( 𖽑 → ' ) MIAO SIGN ASPIRATION → APOSTROPHE# →ʼ→→′→
 {94034, "'"}, // MA# ( 𖽒 → ' ) MIAO SIGN REFORMED VOICING → APOSTROPHE# →ʻ→→‘→
 {119060, "{"}, // MA#* ( 𝄔 → { ) MUSICAL SYMBOL BRACE → LEFT CURLY BRACKET#
 {119149, "."}, // MA# ( 𝅭 → . ) MUSICAL SYMBOL COMBINING AUGMENTATION DOT → FULL STOP#
 {119302, "3"}, // MA#* ( 𝈆 → 3 ) GREEK VOCAL NOTATION SYMBOL-7 → DIGIT THREE#
 {119309, "V"}, // MA#* ( 𝈍 → V ) GREEK VOCAL NOTATION SYMBOL-14 → LATIN CAPITAL LETTER V#
 {119311, "\\"}, // MA#* ( 𝈏 → \ ) GREEK VOCAL NOTATION SYMBOL-16 → REVERSE SOLIDUS#
 {119314, "7"}, // MA#* ( 𝈒 → 7 ) GREEK VOCAL NOTATION SYMBOL-19 → DIGIT SEVEN#
 {119315, "F"}, // MA#* ( 𝈓 → F ) GREEK VOCAL NOTATION SYMBOL-20 → LATIN CAPITAL LETTER F# →Ϝ→
 {119318, "R"}, // MA#* ( 𝈖 → R ) GREEK VOCAL NOTATION SYMBOL-23 → LATIN CAPITAL LETTER R#
 {119338, "L"}, // MA#* ( 𝈪 → L ) GREEK INSTRUMENTAL NOTATION SYMBOL-23 → LATIN CAPITAL LETTER L#
 {119350, "<"}, // MA#* ( 𝈶 → < ) GREEK INSTRUMENTAL NOTATION SYMBOL-40 → LESS-THAN SIGN#
 {119351, ">"}, // MA#* ( 𝈷 → > ) GREEK INSTRUMENTAL NOTATION SYMBOL-42 → GREATER-THAN SIGN#
 {119354, "/"}, // MA#* ( 𝈺 → / ) GREEK INSTRUMENTAL NOTATION SYMBOL-47 → SOLIDUS#
 {119355, "\\"}, // MA#* ( 𝈻 → \ ) GREEK INSTRUMENTAL NOTATION SYMBOL-48 → REVERSE SOLIDUS# →𝈏→
 {119808, "A"}, // MA# ( 𝐀 → A ) MATHEMATICAL BOLD CAPITAL A → LATIN CAPITAL LETTER A#
 {119809, "B"}, // MA# ( 𝐁 → B ) MATHEMATICAL BOLD CAPITAL B → LATIN CAPITAL LETTER B#
 {119810, "C"}, // MA# ( 𝐂 → C ) MATHEMATICAL BOLD CAPITAL C → LATIN CAPITAL LETTER C#
 {119811, "D"}, // MA# ( 𝐃 → D ) MATHEMATICAL BOLD CAPITAL D → LATIN CAPITAL LETTER D#
 {119812, "E"}, // MA# ( 𝐄 → E ) MATHEMATICAL BOLD CAPITAL E → LATIN CAPITAL LETTER E#
 {119813, "F"}, // MA# ( 𝐅 → F ) MATHEMATICAL BOLD CAPITAL F → LATIN CAPITAL LETTER F#
 {119814, "G"}, // MA# ( 𝐆 → G ) MATHEMATICAL BOLD CAPITAL G → LATIN CAPITAL LETTER G#
 {119815, "H"}, // MA# ( 𝐇 → H ) MATHEMATICAL BOLD CAPITAL H → LATIN CAPITAL LETTER H#
 {119816, "l"}, // MA# ( 𝐈 → l ) MATHEMATICAL BOLD CAPITAL I → LATIN SMALL LETTER L# →I→
 {119817, "J"}, // MA# ( 𝐉 → J ) MATHEMATICAL BOLD CAPITAL J → LATIN CAPITAL LETTER J#
 {119818, "K"}, // MA# ( 𝐊 → K ) MATHEMATICAL BOLD CAPITAL K → LATIN CAPITAL LETTER K#
 {119819, "L"}, // MA# ( 𝐋 → L ) MATHEMATICAL BOLD CAPITAL L → LATIN CAPITAL LETTER L#
 {119820, "M"}, // MA# ( 𝐌 → M ) MATHEMATICAL BOLD CAPITAL M → LATIN CAPITAL LETTER M#
 {119821, "N"}, // MA# ( 𝐍 → N ) MATHEMATICAL BOLD CAPITAL N → LATIN CAPITAL LETTER N#
 {119822, "O"}, // MA# ( 𝐎 → O ) MATHEMATICAL BOLD CAPITAL O → LATIN CAPITAL LETTER O#
 {119823, "P"}, // MA# ( 𝐏 → P ) MATHEMATICAL BOLD CAPITAL P → LATIN CAPITAL LETTER P#
 {119824, "Q"}, // MA# ( 𝐐 → Q ) MATHEMATICAL BOLD CAPITAL Q → LATIN CAPITAL LETTER Q#
 {119825, "R"}, // MA# ( 𝐑 → R ) MATHEMATICAL BOLD CAPITAL R → LATIN CAPITAL LETTER R#
 {119826, "S"}, // MA# ( 𝐒 → S ) MATHEMATICAL BOLD CAPITAL S → LATIN CAPITAL LETTER S#
 {119827, "T"}, // MA# ( 𝐓 → T ) MATHEMATICAL BOLD CAPITAL T → LATIN CAPITAL LETTER T#
 {119828, "U"}, // MA# ( 𝐔 → U ) MATHEMATICAL BOLD CAPITAL U → LATIN CAPITAL LETTER U#
 {119829, "V"}, // MA# ( 𝐕 → V ) MATHEMATICAL BOLD CAPITAL V → LATIN CAPITAL LETTER V#
 {119830, "W"}, // MA# ( 𝐖 → W ) MATHEMATICAL BOLD CAPITAL W → LATIN CAPITAL LETTER W#
 {119831, "X"}, // MA# ( 𝐗 → X ) MATHEMATICAL BOLD CAPITAL X → LATIN CAPITAL LETTER X#
 {119832, "Y"}, // MA# ( 𝐘 → Y ) MATHEMATICAL BOLD CAPITAL Y → LATIN CAPITAL LETTER Y#
 {119833, "Z"}, // MA# ( 𝐙 → Z ) MATHEMATICAL BOLD CAPITAL Z → LATIN CAPITAL LETTER Z#
 {119834, "a"}, // MA# ( 𝐚 → a ) MATHEMATICAL BOLD SMALL A → LATIN SMALL LETTER A#
 {119835, "b"}, // MA# ( 𝐛 → b ) MATHEMATICAL BOLD SMALL B → LATIN SMALL LETTER B#
 {119836, "c"}, // MA# ( 𝐜 → c ) MATHEMATICAL BOLD SMALL C → LATIN SMALL LETTER C#
 {119837, "d"}, // MA# ( 𝐝 → d ) MATHEMATICAL BOLD SMALL D → LATIN SMALL LETTER D#
 {119838, "e"}, // MA# ( 𝐞 → e ) MATHEMATICAL BOLD SMALL E → LATIN SMALL LETTER E#
 {119839, "f"}, // MA# ( 𝐟 → f ) MATHEMATICAL BOLD SMALL F → LATIN SMALL LETTER F#
 {119840, "g"}, // MA# ( 𝐠 → g ) MATHEMATICAL BOLD SMALL G → LATIN SMALL LETTER G#
 {119841, "h"}, // MA# ( 𝐡 → h ) MATHEMATICAL BOLD SMALL H → LATIN SMALL LETTER H#
 {119842, "i"}, // MA# ( 𝐢 → i ) MATHEMATICAL BOLD SMALL I → LATIN SMALL LETTER I#
 {119843, "j"}, // MA# ( 𝐣 → j ) MATHEMATICAL BOLD SMALL J → LATIN SMALL LETTER J#
 {119844, "k"}, // MA# ( 𝐤 → k ) MATHEMATICAL BOLD SMALL K → LATIN SMALL LETTER K#
 {119845, "l"}, // MA# ( 𝐥 → l ) MATHEMATICAL BOLD SMALL L → LATIN SMALL LETTER L#
 {119846, "rn"}, // MA# ( 𝐦 → rn ) MATHEMATICAL BOLD SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {119847, "n"}, // MA# ( 𝐧 → n ) MATHEMATICAL BOLD SMALL N → LATIN SMALL LETTER N#
 {119848, "o"}, // MA# ( 𝐨 → o ) MATHEMATICAL BOLD SMALL O → LATIN SMALL LETTER O#
 {119849, "p"}, // MA# ( 𝐩 → p ) MATHEMATICAL BOLD SMALL P → LATIN SMALL LETTER P#
 {119850, "q"}, // MA# ( 𝐪 → q ) MATHEMATICAL BOLD SMALL Q → LATIN SMALL LETTER Q#
 {119851, "r"}, // MA# ( 𝐫 → r ) MATHEMATICAL BOLD SMALL R → LATIN SMALL LETTER R#
 {119852, "s"}, // MA# ( 𝐬 → s ) MATHEMATICAL BOLD SMALL S → LATIN SMALL LETTER S#
 {119853, "t"}, // MA# ( 𝐭 → t ) MATHEMATICAL BOLD SMALL T → LATIN SMALL LETTER T#
 {119854, "u"}, // MA# ( 𝐮 → u ) MATHEMATICAL BOLD SMALL U → LATIN SMALL LETTER U#
 {119855, "v"}, // MA# ( 𝐯 → v ) MATHEMATICAL BOLD SMALL V → LATIN SMALL LETTER V#
 {119856, "w"}, // MA# ( 𝐰 → w ) MATHEMATICAL BOLD SMALL W → LATIN SMALL LETTER W#
 {119857, "x"}, // MA# ( 𝐱 → x ) MATHEMATICAL BOLD SMALL X → LATIN SMALL LETTER X#
 {119858, "y"}, // MA# ( 𝐲 → y ) MATHEMATICAL BOLD SMALL Y → LATIN SMALL LETTER Y#
 {119859, "z"}, // MA# ( 𝐳 → z ) MATHEMATICAL BOLD SMALL Z → LATIN SMALL LETTER Z#
 {119860, "A"}, // MA# ( 𝐴 → A ) MATHEMATICAL ITALIC CAPITAL A → LATIN CAPITAL LETTER A#
 {119861, "B"}, // MA# ( 𝐵 → B ) MATHEMATICAL ITALIC CAPITAL B → LATIN CAPITAL LETTER B#
 {119862, "C"}, // MA# ( 𝐶 → C ) MATHEMATICAL ITALIC CAPITAL C → LATIN CAPITAL LETTER C#
 {119863, "D"}, // MA# ( 𝐷 → D ) MATHEMATICAL ITALIC CAPITAL D → LATIN CAPITAL LETTER D#
 {119864, "E"}, // MA# ( 𝐸 → E ) MATHEMATICAL ITALIC CAPITAL E → LATIN CAPITAL LETTER E#
 {119865, "F"}, // MA# ( 𝐹 → F ) MATHEMATICAL ITALIC CAPITAL F → LATIN CAPITAL LETTER F#
 {119866, "G"}, // MA# ( 𝐺 → G ) MATHEMATICAL ITALIC CAPITAL G → LATIN CAPITAL LETTER G#
 {119867, "H"}, // MA# ( 𝐻 → H ) MATHEMATICAL ITALIC CAPITAL H → LATIN CAPITAL LETTER H#
 {119868, "l"}, // MA# ( 𝐼 → l ) MATHEMATICAL ITALIC CAPITAL I → LATIN SMALL LETTER L# →I→
 {119869, "J"}, // MA# ( 𝐽 → J ) MATHEMATICAL ITALIC CAPITAL J → LATIN CAPITAL LETTER J#
 {119870, "K"}, // MA# ( 𝐾 → K ) MATHEMATICAL ITALIC CAPITAL K → LATIN CAPITAL LETTER K#
 {119871, "L"}, // MA# ( 𝐿 → L ) MATHEMATICAL ITALIC CAPITAL L → LATIN CAPITAL LETTER L#
 {119872, "M"}, // MA# ( 𝑀 → M ) MATHEMATICAL ITALIC CAPITAL M → LATIN CAPITAL LETTER M#
 {119873, "N"}, // MA# ( 𝑁 → N ) MATHEMATICAL ITALIC CAPITAL N → LATIN CAPITAL LETTER N#
 {119874, "O"}, // MA# ( 𝑂 → O ) MATHEMATICAL ITALIC CAPITAL O → LATIN CAPITAL LETTER O#
 {119875, "P"}, // MA# ( 𝑃 → P ) MATHEMATICAL ITALIC CAPITAL P → LATIN CAPITAL LETTER P#
 {119876, "Q"}, // MA# ( 𝑄 → Q ) MATHEMATICAL ITALIC CAPITAL Q → LATIN CAPITAL LETTER Q#
 {119877, "R"}, // MA# ( 𝑅 → R ) MATHEMATICAL ITALIC CAPITAL R → LATIN CAPITAL LETTER R#
 {119878, "S"}, // MA# ( 𝑆 → S ) MATHEMATICAL ITALIC CAPITAL S → LATIN CAPITAL LETTER S#
 {119879, "T"}, // MA# ( 𝑇 → T ) MATHEMATICAL ITALIC CAPITAL T → LATIN CAPITAL LETTER T#
 {119880, "U"}, // MA# ( 𝑈 → U ) MATHEMATICAL ITALIC CAPITAL U → LATIN CAPITAL LETTER U#
 {119881, "V"}, // MA# ( 𝑉 → V ) MATHEMATICAL ITALIC CAPITAL V → LATIN CAPITAL LETTER V#
 {119882, "W"}, // MA# ( 𝑊 → W ) MATHEMATICAL ITALIC CAPITAL W → LATIN CAPITAL LETTER W#
 {119883, "X"}, // MA# ( 𝑋 → X ) MATHEMATICAL ITALIC CAPITAL X → LATIN CAPITAL LETTER X#
 {119884, "Y"}, // MA# ( 𝑌 → Y ) MATHEMATICAL ITALIC CAPITAL Y → LATIN CAPITAL LETTER Y#
 {119885, "Z"}, // MA# ( 𝑍 → Z ) MATHEMATICAL ITALIC CAPITAL Z → LATIN CAPITAL LETTER Z#
 {119886, "a"}, // MA# ( 𝑎 → a ) MATHEMATICAL ITALIC SMALL A → LATIN SMALL LETTER A#
 {119887, "b"}, // MA# ( 𝑏 → b ) MATHEMATICAL ITALIC SMALL B → LATIN SMALL LETTER B#
 {119888, "c"}, // MA# ( 𝑐 → c ) MATHEMATICAL ITALIC SMALL C → LATIN SMALL LETTER C#
 {119889, "d"}, // MA# ( 𝑑 → d ) MATHEMATICAL ITALIC SMALL D → LATIN SMALL LETTER D#
 {119890, "e"}, // MA# ( 𝑒 → e ) MATHEMATICAL ITALIC SMALL E → LATIN SMALL LETTER E#
 {119891, "f"}, // MA# ( 𝑓 → f ) MATHEMATICAL ITALIC SMALL F → LATIN SMALL LETTER F#
 {119892, "g"}, // MA# ( 𝑔 → g ) MATHEMATICAL ITALIC SMALL G → LATIN SMALL LETTER G#
 {119894, "i"}, // MA# ( 𝑖 → i ) MATHEMATICAL ITALIC SMALL I → LATIN SMALL LETTER I#
 {119895, "j"}, // MA# ( 𝑗 → j ) MATHEMATICAL ITALIC SMALL J → LATIN SMALL LETTER J#
 {119896, "k"}, // MA# ( 𝑘 → k ) MATHEMATICAL ITALIC SMALL K → LATIN SMALL LETTER K#
 {119897, "l"}, // MA# ( 𝑙 → l ) MATHEMATICAL ITALIC SMALL L → LATIN SMALL LETTER L#
 {119898, "rn"}, // MA# ( 𝑚 → rn ) MATHEMATICAL ITALIC SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {119899, "n"}, // MA# ( 𝑛 → n ) MATHEMATICAL ITALIC SMALL N → LATIN SMALL LETTER N#
 {119900, "o"}, // MA# ( 𝑜 → o ) MATHEMATICAL ITALIC SMALL O → LATIN SMALL LETTER O#
 {119901, "p"}, // MA# ( 𝑝 → p ) MATHEMATICAL ITALIC SMALL P → LATIN SMALL LETTER P#
 {119902, "q"}, // MA# ( 𝑞 → q ) MATHEMATICAL ITALIC SMALL Q → LATIN SMALL LETTER Q#
 {119903, "r"}, // MA# ( 𝑟 → r ) MATHEMATICAL ITALIC SMALL R → LATIN SMALL LETTER R#
 {119904, "s"}, // MA# ( 𝑠 → s ) MATHEMATICAL ITALIC SMALL S → LATIN SMALL LETTER S#
 {119905, "t"}, // MA# ( 𝑡 → t ) MATHEMATICAL ITALIC SMALL T → LATIN SMALL LETTER T#
 {119906, "u"}, // MA# ( 𝑢 → u ) MATHEMATICAL ITALIC SMALL U → LATIN SMALL LETTER U#
 {119907, "v"}, // MA# ( 𝑣 → v ) MATHEMATICAL ITALIC SMALL V → LATIN SMALL LETTER V#
 {119908, "w"}, // MA# ( 𝑤 → w ) MATHEMATICAL ITALIC SMALL W → LATIN SMALL LETTER W#
 {119909, "x"}, // MA# ( 𝑥 → x ) MATHEMATICAL ITALIC SMALL X → LATIN SMALL LETTER X#
 {119910, "y"}, // MA# ( 𝑦 → y ) MATHEMATICAL ITALIC SMALL Y → LATIN SMALL LETTER Y#
 {119911, "z"}, // MA# ( 𝑧 → z ) MATHEMATICAL ITALIC SMALL Z → LATIN SMALL LETTER Z#
 {119912, "A"}, // MA# ( 𝑨 → A ) MATHEMATICAL BOLD ITALIC CAPITAL A → LATIN CAPITAL LETTER A#
 {119913, "B"}, // MA# ( 𝑩 → B ) MATHEMATICAL BOLD ITALIC CAPITAL B → LATIN CAPITAL LETTER B#
 {119914, "C"}, // MA# ( 𝑪 → C ) MATHEMATICAL BOLD ITALIC CAPITAL C → LATIN CAPITAL LETTER C#
 {119915, "D"}, // MA# ( 𝑫 → D ) MATHEMATICAL BOLD ITALIC CAPITAL D → LATIN CAPITAL LETTER D#
 {119916, "E"}, // MA# ( 𝑬 → E ) MATHEMATICAL BOLD ITALIC CAPITAL E → LATIN CAPITAL LETTER E#
 {119917, "F"}, // MA# ( 𝑭 → F ) MATHEMATICAL BOLD ITALIC CAPITAL F → LATIN CAPITAL LETTER F#
 {119918, "G"}, // MA# ( 𝑮 → G ) MATHEMATICAL BOLD ITALIC CAPITAL G → LATIN CAPITAL LETTER G#
 {119919, "H"}, // MA# ( 𝑯 → H ) MATHEMATICAL BOLD ITALIC CAPITAL H → LATIN CAPITAL LETTER H#
 {119920, "l"}, // MA# ( 𝑰 → l ) MATHEMATICAL BOLD ITALIC CAPITAL I → LATIN SMALL LETTER L# →I→
 {119921, "J"}, // MA# ( 𝑱 → J ) MATHEMATICAL BOLD ITALIC CAPITAL J → LATIN CAPITAL LETTER J#
 {119922, "K"}, // MA# ( 𝑲 → K ) MATHEMATICAL BOLD ITALIC CAPITAL K → LATIN CAPITAL LETTER K#
 {119923, "L"}, // MA# ( 𝑳 → L ) MATHEMATICAL BOLD ITALIC CAPITAL L → LATIN CAPITAL LETTER L#
 {119924, "M"}, // MA# ( 𝑴 → M ) MATHEMATICAL BOLD ITALIC CAPITAL M → LATIN CAPITAL LETTER M#
 {119925, "N"}, // MA# ( 𝑵 → N ) MATHEMATICAL BOLD ITALIC CAPITAL N → LATIN CAPITAL LETTER N#
 {119926, "O"}, // MA# ( 𝑶 → O ) MATHEMATICAL BOLD ITALIC CAPITAL O → LATIN CAPITAL LETTER O#
 {119927, "P"}, // MA# ( 𝑷 → P ) MATHEMATICAL BOLD ITALIC CAPITAL P → LATIN CAPITAL LETTER P#
 {119928, "Q"}, // MA# ( 𝑸 → Q ) MATHEMATICAL BOLD ITALIC CAPITAL Q → LATIN CAPITAL LETTER Q#
 {119929, "R"}, // MA# ( 𝑹 → R ) MATHEMATICAL BOLD ITALIC CAPITAL R → LATIN CAPITAL LETTER R#
 {119930, "S"}, // MA# ( 𝑺 → S ) MATHEMATICAL BOLD ITALIC CAPITAL S → LATIN CAPITAL LETTER S#
 {119931, "T"}, // MA# ( 𝑻 → T ) MATHEMATICAL BOLD ITALIC CAPITAL T → LATIN CAPITAL LETTER T#
 {119932, "U"}, // MA# ( 𝑼 → U ) MATHEMATICAL BOLD ITALIC CAPITAL U → LATIN CAPITAL LETTER U#
 {119933, "V"}, // MA# ( 𝑽 → V ) MATHEMATICAL BOLD ITALIC CAPITAL V → LATIN CAPITAL LETTER V#
 {119934, "W"}, // MA# ( 𝑾 → W ) MATHEMATICAL BOLD ITALIC CAPITAL W → LATIN CAPITAL LETTER W#
 {119935, "X"}, // MA# ( 𝑿 → X ) MATHEMATICAL BOLD ITALIC CAPITAL X → LATIN CAPITAL LETTER X#
 {119936, "Y"}, // MA# ( 𝒀 → Y ) MATHEMATICAL BOLD ITALIC CAPITAL Y → LATIN CAPITAL LETTER Y#
 {119937, "Z"}, // MA# ( 𝒁 → Z ) MATHEMATICAL BOLD ITALIC CAPITAL Z → LATIN CAPITAL LETTER Z#
 {119938, "a"}, // MA# ( 𝒂 → a ) MATHEMATICAL BOLD ITALIC SMALL A → LATIN SMALL LETTER A#
 {119939, "b"}, // MA# ( 𝒃 → b ) MATHEMATICAL BOLD ITALIC SMALL B → LATIN SMALL LETTER B#
 {119940, "c"}, // MA# ( 𝒄 → c ) MATHEMATICAL BOLD ITALIC SMALL C → LATIN SMALL LETTER C#
 {119941, "d"}, // MA# ( 𝒅 → d ) MATHEMATICAL BOLD ITALIC SMALL D → LATIN SMALL LETTER D#
 {119942, "e"}, // MA# ( 𝒆 → e ) MATHEMATICAL BOLD ITALIC SMALL E → LATIN SMALL LETTER E#
 {119943, "f"}, // MA# ( 𝒇 → f ) MATHEMATICAL BOLD ITALIC SMALL F → LATIN SMALL LETTER F#
 {119944, "g"}, // MA# ( 𝒈 → g ) MATHEMATICAL BOLD ITALIC SMALL G → LATIN SMALL LETTER G#
 {119945, "h"}, // MA# ( 𝒉 → h ) MATHEMATICAL BOLD ITALIC SMALL H → LATIN SMALL LETTER H#
 {119946, "i"}, // MA# ( 𝒊 → i ) MATHEMATICAL BOLD ITALIC SMALL I → LATIN SMALL LETTER I#
 {119947, "j"}, // MA# ( 𝒋 → j ) MATHEMATICAL BOLD ITALIC SMALL J → LATIN SMALL LETTER J#
 {119948, "k"}, // MA# ( 𝒌 → k ) MATHEMATICAL BOLD ITALIC SMALL K → LATIN SMALL LETTER K#
 {119949, "l"}, // MA# ( 𝒍 → l ) MATHEMATICAL BOLD ITALIC SMALL L → LATIN SMALL LETTER L#
 {119950, "rn"}, // MA# ( 𝒎 → rn ) MATHEMATICAL BOLD ITALIC SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {119951, "n"}, // MA# ( 𝒏 → n ) MATHEMATICAL BOLD ITALIC SMALL N → LATIN SMALL LETTER N#
 {119952, "o"}, // MA# ( 𝒐 → o ) MATHEMATICAL BOLD ITALIC SMALL O → LATIN SMALL LETTER O#
 {119953, "p"}, // MA# ( 𝒑 → p ) MATHEMATICAL BOLD ITALIC SMALL P → LATIN SMALL LETTER P#
 {119954, "q"}, // MA# ( 𝒒 → q ) MATHEMATICAL BOLD ITALIC SMALL Q → LATIN SMALL LETTER Q#
 {119955, "r"}, // MA# ( 𝒓 → r ) MATHEMATICAL BOLD ITALIC SMALL R → LATIN SMALL LETTER R#
 {119956, "s"}, // MA# ( 𝒔 → s ) MATHEMATICAL BOLD ITALIC SMALL S → LATIN SMALL LETTER S#
 {119957, "t"}, // MA# ( 𝒕 → t ) MATHEMATICAL BOLD ITALIC SMALL T → LATIN SMALL LETTER T#
 {119958, "u"}, // MA# ( 𝒖 → u ) MATHEMATICAL BOLD ITALIC SMALL U → LATIN SMALL LETTER U#
 {119959, "v"}, // MA# ( 𝒗 → v ) MATHEMATICAL BOLD ITALIC SMALL V → LATIN SMALL LETTER V#
 {119960, "w"}, // MA# ( 𝒘 → w ) MATHEMATICAL BOLD ITALIC SMALL W → LATIN SMALL LETTER W#
 {119961, "x"}, // MA# ( 𝒙 → x ) MATHEMATICAL BOLD ITALIC SMALL X → LATIN SMALL LETTER X#
 {119962, "y"}, // MA# ( 𝒚 → y ) MATHEMATICAL BOLD ITALIC SMALL Y → LATIN SMALL LETTER Y#
 {119963, "z"}, // MA# ( 𝒛 → z ) MATHEMATICAL BOLD ITALIC SMALL Z → LATIN SMALL LETTER Z#
 {119964, "A"}, // MA# ( 𝒜 → A ) MATHEMATICAL SCRIPT CAPITAL A → LATIN CAPITAL LETTER A#
 {119966, "C"}, // MA# ( 𝒞 → C ) MATHEMATICAL SCRIPT CAPITAL C → LATIN CAPITAL LETTER C#
 {119967, "D"}, // MA# ( 𝒟 → D ) MATHEMATICAL SCRIPT CAPITAL D → LATIN CAPITAL LETTER D#
 {119970, "G"}, // MA# ( 𝒢 → G ) MATHEMATICAL SCRIPT CAPITAL G → LATIN CAPITAL LETTER G#
 {119973, "J"}, // MA# ( 𝒥 → J ) MATHEMATICAL SCRIPT CAPITAL J → LATIN CAPITAL LETTER J#
 {119974, "K"}, // MA# ( 𝒦 → K ) MATHEMATICAL SCRIPT CAPITAL K → LATIN CAPITAL LETTER K#
 {119977, "N"}, // MA# ( 𝒩 → N ) MATHEMATICAL SCRIPT CAPITAL N → LATIN CAPITAL LETTER N#
 {119978, "O"}, // MA# ( 𝒪 → O ) MATHEMATICAL SCRIPT CAPITAL O → LATIN CAPITAL LETTER O#
 {119979, "P"}, // MA# ( 𝒫 → P ) MATHEMATICAL SCRIPT CAPITAL P → LATIN CAPITAL LETTER P#
 {119980, "Q"}, // MA# ( 𝒬 → Q ) MATHEMATICAL SCRIPT CAPITAL Q → LATIN CAPITAL LETTER Q#
 {119982, "S"}, // MA# ( 𝒮 → S ) MATHEMATICAL SCRIPT CAPITAL S → LATIN CAPITAL LETTER S#
 {119983, "T"}, // MA# ( 𝒯 → T ) MATHEMATICAL SCRIPT CAPITAL T → LATIN CAPITAL LETTER T#
 {119984, "U"}, // MA# ( 𝒰 → U ) MATHEMATICAL SCRIPT CAPITAL U → LATIN CAPITAL LETTER U#
 {119985, "V"}, // MA# ( 𝒱 → V ) MATHEMATICAL SCRIPT CAPITAL V → LATIN CAPITAL LETTER V#
 {119986, "W"}, // MA# ( 𝒲 → W ) MATHEMATICAL SCRIPT CAPITAL W → LATIN CAPITAL LETTER W#
 {119987, "X"}, // MA# ( 𝒳 → X ) MATHEMATICAL SCRIPT CAPITAL X → LATIN CAPITAL LETTER X#
 {119988, "Y"}, // MA# ( 𝒴 → Y ) MATHEMATICAL SCRIPT CAPITAL Y → LATIN CAPITAL LETTER Y#
 {119989, "Z"}, // MA# ( 𝒵 → Z ) MATHEMATICAL SCRIPT CAPITAL Z → LATIN CAPITAL LETTER Z#
 {119990, "a"}, // MA# ( 𝒶 → a ) MATHEMATICAL SCRIPT SMALL A → LATIN SMALL LETTER A#
 {119991, "b"}, // MA# ( 𝒷 → b ) MATHEMATICAL SCRIPT SMALL B → LATIN SMALL LETTER B#
 {119992, "c"}, // MA# ( 𝒸 → c ) MATHEMATICAL SCRIPT SMALL C → LATIN SMALL LETTER C#
 {119993, "d"}, // MA# ( 𝒹 → d ) MATHEMATICAL SCRIPT SMALL D → LATIN SMALL LETTER D#
 {119995, "f"}, // MA# ( 𝒻 → f ) MATHEMATICAL SCRIPT SMALL F → LATIN SMALL LETTER F#
 {119997, "h"}, // MA# ( 𝒽 → h ) MATHEMATICAL SCRIPT SMALL H → LATIN SMALL LETTER H#
 {119998, "i"}, // MA# ( 𝒾 → i ) MATHEMATICAL SCRIPT SMALL I → LATIN SMALL LETTER I#
 {119999, "j"}, // MA# ( 𝒿 → j ) MATHEMATICAL SCRIPT SMALL J → LATIN SMALL LETTER J#
 {120000, "k"}, // MA# ( 𝓀 → k ) MATHEMATICAL SCRIPT SMALL K → LATIN SMALL LETTER K#
 {120001, "l"}, // MA# ( 𝓁 → l ) MATHEMATICAL SCRIPT SMALL L → LATIN SMALL LETTER L#
 {120002, "rn"}, // MA# ( 𝓂 → rn ) MATHEMATICAL SCRIPT SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120003, "n"}, // MA# ( 𝓃 → n ) MATHEMATICAL SCRIPT SMALL N → LATIN SMALL LETTER N#
 {120005, "p"}, // MA# ( 𝓅 → p ) MATHEMATICAL SCRIPT SMALL P → LATIN SMALL LETTER P#
 {120006, "q"}, // MA# ( 𝓆 → q ) MATHEMATICAL SCRIPT SMALL Q → LATIN SMALL LETTER Q#
 {120007, "r"}, // MA# ( 𝓇 → r ) MATHEMATICAL SCRIPT SMALL R → LATIN SMALL LETTER R#
 {120008, "s"}, // MA# ( 𝓈 → s ) MATHEMATICAL SCRIPT SMALL S → LATIN SMALL LETTER S#
 {120009, "t"}, // MA# ( 𝓉 → t ) MATHEMATICAL SCRIPT SMALL T → LATIN SMALL LETTER T#
 {120010, "u"}, // MA# ( 𝓊 → u ) MATHEMATICAL SCRIPT SMALL U → LATIN SMALL LETTER U#
 {120011, "v"}, // MA# ( 𝓋 → v ) MATHEMATICAL SCRIPT SMALL V → LATIN SMALL LETTER V#
 {120012, "w"}, // MA# ( 𝓌 → w ) MATHEMATICAL SCRIPT SMALL W → LATIN SMALL LETTER W#
 {120013, "x"}, // MA# ( 𝓍 → x ) MATHEMATICAL SCRIPT SMALL X → LATIN SMALL LETTER X#
 {120014, "y"}, // MA# ( 𝓎 → y ) MATHEMATICAL SCRIPT SMALL Y → LATIN SMALL LETTER Y#
 {120015, "z"}, // MA# ( 𝓏 → z ) MATHEMATICAL SCRIPT SMALL Z → LATIN SMALL LETTER Z#
 {120016, "A"}, // MA# ( 𝓐 → A ) MATHEMATICAL BOLD SCRIPT CAPITAL A → LATIN CAPITAL LETTER A#
 {120017, "B"}, // MA# ( 𝓑 → B ) MATHEMATICAL BOLD SCRIPT CAPITAL B → LATIN CAPITAL LETTER B#
 {120018, "C"}, // MA# ( 𝓒 → C ) MATHEMATICAL BOLD SCRIPT CAPITAL C → LATIN CAPITAL LETTER C#
 {120019, "D"}, // MA# ( 𝓓 → D ) MATHEMATICAL BOLD SCRIPT CAPITAL D → LATIN CAPITAL LETTER D#
 {120020, "E"}, // MA# ( 𝓔 → E ) MATHEMATICAL BOLD SCRIPT CAPITAL E → LATIN CAPITAL LETTER E#
 {120021, "F"}, // MA# ( 𝓕 → F ) MATHEMATICAL BOLD SCRIPT CAPITAL F → LATIN CAPITAL LETTER F#
 {120022, "G"}, // MA# ( 𝓖 → G ) MATHEMATICAL BOLD SCRIPT CAPITAL G → LATIN CAPITAL LETTER G#
 {120023, "H"}, // MA# ( 𝓗 → H ) MATHEMATICAL BOLD SCRIPT CAPITAL H → LATIN CAPITAL LETTER H#
 {120024, "l"}, // MA# ( 𝓘 → l ) MATHEMATICAL BOLD SCRIPT CAPITAL I → LATIN SMALL LETTER L# →I→
 {120025, "J"}, // MA# ( 𝓙 → J ) MATHEMATICAL BOLD SCRIPT CAPITAL J → LATIN CAPITAL LETTER J#
 {120026, "K"}, // MA# ( 𝓚 → K ) MATHEMATICAL BOLD SCRIPT CAPITAL K → LATIN CAPITAL LETTER K#
 {120027, "L"}, // MA# ( 𝓛 → L ) MATHEMATICAL BOLD SCRIPT CAPITAL L → LATIN CAPITAL LETTER L#
 {120028, "M"}, // MA# ( 𝓜 → M ) MATHEMATICAL BOLD SCRIPT CAPITAL M → LATIN CAPITAL LETTER M#
 {120029, "N"}, // MA# ( 𝓝 → N ) MATHEMATICAL BOLD SCRIPT CAPITAL N → LATIN CAPITAL LETTER N#
 {120030, "O"}, // MA# ( 𝓞 → O ) MATHEMATICAL BOLD SCRIPT CAPITAL O → LATIN CAPITAL LETTER O#
 {120031, "P"}, // MA# ( 𝓟 → P ) MATHEMATICAL BOLD SCRIPT CAPITAL P → LATIN CAPITAL LETTER P#
 {120032, "Q"}, // MA# ( 𝓠 → Q ) MATHEMATICAL BOLD SCRIPT CAPITAL Q → LATIN CAPITAL LETTER Q#
 {120033, "R"}, // MA# ( 𝓡 → R ) MATHEMATICAL BOLD SCRIPT CAPITAL R → LATIN CAPITAL LETTER R#
 {120034, "S"}, // MA# ( 𝓢 → S ) MATHEMATICAL BOLD SCRIPT CAPITAL S → LATIN CAPITAL LETTER S#
 {120035, "T"}, // MA# ( 𝓣 → T ) MATHEMATICAL BOLD SCRIPT CAPITAL T → LATIN CAPITAL LETTER T#
 {120036, "U"}, // MA# ( 𝓤 → U ) MATHEMATICAL BOLD SCRIPT CAPITAL U → LATIN CAPITAL LETTER U#
 {120037, "V"}, // MA# ( 𝓥 → V ) MATHEMATICAL BOLD SCRIPT CAPITAL V → LATIN CAPITAL LETTER V#
 {120038, "W"}, // MA# ( 𝓦 → W ) MATHEMATICAL BOLD SCRIPT CAPITAL W → LATIN CAPITAL LETTER W#
 {120039, "X"}, // MA# ( 𝓧 → X ) MATHEMATICAL BOLD SCRIPT CAPITAL X → LATIN CAPITAL LETTER X#
 {120040, "Y"}, // MA# ( 𝓨 → Y ) MATHEMATICAL BOLD SCRIPT CAPITAL Y → LATIN CAPITAL LETTER Y#
 {120041, "Z"}, // MA# ( 𝓩 → Z ) MATHEMATICAL BOLD SCRIPT CAPITAL Z → LATIN CAPITAL LETTER Z#
 {120042, "a"}, // MA# ( 𝓪 → a ) MATHEMATICAL BOLD SCRIPT SMALL A → LATIN SMALL LETTER A#
 {120043, "b"}, // MA# ( 𝓫 → b ) MATHEMATICAL BOLD SCRIPT SMALL B → LATIN SMALL LETTER B#
 {120044, "c"}, // MA# ( 𝓬 → c ) MATHEMATICAL BOLD SCRIPT SMALL C → LATIN SMALL LETTER C#
 {120045, "d"}, // MA# ( 𝓭 → d ) MATHEMATICAL BOLD SCRIPT SMALL D → LATIN SMALL LETTER D#
 {120046, "e"}, // MA# ( 𝓮 → e ) MATHEMATICAL BOLD SCRIPT SMALL E → LATIN SMALL LETTER E#
 {120047, "f"}, // MA# ( 𝓯 → f ) MATHEMATICAL BOLD SCRIPT SMALL F → LATIN SMALL LETTER F#
 {120048, "g"}, // MA# ( 𝓰 → g ) MATHEMATICAL BOLD SCRIPT SMALL G → LATIN SMALL LETTER G#
 {120049, "h"}, // MA# ( 𝓱 → h ) MATHEMATICAL BOLD SCRIPT SMALL H → LATIN SMALL LETTER H#
 {120050, "i"}, // MA# ( 𝓲 → i ) MATHEMATICAL BOLD SCRIPT SMALL I → LATIN SMALL LETTER I#
 {120051, "j"}, // MA# ( 𝓳 → j ) MATHEMATICAL BOLD SCRIPT SMALL J → LATIN SMALL LETTER J#
 {120052, "k"}, // MA# ( 𝓴 → k ) MATHEMATICAL BOLD SCRIPT SMALL K → LATIN SMALL LETTER K#
 {120053, "l"}, // MA# ( 𝓵 → l ) MATHEMATICAL BOLD SCRIPT SMALL L → LATIN SMALL LETTER L#
 {120054, "rn"}, // MA# ( 𝓶 → rn ) MATHEMATICAL BOLD SCRIPT SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120055, "n"}, // MA# ( 𝓷 → n ) MATHEMATICAL BOLD SCRIPT SMALL N → LATIN SMALL LETTER N#
 {120056, "o"}, // MA# ( 𝓸 → o ) MATHEMATICAL BOLD SCRIPT SMALL O → LATIN SMALL LETTER O#
 {120057, "p"}, // MA# ( 𝓹 → p ) MATHEMATICAL BOLD SCRIPT SMALL P → LATIN SMALL LETTER P#
 {120058, "q"}, // MA# ( 𝓺 → q ) MATHEMATICAL BOLD SCRIPT SMALL Q → LATIN SMALL LETTER Q#
 {120059, "r"}, // MA# ( 𝓻 → r ) MATHEMATICAL BOLD SCRIPT SMALL R → LATIN SMALL LETTER R#
 {120060, "s"}, // MA# ( 𝓼 → s ) MATHEMATICAL BOLD SCRIPT SMALL S → LATIN SMALL LETTER S#
 {120061, "t"}, // MA# ( 𝓽 → t ) MATHEMATICAL BOLD SCRIPT SMALL T → LATIN SMALL LETTER T#
 {120062, "u"}, // MA# ( 𝓾 → u ) MATHEMATICAL BOLD SCRIPT SMALL U → LATIN SMALL LETTER U#
 {120063, "v"}, // MA# ( 𝓿 → v ) MATHEMATICAL BOLD SCRIPT SMALL V → LATIN SMALL LETTER V#
 {120064, "w"}, // MA# ( 𝔀 → w ) MATHEMATICAL BOLD SCRIPT SMALL W → LATIN SMALL LETTER W#
 {120065, "x"}, // MA# ( 𝔁 → x ) MATHEMATICAL BOLD SCRIPT SMALL X → LATIN SMALL LETTER X#
 {120066, "y"}, // MA# ( 𝔂 → y ) MATHEMATICAL BOLD SCRIPT SMALL Y → LATIN SMALL LETTER Y#
 {120067, "z"}, // MA# ( 𝔃 → z ) MATHEMATICAL BOLD SCRIPT SMALL Z → LATIN SMALL LETTER Z#
 {120068, "A"}, // MA# ( 𝔄 → A ) MATHEMATICAL FRAKTUR CAPITAL A → LATIN CAPITAL LETTER A#
 {120069, "B"}, // MA# ( 𝔅 → B ) MATHEMATICAL FRAKTUR CAPITAL B → LATIN CAPITAL LETTER B#
 {120071, "D"}, // MA# ( 𝔇 → D ) MATHEMATICAL FRAKTUR CAPITAL D → LATIN CAPITAL LETTER D#
 {120072, "E"}, // MA# ( 𝔈 → E ) MATHEMATICAL FRAKTUR CAPITAL E → LATIN CAPITAL LETTER E#
 {120073, "F"}, // MA# ( 𝔉 → F ) MATHEMATICAL FRAKTUR CAPITAL F → LATIN CAPITAL LETTER F#
 {120074, "G"}, // MA# ( 𝔊 → G ) MATHEMATICAL FRAKTUR CAPITAL G → LATIN CAPITAL LETTER G#
 {120077, "J"}, // MA# ( 𝔍 → J ) MATHEMATICAL FRAKTUR CAPITAL J → LATIN CAPITAL LETTER J#
 {120078, "K"}, // MA# ( 𝔎 → K ) MATHEMATICAL FRAKTUR CAPITAL K → LATIN CAPITAL LETTER K#
 {120079, "L"}, // MA# ( 𝔏 → L ) MATHEMATICAL FRAKTUR CAPITAL L → LATIN CAPITAL LETTER L#
 {120080, "M"}, // MA# ( 𝔐 → M ) MATHEMATICAL FRAKTUR CAPITAL M → LATIN CAPITAL LETTER M#
 {120081, "N"}, // MA# ( 𝔑 → N ) MATHEMATICAL FRAKTUR CAPITAL N → LATIN CAPITAL LETTER N#
 {120082, "O"}, // MA# ( 𝔒 → O ) MATHEMATICAL FRAKTUR CAPITAL O → LATIN CAPITAL LETTER O#
 {120083, "P"}, // MA# ( 𝔓 → P ) MATHEMATICAL FRAKTUR CAPITAL P → LATIN CAPITAL LETTER P#
 {120084, "Q"}, // MA# ( 𝔔 → Q ) MATHEMATICAL FRAKTUR CAPITAL Q → LATIN CAPITAL LETTER Q#
 {120086, "S"}, // MA# ( 𝔖 → S ) MATHEMATICAL FRAKTUR CAPITAL S → LATIN CAPITAL LETTER S#
 {120087, "T"}, // MA# ( 𝔗 → T ) MATHEMATICAL FRAKTUR CAPITAL T → LATIN CAPITAL LETTER T#
 {120088, "U"}, // MA# ( 𝔘 → U ) MATHEMATICAL FRAKTUR CAPITAL U → LATIN CAPITAL LETTER U#
 {120089, "V"}, // MA# ( 𝔙 → V ) MATHEMATICAL FRAKTUR CAPITAL V → LATIN CAPITAL LETTER V#
 {120090, "W"}, // MA# ( 𝔚 → W ) MATHEMATICAL FRAKTUR CAPITAL W → LATIN CAPITAL LETTER W#
 {120091, "X"}, // MA# ( 𝔛 → X ) MATHEMATICAL FRAKTUR CAPITAL X → LATIN CAPITAL LETTER X#
 {120092, "Y"}, // MA# ( 𝔜 → Y ) MATHEMATICAL FRAKTUR CAPITAL Y → LATIN CAPITAL LETTER Y#
 {120094, "a"}, // MA# ( 𝔞 → a ) MATHEMATICAL FRAKTUR SMALL A → LATIN SMALL LETTER A#
 {120095, "b"}, // MA# ( 𝔟 → b ) MATHEMATICAL FRAKTUR SMALL B → LATIN SMALL LETTER B#
 {120096, "c"}, // MA# ( 𝔠 → c ) MATHEMATICAL FRAKTUR SMALL C → LATIN SMALL LETTER C#
 {120097, "d"}, // MA# ( 𝔡 → d ) MATHEMATICAL FRAKTUR SMALL D → LATIN SMALL LETTER D#
 {120098, "e"}, // MA# ( 𝔢 → e ) MATHEMATICAL FRAKTUR SMALL E → LATIN SMALL LETTER E#
 {120099, "f"}, // MA# ( 𝔣 → f ) MATHEMATICAL FRAKTUR SMALL F → LATIN SMALL LETTER F#
 {120100, "g"}, // MA# ( 𝔤 → g ) MATHEMATICAL FRAKTUR SMALL G → LATIN SMALL LETTER G#
 {120101, "h"}, // MA# ( 𝔥 → h ) MATHEMATICAL FRAKTUR SMALL H → LATIN SMALL LETTER H#
 {120102, "i"}, // MA# ( 𝔦 → i ) MATHEMATICAL FRAKTUR SMALL I → LATIN SMALL LETTER I#
 {120103, "j"}, // MA# ( 𝔧 → j ) MATHEMATICAL FRAKTUR SMALL J → LATIN SMALL LETTER J#
 {120104, "k"}, // MA# ( 𝔨 → k ) MATHEMATICAL FRAKTUR SMALL K → LATIN SMALL LETTER K#
 {120105, "l"}, // MA# ( 𝔩 → l ) MATHEMATICAL FRAKTUR SMALL L → LATIN SMALL LETTER L#
 {120106, "rn"}, // MA# ( 𝔪 → rn ) MATHEMATICAL FRAKTUR SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120107, "n"}, // MA# ( 𝔫 → n ) MATHEMATICAL FRAKTUR SMALL N → LATIN SMALL LETTER N#
 {120108, "o"}, // MA# ( 𝔬 → o ) MATHEMATICAL FRAKTUR SMALL O → LATIN SMALL LETTER O#
 {120109, "p"}, // MA# ( 𝔭 → p ) MATHEMATICAL FRAKTUR SMALL P → LATIN SMALL LETTER P#
 {120110, "q"}, // MA# ( 𝔮 → q ) MATHEMATICAL FRAKTUR SMALL Q → LATIN SMALL LETTER Q#
 {120111, "r"}, // MA# ( 𝔯 → r ) MATHEMATICAL FRAKTUR SMALL R → LATIN SMALL LETTER R#
 {120112, "s"}, // MA# ( 𝔰 → s ) MATHEMATICAL FRAKTUR SMALL S → LATIN SMALL LETTER S#
 {120113, "t"}, // MA# ( 𝔱 → t ) MATHEMATICAL FRAKTUR SMALL T → LATIN SMALL LETTER T#
 {120114, "u"}, // MA# ( 𝔲 → u ) MATHEMATICAL FRAKTUR SMALL U → LATIN SMALL LETTER U#
 {120115, "v"}, // MA# ( 𝔳 → v ) MATHEMATICAL FRAKTUR SMALL V → LATIN SMALL LETTER V#
 {120116, "w"}, // MA# ( 𝔴 → w ) MATHEMATICAL FRAKTUR SMALL W → LATIN SMALL LETTER W#
 {120117, "x"}, // MA# ( 𝔵 → x ) MATHEMATICAL FRAKTUR SMALL X → LATIN SMALL LETTER X#
 {120118, "y"}, // MA# ( 𝔶 → y ) MATHEMATICAL FRAKTUR SMALL Y → LATIN SMALL LETTER Y#
 {120119, "z"}, // MA# ( 𝔷 → z ) MATHEMATICAL FRAKTUR SMALL Z → LATIN SMALL LETTER Z#
 {120120, "A"}, // MA# ( 𝔸 → A ) MATHEMATICAL DOUBLE-STRUCK CAPITAL A → LATIN CAPITAL LETTER A#
 {120121, "B"}, // MA# ( 𝔹 → B ) MATHEMATICAL DOUBLE-STRUCK CAPITAL B → LATIN CAPITAL LETTER B#
 {120123, "D"}, // MA# ( 𝔻 → D ) MATHEMATICAL DOUBLE-STRUCK CAPITAL D → LATIN CAPITAL LETTER D#
 {120124, "E"}, // MA# ( 𝔼 → E ) MATHEMATICAL DOUBLE-STRUCK CAPITAL E → LATIN CAPITAL LETTER E#
 {120125, "F"}, // MA# ( 𝔽 → F ) MATHEMATICAL DOUBLE-STRUCK CAPITAL F → LATIN CAPITAL LETTER F#
 {120126, "G"}, // MA# ( 𝔾 → G ) MATHEMATICAL DOUBLE-STRUCK CAPITAL G → LATIN CAPITAL LETTER G#
 {120128, "l"}, // MA# ( 𝕀 → l ) MATHEMATICAL DOUBLE-STRUCK CAPITAL I → LATIN SMALL LETTER L# →I→
 {120129, "J"}, // MA# ( 𝕁 → J ) MATHEMATICAL DOUBLE-STRUCK CAPITAL J → LATIN CAPITAL LETTER J#
 {120130, "K"}, // MA# ( 𝕂 → K ) MATHEMATICAL DOUBLE-STRUCK CAPITAL K → LATIN CAPITAL LETTER K#
 {120131, "L"}, // MA# ( 𝕃 → L ) MATHEMATICAL DOUBLE-STRUCK CAPITAL L → LATIN CAPITAL LETTER L#
 {120132, "M"}, // MA# ( 𝕄 → M ) MATHEMATICAL DOUBLE-STRUCK CAPITAL M → LATIN CAPITAL LETTER M#
 {120134, "O"}, // MA# ( 𝕆 → O ) MATHEMATICAL DOUBLE-STRUCK CAPITAL O → LATIN CAPITAL LETTER O#
 {120138, "S"}, // MA# ( 𝕊 → S ) MATHEMATICAL DOUBLE-STRUCK CAPITAL S → LATIN CAPITAL LETTER S#
 {120139, "T"}, // MA# ( 𝕋 → T ) MATHEMATICAL DOUBLE-STRUCK CAPITAL T → LATIN CAPITAL LETTER T#
 {120140, "U"}, // MA# ( 𝕌 → U ) MATHEMATICAL DOUBLE-STRUCK CAPITAL U → LATIN CAPITAL LETTER U#
 {120141, "V"}, // MA# ( 𝕍 → V ) MATHEMATICAL DOUBLE-STRUCK CAPITAL V → LATIN CAPITAL LETTER V#
 {120142, "W"}, // MA# ( 𝕎 → W ) MATHEMATICAL DOUBLE-STRUCK CAPITAL W → LATIN CAPITAL LETTER W#
 {120143, "X"}, // MA# ( 𝕏 → X ) MATHEMATICAL DOUBLE-STRUCK CAPITAL X → LATIN CAPITAL LETTER X#
 {120144, "Y"}, // MA# ( 𝕐 → Y ) MATHEMATICAL DOUBLE-STRUCK CAPITAL Y → LATIN CAPITAL LETTER Y#
 {120146, "a"}, // MA# ( 𝕒 → a ) MATHEMATICAL DOUBLE-STRUCK SMALL A → LATIN SMALL LETTER A#
 {120147, "b"}, // MA# ( 𝕓 → b ) MATHEMATICAL DOUBLE-STRUCK SMALL B → LATIN SMALL LETTER B#
 {120148, "c"}, // MA# ( 𝕔 → c ) MATHEMATICAL DOUBLE-STRUCK SMALL C → LATIN SMALL LETTER C#
 {120149, "d"}, // MA# ( 𝕕 → d ) MATHEMATICAL DOUBLE-STRUCK SMALL D → LATIN SMALL LETTER D#
 {120150, "e"}, // MA# ( 𝕖 → e ) MATHEMATICAL DOUBLE-STRUCK SMALL E → LATIN SMALL LETTER E#
 {120151, "f"}, // MA# ( 𝕗 → f ) MATHEMATICAL DOUBLE-STRUCK SMALL F → LATIN SMALL LETTER F#
 {120152, "g"}, // MA# ( 𝕘 → g ) MATHEMATICAL DOUBLE-STRUCK SMALL G → LATIN SMALL LETTER G#
 {120153, "h"}, // MA# ( 𝕙 → h ) MATHEMATICAL DOUBLE-STRUCK SMALL H → LATIN SMALL LETTER H#
 {120154, "i"}, // MA# ( 𝕚 → i ) MATHEMATICAL DOUBLE-STRUCK SMALL I → LATIN SMALL LETTER I#
 {120155, "j"}, // MA# ( 𝕛 → j ) MATHEMATICAL DOUBLE-STRUCK SMALL J → LATIN SMALL LETTER J#
 {120156, "k"}, // MA# ( 𝕜 → k ) MATHEMATICAL DOUBLE-STRUCK SMALL K → LATIN SMALL LETTER K#
 {120157, "l"}, // MA# ( 𝕝 → l ) MATHEMATICAL DOUBLE-STRUCK SMALL L → LATIN SMALL LETTER L#
 {120158, "rn"}, // MA# ( 𝕞 → rn ) MATHEMATICAL DOUBLE-STRUCK SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120159, "n"}, // MA# ( 𝕟 → n ) MATHEMATICAL DOUBLE-STRUCK SMALL N → LATIN SMALL LETTER N#
 {120160, "o"}, // MA# ( 𝕠 → o ) MATHEMATICAL DOUBLE-STRUCK SMALL O → LATIN SMALL LETTER O#
 {120161, "p"}, // MA# ( 𝕡 → p ) MATHEMATICAL DOUBLE-STRUCK SMALL P → LATIN SMALL LETTER P#
 {120162, "q"}, // MA# ( 𝕢 → q ) MATHEMATICAL DOUBLE-STRUCK SMALL Q → LATIN SMALL LETTER Q#
 {120163, "r"}, // MA# ( 𝕣 → r ) MATHEMATICAL DOUBLE-STRUCK SMALL R → LATIN SMALL LETTER R#
 {120164, "s"}, // MA# ( 𝕤 → s ) MATHEMATICAL DOUBLE-STRUCK SMALL S → LATIN SMALL LETTER S#
 {120165, "t"}, // MA# ( 𝕥 → t ) MATHEMATICAL DOUBLE-STRUCK SMALL T → LATIN SMALL LETTER T#
 {120166, "u"}, // MA# ( 𝕦 → u ) MATHEMATICAL DOUBLE-STRUCK SMALL U → LATIN SMALL LETTER U#
 {120167, "v"}, // MA# ( 𝕧 → v ) MATHEMATICAL DOUBLE-STRUCK SMALL V → LATIN SMALL LETTER V#
 {120168, "w"}, // MA# ( 𝕨 → w ) MATHEMATICAL DOUBLE-STRUCK SMALL W → LATIN SMALL LETTER W#
 {120169, "x"}, // MA# ( 𝕩 → x ) MATHEMATICAL DOUBLE-STRUCK SMALL X → LATIN SMALL LETTER X#
 {120170, "y"}, // MA# ( 𝕪 → y ) MATHEMATICAL DOUBLE-STRUCK SMALL Y → LATIN SMALL LETTER Y#
 {120171, "z"}, // MA# ( 𝕫 → z ) MATHEMATICAL DOUBLE-STRUCK SMALL Z → LATIN SMALL LETTER Z#
 {120172, "A"}, // MA# ( 𝕬 → A ) MATHEMATICAL BOLD FRAKTUR CAPITAL A → LATIN CAPITAL LETTER A#
 {120173, "B"}, // MA# ( 𝕭 → B ) MATHEMATICAL BOLD FRAKTUR CAPITAL B → LATIN CAPITAL LETTER B#
 {120174, "C"}, // MA# ( 𝕮 → C ) MATHEMATICAL BOLD FRAKTUR CAPITAL C → LATIN CAPITAL LETTER C#
 {120175, "D"}, // MA# ( 𝕯 → D ) MATHEMATICAL BOLD FRAKTUR CAPITAL D → LATIN CAPITAL LETTER D#
 {120176, "E"}, // MA# ( 𝕰 → E ) MATHEMATICAL BOLD FRAKTUR CAPITAL E → LATIN CAPITAL LETTER E#
 {120177, "F"}, // MA# ( 𝕱 → F ) MATHEMATICAL BOLD FRAKTUR CAPITAL F → LATIN CAPITAL LETTER F#
 {120178, "G"}, // MA# ( 𝕲 → G ) MATHEMATICAL BOLD FRAKTUR CAPITAL G → LATIN CAPITAL LETTER G#
 {120179, "H"}, // MA# ( 𝕳 → H ) MATHEMATICAL BOLD FRAKTUR CAPITAL H → LATIN CAPITAL LETTER H#
 {120180, "l"}, // MA# ( 𝕴 → l ) MATHEMATICAL BOLD FRAKTUR CAPITAL I → LATIN SMALL LETTER L# →I→
 {120181, "J"}, // MA# ( 𝕵 → J ) MATHEMATICAL BOLD FRAKTUR CAPITAL J → LATIN CAPITAL LETTER J#
 {120182, "K"}, // MA# ( 𝕶 → K ) MATHEMATICAL BOLD FRAKTUR CAPITAL K → LATIN CAPITAL LETTER K#
 {120183, "L"}, // MA# ( 𝕷 → L ) MATHEMATICAL BOLD FRAKTUR CAPITAL L → LATIN CAPITAL LETTER L#
 {120184, "M"}, // MA# ( 𝕸 → M ) MATHEMATICAL BOLD FRAKTUR CAPITAL M → LATIN CAPITAL LETTER M#
 {120185, "N"}, // MA# ( 𝕹 → N ) MATHEMATICAL BOLD FRAKTUR CAPITAL N → LATIN CAPITAL LETTER N#
 {120186, "O"}, // MA# ( 𝕺 → O ) MATHEMATICAL BOLD FRAKTUR CAPITAL O → LATIN CAPITAL LETTER O#
 {120187, "P"}, // MA# ( 𝕻 → P ) MATHEMATICAL BOLD FRAKTUR CAPITAL P → LATIN CAPITAL LETTER P#
 {120188, "Q"}, // MA# ( 𝕼 → Q ) MATHEMATICAL BOLD FRAKTUR CAPITAL Q → LATIN CAPITAL LETTER Q#
 {120189, "R"}, // MA# ( 𝕽 → R ) MATHEMATICAL BOLD FRAKTUR CAPITAL R → LATIN CAPITAL LETTER R#
 {120190, "S"}, // MA# ( 𝕾 → S ) MATHEMATICAL BOLD FRAKTUR CAPITAL S → LATIN CAPITAL LETTER S#
 {120191, "T"}, // MA# ( 𝕿 → T ) MATHEMATICAL BOLD FRAKTUR CAPITAL T → LATIN CAPITAL LETTER T#
 {120192, "U"}, // MA# ( 𝖀 → U ) MATHEMATICAL BOLD FRAKTUR CAPITAL U → LATIN CAPITAL LETTER U#
 {120193, "V"}, // MA# ( 𝖁 → V ) MATHEMATICAL BOLD FRAKTUR CAPITAL V → LATIN CAPITAL LETTER V#
 {120194, "W"}, // MA# ( 𝖂 → W ) MATHEMATICAL BOLD FRAKTUR CAPITAL W → LATIN CAPITAL LETTER W#
 {120195, "X"}, // MA# ( 𝖃 → X ) MATHEMATICAL BOLD FRAKTUR CAPITAL X → LATIN CAPITAL LETTER X#
 {120196, "Y"}, // MA# ( 𝖄 → Y ) MATHEMATICAL BOLD FRAKTUR CAPITAL Y → LATIN CAPITAL LETTER Y#
 {120197, "Z"}, // MA# ( 𝖅 → Z ) MATHEMATICAL BOLD FRAKTUR CAPITAL Z → LATIN CAPITAL LETTER Z#
 {120198, "a"}, // MA# ( 𝖆 → a ) MATHEMATICAL BOLD FRAKTUR SMALL A → LATIN SMALL LETTER A#
 {120199, "b"}, // MA# ( 𝖇 → b ) MATHEMATICAL BOLD FRAKTUR SMALL B → LATIN SMALL LETTER B#
 {120200, "c"}, // MA# ( 𝖈 → c ) MATHEMATICAL BOLD FRAKTUR SMALL C → LATIN SMALL LETTER C#
 {120201, "d"}, // MA# ( 𝖉 → d ) MATHEMATICAL BOLD FRAKTUR SMALL D → LATIN SMALL LETTER D#
 {120202, "e"}, // MA# ( 𝖊 → e ) MATHEMATICAL BOLD FRAKTUR SMALL E → LATIN SMALL LETTER E#
 {120203, "f"}, // MA# ( 𝖋 → f ) MATHEMATICAL BOLD FRAKTUR SMALL F → LATIN SMALL LETTER F#
 {120204, "g"}, // MA# ( 𝖌 → g ) MATHEMATICAL BOLD FRAKTUR SMALL G → LATIN SMALL LETTER G#
 {120205, "h"}, // MA# ( 𝖍 → h ) MATHEMATICAL BOLD FRAKTUR SMALL H → LATIN SMALL LETTER H#
 {120206, "i"}, // MA# ( 𝖎 → i ) MATHEMATICAL BOLD FRAKTUR SMALL I → LATIN SMALL LETTER I#
 {120207, "j"}, // MA# ( 𝖏 → j ) MATHEMATICAL BOLD FRAKTUR SMALL J → LATIN SMALL LETTER J#
 {120208, "k"}, // MA# ( 𝖐 → k ) MATHEMATICAL BOLD FRAKTUR SMALL K → LATIN SMALL LETTER K#
 {120209, "l"}, // MA# ( 𝖑 → l ) MATHEMATICAL BOLD FRAKTUR SMALL L → LATIN SMALL LETTER L#
 {120210, "rn"}, // MA# ( 𝖒 → rn ) MATHEMATICAL BOLD FRAKTUR SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120211, "n"}, // MA# ( 𝖓 → n ) MATHEMATICAL BOLD FRAKTUR SMALL N → LATIN SMALL LETTER N#
 {120212, "o"}, // MA# ( 𝖔 → o ) MATHEMATICAL BOLD FRAKTUR SMALL O → LATIN SMALL LETTER O#
 {120213, "p"}, // MA# ( 𝖕 → p ) MATHEMATICAL BOLD FRAKTUR SMALL P → LATIN SMALL LETTER P#
 {120214, "q"}, // MA# ( 𝖖 → q ) MATHEMATICAL BOLD FRAKTUR SMALL Q → LATIN SMALL LETTER Q#
 {120215, "r"}, // MA# ( 𝖗 → r ) MATHEMATICAL BOLD FRAKTUR SMALL R → LATIN SMALL LETTER R#
 {120216, "s"}, // MA# ( 𝖘 → s ) MATHEMATICAL BOLD FRAKTUR SMALL S → LATIN SMALL LETTER S#
 {120217, "t"}, // MA# ( 𝖙 → t ) MATHEMATICAL BOLD FRAKTUR SMALL T → LATIN SMALL LETTER T#
 {120218, "u"}, // MA# ( 𝖚 → u ) MATHEMATICAL BOLD FRAKTUR SMALL U → LATIN SMALL LETTER U#
 {120219, "v"}, // MA# ( 𝖛 → v ) MATHEMATICAL BOLD FRAKTUR SMALL V → LATIN SMALL LETTER V#
 {120220, "w"}, // MA# ( 𝖜 → w ) MATHEMATICAL BOLD FRAKTUR SMALL W → LATIN SMALL LETTER W#
 {120221, "x"}, // MA# ( 𝖝 → x ) MATHEMATICAL BOLD FRAKTUR SMALL X → LATIN SMALL LETTER X#
 {120222, "y"}, // MA# ( 𝖞 → y ) MATHEMATICAL BOLD FRAKTUR SMALL Y → LATIN SMALL LETTER Y#
 {120223, "z"}, // MA# ( 𝖟 → z ) MATHEMATICAL BOLD FRAKTUR SMALL Z → LATIN SMALL LETTER Z#
 {120224, "A"}, // MA# ( 𝖠 → A ) MATHEMATICAL SANS-SERIF CAPITAL A → LATIN CAPITAL LETTER A#
 {120225, "B"}, // MA# ( 𝖡 → B ) MATHEMATICAL SANS-SERIF CAPITAL B → LATIN CAPITAL LETTER B#
 {120226, "C"}, // MA# ( 𝖢 → C ) MATHEMATICAL SANS-SERIF CAPITAL C → LATIN CAPITAL LETTER C#
 {120227, "D"}, // MA# ( 𝖣 → D ) MATHEMATICAL SANS-SERIF CAPITAL D → LATIN CAPITAL LETTER D#
 {120228, "E"}, // MA# ( 𝖤 → E ) MATHEMATICAL SANS-SERIF CAPITAL E → LATIN CAPITAL LETTER E#
 {120229, "F"}, // MA# ( 𝖥 → F ) MATHEMATICAL SANS-SERIF CAPITAL F → LATIN CAPITAL LETTER F#
 {120230, "G"}, // MA# ( 𝖦 → G ) MATHEMATICAL SANS-SERIF CAPITAL G → LATIN CAPITAL LETTER G#
 {120231, "H"}, // MA# ( 𝖧 → H ) MATHEMATICAL SANS-SERIF CAPITAL H → LATIN CAPITAL LETTER H#
 {120232, "l"}, // MA# ( 𝖨 → l ) MATHEMATICAL SANS-SERIF CAPITAL I → LATIN SMALL LETTER L# →I→
 {120233, "J"}, // MA# ( 𝖩 → J ) MATHEMATICAL SANS-SERIF CAPITAL J → LATIN CAPITAL LETTER J#
 {120234, "K"}, // MA# ( 𝖪 → K ) MATHEMATICAL SANS-SERIF CAPITAL K → LATIN CAPITAL LETTER K#
 {120235, "L"}, // MA# ( 𝖫 → L ) MATHEMATICAL SANS-SERIF CAPITAL L → LATIN CAPITAL LETTER L#
 {120236, "M"}, // MA# ( 𝖬 → M ) MATHEMATICAL SANS-SERIF CAPITAL M → LATIN CAPITAL LETTER M#
 {120237, "N"}, // MA# ( 𝖭 → N ) MATHEMATICAL SANS-SERIF CAPITAL N → LATIN CAPITAL LETTER N#
 {120238, "O"}, // MA# ( 𝖮 → O ) MATHEMATICAL SANS-SERIF CAPITAL O → LATIN CAPITAL LETTER O#
 {120239, "P"}, // MA# ( 𝖯 → P ) MATHEMATICAL SANS-SERIF CAPITAL P → LATIN CAPITAL LETTER P#
 {120240, "Q"}, // MA# ( 𝖰 → Q ) MATHEMATICAL SANS-SERIF CAPITAL Q → LATIN CAPITAL LETTER Q#
 {120241, "R"}, // MA# ( 𝖱 → R ) MATHEMATICAL SANS-SERIF CAPITAL R → LATIN CAPITAL LETTER R#
 {120242, "S"}, // MA# ( 𝖲 → S ) MATHEMATICAL SANS-SERIF CAPITAL S → LATIN CAPITAL LETTER S#
 {120243, "T"}, // MA# ( 𝖳 → T ) MATHEMATICAL SANS-SERIF CAPITAL T → LATIN CAPITAL LETTER T#
 {120244, "U"}, // MA# ( 𝖴 → U ) MATHEMATICAL SANS-SERIF CAPITAL U → LATIN CAPITAL LETTER U#
 {120245, "V"}, // MA# ( 𝖵 → V ) MATHEMATICAL SANS-SERIF CAPITAL V → LATIN CAPITAL LETTER V#
 {120246, "W"}, // MA# ( 𝖶 → W ) MATHEMATICAL SANS-SERIF CAPITAL W → LATIN CAPITAL LETTER W#
 {120247, "X"}, // MA# ( 𝖷 → X ) MATHEMATICAL SANS-SERIF CAPITAL X → LATIN CAPITAL LETTER X#
 {120248, "Y"}, // MA# ( 𝖸 → Y ) MATHEMATICAL SANS-SERIF CAPITAL Y → LATIN CAPITAL LETTER Y#
 {120249, "Z"}, // MA# ( 𝖹 → Z ) MATHEMATICAL SANS-SERIF CAPITAL Z → LATIN CAPITAL LETTER Z#
 {120250, "a"}, // MA# ( 𝖺 → a ) MATHEMATICAL SANS-SERIF SMALL A → LATIN SMALL LETTER A#
 {120251, "b"}, // MA# ( 𝖻 → b ) MATHEMATICAL SANS-SERIF SMALL B → LATIN SMALL LETTER B#
 {120252, "c"}, // MA# ( 𝖼 → c ) MATHEMATICAL SANS-SERIF SMALL C → LATIN SMALL LETTER C#
 {120253, "d"}, // MA# ( 𝖽 → d ) MATHEMATICAL SANS-SERIF SMALL D → LATIN SMALL LETTER D#
 {120254, "e"}, // MA# ( 𝖾 → e ) MATHEMATICAL SANS-SERIF SMALL E → LATIN SMALL LETTER E#
 {120255, "f"}, // MA# ( 𝖿 → f ) MATHEMATICAL SANS-SERIF SMALL F → LATIN SMALL LETTER F#
 {120256, "g"}, // MA# ( 𝗀 → g ) MATHEMATICAL SANS-SERIF SMALL G → LATIN SMALL LETTER G#
 {120257, "h"}, // MA# ( 𝗁 → h ) MATHEMATICAL SANS-SERIF SMALL H → LATIN SMALL LETTER H#
 {120258, "i"}, // MA# ( 𝗂 → i ) MATHEMATICAL SANS-SERIF SMALL I → LATIN SMALL LETTER I#
 {120259, "j"}, // MA# ( 𝗃 → j ) MATHEMATICAL SANS-SERIF SMALL J → LATIN SMALL LETTER J#
 {120260, "k"}, // MA# ( 𝗄 → k ) MATHEMATICAL SANS-SERIF SMALL K → LATIN SMALL LETTER K#
 {120261, "l"}, // MA# ( 𝗅 → l ) MATHEMATICAL SANS-SERIF SMALL L → LATIN SMALL LETTER L#
 {120262, "rn"}, // MA# ( 𝗆 → rn ) MATHEMATICAL SANS-SERIF SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120263, "n"}, // MA# ( 𝗇 → n ) MATHEMATICAL SANS-SERIF SMALL N → LATIN SMALL LETTER N#
 {120264, "o"}, // MA# ( 𝗈 → o ) MATHEMATICAL SANS-SERIF SMALL O → LATIN SMALL LETTER O#
 {120265, "p"}, // MA# ( 𝗉 → p ) MATHEMATICAL SANS-SERIF SMALL P → LATIN SMALL LETTER P#
 {120266, "q"}, // MA# ( 𝗊 → q ) MATHEMATICAL SANS-SERIF SMALL Q → LATIN SMALL LETTER Q#
 {120267, "r"}, // MA# ( 𝗋 → r ) MATHEMATICAL SANS-SERIF SMALL R → LATIN SMALL LETTER R#
 {120268, "s"}, // MA# ( 𝗌 → s ) MATHEMATICAL SANS-SERIF SMALL S → LATIN SMALL LETTER S#
 {120269, "t"}, // MA# ( 𝗍 → t ) MATHEMATICAL SANS-SERIF SMALL T → LATIN SMALL LETTER T#
 {120270, "u"}, // MA# ( 𝗎 → u ) MATHEMATICAL SANS-SERIF SMALL U → LATIN SMALL LETTER U#
 {120271, "v"}, // MA# ( 𝗏 → v ) MATHEMATICAL SANS-SERIF SMALL V → LATIN SMALL LETTER V#
 {120272, "w"}, // MA# ( 𝗐 → w ) MATHEMATICAL SANS-SERIF SMALL W → LATIN SMALL LETTER W#
 {120273, "x"}, // MA# ( 𝗑 → x ) MATHEMATICAL SANS-SERIF SMALL X → LATIN SMALL LETTER X#
 {120274, "y"}, // MA# ( 𝗒 → y ) MATHEMATICAL SANS-SERIF SMALL Y → LATIN SMALL LETTER Y#
 {120275, "z"}, // MA# ( 𝗓 → z ) MATHEMATICAL SANS-SERIF SMALL Z → LATIN SMALL LETTER Z#
 {120276, "A"}, // MA# ( 𝗔 → A ) MATHEMATICAL SANS-SERIF BOLD CAPITAL A → LATIN CAPITAL LETTER A#
 {120277, "B"}, // MA# ( 𝗕 → B ) MATHEMATICAL SANS-SERIF BOLD CAPITAL B → LATIN CAPITAL LETTER B#
 {120278, "C"}, // MA# ( 𝗖 → C ) MATHEMATICAL SANS-SERIF BOLD CAPITAL C → LATIN CAPITAL LETTER C#
 {120279, "D"}, // MA# ( 𝗗 → D ) MATHEMATICAL SANS-SERIF BOLD CAPITAL D → LATIN CAPITAL LETTER D#
 {120280, "E"}, // MA# ( 𝗘 → E ) MATHEMATICAL SANS-SERIF BOLD CAPITAL E → LATIN CAPITAL LETTER E#
 {120281, "F"}, // MA# ( 𝗙 → F ) MATHEMATICAL SANS-SERIF BOLD CAPITAL F → LATIN CAPITAL LETTER F#
 {120282, "G"}, // MA# ( 𝗚 → G ) MATHEMATICAL SANS-SERIF BOLD CAPITAL G → LATIN CAPITAL LETTER G#
 {120283, "H"}, // MA# ( 𝗛 → H ) MATHEMATICAL SANS-SERIF BOLD CAPITAL H → LATIN CAPITAL LETTER H#
 {120284, "l"}, // MA# ( 𝗜 → l ) MATHEMATICAL SANS-SERIF BOLD CAPITAL I → LATIN SMALL LETTER L# →I→
 {120285, "J"}, // MA# ( 𝗝 → J ) MATHEMATICAL SANS-SERIF BOLD CAPITAL J → LATIN CAPITAL LETTER J#
 {120286, "K"}, // MA# ( 𝗞 → K ) MATHEMATICAL SANS-SERIF BOLD CAPITAL K → LATIN CAPITAL LETTER K#
 {120287, "L"}, // MA# ( 𝗟 → L ) MATHEMATICAL SANS-SERIF BOLD CAPITAL L → LATIN CAPITAL LETTER L#
 {120288, "M"}, // MA# ( 𝗠 → M ) MATHEMATICAL SANS-SERIF BOLD CAPITAL M → LATIN CAPITAL LETTER M#
 {120289, "N"}, // MA# ( 𝗡 → N ) MATHEMATICAL SANS-SERIF BOLD CAPITAL N → LATIN CAPITAL LETTER N#
 {120290, "O"}, // MA# ( 𝗢 → O ) MATHEMATICAL SANS-SERIF BOLD CAPITAL O → LATIN CAPITAL LETTER O#
 {120291, "P"}, // MA# ( 𝗣 → P ) MATHEMATICAL SANS-SERIF BOLD CAPITAL P → LATIN CAPITAL LETTER P#
 {120292, "Q"}, // MA# ( 𝗤 → Q ) MATHEMATICAL SANS-SERIF BOLD CAPITAL Q → LATIN CAPITAL LETTER Q#
 {120293, "R"}, // MA# ( 𝗥 → R ) MATHEMATICAL SANS-SERIF BOLD CAPITAL R → LATIN CAPITAL LETTER R#
 {120294, "S"}, // MA# ( 𝗦 → S ) MATHEMATICAL SANS-SERIF BOLD CAPITAL S → LATIN CAPITAL LETTER S#
 {120295, "T"}, // MA# ( 𝗧 → T ) MATHEMATICAL SANS-SERIF BOLD CAPITAL T → LATIN CAPITAL LETTER T#
 {120296, "U"}, // MA# ( 𝗨 → U ) MATHEMATICAL SANS-SERIF BOLD CAPITAL U → LATIN CAPITAL LETTER U#
 {120297, "V"}, // MA# ( 𝗩 → V ) MATHEMATICAL SANS-SERIF BOLD CAPITAL V → LATIN CAPITAL LETTER V#
 {120298, "W"}, // MA# ( 𝗪 → W ) MATHEMATICAL SANS-SERIF BOLD CAPITAL W → LATIN CAPITAL LETTER W#
 {120299, "X"}, // MA# ( 𝗫 → X ) MATHEMATICAL SANS-SERIF BOLD CAPITAL X → LATIN CAPITAL LETTER X#
 {120300, "Y"}, // MA# ( 𝗬 → Y ) MATHEMATICAL SANS-SERIF BOLD CAPITAL Y → LATIN CAPITAL LETTER Y#
 {120301, "Z"}, // MA# ( 𝗭 → Z ) MATHEMATICAL SANS-SERIF BOLD CAPITAL Z → LATIN CAPITAL LETTER Z#
 {120302, "a"}, // MA# ( 𝗮 → a ) MATHEMATICAL SANS-SERIF BOLD SMALL A → LATIN SMALL LETTER A#
 {120303, "b"}, // MA# ( 𝗯 → b ) MATHEMATICAL SANS-SERIF BOLD SMALL B → LATIN SMALL LETTER B#
 {120304, "c"}, // MA# ( 𝗰 → c ) MATHEMATICAL SANS-SERIF BOLD SMALL C → LATIN SMALL LETTER C#
 {120305, "d"}, // MA# ( 𝗱 → d ) MATHEMATICAL SANS-SERIF BOLD SMALL D → LATIN SMALL LETTER D#
 {120306, "e"}, // MA# ( 𝗲 → e ) MATHEMATICAL SANS-SERIF BOLD SMALL E → LATIN SMALL LETTER E#
 {120307, "f"}, // MA# ( 𝗳 → f ) MATHEMATICAL SANS-SERIF BOLD SMALL F → LATIN SMALL LETTER F#
 {120308, "g"}, // MA# ( 𝗴 → g ) MATHEMATICAL SANS-SERIF BOLD SMALL G → LATIN SMALL LETTER G#
 {120309, "h"}, // MA# ( 𝗵 → h ) MATHEMATICAL SANS-SERIF BOLD SMALL H → LATIN SMALL LETTER H#
 {120310, "i"}, // MA# ( 𝗶 → i ) MATHEMATICAL SANS-SERIF BOLD SMALL I → LATIN SMALL LETTER I#
 {120311, "j"}, // MA# ( 𝗷 → j ) MATHEMATICAL SANS-SERIF BOLD SMALL J → LATIN SMALL LETTER J#
 {120312, "k"}, // MA# ( 𝗸 → k ) MATHEMATICAL SANS-SERIF BOLD SMALL K → LATIN SMALL LETTER K#
 {120313, "l"}, // MA# ( 𝗹 → l ) MATHEMATICAL SANS-SERIF BOLD SMALL L → LATIN SMALL LETTER L#
 {120314, "rn"}, // MA# ( 𝗺 → rn ) MATHEMATICAL SANS-SERIF BOLD SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120315, "n"}, // MA# ( 𝗻 → n ) MATHEMATICAL SANS-SERIF BOLD SMALL N → LATIN SMALL LETTER N#
 {120316, "o"}, // MA# ( 𝗼 → o ) MATHEMATICAL SANS-SERIF BOLD SMALL O → LATIN SMALL LETTER O#
 {120317, "p"}, // MA# ( 𝗽 → p ) MATHEMATICAL SANS-SERIF BOLD SMALL P → LATIN SMALL LETTER P#
 {120318, "q"}, // MA# ( 𝗾 → q ) MATHEMATICAL SANS-SERIF BOLD SMALL Q → LATIN SMALL LETTER Q#
 {120319, "r"}, // MA# ( 𝗿 → r ) MATHEMATICAL SANS-SERIF BOLD SMALL R → LATIN SMALL LETTER R#
 {120320, "s"}, // MA# ( 𝘀 → s ) MATHEMATICAL SANS-SERIF BOLD SMALL S → LATIN SMALL LETTER S#
 {120321, "t"}, // MA# ( 𝘁 → t ) MATHEMATICAL SANS-SERIF BOLD SMALL T → LATIN SMALL LETTER T#
 {120322, "u"}, // MA# ( 𝘂 → u ) MATHEMATICAL SANS-SERIF BOLD SMALL U → LATIN SMALL LETTER U#
 {120323, "v"}, // MA# ( 𝘃 → v ) MATHEMATICAL SANS-SERIF BOLD SMALL V → LATIN SMALL LETTER V#
 {120324, "w"}, // MA# ( 𝘄 → w ) MATHEMATICAL SANS-SERIF BOLD SMALL W → LATIN SMALL LETTER W#
 {120325, "x"}, // MA# ( 𝘅 → x ) MATHEMATICAL SANS-SERIF BOLD SMALL X → LATIN SMALL LETTER X#
 {120326, "y"}, // MA# ( 𝘆 → y ) MATHEMATICAL SANS-SERIF BOLD SMALL Y → LATIN SMALL LETTER Y#
 {120327, "z"}, // MA# ( 𝘇 → z ) MATHEMATICAL SANS-SERIF BOLD SMALL Z → LATIN SMALL LETTER Z#
 {120328, "A"}, // MA# ( 𝘈 → A ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL A → LATIN CAPITAL LETTER A#
 {120329, "B"}, // MA# ( 𝘉 → B ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL B → LATIN CAPITAL LETTER B#
 {120330, "C"}, // MA# ( 𝘊 → C ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL C → LATIN CAPITAL LETTER C#
 {120331, "D"}, // MA# ( 𝘋 → D ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL D → LATIN CAPITAL LETTER D#
 {120332, "E"}, // MA# ( 𝘌 → E ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL E → LATIN CAPITAL LETTER E#
 {120333, "F"}, // MA# ( 𝘍 → F ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL F → LATIN CAPITAL LETTER F#
 {120334, "G"}, // MA# ( 𝘎 → G ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL G → LATIN CAPITAL LETTER G#
 {120335, "H"}, // MA# ( 𝘏 → H ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL H → LATIN CAPITAL LETTER H#
 {120336, "l"}, // MA# ( 𝘐 → l ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL I → LATIN SMALL LETTER L# →I→
 {120337, "J"}, // MA# ( 𝘑 → J ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL J → LATIN CAPITAL LETTER J#
 {120338, "K"}, // MA# ( 𝘒 → K ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL K → LATIN CAPITAL LETTER K#
 {120339, "L"}, // MA# ( 𝘓 → L ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL L → LATIN CAPITAL LETTER L#
 {120340, "M"}, // MA# ( 𝘔 → M ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL M → LATIN CAPITAL LETTER M#
 {120341, "N"}, // MA# ( 𝘕 → N ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL N → LATIN CAPITAL LETTER N#
 {120342, "O"}, // MA# ( 𝘖 → O ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL O → LATIN CAPITAL LETTER O#
 {120343, "P"}, // MA# ( 𝘗 → P ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL P → LATIN CAPITAL LETTER P#
 {120344, "Q"}, // MA# ( 𝘘 → Q ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL Q → LATIN CAPITAL LETTER Q#
 {120345, "R"}, // MA# ( 𝘙 → R ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL R → LATIN CAPITAL LETTER R#
 {120346, "S"}, // MA# ( 𝘚 → S ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL S → LATIN CAPITAL LETTER S#
 {120347, "T"}, // MA# ( 𝘛 → T ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL T → LATIN CAPITAL LETTER T#
 {120348, "U"}, // MA# ( 𝘜 → U ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL U → LATIN CAPITAL LETTER U#
 {120349, "V"}, // MA# ( 𝘝 → V ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL V → LATIN CAPITAL LETTER V#
 {120350, "W"}, // MA# ( 𝘞 → W ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL W → LATIN CAPITAL LETTER W#
 {120351, "X"}, // MA# ( 𝘟 → X ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL X → LATIN CAPITAL LETTER X#
 {120352, "Y"}, // MA# ( 𝘠 → Y ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL Y → LATIN CAPITAL LETTER Y#
 {120353, "Z"}, // MA# ( 𝘡 → Z ) MATHEMATICAL SANS-SERIF ITALIC CAPITAL Z → LATIN CAPITAL LETTER Z#
 {120354, "a"}, // MA# ( 𝘢 → a ) MATHEMATICAL SANS-SERIF ITALIC SMALL A → LATIN SMALL LETTER A#
 {120355, "b"}, // MA# ( 𝘣 → b ) MATHEMATICAL SANS-SERIF ITALIC SMALL B → LATIN SMALL LETTER B#
 {120356, "c"}, // MA# ( 𝘤 → c ) MATHEMATICAL SANS-SERIF ITALIC SMALL C → LATIN SMALL LETTER C#
 {120357, "d"}, // MA# ( 𝘥 → d ) MATHEMATICAL SANS-SERIF ITALIC SMALL D → LATIN SMALL LETTER D#
 {120358, "e"}, // MA# ( 𝘦 → e ) MATHEMATICAL SANS-SERIF ITALIC SMALL E → LATIN SMALL LETTER E#
 {120359, "f"}, // MA# ( 𝘧 → f ) MATHEMATICAL SANS-SERIF ITALIC SMALL F → LATIN SMALL LETTER F#
 {120360, "g"}, // MA# ( 𝘨 → g ) MATHEMATICAL SANS-SERIF ITALIC SMALL G → LATIN SMALL LETTER G#
 {120361, "h"}, // MA# ( 𝘩 → h ) MATHEMATICAL SANS-SERIF ITALIC SMALL H → LATIN SMALL LETTER H#
 {120362, "i"}, // MA# ( 𝘪 → i ) MATHEMATICAL SANS-SERIF ITALIC SMALL I → LATIN SMALL LETTER I#
 {120363, "j"}, // MA# ( 𝘫 → j ) MATHEMATICAL SANS-SERIF ITALIC SMALL J → LATIN SMALL LETTER J#
 {120364, "k"}, // MA# ( 𝘬 → k ) MATHEMATICAL SANS-SERIF ITALIC SMALL K → LATIN SMALL LETTER K#
 {120365, "l"}, // MA# ( 𝘭 → l ) MATHEMATICAL SANS-SERIF ITALIC SMALL L → LATIN SMALL LETTER L#
 {120366, "rn"}, // MA# ( 𝘮 → rn ) MATHEMATICAL SANS-SERIF ITALIC SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120367, "n"}, // MA# ( 𝘯 → n ) MATHEMATICAL SANS-SERIF ITALIC SMALL N → LATIN SMALL LETTER N#
 {120368, "o"}, // MA# ( 𝘰 → o ) MATHEMATICAL SANS-SERIF ITALIC SMALL O → LATIN SMALL LETTER O#
 {120369, "p"}, // MA# ( 𝘱 → p ) MATHEMATICAL SANS-SERIF ITALIC SMALL P → LATIN SMALL LETTER P#
 {120370, "q"}, // MA# ( 𝘲 → q ) MATHEMATICAL SANS-SERIF ITALIC SMALL Q → LATIN SMALL LETTER Q#
 {120371, "r"}, // MA# ( 𝘳 → r ) MATHEMATICAL SANS-SERIF ITALIC SMALL R → LATIN SMALL LETTER R#
 {120372, "s"}, // MA# ( 𝘴 → s ) MATHEMATICAL SANS-SERIF ITALIC SMALL S → LATIN SMALL LETTER S#
 {120373, "t"}, // MA# ( 𝘵 → t ) MATHEMATICAL SANS-SERIF ITALIC SMALL T → LATIN SMALL LETTER T#
 {120374, "u"}, // MA# ( 𝘶 → u ) MATHEMATICAL SANS-SERIF ITALIC SMALL U → LATIN SMALL LETTER U#
 {120375, "v"}, // MA# ( 𝘷 → v ) MATHEMATICAL SANS-SERIF ITALIC SMALL V → LATIN SMALL LETTER V#
 {120376, "w"}, // MA# ( 𝘸 → w ) MATHEMATICAL SANS-SERIF ITALIC SMALL W → LATIN SMALL LETTER W#
 {120377, "x"}, // MA# ( 𝘹 → x ) MATHEMATICAL SANS-SERIF ITALIC SMALL X → LATIN SMALL LETTER X#
 {120378, "y"}, // MA# ( 𝘺 → y ) MATHEMATICAL SANS-SERIF ITALIC SMALL Y → LATIN SMALL LETTER Y#
 {120379, "z"}, // MA# ( 𝘻 → z ) MATHEMATICAL SANS-SERIF ITALIC SMALL Z → LATIN SMALL LETTER Z#
 {120380, "A"}, // MA# ( 𝘼 → A ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL A → LATIN CAPITAL LETTER A#
 {120381, "B"}, // MA# ( 𝘽 → B ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL B → LATIN CAPITAL LETTER B#
 {120382, "C"}, // MA# ( 𝘾 → C ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL C → LATIN CAPITAL LETTER C#
 {120383, "D"}, // MA# ( 𝘿 → D ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL D → LATIN CAPITAL LETTER D#
 {120384, "E"}, // MA# ( 𝙀 → E ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL E → LATIN CAPITAL LETTER E#
 {120385, "F"}, // MA# ( 𝙁 → F ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL F → LATIN CAPITAL LETTER F#
 {120386, "G"}, // MA# ( 𝙂 → G ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL G → LATIN CAPITAL LETTER G#
 {120387, "H"}, // MA# ( 𝙃 → H ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL H → LATIN CAPITAL LETTER H#
 {120388, "l"}, // MA# ( 𝙄 → l ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL I → LATIN SMALL LETTER L# →I→
 {120389, "J"}, // MA# ( 𝙅 → J ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL J → LATIN CAPITAL LETTER J#
 {120390, "K"}, // MA# ( 𝙆 → K ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL K → LATIN CAPITAL LETTER K#
 {120391, "L"}, // MA# ( 𝙇 → L ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL L → LATIN CAPITAL LETTER L#
 {120392, "M"}, // MA# ( 𝙈 → M ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL M → LATIN CAPITAL LETTER M#
 {120393, "N"}, // MA# ( 𝙉 → N ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL N → LATIN CAPITAL LETTER N#
 {120394, "O"}, // MA# ( 𝙊 → O ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL O → LATIN CAPITAL LETTER O#
 {120395, "P"}, // MA# ( 𝙋 → P ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL P → LATIN CAPITAL LETTER P#
 {120396, "Q"}, // MA# ( 𝙌 → Q ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL Q → LATIN CAPITAL LETTER Q#
 {120397, "R"}, // MA# ( 𝙍 → R ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL R → LATIN CAPITAL LETTER R#
 {120398, "S"}, // MA# ( 𝙎 → S ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL S → LATIN CAPITAL LETTER S#
 {120399, "T"}, // MA# ( 𝙏 → T ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL T → LATIN CAPITAL LETTER T#
 {120400, "U"}, // MA# ( 𝙐 → U ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL U → LATIN CAPITAL LETTER U#
 {120401, "V"}, // MA# ( 𝙑 → V ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL V → LATIN CAPITAL LETTER V#
 {120402, "W"}, // MA# ( 𝙒 → W ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL W → LATIN CAPITAL LETTER W#
 {120403, "X"}, // MA# ( 𝙓 → X ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL X → LATIN CAPITAL LETTER X#
 {120404, "Y"}, // MA# ( 𝙔 → Y ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL Y → LATIN CAPITAL LETTER Y#
 {120405, "Z"}, // MA# ( 𝙕 → Z ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL Z → LATIN CAPITAL LETTER Z#
 {120406, "a"}, // MA# ( 𝙖 → a ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL A → LATIN SMALL LETTER A#
 {120407, "b"}, // MA# ( 𝙗 → b ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL B → LATIN SMALL LETTER B#
 {120408, "c"}, // MA# ( 𝙘 → c ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL C → LATIN SMALL LETTER C#
 {120409, "d"}, // MA# ( 𝙙 → d ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL D → LATIN SMALL LETTER D#
 {120410, "e"}, // MA# ( 𝙚 → e ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL E → LATIN SMALL LETTER E#
 {120411, "f"}, // MA# ( 𝙛 → f ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL F → LATIN SMALL LETTER F#
 {120412, "g"}, // MA# ( 𝙜 → g ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL G → LATIN SMALL LETTER G#
 {120413, "h"}, // MA# ( 𝙝 → h ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL H → LATIN SMALL LETTER H#
 {120414, "i"}, // MA# ( 𝙞 → i ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL I → LATIN SMALL LETTER I#
 {120415, "j"}, // MA# ( 𝙟 → j ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL J → LATIN SMALL LETTER J#
 {120416, "k"}, // MA# ( 𝙠 → k ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL K → LATIN SMALL LETTER K#
 {120417, "l"}, // MA# ( 𝙡 → l ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL L → LATIN SMALL LETTER L#
 {120418, "rn"}, // MA# ( 𝙢 → rn ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120419, "n"}, // MA# ( 𝙣 → n ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL N → LATIN SMALL LETTER N#
 {120420, "o"}, // MA# ( 𝙤 → o ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL O → LATIN SMALL LETTER O#
 {120421, "p"}, // MA# ( 𝙥 → p ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL P → LATIN SMALL LETTER P#
 {120422, "q"}, // MA# ( 𝙦 → q ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL Q → LATIN SMALL LETTER Q#
 {120423, "r"}, // MA# ( 𝙧 → r ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL R → LATIN SMALL LETTER R#
 {120424, "s"}, // MA# ( 𝙨 → s ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL S → LATIN SMALL LETTER S#
 {120425, "t"}, // MA# ( 𝙩 → t ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL T → LATIN SMALL LETTER T#
 {120426, "u"}, // MA# ( 𝙪 → u ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL U → LATIN SMALL LETTER U#
 {120427, "v"}, // MA# ( 𝙫 → v ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL V → LATIN SMALL LETTER V#
 {120428, "w"}, // MA# ( 𝙬 → w ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL W → LATIN SMALL LETTER W#
 {120429, "x"}, // MA# ( 𝙭 → x ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL X → LATIN SMALL LETTER X#
 {120430, "y"}, // MA# ( 𝙮 → y ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL Y → LATIN SMALL LETTER Y#
 {120431, "z"}, // MA# ( 𝙯 → z ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL Z → LATIN SMALL LETTER Z#
 {120432, "A"}, // MA# ( 𝙰 → A ) MATHEMATICAL MONOSPACE CAPITAL A → LATIN CAPITAL LETTER A#
 {120433, "B"}, // MA# ( 𝙱 → B ) MATHEMATICAL MONOSPACE CAPITAL B → LATIN CAPITAL LETTER B#
 {120434, "C"}, // MA# ( 𝙲 → C ) MATHEMATICAL MONOSPACE CAPITAL C → LATIN CAPITAL LETTER C#
 {120435, "D"}, // MA# ( 𝙳 → D ) MATHEMATICAL MONOSPACE CAPITAL D → LATIN CAPITAL LETTER D#
 {120436, "E"}, // MA# ( 𝙴 → E ) MATHEMATICAL MONOSPACE CAPITAL E → LATIN CAPITAL LETTER E#
 {120437, "F"}, // MA# ( 𝙵 → F ) MATHEMATICAL MONOSPACE CAPITAL F → LATIN CAPITAL LETTER F#
 {120438, "G"}, // MA# ( 𝙶 → G ) MATHEMATICAL MONOSPACE CAPITAL G → LATIN CAPITAL LETTER G#
 {120439, "H"}, // MA# ( 𝙷 → H ) MATHEMATICAL MONOSPACE CAPITAL H → LATIN CAPITAL LETTER H#
 {120440, "l"}, // MA# ( 𝙸 → l ) MATHEMATICAL MONOSPACE CAPITAL I → LATIN SMALL LETTER L# →I→
 {120441, "J"}, // MA# ( 𝙹 → J ) MATHEMATICAL MONOSPACE CAPITAL J → LATIN CAPITAL LETTER J#
 {120442, "K"}, // MA# ( 𝙺 → K ) MATHEMATICAL MONOSPACE CAPITAL K → LATIN CAPITAL LETTER K#
 {120443, "L"}, // MA# ( 𝙻 → L ) MATHEMATICAL MONOSPACE CAPITAL L → LATIN CAPITAL LETTER L#
 {120444, "M"}, // MA# ( 𝙼 → M ) MATHEMATICAL MONOSPACE CAPITAL M → LATIN CAPITAL LETTER M#
 {120445, "N"}, // MA# ( 𝙽 → N ) MATHEMATICAL MONOSPACE CAPITAL N → LATIN CAPITAL LETTER N#
 {120446, "O"}, // MA# ( 𝙾 → O ) MATHEMATICAL MONOSPACE CAPITAL O → LATIN CAPITAL LETTER O#
 {120447, "P"}, // MA# ( 𝙿 → P ) MATHEMATICAL MONOSPACE CAPITAL P → LATIN CAPITAL LETTER P#
 {120448, "Q"}, // MA# ( 𝚀 → Q ) MATHEMATICAL MONOSPACE CAPITAL Q → LATIN CAPITAL LETTER Q#
 {120449, "R"}, // MA# ( 𝚁 → R ) MATHEMATICAL MONOSPACE CAPITAL R → LATIN CAPITAL LETTER R#
 {120450, "S"}, // MA# ( 𝚂 → S ) MATHEMATICAL MONOSPACE CAPITAL S → LATIN CAPITAL LETTER S#
 {120451, "T"}, // MA# ( 𝚃 → T ) MATHEMATICAL MONOSPACE CAPITAL T → LATIN CAPITAL LETTER T#
 {120452, "U"}, // MA# ( 𝚄 → U ) MATHEMATICAL MONOSPACE CAPITAL U → LATIN CAPITAL LETTER U#
 {120453, "V"}, // MA# ( 𝚅 → V ) MATHEMATICAL MONOSPACE CAPITAL V → LATIN CAPITAL LETTER V#
 {120454, "W"}, // MA# ( 𝚆 → W ) MATHEMATICAL MONOSPACE CAPITAL W → LATIN CAPITAL LETTER W#
 {120455, "X"}, // MA# ( 𝚇 → X ) MATHEMATICAL MONOSPACE CAPITAL X → LATIN CAPITAL LETTER X#
 {120456, "Y"}, // MA# ( 𝚈 → Y ) MATHEMATICAL MONOSPACE CAPITAL Y → LATIN CAPITAL LETTER Y#
 {120457, "Z"}, // MA# ( 𝚉 → Z ) MATHEMATICAL MONOSPACE CAPITAL Z → LATIN CAPITAL LETTER Z#
 {120458, "a"}, // MA# ( 𝚊 → a ) MATHEMATICAL MONOSPACE SMALL A → LATIN SMALL LETTER A#
 {120459, "b"}, // MA# ( 𝚋 → b ) MATHEMATICAL MONOSPACE SMALL B → LATIN SMALL LETTER B#
 {120460, "c"}, // MA# ( 𝚌 → c ) MATHEMATICAL MONOSPACE SMALL C → LATIN SMALL LETTER C#
 {120461, "d"}, // MA# ( 𝚍 → d ) MATHEMATICAL MONOSPACE SMALL D → LATIN SMALL LETTER D#
 {120462, "e"}, // MA# ( 𝚎 → e ) MATHEMATICAL MONOSPACE SMALL E → LATIN SMALL LETTER E#
 {120463, "f"}, // MA# ( 𝚏 → f ) MATHEMATICAL MONOSPACE SMALL F → LATIN SMALL LETTER F#
 {120464, "g"}, // MA# ( 𝚐 → g ) MATHEMATICAL MONOSPACE SMALL G → LATIN SMALL LETTER G#
 {120465, "h"}, // MA# ( 𝚑 → h ) MATHEMATICAL MONOSPACE SMALL H → LATIN SMALL LETTER H#
 {120466, "i"}, // MA# ( 𝚒 → i ) MATHEMATICAL MONOSPACE SMALL I → LATIN SMALL LETTER I#
 {120467, "j"}, // MA# ( 𝚓 → j ) MATHEMATICAL MONOSPACE SMALL J → LATIN SMALL LETTER J#
 {120468, "k"}, // MA# ( 𝚔 → k ) MATHEMATICAL MONOSPACE SMALL K → LATIN SMALL LETTER K#
 {120469, "l"}, // MA# ( 𝚕 → l ) MATHEMATICAL MONOSPACE SMALL L → LATIN SMALL LETTER L#
 {120470, "rn"}, // MA# ( 𝚖 → rn ) MATHEMATICAL MONOSPACE SMALL M → LATIN SMALL LETTER R, LATIN SMALL LETTER N# →m→
 {120471, "n"}, // MA# ( 𝚗 → n ) MATHEMATICAL MONOSPACE SMALL N → LATIN SMALL LETTER N#
 {120472, "o"}, // MA# ( 𝚘 → o ) MATHEMATICAL MONOSPACE SMALL O → LATIN SMALL LETTER O#
 {120473, "p"}, // MA# ( 𝚙 → p ) MATHEMATICAL MONOSPACE SMALL P → LATIN SMALL LETTER P#
 {120474, "q"}, // MA# ( 𝚚 → q ) MATHEMATICAL MONOSPACE SMALL Q → LATIN SMALL LETTER Q#
 {120475, "r"}, // MA# ( 𝚛 → r ) MATHEMATICAL MONOSPACE SMALL R → LATIN SMALL LETTER R#
 {120476, "s"}, // MA# ( 𝚜 → s ) MATHEMATICAL MONOSPACE SMALL S → LATIN SMALL LETTER S#
 {120477, "t"}, // MA# ( 𝚝 → t ) MATHEMATICAL MONOSPACE SMALL T → LATIN SMALL LETTER T#
 {120478, "u"}, // MA# ( 𝚞 → u ) MATHEMATICAL MONOSPACE SMALL U → LATIN SMALL LETTER U#
 {120479, "v"}, // MA# ( 𝚟 → v ) MATHEMATICAL MONOSPACE SMALL V → LATIN SMALL LETTER V#
 {120480, "w"}, // MA# ( 𝚠 → w ) MATHEMATICAL MONOSPACE SMALL W → LATIN SMALL LETTER W#
 {120481, "x"}, // MA# ( 𝚡 → x ) MATHEMATICAL MONOSPACE SMALL X → LATIN SMALL LETTER X#
 {120482, "y"}, // MA# ( 𝚢 → y ) MATHEMATICAL MONOSPACE SMALL Y → LATIN SMALL LETTER Y#
 {120483, "z"}, // MA# ( 𝚣 → z ) MATHEMATICAL MONOSPACE SMALL Z → LATIN SMALL LETTER Z#
 {120484, "i"}, // MA# ( 𝚤 → i ) MATHEMATICAL ITALIC SMALL DOTLESS I → LATIN SMALL LETTER I# →ı→
 {120488, "A"}, // MA# ( 𝚨 → A ) MATHEMATICAL BOLD CAPITAL ALPHA → LATIN CAPITAL LETTER A# →𝐀→
 {120489, "B"}, // MA# ( 𝚩 → B ) MATHEMATICAL BOLD CAPITAL BETA → LATIN CAPITAL LETTER B# →Β→
 {120492, "E"}, // MA# ( 𝚬 → E ) MATHEMATICAL BOLD CAPITAL EPSILON → LATIN CAPITAL LETTER E# →𝐄→
 {120493, "Z"}, // MA# ( 𝚭 → Z ) MATHEMATICAL BOLD CAPITAL ZETA → LATIN CAPITAL LETTER Z# →Ζ→
 {120494, "H"}, // MA# ( 𝚮 → H ) MATHEMATICAL BOLD CAPITAL ETA → LATIN CAPITAL LETTER H# →Η→
 {120496, "l"}, // MA# ( 𝚰 → l ) MATHEMATICAL BOLD CAPITAL IOTA → LATIN SMALL LETTER L# →Ι→
 {120497, "K"}, // MA# ( 𝚱 → K ) MATHEMATICAL BOLD CAPITAL KAPPA → LATIN CAPITAL LETTER K# →Κ→
 {120499, "M"}, // MA# ( 𝚳 → M ) MATHEMATICAL BOLD CAPITAL MU → LATIN CAPITAL LETTER M# →𝐌→
 {120500, "N"}, // MA# ( 𝚴 → N ) MATHEMATICAL BOLD CAPITAL NU → LATIN CAPITAL LETTER N# →𝐍→
 {120502, "O"}, // MA# ( 𝚶 → O ) MATHEMATICAL BOLD CAPITAL OMICRON → LATIN CAPITAL LETTER O# →𝐎→
 {120504, "P"}, // MA# ( 𝚸 → P ) MATHEMATICAL BOLD CAPITAL RHO → LATIN CAPITAL LETTER P# →𝐏→
 {120507, "T"}, // MA# ( 𝚻 → T ) MATHEMATICAL BOLD CAPITAL TAU → LATIN CAPITAL LETTER T# →Τ→
 {120508, "Y"}, // MA# ( 𝚼 → Y ) MATHEMATICAL BOLD CAPITAL UPSILON → LATIN CAPITAL LETTER Y# →Υ→
 {120510, "X"}, // MA# ( 𝚾 → X ) MATHEMATICAL BOLD CAPITAL CHI → LATIN CAPITAL LETTER X# →Χ→
 {120514, "a"}, // MA# ( 𝛂 → a ) MATHEMATICAL BOLD SMALL ALPHA → LATIN SMALL LETTER A# →α→
 {120516, "y"}, // MA# ( 𝛄 → y ) MATHEMATICAL BOLD SMALL GAMMA → LATIN SMALL LETTER Y# →γ→
 {120522, "i"}, // MA# ( 𝛊 → i ) MATHEMATICAL BOLD SMALL IOTA → LATIN SMALL LETTER I# →ι→
 {120526, "v"}, // MA# ( 𝛎 → v ) MATHEMATICAL BOLD SMALL NU → LATIN SMALL LETTER V# →ν→
 {120528, "o"}, // MA# ( 𝛐 → o ) MATHEMATICAL BOLD SMALL OMICRON → LATIN SMALL LETTER O# →𝐨→
 {120530, "p"}, // MA# ( 𝛒 → p ) MATHEMATICAL BOLD SMALL RHO → LATIN SMALL LETTER P# →ρ→
 {120532, "o"}, // MA# ( 𝛔 → o ) MATHEMATICAL BOLD SMALL SIGMA → LATIN SMALL LETTER O# →σ→
 {120534, "u"}, // MA# ( 𝛖 → u ) MATHEMATICAL BOLD SMALL UPSILON → LATIN SMALL LETTER U# →υ→→ʋ→
 {120544, "p"}, // MA# ( 𝛠 → p ) MATHEMATICAL BOLD RHO SYMBOL → LATIN SMALL LETTER P# →ρ→
 {120546, "A"}, // MA# ( 𝛢 → A ) MATHEMATICAL ITALIC CAPITAL ALPHA → LATIN CAPITAL LETTER A# →Α→
 {120547, "B"}, // MA# ( 𝛣 → B ) MATHEMATICAL ITALIC CAPITAL BETA → LATIN CAPITAL LETTER B# →Β→
 {120550, "E"}, // MA# ( 𝛦 → E ) MATHEMATICAL ITALIC CAPITAL EPSILON → LATIN CAPITAL LETTER E# →Ε→
 {120551, "Z"}, // MA# ( 𝛧 → Z ) MATHEMATICAL ITALIC CAPITAL ZETA → LATIN CAPITAL LETTER Z# →𝑍→
 {120552, "H"}, // MA# ( 𝛨 → H ) MATHEMATICAL ITALIC CAPITAL ETA → LATIN CAPITAL LETTER H# →Η→
 {120554, "l"}, // MA# ( 𝛪 → l ) MATHEMATICAL ITALIC CAPITAL IOTA → LATIN SMALL LETTER L# →Ι→
 {120555, "K"}, // MA# ( 𝛫 → K ) MATHEMATICAL ITALIC CAPITAL KAPPA → LATIN CAPITAL LETTER K# →𝐾→
 {120557, "M"}, // MA# ( 𝛭 → M ) MATHEMATICAL ITALIC CAPITAL MU → LATIN CAPITAL LETTER M# →𝑀→
 {120558, "N"}, // MA# ( 𝛮 → N ) MATHEMATICAL ITALIC CAPITAL NU → LATIN CAPITAL LETTER N# →𝑁→
 {120560, "O"}, // MA# ( 𝛰 → O ) MATHEMATICAL ITALIC CAPITAL OMICRON → LATIN CAPITAL LETTER O# →𝑂→
 {120562, "P"}, // MA# ( 𝛲 → P ) MATHEMATICAL ITALIC CAPITAL RHO → LATIN CAPITAL LETTER P# →Ρ→
 {120565, "T"}, // MA# ( 𝛵 → T ) MATHEMATICAL ITALIC CAPITAL TAU → LATIN CAPITAL LETTER T# →Τ→
 {120566, "Y"}, // MA# ( 𝛶 → Y ) MATHEMATICAL ITALIC CAPITAL UPSILON → LATIN CAPITAL LETTER Y# →Υ→
 {120568, "X"}, // MA# ( 𝛸 → X ) MATHEMATICAL ITALIC CAPITAL CHI → LATIN CAPITAL LETTER X# →Χ→
 {120572, "a"}, // MA# ( 𝛼 → a ) MATHEMATICAL ITALIC SMALL ALPHA → LATIN SMALL LETTER A# →α→
 {120574, "y"}, // MA# ( 𝛾 → y ) MATHEMATICAL ITALIC SMALL GAMMA → LATIN SMALL LETTER Y# →γ→
 {120580, "i"}, // MA# ( 𝜄 → i ) MATHEMATICAL ITALIC SMALL IOTA → LATIN SMALL LETTER I# →ι→
 {120584, "v"}, // MA# ( 𝜈 → v ) MATHEMATICAL ITALIC SMALL NU → LATIN SMALL LETTER V# →ν→
 {120586, "o"}, // MA# ( 𝜊 → o ) MATHEMATICAL ITALIC SMALL OMICRON → LATIN SMALL LETTER O# →𝑜→
 {120588, "p"}, // MA# ( 𝜌 → p ) MATHEMATICAL ITALIC SMALL RHO → LATIN SMALL LETTER P# →ρ→
 {120590, "o"}, // MA# ( 𝜎 → o ) MATHEMATICAL ITALIC SMALL SIGMA → LATIN SMALL LETTER O# →σ→
 {120592, "u"}, // MA# ( 𝜐 → u ) MATHEMATICAL ITALIC SMALL UPSILON → LATIN SMALL LETTER U# →υ→→ʋ→
 {120602, "p"}, // MA# ( 𝜚 → p ) MATHEMATICAL ITALIC RHO SYMBOL → LATIN SMALL LETTER P# →ρ→
 {120604, "A"}, // MA# ( 𝜜 → A ) MATHEMATICAL BOLD ITALIC CAPITAL ALPHA → LATIN CAPITAL LETTER A# →Α→
 {120605, "B"}, // MA# ( 𝜝 → B ) MATHEMATICAL BOLD ITALIC CAPITAL BETA → LATIN CAPITAL LETTER B# →Β→
 {120608, "E"}, // MA# ( 𝜠 → E ) MATHEMATICAL BOLD ITALIC CAPITAL EPSILON → LATIN CAPITAL LETTER E# →Ε→
 {120609, "Z"}, // MA# ( 𝜡 → Z ) MATHEMATICAL BOLD ITALIC CAPITAL ZETA → LATIN CAPITAL LETTER Z# →Ζ→
 {120610, "H"}, // MA# ( 𝜢 → H ) MATHEMATICAL BOLD ITALIC CAPITAL ETA → LATIN CAPITAL LETTER H# →𝑯→
 {120612, "l"}, // MA# ( 𝜤 → l ) MATHEMATICAL BOLD ITALIC CAPITAL IOTA → LATIN SMALL LETTER L# →Ι→
 {120613, "K"}, // MA# ( 𝜥 → K ) MATHEMATICAL BOLD ITALIC CAPITAL KAPPA → LATIN CAPITAL LETTER K# →𝑲→
 {120615, "M"}, // MA# ( 𝜧 → M ) MATHEMATICAL BOLD ITALIC CAPITAL MU → LATIN CAPITAL LETTER M# →𝑴→
 {120616, "N"}, // MA# ( 𝜨 → N ) MATHEMATICAL BOLD ITALIC CAPITAL NU → LATIN CAPITAL LETTER N# →𝑵→
 {120618, "O"}, // MA# ( 𝜪 → O ) MATHEMATICAL BOLD ITALIC CAPITAL OMICRON → LATIN CAPITAL LETTER O# →𝑶→
 {120620, "P"}, // MA# ( 𝜬 → P ) MATHEMATICAL BOLD ITALIC CAPITAL RHO → LATIN CAPITAL LETTER P# →Ρ→
 {120623, "T"}, // MA# ( 𝜯 → T ) MATHEMATICAL BOLD ITALIC CAPITAL TAU → LATIN CAPITAL LETTER T# →Τ→
 {120624, "Y"}, // MA# ( 𝜰 → Y ) MATHEMATICAL BOLD ITALIC CAPITAL UPSILON → LATIN CAPITAL LETTER Y# →Υ→
 {120626, "X"}, // MA# ( 𝜲 → X ) MATHEMATICAL BOLD ITALIC CAPITAL CHI → LATIN CAPITAL LETTER X# →𝑿→
 {120630, "a"}, // MA# ( 𝜶 → a ) MATHEMATICAL BOLD ITALIC SMALL ALPHA → LATIN SMALL LETTER A# →α→
 {120632, "y"}, // MA# ( 𝜸 → y ) MATHEMATICAL BOLD ITALIC SMALL GAMMA → LATIN SMALL LETTER Y# →γ→
 {120638, "i"}, // MA# ( 𝜾 → i ) MATHEMATICAL BOLD ITALIC SMALL IOTA → LATIN SMALL LETTER I# →ι→
 {120642, "v"}, // MA# ( 𝝂 → v ) MATHEMATICAL BOLD ITALIC SMALL NU → LATIN SMALL LETTER V# →ν→
 {120644, "o"}, // MA# ( 𝝄 → o ) MATHEMATICAL BOLD ITALIC SMALL OMICRON → LATIN SMALL LETTER O# →𝒐→
 {120646, "p"}, // MA# ( 𝝆 → p ) MATHEMATICAL BOLD ITALIC SMALL RHO → LATIN SMALL LETTER P# →ρ→
 {120648, "o"}, // MA# ( 𝝈 → o ) MATHEMATICAL BOLD ITALIC SMALL SIGMA → LATIN SMALL LETTER O# →σ→
 {120650, "u"}, // MA# ( 𝝊 → u ) MATHEMATICAL BOLD ITALIC SMALL UPSILON → LATIN SMALL LETTER U# →υ→→ʋ→
 {120660, "p"}, // MA# ( 𝝔 → p ) MATHEMATICAL BOLD ITALIC RHO SYMBOL → LATIN SMALL LETTER P# →ρ→
 {120662, "A"}, // MA# ( 𝝖 → A ) MATHEMATICAL SANS-SERIF BOLD CAPITAL ALPHA → LATIN CAPITAL LETTER A# →Α→
 {120663, "B"}, // MA# ( 𝝗 → B ) MATHEMATICAL SANS-SERIF BOLD CAPITAL BETA → LATIN CAPITAL LETTER B# →Β→
 {120666, "E"}, // MA# ( 𝝚 → E ) MATHEMATICAL SANS-SERIF BOLD CAPITAL EPSILON → LATIN CAPITAL LETTER E# →Ε→
 {120667, "Z"}, // MA# ( 𝝛 → Z ) MATHEMATICAL SANS-SERIF BOLD CAPITAL ZETA → LATIN CAPITAL LETTER Z# →Ζ→
 {120668, "H"}, // MA# ( 𝝜 → H ) MATHEMATICAL SANS-SERIF BOLD CAPITAL ETA → LATIN CAPITAL LETTER H# →Η→
 {120670, "l"}, // MA# ( 𝝞 → l ) MATHEMATICAL SANS-SERIF BOLD CAPITAL IOTA → LATIN SMALL LETTER L# →Ι→
 {120671, "K"}, // MA# ( 𝝟 → K ) MATHEMATICAL SANS-SERIF BOLD CAPITAL KAPPA → LATIN CAPITAL LETTER K# →Κ→
 {120673, "M"}, // MA# ( 𝝡 → M ) MATHEMATICAL SANS-SERIF BOLD CAPITAL MU → LATIN CAPITAL LETTER M# →Μ→
 {120674, "N"}, // MA# ( 𝝢 → N ) MATHEMATICAL SANS-SERIF BOLD CAPITAL NU → LATIN CAPITAL LETTER N# →Ν→
 {120676, "O"}, // MA# ( 𝝤 → O ) MATHEMATICAL SANS-SERIF BOLD CAPITAL OMICRON → LATIN CAPITAL LETTER O# →Ο→
 {120678, "P"}, // MA# ( 𝝦 → P ) MATHEMATICAL SANS-SERIF BOLD CAPITAL RHO → LATIN CAPITAL LETTER P# →Ρ→
 {120681, "T"}, // MA# ( 𝝩 → T ) MATHEMATICAL SANS-SERIF BOLD CAPITAL TAU → LATIN CAPITAL LETTER T# →Τ→
 {120682, "Y"}, // MA# ( 𝝪 → Y ) MATHEMATICAL SANS-SERIF BOLD CAPITAL UPSILON → LATIN CAPITAL LETTER Y# →Υ→
 {120684, "X"}, // MA# ( 𝝬 → X ) MATHEMATICAL SANS-SERIF BOLD CAPITAL CHI → LATIN CAPITAL LETTER X# →Χ→
 {120688, "a"}, // MA# ( 𝝰 → a ) MATHEMATICAL SANS-SERIF BOLD SMALL ALPHA → LATIN SMALL LETTER A# →α→
 {120690, "y"}, // MA# ( 𝝲 → y ) MATHEMATICAL SANS-SERIF BOLD SMALL GAMMA → LATIN SMALL LETTER Y# →γ→
 {120696, "i"}, // MA# ( 𝝸 → i ) MATHEMATICAL SANS-SERIF BOLD SMALL IOTA → LATIN SMALL LETTER I# →ι→
 {120700, "v"}, // MA# ( 𝝼 → v ) MATHEMATICAL SANS-SERIF BOLD SMALL NU → LATIN SMALL LETTER V# →ν→
 {120702, "o"}, // MA# ( 𝝾 → o ) MATHEMATICAL SANS-SERIF BOLD SMALL OMICRON → LATIN SMALL LETTER O# →ο→
 {120704, "p"}, // MA# ( 𝞀 → p ) MATHEMATICAL SANS-SERIF BOLD SMALL RHO → LATIN SMALL LETTER P# →ρ→
 {120706, "o"}, // MA# ( 𝞂 → o ) MATHEMATICAL SANS-SERIF BOLD SMALL SIGMA → LATIN SMALL LETTER O# →σ→
 {120708, "u"}, // MA# ( 𝞄 → u ) MATHEMATICAL SANS-SERIF BOLD SMALL UPSILON → LATIN SMALL LETTER U# →υ→→ʋ→
 {120718, "p"}, // MA# ( 𝞎 → p ) MATHEMATICAL SANS-SERIF BOLD RHO SYMBOL → LATIN SMALL LETTER P# →ρ→
 {120720, "A"}, // MA# ( 𝞐 → A ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL ALPHA → LATIN CAPITAL LETTER A# →Α→
 {120721, "B"}, // MA# ( 𝞑 → B ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL BETA → LATIN CAPITAL LETTER B# →Β→
 {120724, "E"}, // MA# ( 𝞔 → E ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL EPSILON → LATIN CAPITAL LETTER E# →Ε→
 {120725, "Z"}, // MA# ( 𝞕 → Z ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL ZETA → LATIN CAPITAL LETTER Z# →Ζ→
 {120726, "H"}, // MA# ( 𝞖 → H ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL ETA → LATIN CAPITAL LETTER H# →Η→
 {120728, "l"}, // MA# ( 𝞘 → l ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL IOTA → LATIN SMALL LETTER L# →Ι→
 {120729, "K"}, // MA# ( 𝞙 → K ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL KAPPA → LATIN CAPITAL LETTER K# →Κ→
 {120731, "M"}, // MA# ( 𝞛 → M ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL MU → LATIN CAPITAL LETTER M# →Μ→
 {120732, "N"}, // MA# ( 𝞜 → N ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL NU → LATIN CAPITAL LETTER N# →Ν→
 {120734, "O"}, // MA# ( 𝞞 → O ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL OMICRON → LATIN CAPITAL LETTER O# →Ο→
 {120736, "P"}, // MA# ( 𝞠 → P ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL RHO → LATIN CAPITAL LETTER P# →Ρ→
 {120739, "T"}, // MA# ( 𝞣 → T ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL TAU → LATIN CAPITAL LETTER T# →Τ→
 {120740, "Y"}, // MA# ( 𝞤 → Y ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL UPSILON → LATIN CAPITAL LETTER Y# →Υ→
 {120742, "X"}, // MA# ( 𝞦 → X ) MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL CHI → LATIN CAPITAL LETTER X# →Χ→
 {120746, "a"}, // MA# ( 𝞪 → a ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL ALPHA → LATIN SMALL LETTER A# →α→
 {120748, "y"}, // MA# ( 𝞬 → y ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL GAMMA → LATIN SMALL LETTER Y# →γ→
 {120754, "i"}, // MA# ( 𝞲 → i ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL IOTA → LATIN SMALL LETTER I# →ι→
 {120758, "v"}, // MA# ( 𝞶 → v ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL NU → LATIN SMALL LETTER V# →ν→
 {120760, "o"}, // MA# ( 𝞸 → o ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL OMICRON → LATIN SMALL LETTER O# →ο→
 {120762, "p"}, // MA# ( 𝞺 → p ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL RHO → LATIN SMALL LETTER P# →ρ→
 {120764, "o"}, // MA# ( 𝞼 → o ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL SIGMA → LATIN SMALL LETTER O# →σ→
 {120766, "u"}, // MA# ( 𝞾 → u ) MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL UPSILON → LATIN SMALL LETTER U# →υ→→ʋ→
 {120776, "p"}, // MA# ( 𝟈 → p ) MATHEMATICAL SANS-SERIF BOLD ITALIC RHO SYMBOL → LATIN SMALL LETTER P# →ρ→
 {120778, "F"}, // MA# ( 𝟊 → F ) MATHEMATICAL BOLD CAPITAL DIGAMMA → LATIN CAPITAL LETTER F# →Ϝ→
 {120782, "O"}, // MA# ( 𝟎 → O ) MATHEMATICAL BOLD DIGIT ZERO → LATIN CAPITAL LETTER O# →0→
 {120783, "l"}, // MA# ( 𝟏 → l ) MATHEMATICAL BOLD DIGIT ONE → LATIN SMALL LETTER L# →1→
 {120784, "2"}, // MA# ( 𝟐 → 2 ) MATHEMATICAL BOLD DIGIT TWO → DIGIT TWO#
 {120785, "3"}, // MA# ( 𝟑 → 3 ) MATHEMATICAL BOLD DIGIT THREE → DIGIT THREE#
 {120786, "4"}, // MA# ( 𝟒 → 4 ) MATHEMATICAL BOLD DIGIT FOUR → DIGIT FOUR#
 {120787, "5"}, // MA# ( 𝟓 → 5 ) MATHEMATICAL BOLD DIGIT FIVE → DIGIT FIVE#
 {120788, "6"}, // MA# ( 𝟔 → 6 ) MATHEMATICAL BOLD DIGIT SIX → DIGIT SIX#
 {120789, "7"}, // MA# ( 𝟕 → 7 ) MATHEMATICAL BOLD DIGIT SEVEN → DIGIT SEVEN#
 {120790, "8"}, // MA# ( 𝟖 → 8 ) MATHEMATICAL BOLD DIGIT EIGHT → DIGIT EIGHT#
 {120791, "9"}, // MA# ( 𝟗 → 9 ) MATHEMATICAL BOLD DIGIT NINE → DIGIT NINE#
 {120792, "O"}, // MA# ( 𝟘 → O ) MATHEMATICAL DOUBLE-STRUCK DIGIT ZERO → LATIN CAPITAL LETTER O# →0→
 {120793, "l"}, // MA# ( 𝟙 → l ) MATHEMATICAL DOUBLE-STRUCK DIGIT ONE → LATIN SMALL LETTER L# →1→
 {120794, "2"}, // MA# ( 𝟚 → 2 ) MATHEMATICAL DOUBLE-STRUCK DIGIT TWO → DIGIT TWO#
 {120795, "3"}, // MA# ( 𝟛 → 3 ) MATHEMATICAL DOUBLE-STRUCK DIGIT THREE → DIGIT THREE#
 {120796, "4"}, // MA# ( 𝟜 → 4 ) MATHEMATICAL DOUBLE-STRUCK DIGIT FOUR → DIGIT FOUR#
 {120797, "5"}, // MA# ( 𝟝 → 5 ) MATHEMATICAL DOUBLE-STRUCK DIGIT FIVE → DIGIT FIVE#
 {120798, "6"}, // MA# ( 𝟞 → 6 ) MATHEMATICAL DOUBLE-STRUCK DIGIT SIX → DIGIT SIX#
 {120799, "7"}, // MA# ( 𝟟 → 7 ) MATHEMATICAL DOUBLE-STRUCK DIGIT SEVEN → DIGIT SEVEN#
 {120800, "8"}, // MA# ( 𝟠 → 8 ) MATHEMATICAL DOUBLE-STRUCK DIGIT EIGHT → DIGIT EIGHT#
 {120801, "9"}, // MA# ( 𝟡 → 9 ) MATHEMATICAL DOUBLE-STRUCK DIGIT NINE → DIGIT NINE#
 {120802, "O"}, // MA# ( 𝟢 → O ) MATHEMATICAL SANS-SERIF DIGIT ZERO → LATIN CAPITAL LETTER O# →0→
 {120803, "l"}, // MA# ( 𝟣 → l ) MATHEMATICAL SANS-SERIF DIGIT ONE → LATIN SMALL LETTER L# →1→
 {120804, "2"}, // MA# ( 𝟤 → 2 ) MATHEMATICAL SANS-SERIF DIGIT TWO → DIGIT TWO#
 {120805, "3"}, // MA# ( 𝟥 → 3 ) MATHEMATICAL SANS-SERIF DIGIT THREE → DIGIT THREE#
 {120806, "4"}, // MA# ( 𝟦 → 4 ) MATHEMATICAL SANS-SERIF DIGIT FOUR → DIGIT FOUR#
 {120807, "5"}, // MA# ( 𝟧 → 5 ) MATHEMATICAL SANS-SERIF DIGIT FIVE → DIGIT FIVE#
 {120808, "6"}, // MA# ( 𝟨 → 6 ) MATHEMATICAL SANS-SERIF DIGIT SIX → DIGIT SIX#
 {120809, "7"}, // MA# ( 𝟩 → 7 ) MATHEMATICAL SANS-SERIF DIGIT SEVEN → DIGIT SEVEN#
 {120810, "8"}, // MA# ( 𝟪 → 8 ) MATHEMATICAL SANS-SERIF DIGIT EIGHT → DIGIT EIGHT#
 {120811, "9"}, // MA# ( 𝟫 → 9 ) MATHEMATICAL SANS-SERIF DIGIT NINE → DIGIT NINE#
 {120812, "O"}, // MA# ( 𝟬 → O ) MATHEMATICAL SANS-SERIF BOLD DIGIT ZERO → LATIN CAPITAL LETTER O# →0→
 {120813, "l"}, // MA# ( 𝟭 → l ) MATHEMATICAL SANS-SERIF BOLD DIGIT ONE → LATIN SMALL LETTER L# →1→
 {120814, "2"}, // MA# ( 𝟮 → 2 ) MATHEMATICAL SANS-SERIF BOLD DIGIT TWO → DIGIT TWO#
 {120815, "3"}, // MA# ( 𝟯 → 3 ) MATHEMATICAL SANS-SERIF BOLD DIGIT THREE → DIGIT THREE#
 {120816, "4"}, // MA# ( 𝟰 → 4 ) MATHEMATICAL SANS-SERIF BOLD DIGIT FOUR → DIGIT FOUR#
 {120817, "5"}, // MA# ( 𝟱 → 5 ) MATHEMATICAL SANS-SERIF BOLD DIGIT FIVE → DIGIT FIVE#
 {120818, "6"}, // MA# ( 𝟲 → 6 ) MATHEMATICAL SANS-SERIF BOLD DIGIT SIX → DIGIT SIX#
 {120819, "7"}, // MA# ( 𝟳 → 7 ) MATHEMATICAL SANS-SERIF BOLD DIGIT SEVEN → DIGIT SEVEN#
 {120820, "8"}, // MA# ( 𝟴 → 8 ) MATHEMATICAL SANS-SERIF BOLD DIGIT EIGHT → DIGIT EIGHT#
 {120821, "9"}, // MA# ( 𝟵 → 9 ) MATHEMATICAL SANS-SERIF BOLD DIGIT NINE → DIGIT NINE#
 {120822, "O"}, // MA# ( 𝟶 → O ) MATHEMATICAL MONOSPACE DIGIT ZERO → LATIN CAPITAL LETTER O# →0→
 {120823, "l"}, // MA# ( 𝟷 → l ) MATHEMATICAL MONOSPACE DIGIT ONE → LATIN SMALL LETTER L# →1→
 {120824, "2"}, // MA# ( 𝟸 → 2 ) MATHEMATICAL MONOSPACE DIGIT TWO → DIGIT TWO#
 {120825, "3"}, // MA# ( 𝟹 → 3 ) MATHEMATICAL MONOSPACE DIGIT THREE → DIGIT THREE#
 {120826, "4"}, // MA# ( 𝟺 → 4 ) MATHEMATICAL MONOSPACE DIGIT FOUR → DIGIT FOUR#
 {120827, "5"}, // MA# ( 𝟻 → 5 ) MATHEMATICAL MONOSPACE DIGIT FIVE → DIGIT FIVE#
 {120828, "6"}, // MA# ( 𝟼 → 6 ) MATHEMATICAL MONOSPACE DIGIT SIX → DIGIT SIX#
 {120829, "7"}, // MA# ( 𝟽 → 7 ) MATHEMATICAL MONOSPACE DIGIT SEVEN → DIGIT SEVEN#
 {120830, "8"}, // MA# ( 𝟾 → 8 ) MATHEMATICAL MONOSPACE DIGIT EIGHT → DIGIT EIGHT#
 {120831, "9"}, // MA# ( 𝟿 → 9 ) MATHEMATICAL MONOSPACE DIGIT NINE → DIGIT NINE#
 {125127, "l"}, // MA#* ( ‎𞣇‎ → l ) MENDE KIKAKUI DIGIT ONE → LATIN SMALL LETTER L#
 {125131, "8"}, // MA#* ( ‎𞣋‎ → 8 ) MENDE KIKAKUI DIGIT FIVE → DIGIT EIGHT#
 {126464, "l"}, // MA# ( ‎𞸀‎ → l ) ARABIC MATHEMATICAL ALEF → LATIN SMALL LETTER L# →‎ا‎→→1→
 {126500, "o"}, // MA# ( ‎𞸤‎ → o ) ARABIC MATHEMATICAL INITIAL HEH → LATIN SMALL LETTER O# →‎ه‎→
 {126564, "o"}, // MA# ( ‎𞹤‎ → o ) ARABIC MATHEMATICAL STRETCHED HEH → LATIN SMALL LETTER O# →‎ه‎→
 {126592, "l"}, // MA# ( ‎𞺀‎ → l ) ARABIC MATHEMATICAL LOOPED ALEF → LATIN SMALL LETTER L# →‎ا‎→→1→
 {126596, "o"}, // MA# ( ‎𞺄‎ → o ) ARABIC MATHEMATICAL LOOPED HEH → LATIN SMALL LETTER O# →‎ه‎→
 {127232, "O."}, // MA#* ( 🄀 → O. ) DIGIT ZERO FULL STOP → LATIN CAPITAL LETTER O, FULL STOP# →0.→
 {127233, "O,"}, // MA#* ( 🄁 → O, ) DIGIT ZERO COMMA → LATIN CAPITAL LETTER O, COMMA# →0,→
 {127234, "l,"}, // MA#* ( 🄂 → l, ) DIGIT ONE COMMA → LATIN SMALL LETTER L, COMMA# →1,→
 {127235, "2,"}, // MA#* ( 🄃 → 2, ) DIGIT TWO COMMA → DIGIT TWO, COMMA#
 {127236, "3,"}, // MA#* ( 🄄 → 3, ) DIGIT THREE COMMA → DIGIT THREE, COMMA#
 {127237, "4,"}, // MA#* ( 🄅 → 4, ) DIGIT FOUR COMMA → DIGIT FOUR, COMMA#
 {127238, "5,"}, // MA#* ( 🄆 → 5, ) DIGIT FIVE COMMA → DIGIT FIVE, COMMA#
 {127239, "6,"}, // MA#* ( 🄇 → 6, ) DIGIT SIX COMMA → DIGIT SIX, COMMA#
 {127240, "7,"}, // MA#* ( 🄈 → 7, ) DIGIT SEVEN COMMA → DIGIT SEVEN, COMMA#
 {127241, "8,"}, // MA#* ( 🄉 → 8, ) DIGIT EIGHT COMMA → DIGIT EIGHT, COMMA#
 {127242, "9,"}, // MA#* ( 🄊 → 9, ) DIGIT NINE COMMA → DIGIT NINE, COMMA#
 {127248, "(A)"}, // MA#* ( 🄐 → (A) ) PARENTHESIZED LATIN CAPITAL LETTER A → LEFT PARENTHESIS, LATIN CAPITAL LETTER A, RIGHT PARENTHESIS#
 {127249, "(B)"}, // MA#* ( 🄑 → (B) ) PARENTHESIZED LATIN CAPITAL LETTER B → LEFT PARENTHESIS, LATIN CAPITAL LETTER B, RIGHT PARENTHESIS#
 {127250, "(C)"}, // MA#* ( 🄒 → (C) ) PARENTHESIZED LATIN CAPITAL LETTER C → LEFT PARENTHESIS, LATIN CAPITAL LETTER C, RIGHT PARENTHESIS#
 {127251, "(D)"}, // MA#* ( 🄓 → (D) ) PARENTHESIZED LATIN CAPITAL LETTER D → LEFT PARENTHESIS, LATIN CAPITAL LETTER D, RIGHT PARENTHESIS#
 {127252, "(E)"}, // MA#* ( 🄔 → (E) ) PARENTHESIZED LATIN CAPITAL LETTER E → LEFT PARENTHESIS, LATIN CAPITAL LETTER E, RIGHT PARENTHESIS#
 {127253, "(F)"}, // MA#* ( 🄕 → (F) ) PARENTHESIZED LATIN CAPITAL LETTER F → LEFT PARENTHESIS, LATIN CAPITAL LETTER F, RIGHT PARENTHESIS#
 {127254, "(G)"}, // MA#* ( 🄖 → (G) ) PARENTHESIZED LATIN CAPITAL LETTER G → LEFT PARENTHESIS, LATIN CAPITAL LETTER G, RIGHT PARENTHESIS#
 {127255, "(H)"}, // MA#* ( 🄗 → (H) ) PARENTHESIZED LATIN CAPITAL LETTER H → LEFT PARENTHESIS, LATIN CAPITAL LETTER H, RIGHT PARENTHESIS#
 {127256, "(l)"}, // MA#* ( 🄘 → (l) ) PARENTHESIZED LATIN CAPITAL LETTER I → LEFT PARENTHESIS, LATIN SMALL LETTER L, RIGHT PARENTHESIS# →(I)→
 {127257, "(J)"}, // MA#* ( 🄙 → (J) ) PARENTHESIZED LATIN CAPITAL LETTER J → LEFT PARENTHESIS, LATIN CAPITAL LETTER J, RIGHT PARENTHESIS#
 {127258, "(K)"}, // MA#* ( 🄚 → (K) ) PARENTHESIZED LATIN CAPITAL LETTER K → LEFT PARENTHESIS, LATIN CAPITAL LETTER K, RIGHT PARENTHESIS#
 {127259, "(L)"}, // MA#* ( 🄛 → (L) ) PARENTHESIZED LATIN CAPITAL LETTER L → LEFT PARENTHESIS, LATIN CAPITAL LETTER L, RIGHT PARENTHESIS#
 {127260, "(M)"}, // MA#* ( 🄜 → (M) ) PARENTHESIZED LATIN CAPITAL LETTER M → LEFT PARENTHESIS, LATIN CAPITAL LETTER M, RIGHT PARENTHESIS#
 {127261, "(N)"}, // MA#* ( 🄝 → (N) ) PARENTHESIZED LATIN CAPITAL LETTER N → LEFT PARENTHESIS, LATIN CAPITAL LETTER N, RIGHT PARENTHESIS#
 {127262, "(O)"}, // MA#* ( 🄞 → (O) ) PARENTHESIZED LATIN CAPITAL LETTER O → LEFT PARENTHESIS, LATIN CAPITAL LETTER O, RIGHT PARENTHESIS#
 {127263, "(P)"}, // MA#* ( 🄟 → (P) ) PARENTHESIZED LATIN CAPITAL LETTER P → LEFT PARENTHESIS, LATIN CAPITAL LETTER P, RIGHT PARENTHESIS#
 {127264, "(Q)"}, // MA#* ( 🄠 → (Q) ) PARENTHESIZED LATIN CAPITAL LETTER Q → LEFT PARENTHESIS, LATIN CAPITAL LETTER Q, RIGHT PARENTHESIS#
 {127265, "(R)"}, // MA#* ( 🄡 → (R) ) PARENTHESIZED LATIN CAPITAL LETTER R → LEFT PARENTHESIS, LATIN CAPITAL LETTER R, RIGHT PARENTHESIS#
 {127266, "(S)"}, // MA#* ( 🄢 → (S) ) PARENTHESIZED LATIN CAPITAL LETTER S → LEFT PARENTHESIS, LATIN CAPITAL LETTER S, RIGHT PARENTHESIS#
 {127267, "(T)"}, // MA#* ( 🄣 → (T) ) PARENTHESIZED LATIN CAPITAL LETTER T → LEFT PARENTHESIS, LATIN CAPITAL LETTER T, RIGHT PARENTHESIS#
 {127268, "(U)"}, // MA#* ( 🄤 → (U) ) PARENTHESIZED LATIN CAPITAL LETTER U → LEFT PARENTHESIS, LATIN CAPITAL LETTER U, RIGHT PARENTHESIS#
 {127269, "(V)"}, // MA#* ( 🄥 → (V) ) PARENTHESIZED LATIN CAPITAL LETTER V → LEFT PARENTHESIS, LATIN CAPITAL LETTER V, RIGHT PARENTHESIS#
 {127270, "(W)"}, // MA#* ( 🄦 → (W) ) PARENTHESIZED LATIN CAPITAL LETTER W → LEFT PARENTHESIS, LATIN CAPITAL LETTER W, RIGHT PARENTHESIS#
 {127271, "(X)"}, // MA#* ( 🄧 → (X) ) PARENTHESIZED LATIN CAPITAL LETTER X → LEFT PARENTHESIS, LATIN CAPITAL LETTER X, RIGHT PARENTHESIS#
 {127272, "(Y)"}, // MA#* ( 🄨 → (Y) ) PARENTHESIZED LATIN CAPITAL LETTER Y → LEFT PARENTHESIS, LATIN CAPITAL LETTER Y, RIGHT PARENTHESIS#
 {127273, "(Z)"}, // MA#* ( 🄩 → (Z) ) PARENTHESIZED LATIN CAPITAL LETTER Z → LEFT PARENTHESIS, LATIN CAPITAL LETTER Z, RIGHT PARENTHESIS#
 {127274, "(S)"}, // MA#* ( 🄪 → (S) ) TORTOISE SHELL BRACKETED LATIN CAPITAL LETTER S → LEFT PARENTHESIS, LATIN CAPITAL LETTER S, RIGHT PARENTHESIS# →〔S〕→
 {128768, "QE"}, // MA#* ( 🜀 → QE ) ALCHEMICAL SYMBOL FOR QUINTESSENCE → LATIN CAPITAL LETTER Q, LATIN CAPITAL LETTER E#
 {128775, "AR"}, // MA#* ( 🜇 → AR ) ALCHEMICAL SYMBOL FOR AQUA REGIA-2 → LATIN CAPITAL LETTER A, LATIN CAPITAL LETTER R#
 {128844, "C"}, // MA#* ( 🝌 → C ) ALCHEMICAL SYMBOL FOR CALX → LATIN CAPITAL LETTER C#
 {128860, "sss"}, // MA#* ( 🝜 → sss ) ALCHEMICAL SYMBOL FOR STRATUM SUPER STRATUM → LATIN SMALL LETTER S, LATIN SMALL LETTER S, LATIN SMALL LETTER S#
 {128872, "T"}, // MA#* ( 🝨 → T ) ALCHEMICAL SYMBOL FOR CRUCIBLE-4 → LATIN CAPITAL LETTER T#
 {128875, "MB"}, // MA#* ( 🝫 → MB ) ALCHEMICAL SYMBOL FOR BATH OF MARY → LATIN CAPITAL LETTER M, LATIN CAPITAL LETTER B#
 {128876, "VB"}, // MA#* ( 🝬 → VB ) ALCHEMICAL SYMBOL FOR BATH OF VAPOURS → LATIN CAPITAL LETTER V, LATIN CAPITAL LETTER B#
};
const char* findConfusable(uint32_t codepoint)
{
 auto it = std::lower_bound(std::begin(kConfusables), std::end(kConfusables), codepoint, [](const Confusable& lhs, uint32_t rhs) {
 return lhs.codepoint < rhs;
 });
 return (it != std::end(kConfusables) && it->codepoint == codepoint) ? it->text : nullptr;
}
}
#include <vector>
namespace Luau
{
class Allocator
{
public:
 Allocator();
 Allocator(Allocator&&);
 Allocator& operator=(Allocator&&) = delete;
 ~Allocator();
 void* allocate(size_t size);
 template<typename T, typename... Args>
 T* alloc(Args&&... args)
 {
 static_assert(std::is_trivially_destructible<T>::value, "Objects allocated with this allocator will never have their destructors run!");
 T* t = static_cast<T*>(allocate(sizeof(T)));
 new (t) T(std::forward<Args>(args)...);
 return t;
 }
private:
 struct Page
 {
 Page* next;
 char data[8192];
 };
 Page* root;
 size_t offset;
};
struct Lexeme
{
 enum Type
 {
 Eof = 0,
 Char_END = 256,
 Equal,
 LessEqual,
 GreaterEqual,
 NotEqual,
 Dot2,
 Dot3,
 SkinnyArrow,
 DoubleColon,
 InterpStringBegin,
 InterpStringMid,
 InterpStringEnd,
 InterpStringSimple,
 AddAssign,
 SubAssign,
 MulAssign,
 DivAssign,
 ModAssign,
 PowAssign,
 ConcatAssign,
 RawString,
 QuotedString,
 Number,
 Name,
 Comment,
 BlockComment,
 BrokenString,
 BrokenComment,
 BrokenUnicode,
 BrokenInterpDoubleBrace,
 Error,
 Reserved_BEGIN,
 ReservedAnd = Reserved_BEGIN,
 ReservedBreak,
 ReservedDo,
 ReservedElse,
 ReservedElseif,
 ReservedEnd,
 ReservedFalse,
 ReservedFor,
 ReservedFunction,
 ReservedIf,
 ReservedIn,
 ReservedLocal,
 ReservedNil,
 ReservedNot,
 ReservedOr,
 ReservedRepeat,
 ReservedReturn,
 ReservedThen,
 ReservedTrue,
 ReservedUntil,
 ReservedWhile,
 Reserved_END
 };
 Type type;
 Location location;
 unsigned int length;
 union
 {
 const char* data;
 const char* name; // Name
 unsigned int codepoint;
 };
 Lexeme(const Location& location, Type type);
 Lexeme(const Location& location, char character);
 Lexeme(const Location& location, Type type, const char* data, size_t size);
 Lexeme(const Location& location, Type type, const char* name);
 std::string toString() const;
};
class AstNameTable
{
public:
 AstNameTable(Allocator& allocator);
 AstName addStatic(const char* name, Lexeme::Type type = Lexeme::Name);
 std::pair<AstName, Lexeme::Type> getOrAddWithType(const char* name, size_t length);
 std::pair<AstName, Lexeme::Type> getWithType(const char* name, size_t length) const;
 AstName getOrAdd(const char* name);
 AstName get(const char* name) const;
private:
 struct Entry
 {
 AstName value;
 uint32_t length;
 Lexeme::Type type;
 bool operator==(const Entry& other) const;
 };
 struct EntryHash
 {
 size_t operator()(const Entry& e) const;
 };
 DenseHashSet<Entry, EntryHash> data;
 Allocator& allocator;
};
class Lexer
{
public:
 Lexer(const char* buffer, std::size_t bufferSize, AstNameTable& names);
 void setSkipComments(bool skip);
 void setReadNames(bool read);
 const Location& previousLocation() const
 {
 return prevLocation;
 }
 const Lexeme& next();
 const Lexeme& next(bool skipComments, bool updatePrevLocation);
 void nextline();
 Lexeme lookahead();
 const Lexeme& current() const
 {
 return lexeme;
 }
 static bool isReserved(const std::string& word);
 static bool fixupQuotedString(std::string& data);
 static void fixupMultilineString(std::string& data);
private:
 char peekch() const;
 char peekch(unsigned int lookahead) const;
 Position position() const;
 void consume();
 Lexeme readCommentBody();
 int skipLongSeparator();
 Lexeme readLongString(const Position& start, int sep, Lexeme::Type ok, Lexeme::Type broken);
 Lexeme readQuotedString();
 Lexeme readInterpolatedStringBegin();
 Lexeme readInterpolatedStringSection(Position start, Lexeme::Type formatType, Lexeme::Type endType);
 void readBackslashInString();
 std::pair<AstName, Lexeme::Type> readName();
 Lexeme readNumber(const Position& start, unsigned int startOffset);
 Lexeme readUtf8Error();
 Lexeme readNext();
 const char* buffer;
 std::size_t bufferSize;
 unsigned int offset;
 unsigned int line;
 unsigned int lineOffset;
 Lexeme lexeme;
 Location prevLocation;
 AstNameTable& names;
 bool skipComments;
 bool readNames;
 enum class BraceType
 {
 InterpolatedString,
 Normal
 };
 std::vector<BraceType> braceStack;
};
inline bool isSpace(char ch)
{
 return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '\v' || ch == '\f';
}
}
#include <stdarg.h>
namespace Luau
{
std::string format(const char* fmt, ...) LUAU_PRINTF_ATTR(1, 2);
std::string vformat(const char* fmt, va_list args);
void formatAppend(std::string& str, const char* fmt, ...) LUAU_PRINTF_ATTR(2, 3);
void vformatAppend(std::string& ret, const char* fmt, va_list args);
std::string join(const std::vector<std::string_view>& segments, std::string_view delimiter);
std::string join(const std::vector<std::string>& segments, std::string_view delimiter);
std::vector<std::string_view> split(std::string_view s, char delimiter);
size_t editDistance(std::string_view a, std::string_view b);
bool startsWith(std::string_view lhs, std::string_view rhs);
bool equalsLower(std::string_view lhs, std::string_view rhs);
size_t hashRange(const char* data, size_t size);
std::string escape(std::string_view s, bool escapeForInterpString = false);
bool isIdentifier(std::string_view s);
}
namespace Luau
{
Allocator::Allocator()
 : root(static_cast<Page*>(operator new(sizeof(Page))))
 , offset(0)
{
 root->next = nullptr;
}
Allocator::Allocator(Allocator&& rhs)
 : root(rhs.root)
 , offset(rhs.offset)
{
 rhs.root = nullptr;
 rhs.offset = 0;
}
Allocator::~Allocator()
{
 Page* page = root;
 while (page)
 {
 Page* next = page->next;
 operator delete(page);
 page = next;
 }
}
void* Allocator::allocate(size_t size)
{
 constexpr size_t align = alignof(void*) > alignof(double) ? alignof(void*) : alignof(double);
 if (root)
 {
 uintptr_t data = reinterpret_cast<uintptr_t>(root->data);
 uintptr_t result = (data + offset + align - 1) & ~(align - 1);
 if (result + size <= data + sizeof(root->data))
 {
 offset = result - data + size;
 return reinterpret_cast<void*>(result);
 }
 }
 size_t pageSize = size > sizeof(root->data) ? size : sizeof(root->data);
 void* pageData = operator new(offsetof(Page, data) + pageSize);
 Page* page = static_cast<Page*>(pageData);
 page->next = root;
 root = page;
 offset = size;
 return page->data;
}
Lexeme::Lexeme(const Location& location, Type type)
 : type(type)
 , location(location)
 , length(0)
 , data(nullptr)
{
}
Lexeme::Lexeme(const Location& location, char character)
 : type(static_cast<Type>(static_cast<unsigned char>(character)))
 , location(location)
 , length(0)
 , data(nullptr)
{
}
Lexeme::Lexeme(const Location& location, Type type, const char* data, size_t size)
 : type(type)
 , location(location)
 , length(unsigned(size))
 , data(data)
{
 LUAU_ASSERT(type == RawString || type == QuotedString || type == InterpStringBegin || type == InterpStringMid || type == InterpStringEnd ||
 type == InterpStringSimple || type == BrokenInterpDoubleBrace || type == Number || type == Comment || type == BlockComment);
}
Lexeme::Lexeme(const Location& location, Type type, const char* name)
 : type(type)
 , location(location)
 , length(0)
 , name(name)
{
 LUAU_ASSERT(type == Name || (type >= Reserved_BEGIN && type < Lexeme::Reserved_END));
}
static const char* kReserved[] = {"and", "break", "do", "else", "elseif", "end", "false", "for", "function", "if", "in", "local", "nil", "not", "or",
 "repeat", "return", "then", "true", "until", "while"};
std::string Lexeme::toString() const
{
 switch (type)
 {
 case Eof:
 return "<eof>";
 case Equal:
 return "'=='";
 case LessEqual:
 return "'<='";
 case GreaterEqual:
 return "'>='";
 case NotEqual:
 return "'~='";
 case Dot2:
 return "'..'";
 case Dot3:
 return "'...'";
 case SkinnyArrow:
 return "'->'";
 case DoubleColon:
 return "'::'";
 case AddAssign:
 return "'+='";
 case SubAssign:
 return "'-='";
 case MulAssign:
 return "'*='";
 case DivAssign:
 return "'/='";
 case ModAssign:
 return "'%='";
 case PowAssign:
 return "'^='";
 case ConcatAssign:
 return "'..='";
 case RawString:
 case QuotedString:
 return data ? format("\"%.*s\"", length, data) : "string";
 case InterpStringBegin:
 return data ? format("`%.*s{", length, data) : "the beginning of an interpolated string";
 case InterpStringMid:
 return data ? format("}%.*s{", length, data) : "the middle of an interpolated string";
 case InterpStringEnd:
 return data ? format("}%.*s`", length, data) : "the end of an interpolated string";
 case InterpStringSimple:
 return data ? format("`%.*s`", length, data) : "interpolated string";
 case Number:
 return data ? format("'%.*s'", length, data) : "number";
 case Name:
 return name ? format("'%s'", name) : "identifier";
 case Comment:
 return "comment";
 case BrokenString:
 return "malformed string";
 case BrokenComment:
 return "unfinished comment";
 case BrokenInterpDoubleBrace:
 return "'{{', which is invalid (did you mean '\\{'?)";
 case BrokenUnicode:
 if (codepoint)
 {
 if (const char* confusable = findConfusable(codepoint))
 return format("Unicode character U+%x (did you mean '%s'?)", codepoint, confusable);
 return format("Unicode character U+%x", codepoint);
 }
 else
 {
 return "invalid UTF-8 sequence";
 }
 default:
 if (type < Char_END)
 return format("'%c'", type);
 else if (type >= Reserved_BEGIN && type < Reserved_END)
 return format("'%s'", kReserved[type - Reserved_BEGIN]);
 else
 return "<unknown>";
 }
}
bool AstNameTable::Entry::operator==(const Entry& other) const
{
 return length == other.length && memcmp(value.value, other.value.value, length) == 0;
}
size_t AstNameTable::EntryHash::operator()(const Entry& e) const
{
 uint32_t hash = 2166136261;
 for (size_t i = 0; i < e.length; ++i)
 {
 hash ^= uint8_t(e.value.value[i]);
 hash *= 16777619;
 }
 return hash;
}
AstNameTable::AstNameTable(Allocator& allocator)
 : data({AstName(""), 0, Lexeme::Eof}, 128)
 , allocator(allocator)
{
 static_assert(sizeof(kReserved) / sizeof(kReserved[0]) == Lexeme::Reserved_END - Lexeme::Reserved_BEGIN);
 for (int i = Lexeme::Reserved_BEGIN; i < Lexeme::Reserved_END; ++i)
 addStatic(kReserved[i - Lexeme::Reserved_BEGIN], static_cast<Lexeme::Type>(i));
}
AstName AstNameTable::addStatic(const char* name, Lexeme::Type type)
{
 AstNameTable::Entry entry = {AstName(name), uint32_t(strlen(name)), type};
 LUAU_ASSERT(!data.contains(entry));
 data.insert(entry);
 return entry.value;
}
std::pair<AstName, Lexeme::Type> AstNameTable::getOrAddWithType(const char* name, size_t length)
{
 AstNameTable::Entry key = {AstName(name), uint32_t(length), Lexeme::Eof};
 const Entry& entry = data.insert(key);
 if (entry.type != Lexeme::Eof)
 return std::make_pair(entry.value, entry.type);
 char* nameData = static_cast<char*>(allocator.allocate(length + 1));
 memcpy(nameData, name, length);
 nameData[length] = 0;
 const_cast<Entry&>(entry).value = AstName(nameData);
 const_cast<Entry&>(entry).type = Lexeme::Name;
 return std::make_pair(entry.value, entry.type);
}
std::pair<AstName, Lexeme::Type> AstNameTable::getWithType(const char* name, size_t length) const
{
 if (const Entry* entry = data.find({AstName(name), uint32_t(length), Lexeme::Eof}))
 {
 return std::make_pair(entry->value, entry->type);
 }
 return std::make_pair(AstName(), Lexeme::Name);
}
AstName AstNameTable::getOrAdd(const char* name)
{
 return getOrAddWithType(name, strlen(name)).first;
}
AstName AstNameTable::get(const char* name) const
{
 return getWithType(name, strlen(name)).first;
}
inline bool isAlpha(char ch)
{
 return unsigned((ch | ' ') - 'a') < 26;
}
inline bool isDigit(char ch)
{
 return unsigned(ch - '0') < 10;
}
inline bool isHexDigit(char ch)
{
 return unsigned(ch - '0') < 10 || unsigned((ch | ' ') - 'a') < 6;
}
inline bool isNewline(char ch)
{
 return ch == '\n';
}
static char unescape(char ch)
{
 switch (ch)
 {
 case 'a':
 return '\a';
 case 'b':
 return '\b';
 case 'f':
 return '\f';
 case 'n':
 return '\n';
 case 'r':
 return '\r';
 case 't':
 return '\t';
 case 'v':
 return '\v';
 default:
 return ch;
 }
}
Lexer::Lexer(const char* buffer, size_t bufferSize, AstNameTable& names)
 : buffer(buffer)
 , bufferSize(bufferSize)
 , offset(0)
 , line(0)
 , lineOffset(0)
 , lexeme(Location(Position(0, 0), 0), Lexeme::Eof)
 , names(names)
 , skipComments(false)
 , readNames(true)
{
}
void Lexer::setSkipComments(bool skip)
{
 skipComments = skip;
}
void Lexer::setReadNames(bool read)
{
 readNames = read;
}
const Lexeme& Lexer::next()
{
 return next(this->skipComments, true);
}
const Lexeme& Lexer::next(bool skipComments, bool updatePrevLocation)
{
 do
 {
 while (isSpace(peekch()))
 consume();
 if (updatePrevLocation)
 prevLocation = lexeme.location;
 lexeme = readNext();
 updatePrevLocation = false;
 } while (skipComments && (lexeme.type == Lexeme::Comment || lexeme.type == Lexeme::BlockComment));
 return lexeme;
}
void Lexer::nextline()
{
 while (peekch() != 0 && peekch() != '\r' && !isNewline(peekch()))
 consume();
 next();
}
Lexeme Lexer::lookahead()
{
 unsigned int currentOffset = offset;
 unsigned int currentLine = line;
 unsigned int currentLineOffset = lineOffset;
 Lexeme currentLexeme = lexeme;
 Location currentPrevLocation = prevLocation;
 Lexeme result = next();
 offset = currentOffset;
 line = currentLine;
 lineOffset = currentLineOffset;
 lexeme = currentLexeme;
 prevLocation = currentPrevLocation;
 return result;
}
bool Lexer::isReserved(const std::string& word)
{
 for (int i = Lexeme::Reserved_BEGIN; i < Lexeme::Reserved_END; ++i)
 if (word == kReserved[i - Lexeme::Reserved_BEGIN])
 return true;
 return false;
}
LUAU_FORCEINLINE
char Lexer::peekch() const
{
 return (offset < bufferSize) ? buffer[offset] : 0;
}
LUAU_FORCEINLINE
char Lexer::peekch(unsigned int lookahead) const
{
 return (offset + lookahead < bufferSize) ? buffer[offset + lookahead] : 0;
}
Position Lexer::position() const
{
 return Position(line, offset - lineOffset);
}
void Lexer::consume()
{
 if (isNewline(buffer[offset]))
 {
 line++;
 lineOffset = offset + 1;
 }
 offset++;
}
Lexeme Lexer::readCommentBody()
{
 Position start = position();
 LUAU_ASSERT(peekch(0) == '-' && peekch(1) == '-');
 consume();
 consume();
 size_t startOffset = offset;
 if (peekch() == '[')
 {
 int sep = skipLongSeparator();
 if (sep >= 0)
 {
 return readLongString(start, sep, Lexeme::BlockComment, Lexeme::BrokenComment);
 }
 }
 while (peekch() != 0 && peekch() != '\r' && !isNewline(peekch()))
 consume();
 return Lexeme(Location(start, position()), Lexeme::Comment, &buffer[startOffset], offset - startOffset);
}
int Lexer::skipLongSeparator()
{
 char start = peekch();
 LUAU_ASSERT(start == '[' || start == ']');
 consume();
 int count = 0;
 while (peekch() == '=')
 {
 consume();
 count++;
 }
 return (start == peekch()) ? count : (-count) - 1;
}
Lexeme Lexer::readLongString(const Position& start, int sep, Lexeme::Type ok, Lexeme::Type broken)
{
 LUAU_ASSERT(peekch() == '[');
 consume();
 unsigned int startOffset = offset;
 while (peekch())
 {
 if (peekch() == ']')
 {
 if (skipLongSeparator() == sep)
 {
 LUAU_ASSERT(peekch() == ']');
 consume();
 unsigned int endOffset = offset - sep - 2;
 LUAU_ASSERT(endOffset >= startOffset);
 return Lexeme(Location(start, position()), ok, &buffer[startOffset], endOffset - startOffset);
 }
 }
 else
 {
 consume();
 }
 }
 return Lexeme(Location(start, position()), broken);
}
void Lexer::readBackslashInString()
{
 LUAU_ASSERT(peekch() == '\\');
 consume();
 switch (peekch())
 {
 case '\r':
 consume();
 if (peekch() == '\n')
 consume();
 break;
 case 0:
 break;
 case 'z':
 consume();
 while (isSpace(peekch()))
 consume();
 break;
 default:
 consume();
 }
}
Lexeme Lexer::readQuotedString()
{
 Position start = position();
 char delimiter = peekch();
 LUAU_ASSERT(delimiter == '\'' || delimiter == '"');
 consume();
 unsigned int startOffset = offset;
 while (peekch() != delimiter)
 {
 switch (peekch())
 {
 case 0:
 case '\r':
 case '\n':
 return Lexeme(Location(start, position()), Lexeme::BrokenString);
 case '\\':
 readBackslashInString();
 break;
 default:
 consume();
 }
 }
 consume();
 return Lexeme(Location(start, position()), Lexeme::QuotedString, &buffer[startOffset], offset - startOffset - 1);
}
Lexeme Lexer::readInterpolatedStringBegin()
{
 LUAU_ASSERT(peekch() == '`');
 Position start = position();
 consume();
 return readInterpolatedStringSection(start, Lexeme::InterpStringBegin, Lexeme::InterpStringSimple);
}
Lexeme Lexer::readInterpolatedStringSection(Position start, Lexeme::Type formatType, Lexeme::Type endType)
{
 unsigned int startOffset = offset;
 while (peekch() != '`')
 {
 switch (peekch())
 {
 case 0:
 case '\r':
 case '\n':
 return Lexeme(Location(start, position()), Lexeme::BrokenString);
 case '\\':
 if (peekch(1) == 'u' && peekch(2) == '{')
 {
 consume();
 consume(); // u
 consume();
 break;
 }
 readBackslashInString();
 break;
 case '{':
 {
 braceStack.push_back(BraceType::InterpolatedString);
 if (peekch(1) == '{')
 {
 Lexeme brokenDoubleBrace =
 Lexeme(Location(start, position()), Lexeme::BrokenInterpDoubleBrace, &buffer[startOffset], offset - startOffset);
 consume();
 consume();
 return brokenDoubleBrace;
 }
 consume();
 return Lexeme(Location(start, position()), formatType, &buffer[startOffset], offset - startOffset - 1);
 }
 default:
 consume();
 }
 }
 consume();
 return Lexeme(Location(start, position()), endType, &buffer[startOffset], offset - startOffset - 1);
}
Lexeme Lexer::readNumber(const Position& start, unsigned int startOffset)
{
 LUAU_ASSERT(isDigit(peekch()));
 do
 {
 consume();
 } while (isDigit(peekch()) || peekch() == '.' || peekch() == '_');
 if (peekch() == 'e' || peekch() == 'E')
 {
 consume();
 if (peekch() == '+' || peekch() == '-')
 consume();
 }
 while (isAlpha(peekch()) || isDigit(peekch()) || peekch() == '_')
 consume();
 return Lexeme(Location(start, position()), Lexeme::Number, &buffer[startOffset], offset - startOffset);
}
std::pair<AstName, Lexeme::Type> Lexer::readName()
{
 LUAU_ASSERT(isAlpha(peekch()) || peekch() == '_');
 unsigned int startOffset = offset;
 do
 consume();
 while (isAlpha(peekch()) || isDigit(peekch()) || peekch() == '_');
 return readNames ? names.getOrAddWithType(&buffer[startOffset], offset - startOffset)
 : names.getWithType(&buffer[startOffset], offset - startOffset);
}
Lexeme Lexer::readNext()
{
 Position start = position();
 switch (peekch())
 {
 case 0:
 return Lexeme(Location(start, 0), Lexeme::Eof);
 case '-':
 {
 if (peekch(1) == '>')
 {
 consume();
 consume();
 return Lexeme(Location(start, 2), Lexeme::SkinnyArrow);
 }
 else if (peekch(1) == '=')
 {
 consume();
 consume();
 return Lexeme(Location(start, 2), Lexeme::SubAssign);
 }
 else if (peekch(1) == '-')
 {
 return readCommentBody();
 }
 else
 {
 consume();
 return Lexeme(Location(start, 1), '-');
 }
 }
 case '[':
 {
 int sep = skipLongSeparator();
 if (sep >= 0)
 {
 return readLongString(start, sep, Lexeme::RawString, Lexeme::BrokenString);
 }
 else if (sep == -1)
 {
 return Lexeme(Location(start, 1), '[');
 }
 else
 {
 return Lexeme(Location(start, position()), Lexeme::BrokenString);
 }
 }
 case '{':
 {
 consume();
 if (!braceStack.empty())
 braceStack.push_back(BraceType::Normal);
 return Lexeme(Location(start, 1), '{');
 }
 case '}':
 {
 consume();
 if (braceStack.empty())
 {
 return Lexeme(Location(start, 1), '}');
 }
 const BraceType braceStackTop = braceStack.back();
 braceStack.pop_back();
 if (braceStackTop != BraceType::InterpolatedString)
 {
 return Lexeme(Location(start, 1), '}');
 }
 return readInterpolatedStringSection(position(), Lexeme::InterpStringMid, Lexeme::InterpStringEnd);
 }
 case '=':
 {
 consume();
 if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::Equal);
 }
 else
 return Lexeme(Location(start, 1), '=');
 }
 case '<':
 {
 consume();
 if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::LessEqual);
 }
 else
 return Lexeme(Location(start, 1), '<');
 }
 case '>':
 {
 consume();
 if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::GreaterEqual);
 }
 else
 return Lexeme(Location(start, 1), '>');
 }
 case '~':
 {
 consume();
 if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::NotEqual);
 }
 else
 return Lexeme(Location(start, 1), '~');
 }
 case '"':
 case '\'':
 return readQuotedString();
 case '`':
 return readInterpolatedStringBegin();
 case '.':
 consume();
 if (peekch() == '.')
 {
 consume();
 if (peekch() == '.')
 {
 consume();
 return Lexeme(Location(start, 3), Lexeme::Dot3);
 }
 else if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 3), Lexeme::ConcatAssign);
 }
 else
 return Lexeme(Location(start, 2), Lexeme::Dot2);
 }
 else
 {
 if (isDigit(peekch()))
 {
 return readNumber(start, offset - 1);
 }
 else
 return Lexeme(Location(start, 1), '.');
 }
 case '+':
 consume();
 if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::AddAssign);
 }
 else
 return Lexeme(Location(start, 1), '+');
 case '/':
 consume();
 if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::DivAssign);
 }
 else
 return Lexeme(Location(start, 1), '/');
 case '*':
 consume();
 if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::MulAssign);
 }
 else
 return Lexeme(Location(start, 1), '*');
 case '%':
 consume();
 if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::ModAssign);
 }
 else
 return Lexeme(Location(start, 1), '%');
 case '^':
 consume();
 if (peekch() == '=')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::PowAssign);
 }
 else
 return Lexeme(Location(start, 1), '^');
 case ':':
 {
 consume();
 if (peekch() == ':')
 {
 consume();
 return Lexeme(Location(start, 2), Lexeme::DoubleColon);
 }
 else
 return Lexeme(Location(start, 1), ':');
 }
 case '(':
 case ')':
 case ']':
 case ';':
 case ',':
 case '#':
 {
 char ch = peekch();
 consume();
 return Lexeme(Location(start, 1), ch);
 }
 default:
 if (isDigit(peekch()))
 {
 return readNumber(start, offset);
 }
 else if (isAlpha(peekch()) || peekch() == '_')
 {
 std::pair<AstName, Lexeme::Type> name = readName();
 return Lexeme(Location(start, position()), name.second, name.first.value);
 }
 else if (peekch() & 0x80)
 {
 return readUtf8Error();
 }
 else
 {
 char ch = peekch();
 consume();
 return Lexeme(Location(start, 1), ch);
 }
 }
}
LUAU_NOINLINE Lexeme Lexer::readUtf8Error()
{
 Position start = position();
 uint32_t codepoint = 0;
 int size = 0;
 if ((peekch() & 0b10000000) == 0b00000000)
 {
 size = 1;
 codepoint = peekch() & 0x7F;
 }
 else if ((peekch() & 0b11100000) == 0b11000000)
 {
 size = 2;
 codepoint = peekch() & 0b11111;
 }
 else if ((peekch() & 0b11110000) == 0b11100000)
 {
 size = 3;
 codepoint = peekch() & 0b1111;
 }
 else if ((peekch() & 0b11111000) == 0b11110000)
 {
 size = 4;
 codepoint = peekch() & 0b111;
 }
 else
 {
 consume();
 return Lexeme(Location(start, position()), Lexeme::BrokenUnicode);
 }
 consume();
 for (int i = 1; i < size; ++i)
 {
 if ((peekch() & 0b11000000) != 0b10000000)
 return Lexeme(Location(start, position()), Lexeme::BrokenUnicode);
 codepoint = codepoint << 6;
 codepoint |= (peekch() & 0b00111111);
 consume();
 }
 Lexeme result(Location(start, position()), Lexeme::BrokenUnicode);
 result.codepoint = codepoint;
 return result;
}
static size_t toUtf8(char* data, unsigned int code)
{
 if (code < 0x80)
 {
 data[0] = char(code);
 return 1;
 }
 else if (code < 0x800)
 {
 data[0] = char(0xC0 | (code >> 6));
 data[1] = char(0x80 | (code & 0x3F));
 return 2;
 }
 else if (code < 0x10000)
 {
 data[0] = char(0xE0 | (code >> 12));
 data[1] = char(0x80 | ((code >> 6) & 0x3F));
 data[2] = char(0x80 | (code & 0x3F));
 return 3;
 }
 else if (code < 0x110000)
 {
 data[0] = char(0xF0 | (code >> 18));
 data[1] = char(0x80 | ((code >> 12) & 0x3F));
 data[2] = char(0x80 | ((code >> 6) & 0x3F));
 data[3] = char(0x80 | (code & 0x3F));
 return 4;
 }
 else
 {
 return 0;
 }
}
bool Lexer::fixupQuotedString(std::string& data)
{
 if (data.empty() || data.find('\\') == std::string::npos)
 return true;
 size_t size = data.size();
 size_t write = 0;
 for (size_t i = 0; i < size;)
 {
 if (data[i] != '\\')
 {
 data[write++] = data[i];
 i++;
 continue;
 }
 if (i + 1 == size)
 return false;
 char escape = data[i + 1];
 i += 2;
 switch (escape)
 {
 case '\n':
 data[write++] = '\n';
 break;
 case '\r':
 data[write++] = '\n';
 if (i < size && data[i] == '\n')
 i++;
 break;
 case 0:
 return false;
 case 'x':
 {
 if (i + 2 > size)
 return false;
 unsigned int code = 0;
 for (int j = 0; j < 2; ++j)
 {
 char ch = data[i + j];
 if (!isHexDigit(ch))
 return false;
 code = 16 * code + (isDigit(ch) ? ch - '0' : (ch | ' ') - 'a' + 10);
 }
 data[write++] = char(code);
 i += 2;
 break;
 }
 case 'z':
 {
 while (i < size && isSpace(data[i]))
 i++;
 break;
 }
 case 'u':
 {
 if (i + 3 > size)
 return false;
 if (data[i] != '{')
 return false;
 i++;
 if (data[i] == '}')
 return false;
 unsigned int code = 0;
 for (int j = 0; j < 16; ++j)
 {
 if (i == size)
 return false;
 char ch = data[i];
 if (ch == '}')
 break;
 if (!isHexDigit(ch))
 return false;
 code = 16 * code + (isDigit(ch) ? ch - '0' : (ch | ' ') - 'a' + 10);
 i++;
 }
 if (i == size || data[i] != '}')
 return false;
 i++;
 size_t utf8 = toUtf8(&data[write], code);
 if (utf8 == 0)
 return false;
 write += utf8;
 break;
 }
 default:
 {
 if (isDigit(escape))
 {
 unsigned int code = escape - '0';
 for (int j = 0; j < 2; ++j)
 {
 if (i == size || !isDigit(data[i]))
 break;
 code = 10 * code + (data[i] - '0');
 i++;
 }
 if (code > UCHAR_MAX)
 return false;
 data[write++] = char(code);
 }
 else
 {
 data[write++] = unescape(escape);
 }
 }
 }
 }
 LUAU_ASSERT(write <= size);
 data.resize(write);
 return true;
}
void Lexer::fixupMultilineString(std::string& data)
{
 if (data.empty())
 return;
 const char* src = data.c_str();
 char* dst = &data[0];
 if (src[0] == '\r' && src[1] == '\n')
 {
 src += 2;
 }
 else if (src[0] == '\n')
 {
 src += 1;
 }
 while (*src)
 {
 if (src[0] == '\r' && src[1] == '\n')
 {
 *dst++ = '\n';
 src += 2;
 }
 else
 {
 *dst++ = *src;
 src += 1;
 }
 }
 data.resize(dst - &data[0]);
}
}
namespace Luau
{
Position::Position(unsigned int line, unsigned int column)
 : line(line)
 , column(column)
{
}
bool Position::operator==(const Position& rhs) const
{
 return this->column == rhs.column && this->line == rhs.line;
}
bool Position::operator!=(const Position& rhs) const
{
 return !(*this == rhs);
}
bool Position::operator<(const Position& rhs) const
{
 if (line == rhs.line)
 return column < rhs.column;
 else
 return line < rhs.line;
}
bool Position::operator>(const Position& rhs) const
{
 if (line == rhs.line)
 return column > rhs.column;
 else
 return line > rhs.line;
}
bool Position::operator<=(const Position& rhs) const
{
 return *this == rhs || *this < rhs;
}
bool Position::operator>=(const Position& rhs) const
{
 return *this == rhs || *this > rhs;
}
void Position::shift(const Position& start, const Position& oldEnd, const Position& newEnd)
{
 if (*this >= start)
 {
 if (this->line > oldEnd.line)
 this->line += (newEnd.line - oldEnd.line);
 else
 {
 this->line = newEnd.line;
 this->column += (newEnd.column - oldEnd.column);
 }
 }
}
Location::Location()
 : begin(0, 0)
 , end(0, 0)
{
}
Location::Location(const Position& begin, const Position& end)
 : begin(begin)
 , end(end)
{
}
Location::Location(const Position& begin, unsigned int length)
 : begin(begin)
 , end(begin.line, begin.column + length)
{
}
Location::Location(const Location& begin, const Location& end)
 : begin(begin.begin)
 , end(end.end)
{
}
bool Location::operator==(const Location& rhs) const
{
 return this->begin == rhs.begin && this->end == rhs.end;
}
bool Location::operator!=(const Location& rhs) const
{
 return !(*this == rhs);
}
bool Location::encloses(const Location& l) const
{
 return begin <= l.begin && end >= l.end;
}
bool Location::overlaps(const Location& l) const
{
 return (begin <= l.begin && end >= l.begin) || (begin <= l.end && end >= l.end) || (begin >= l.begin && end <= l.end);
}
bool Location::contains(const Position& p) const
{
 return begin <= p && p < end;
}
bool Location::containsClosed(const Position& p) const
{
 return begin <= p && p <= end;
}
void Location::extend(const Location& other)
{
 if (other.begin < begin)
 begin = other.begin;
 if (other.end > end)
 end = other.end;
}
void Location::shift(const Position& start, const Position& oldEnd, const Position& newEnd)
{
 begin.shift(start, oldEnd, newEnd);
 end.shift(start, oldEnd, newEnd);
}
}
namespace Luau
{
enum class Mode
{
 NoCheck,
 Nonstrict, // Unannotated symbols are any
 Strict,
 Definition, // Type definition module, has special parsing rules
};
struct ParseOptions
{
 bool allowDeclarationSyntax = false;
 bool captureComments = false;
};
}
namespace Luau
{
class AstStatBlock;
class ParseError : public std::exception
{
public:
 ParseError(const Location& location, const std::string& message);
 virtual const char* what() const throw();
 const Location& getLocation() const;
 const std::string& getMessage() const;
 static LUAU_NORETURN void raise(const Location& location, const char* format, ...) LUAU_PRINTF_ATTR(2, 3);
private:
 Location location;
 std::string message;
};
class ParseErrors : public std::exception
{
public:
 ParseErrors(std::vector<ParseError> errors);
 virtual const char* what() const throw();
 const std::vector<ParseError>& getErrors() const;
private:
 std::vector<ParseError> errors;
 std::string message;
};
struct HotComment
{
 bool header;
 Location location;
 std::string content;
};
struct Comment
{
 Lexeme::Type type;
 Location location;
};
struct ParseResult
{
 AstStatBlock* root;
 size_t lines = 0;
 std::vector<HotComment> hotcomments;
 std::vector<ParseError> errors;
 std::vector<Comment> commentLocations;
};
static constexpr const char* kParseNameError = "%error-id%";
}
#include <initializer_list>
#include <tuple>
namespace Luau
{
template<typename T>
class TempVector
{
public:
 explicit TempVector(std::vector<T>& storage);
 ~TempVector();
 const T& operator[](std::size_t index) const;
 const T& front() const;
 const T& back() const;
 bool empty() const;
 std::size_t size() const;
 void push_back(const T& item);
 typename std::vector<T>::const_iterator begin() const
 {
 return storage.begin() + offset;
 }
 typename std::vector<T>::const_iterator end() const
 {
 return storage.begin() + offset + size_;
 }
private:
 std::vector<T>& storage;
 size_t offset;
 size_t size_;
};
class Parser
{
public:
 static ParseResult parse(
 const char* buffer, std::size_t bufferSize, AstNameTable& names, Allocator& allocator, ParseOptions options = ParseOptions());
private:
 struct Name;
 struct Binding;
 Parser(const char* buffer, std::size_t bufferSize, AstNameTable& names, Allocator& allocator, const ParseOptions& options);
 bool blockFollow(const Lexeme& l);
 AstStatBlock* parseChunk();
 AstStatBlock* parseBlock();
 AstStatBlock* parseBlockNoScope();
 AstStat* parseStat();
 AstStat* parseIf();
 AstStat* parseWhile();
 AstStat* parseRepeat();
 AstStat* parseDo();
 AstStat* parseBreak();
 AstStat* parseContinue(const Location& start);
 AstStat* parseFor();
 AstExpr* parseFunctionName(Location start, bool& hasself, AstName& debugname);
 AstStat* parseFunctionStat();
 AstStat* parseLocal();
 AstStat* parseReturn();
 AstStat* parseTypeAlias(const Location& start, bool exported);
 AstDeclaredClassProp parseDeclaredClassMethod();
 AstStat* parseDeclaration(const Location& start);
 AstStat* parseAssignment(AstExpr* initial);
 AstStat* parseCompoundAssignment(AstExpr* initial, AstExprBinary::Op op);
 std::pair<AstLocal*, AstArray<AstLocal*>> prepareFunctionArguments(const Location& start, bool hasself, const TempVector<Binding>& args);
 std::pair<AstExprFunction*, AstLocal*> parseFunctionBody(
 bool hasself, const Lexeme& matchFunction, const AstName& debugname, const Name* localName);
 void parseExprList(TempVector<AstExpr*>& result);
 Binding parseBinding();
 std::tuple<bool, Location, AstTypePack*> parseBindingList(TempVector<Binding>& result, bool allowDot3 = false);
 AstType* parseOptionalType();
 AstTypePack* parseTypeList(TempVector<AstType*>& result, TempVector<std::optional<AstArgumentName>>& resultNames);
 std::optional<AstTypeList> parseOptionalReturnType();
 std::pair<Location, AstTypeList> parseReturnType();
 AstTableIndexer* parseTableIndexer();
 AstTypeOrPack parseFunctionType(bool allowPack);
 AstType* parseFunctionTypeTail(const Lexeme& begin, AstArray<AstGenericType> generics, AstArray<AstGenericTypePack> genericPacks,
 AstArray<AstType*> params, AstArray<std::optional<AstArgumentName>> paramNames, AstTypePack* varargAnnotation);
 AstType* parseTableType();
 AstTypeOrPack parseSimpleType(bool allowPack);
 AstTypeOrPack parseTypeOrPack();
 AstType* parseType();
 AstTypePack* parseTypePack();
 AstTypePack* parseVariadicArgumentTypePack();
 AstType* parseTypeSuffix(AstType* type, const Location& begin);
 static std::optional<AstExprUnary::Op> parseUnaryOp(const Lexeme& l);
 static std::optional<AstExprBinary::Op> parseBinaryOp(const Lexeme& l);
 static std::optional<AstExprBinary::Op> parseCompoundOp(const Lexeme& l);
 struct BinaryOpPriority
 {
 unsigned char left, right;
 };
 std::optional<AstExprUnary::Op> checkUnaryConfusables();
 std::optional<AstExprBinary::Op> checkBinaryConfusables(const BinaryOpPriority binaryPriority[], unsigned int limit);
 AstExpr* parseExpr(unsigned int limit = 0);
 AstExpr* parseNameExpr(const char* context = nullptr);
 AstExpr* parsePrefixExpr();
 AstExpr* parsePrimaryExpr(bool asStatement);
 AstExpr* parseAssertionExpr();
 AstExpr* parseSimpleExpr();
 AstExpr* parseFunctionArgs(AstExpr* func, bool self);
 AstExpr* parseTableConstructor();
 AstExpr* parseIfElseExpr();
 AstExpr* parseInterpString();
 std::optional<Name> parseNameOpt(const char* context = nullptr);
 Name parseName(const char* context = nullptr);
 Name parseIndexName(const char* context, const Position& previous);
 std::pair<AstArray<AstGenericType>, AstArray<AstGenericTypePack>> parseGenericTypeList(bool withDefaultValues);
 AstArray<AstTypeOrPack> parseTypeParams();
 std::optional<AstArray<char>> parseCharArray();
 AstExpr* parseString();
 AstExpr* parseNumber();
 AstLocal* pushLocal(const Binding& binding);
 unsigned int saveLocals();
 void restoreLocals(unsigned int offset);
 bool expectAndConsume(char value, const char* context = nullptr);
 bool expectAndConsume(Lexeme::Type type, const char* context = nullptr);
 void expectAndConsumeFail(Lexeme::Type type, const char* context);
 struct MatchLexeme
 {
 MatchLexeme(const Lexeme& l)
 : type(l.type)
 , position(l.location.begin)
 {
 }
 Lexeme::Type type;
 Position position;
 };
 bool expectMatchAndConsume(char value, const MatchLexeme& begin, bool searchForMissing = false);
 void expectMatchAndConsumeFail(Lexeme::Type type, const MatchLexeme& begin, const char* extra = nullptr);
 bool expectMatchAndConsumeRecover(char value, const MatchLexeme& begin, bool searchForMissing);
 bool expectMatchEndAndConsume(Lexeme::Type type, const MatchLexeme& begin);
 void expectMatchEndAndConsumeFail(Lexeme::Type type, const MatchLexeme& begin);
 template<typename T>
 AstArray<T> copy(const T* data, std::size_t size);
 template<typename T>
 AstArray<T> copy(const TempVector<T>& data);
 template<typename T>
 AstArray<T> copy(std::initializer_list<T> data);
 AstArray<char> copy(const std::string& data);
 void incrementRecursionCounter(const char* context);
 void report(const Location& location, const char* format, va_list args);
 void report(const Location& location, const char* format, ...) LUAU_PRINTF_ATTR(3, 4);
 void reportNameError(const char* context);
 AstStatError* reportStatError(const Location& location, const AstArray<AstExpr*>& expressions, const AstArray<AstStat*>& statements,
 const char* format, ...) LUAU_PRINTF_ATTR(5, 6);
 AstExprError* reportExprError(const Location& location, const AstArray<AstExpr*>& expressions, const char* format, ...) LUAU_PRINTF_ATTR(4, 5);
 AstTypeError* reportTypeError(const Location& location, const AstArray<AstType*>& types, const char* format, ...) LUAU_PRINTF_ATTR(4, 5);
 AstTypeError* reportMissingTypeError(const Location& parseErrorLocation, const Location& astErrorLocation, const char* format, ...)
 LUAU_PRINTF_ATTR(4, 5);
 AstExpr* reportFunctionArgsError(AstExpr* func, bool self);
 void reportAmbiguousCallError();
 void nextLexeme();
 struct Function
 {
 bool vararg;
 unsigned int loopDepth;
 Function()
 : vararg(false)
 , loopDepth(0)
 {
 }
 };
 struct Local
 {
 AstLocal* local;
 unsigned int offset;
 Local()
 : local(nullptr)
 , offset(0)
 {
 }
 };
 struct Name
 {
 AstName name;
 Location location;
 Name(const AstName& name, const Location& location)
 : name(name)
 , location(location)
 {
 }
 };
 struct Binding
 {
 Name name;
 AstType* annotation;
 explicit Binding(const Name& name, AstType* annotation = nullptr)
 : name(name)
 , annotation(annotation)
 {
 }
 };
 ParseOptions options;
 Lexer lexer;
 Allocator& allocator;
 std::vector<Comment> commentLocations;
 std::vector<HotComment> hotcomments;
 bool hotcommentHeader = true;
 unsigned int recursionCounter;
 AstName nameSelf;
 AstName nameNumber;
 AstName nameError;
 AstName nameNil;
 MatchLexeme endMismatchSuspect;
 std::vector<Function> functionStack;
 DenseHashMap<AstName, AstLocal*> localMap;
 std::vector<AstLocal*> localStack;
 std::vector<ParseError> parseErrors;
 std::vector<unsigned int> matchRecoveryStopOnToken;
 std::vector<AstStat*> scratchStat;
 std::vector<AstArray<char>> scratchString;
 std::vector<AstExpr*> scratchExpr;
 std::vector<AstExpr*> scratchExprAux;
 std::vector<AstName> scratchName;
 std::vector<AstName> scratchPackName;
 std::vector<Binding> scratchBinding;
 std::vector<AstLocal*> scratchLocal;
 std::vector<AstTableProp> scratchTableTypeProps;
 std::vector<AstType*> scratchType;
 std::vector<AstTypeOrPack> scratchTypeOrPack;
 std::vector<AstDeclaredClassProp> scratchDeclaredClassProps;
 std::vector<AstExprTable::Item> scratchItem;
 std::vector<AstArgumentName> scratchArgName;
 std::vector<AstGenericType> scratchGenericTypes;
 std::vector<AstGenericTypePack> scratchGenericTypePacks;
 std::vector<std::optional<AstArgumentName>> scratchOptArgName;
 std::string scratchData;
};
}
LUAU_FASTFLAG(DebugLuauTimeTracing)
namespace Luau
{
namespace TimeTrace
{
double getClock();
uint32_t getClockMicroseconds();
}
} // namespace Luau
#if defined(LUAU_ENABLE_TIME_TRACE)
namespace Luau
{
namespace TimeTrace
{
struct Token
{
 const char* name;
 const char* category;
};
enum class EventType : uint8_t
{
 Enter,
 Leave,
 ArgName,
 ArgValue,
};
struct Event
{
 EventType type;
 uint16_t token;
 union
 {
 uint32_t microsec;
 uint32_t dataPos;
 } data;
};
struct GlobalContext;
struct ThreadContext;
GlobalContext& getGlobalContext();
uint16_t createToken(GlobalContext& context, const char* name, const char* category);
uint32_t createThread(GlobalContext& context, ThreadContext* threadContext);
void releaseThread(GlobalContext& context, ThreadContext* threadContext);
void flushEvents(GlobalContext& context, uint32_t threadId, const std::vector<Event>& events, const std::vector<char>& data);
struct ThreadContext
{
 ThreadContext()
 : globalContext(getGlobalContext())
 {
 threadId = createThread(globalContext, this);
 }
 ~ThreadContext()
 {
 if (!events.empty())
 flushEvents();
 releaseThread(globalContext, this);
 }
 void flushEvents()
 {
 static uint16_t flushToken = createToken(globalContext, "flushEvents", "TimeTrace");
 events.push_back({EventType::Enter, flushToken, {getClockMicroseconds()}});
 TimeTrace::flushEvents(globalContext, threadId, events, data);
 events.clear();
 data.clear();
 events.push_back({EventType::Leave, 0, {getClockMicroseconds()}});
 }
 void eventEnter(uint16_t token)
 {
 eventEnter(token, getClockMicroseconds());
 }
 void eventEnter(uint16_t token, uint32_t microsec)
 {
 events.push_back({EventType::Enter, token, {microsec}});
 }
 void eventLeave()
 {
 eventLeave(getClockMicroseconds());
 }
 void eventLeave(uint32_t microsec)
 {
 events.push_back({EventType::Leave, 0, {microsec}});
 if (events.size() > kEventFlushLimit)
 flushEvents();
 }
 void eventArgument(const char* name, const char* value)
 {
 uint32_t pos = uint32_t(data.size());
 data.insert(data.end(), name, name + strlen(name) + 1);
 events.push_back({EventType::ArgName, 0, {pos}});
 pos = uint32_t(data.size());
 data.insert(data.end(), value, value + strlen(value) + 1);
 events.push_back({EventType::ArgValue, 0, {pos}});
 }
 GlobalContext& globalContext;
 uint32_t threadId;
 std::vector<Event> events;
 std::vector<char> data;
 static constexpr size_t kEventFlushLimit = 8192;
};
ThreadContext& getThreadContext();
struct Scope
{
 explicit Scope(uint16_t token)
 : context(getThreadContext())
 {
 if (!FFlag::DebugLuauTimeTracing)
 return;
 context.eventEnter(token);
 }
 ~Scope()
 {
 if (!FFlag::DebugLuauTimeTracing)
 return;
 context.eventLeave();
 }
 ThreadContext& context;
};
struct OptionalTailScope
{
 explicit OptionalTailScope(uint16_t token, uint32_t threshold)
 : context(getThreadContext())
 , token(token)
 , threshold(threshold)
 {
 if (!FFlag::DebugLuauTimeTracing)
 return;
 pos = uint32_t(context.events.size());
 microsec = getClockMicroseconds();
 }
 ~OptionalTailScope()
 {
 if (!FFlag::DebugLuauTimeTracing)
 return;
 if (pos == context.events.size())
 {
 uint32_t curr = getClockMicroseconds();
 if (curr - microsec > threshold)
 {
 context.eventEnter(token, microsec);
 context.eventLeave(curr);
 }
 }
 }
 ThreadContext& context;
 uint16_t token;
 uint32_t threshold;
 uint32_t microsec;
 uint32_t pos;
};
LUAU_NOINLINE uint16_t createScopeData(const char* name, const char* category);
}
} // namespace Luau
#define LUAU_TIMETRACE_SCOPE(name, category) static uint16_t lttScopeStatic = Luau::TimeTrace::createScopeData(name, category); Luau::TimeTrace::Scope lttScope(lttScopeStatic)
#define LUAU_TIMETRACE_OPTIONAL_TAIL_SCOPE(name, category, microsec) static uint16_t lttScopeStaticOptTail = Luau::TimeTrace::createScopeData(name, category); Luau::TimeTrace::OptionalTailScope lttScope(lttScopeStaticOptTail, microsec)
#define LUAU_TIMETRACE_ARGUMENT(name, value) do { if (FFlag::DebugLuauTimeTracing) lttScope.context.eventArgument(name, value); } while (false)
#else
#define LUAU_TIMETRACE_SCOPE(name, category)
#define LUAU_TIMETRACE_OPTIONAL_TAIL_SCOPE(name, category, microsec)
#define LUAU_TIMETRACE_ARGUMENT(name, value) do { } while (false)
#endif
#include <errno.h>
LUAU_FASTINTVARIABLE(LuauRecursionLimit, 1000)
LUAU_FASTINTVARIABLE(LuauParseErrorLimit, 100)
#define ERROR_INVALID_INTERP_DOUBLE_BRACE "Double braces are not permitted within interpolated strings. Did you mean '\\{'?"
namespace Luau
{
ParseError::ParseError(const Location& location, const std::string& message)
 : location(location)
 , message(message)
{
}
const char* ParseError::what() const throw()
{
 return message.c_str();
}
const Location& ParseError::getLocation() const
{
 return location;
}
const std::string& ParseError::getMessage() const
{
 return message;
}
LUAU_NOINLINE void ParseError::raise(const Location& location, const char* format, ...)
{
 va_list args;
 va_start(args, format);
 std::string message = vformat(format, args);
 va_end(args);
 throw ParseError(location, message);
}
ParseErrors::ParseErrors(std::vector<ParseError> errors)
 : errors(std::move(errors))
{
 LUAU_ASSERT(!this->errors.empty());
 if (this->errors.size() == 1)
 message = this->errors.front().what();
 else
 message = format("%d parse errors", int(this->errors.size()));
}
const char* ParseErrors::what() const throw()
{
 return message.c_str();
}
const std::vector<ParseError>& ParseErrors::getErrors() const
{
 return errors;
}
template<typename T>
TempVector<T>::TempVector(std::vector<T>& storage)
 : storage(storage)
 , offset(storage.size())
 , size_(0)
{
}
template<typename T>
TempVector<T>::~TempVector()
{
 LUAU_ASSERT(storage.size() == offset + size_);
 storage.erase(storage.begin() + offset, storage.end());
}
template<typename T>
const T& TempVector<T>::operator[](size_t index) const
{
 LUAU_ASSERT(index < size_);
 return storage[offset + index];
}
template<typename T>
const T& TempVector<T>::front() const
{
 LUAU_ASSERT(size_ > 0);
 return storage[offset];
}
template<typename T>
const T& TempVector<T>::back() const
{
 LUAU_ASSERT(size_ > 0);
 return storage.back();
}
template<typename T>
bool TempVector<T>::empty() const
{
 return size_ == 0;
}
template<typename T>
size_t TempVector<T>::size() const
{
 return size_;
}
template<typename T>
void TempVector<T>::push_back(const T& item)
{
 LUAU_ASSERT(storage.size() == offset + size_);
 storage.push_back(item);
 size_++;
}
static bool shouldParseTypePack(Lexer& lexer)
{
 if (lexer.current().type == Lexeme::Dot3)
 return true;
 else if (lexer.current().type == Lexeme::Name && lexer.lookahead().type == Lexeme::Dot3)
 return true;
 return false;
}
ParseResult Parser::parse(const char* buffer, size_t bufferSize, AstNameTable& names, Allocator& allocator, ParseOptions options)
{
 LUAU_TIMETRACE_SCOPE("Parser::parse", "Parser");
 Parser p(buffer, bufferSize, names, allocator, options);
 try
 {
 AstStatBlock* root = p.parseChunk();
 size_t lines = p.lexer.current().location.end.line + (bufferSize > 0 && buffer[bufferSize - 1] != '\n');
 return ParseResult{root, lines, std::move(p.hotcomments), std::move(p.parseErrors), std::move(p.commentLocations)};
 }
 catch (ParseError& err)
 {
 p.parseErrors.push_back(err);
 return ParseResult{nullptr, 0, {}, p.parseErrors};
 }
}
Parser::Parser(const char* buffer, size_t bufferSize, AstNameTable& names, Allocator& allocator, const ParseOptions& options)
 : options(options)
 , lexer(buffer, bufferSize, names)
 , allocator(allocator)
 , recursionCounter(0)
 , endMismatchSuspect(Lexeme(Location(), Lexeme::Eof))
 , localMap(AstName())
{
 Function top;
 top.vararg = true;
 functionStack.reserve(8);
 functionStack.push_back(top);
 nameSelf = names.addStatic("self");
 nameNumber = names.addStatic("number");
 nameError = names.addStatic(kParseNameError);
 nameNil = names.getOrAdd("nil"); // nil is a reserved keyword
 matchRecoveryStopOnToken.assign(Lexeme::Type::Reserved_END, 0);
 matchRecoveryStopOnToken[Lexeme::Type::Eof] = 1;
 lexer.setSkipComments(true);
 LUAU_ASSERT(hotcommentHeader);
 nextLexeme();
 hotcommentHeader = false;
 localStack.reserve(16);
 scratchStat.reserve(16);
 scratchExpr.reserve(16);
 scratchLocal.reserve(16);
 scratchBinding.reserve(16);
}
bool Parser::blockFollow(const Lexeme& l)
{
 return l.type == Lexeme::Eof || l.type == Lexeme::ReservedElse || l.type == Lexeme::ReservedElseif || l.type == Lexeme::ReservedEnd ||
 l.type == Lexeme::ReservedUntil;
}
AstStatBlock* Parser::parseChunk()
{
 AstStatBlock* result = parseBlock();
 if (lexer.current().type != Lexeme::Eof)
 expectAndConsumeFail(Lexeme::Eof, nullptr);
 return result;
}
AstStatBlock* Parser::parseBlock()
{
 unsigned int localsBegin = saveLocals();
 AstStatBlock* result = parseBlockNoScope();
 restoreLocals(localsBegin);
 return result;
}
static bool isStatLast(AstStat* stat)
{
 return stat->is<AstStatBreak>() || stat->is<AstStatContinue>() || stat->is<AstStatReturn>();
}
AstStatBlock* Parser::parseBlockNoScope()
{
 TempVector<AstStat*> body(scratchStat);
 const Position prevPosition = lexer.previousLocation().end;
 while (!blockFollow(lexer.current()))
 {
 unsigned int recursionCounterOld = recursionCounter;
 incrementRecursionCounter("block");
 AstStat* stat = parseStat();
 recursionCounter = recursionCounterOld;
 if (lexer.current().type == ';')
 {
 nextLexeme();
 stat->hasSemicolon = true;
 }
 body.push_back(stat);
 if (isStatLast(stat))
 break;
 }
 const Location location = Location(prevPosition, lexer.current().location.begin);
 return allocator.alloc<AstStatBlock>(location, copy(body));
}
AstStat* Parser::parseStat()
{
 switch (lexer.current().type)
 {
 case Lexeme::ReservedIf:
 return parseIf();
 case Lexeme::ReservedWhile:
 return parseWhile();
 case Lexeme::ReservedDo:
 return parseDo();
 case Lexeme::ReservedFor:
 return parseFor();
 case Lexeme::ReservedRepeat:
 return parseRepeat();
 case Lexeme::ReservedFunction:
 return parseFunctionStat();
 case Lexeme::ReservedLocal:
 return parseLocal();
 case Lexeme::ReservedReturn:
 return parseReturn();
 case Lexeme::ReservedBreak:
 return parseBreak();
 default:;
 }
 Location start = lexer.current().location;
 AstExpr* expr = parsePrimaryExpr( true);
 if (expr->is<AstExprCall>())
 return allocator.alloc<AstStatExpr>(expr->location, expr);
 if (lexer.current().type == ',' || lexer.current().type == '=')
 return parseAssignment(expr);
 if (std::optional<AstExprBinary::Op> op = parseCompoundOp(lexer.current()))
 return parseCompoundAssignment(expr, *op);
 AstName ident = getIdentifier(expr);
 if (ident == "type")
 return parseTypeAlias(expr->location, false);
 if (ident == "export" && lexer.current().type == Lexeme::Name && AstName(lexer.current().name) == "type")
 {
 nextLexeme();
 return parseTypeAlias(expr->location, true);
 }
 if (ident == "continue")
 return parseContinue(expr->location);
 if (options.allowDeclarationSyntax)
 {
 if (ident == "declare")
 return parseDeclaration(expr->location);
 }
 if (start == lexer.current().location)
 nextLexeme();
 return reportStatError(expr->location, copy({expr}), {}, "Incomplete statement: expected assignment or a function call");
}
AstStat* Parser::parseIf()
{
 Location start = lexer.current().location;
 nextLexeme();
 AstExpr* cond = parseExpr();
 Lexeme matchThen = lexer.current();
 std::optional<Location> thenLocation;
 if (expectAndConsume(Lexeme::ReservedThen, "if statement"))
 thenLocation = matchThen.location;
 AstStatBlock* thenbody = parseBlock();
 AstStat* elsebody = nullptr;
 Location end = start;
 std::optional<Location> elseLocation;
 bool hasEnd = false;
 if (lexer.current().type == Lexeme::ReservedElseif)
 {
 unsigned int recursionCounterOld = recursionCounter;
 incrementRecursionCounter("elseif");
 elseLocation = lexer.current().location;
 elsebody = parseIf();
 end = elsebody->location;
 hasEnd = elsebody->as<AstStatIf>()->hasEnd;
 recursionCounter = recursionCounterOld;
 }
 else
 {
 Lexeme matchThenElse = matchThen;
 if (lexer.current().type == Lexeme::ReservedElse)
 {
 elseLocation = lexer.current().location;
 matchThenElse = lexer.current();
 nextLexeme();
 elsebody = parseBlock();
 elsebody->location.begin = matchThenElse.location.end;
 }
 end = lexer.current().location;
 hasEnd = expectMatchEndAndConsume(Lexeme::ReservedEnd, matchThenElse);
 }
 return allocator.alloc<AstStatIf>(Location(start, end), cond, thenbody, elsebody, thenLocation, elseLocation, hasEnd);
}
AstStat* Parser::parseWhile()
{
 Location start = lexer.current().location;
 nextLexeme();
 AstExpr* cond = parseExpr();
 Lexeme matchDo = lexer.current();
 bool hasDo = expectAndConsume(Lexeme::ReservedDo, "while loop");
 functionStack.back().loopDepth++;
 AstStatBlock* body = parseBlock();
 functionStack.back().loopDepth--;
 Location end = lexer.current().location;
 bool hasEnd = expectMatchEndAndConsume(Lexeme::ReservedEnd, matchDo);
 return allocator.alloc<AstStatWhile>(Location(start, end), cond, body, hasDo, matchDo.location, hasEnd);
}
AstStat* Parser::parseRepeat()
{
 Location start = lexer.current().location;
 Lexeme matchRepeat = lexer.current();
 nextLexeme();
 unsigned int localsBegin = saveLocals();
 functionStack.back().loopDepth++;
 AstStatBlock* body = parseBlockNoScope();
 functionStack.back().loopDepth--;
 bool hasUntil = expectMatchEndAndConsume(Lexeme::ReservedUntil, matchRepeat);
 AstExpr* cond = parseExpr();
 restoreLocals(localsBegin);
 return allocator.alloc<AstStatRepeat>(Location(start, cond->location), cond, body, hasUntil);
}
AstStat* Parser::parseDo()
{
 Location start = lexer.current().location;
 Lexeme matchDo = lexer.current();
 nextLexeme();
 AstStat* body = parseBlock();
 body->location.begin = start.begin;
 expectMatchEndAndConsume(Lexeme::ReservedEnd, matchDo);
 return body;
}
AstStat* Parser::parseBreak()
{
 Location start = lexer.current().location;
 nextLexeme();
 if (functionStack.back().loopDepth == 0)
 return reportStatError(start, {}, copy<AstStat*>({allocator.alloc<AstStatBreak>(start)}), "break statement must be inside a loop");
 return allocator.alloc<AstStatBreak>(start);
}
AstStat* Parser::parseContinue(const Location& start)
{
 if (functionStack.back().loopDepth == 0)
 return reportStatError(start, {}, copy<AstStat*>({allocator.alloc<AstStatContinue>(start)}), "continue statement must be inside a loop");
 return allocator.alloc<AstStatContinue>(start);
}
AstStat* Parser::parseFor()
{
 Location start = lexer.current().location;
 nextLexeme();
 Binding varname = parseBinding();
 if (lexer.current().type == '=')
 {
 nextLexeme();
 AstExpr* from = parseExpr();
 expectAndConsume(',', "index range");
 AstExpr* to = parseExpr();
 AstExpr* step = nullptr;
 if (lexer.current().type == ',')
 {
 nextLexeme();
 step = parseExpr();
 }
 Lexeme matchDo = lexer.current();
 bool hasDo = expectAndConsume(Lexeme::ReservedDo, "for loop");
 unsigned int localsBegin = saveLocals();
 functionStack.back().loopDepth++;
 AstLocal* var = pushLocal(varname);
 AstStatBlock* body = parseBlock();
 functionStack.back().loopDepth--;
 restoreLocals(localsBegin);
 Location end = lexer.current().location;
 bool hasEnd = expectMatchEndAndConsume(Lexeme::ReservedEnd, matchDo);
 return allocator.alloc<AstStatFor>(Location(start, end), var, from, to, step, body, hasDo, matchDo.location, hasEnd);
 }
 else
 {
 TempVector<Binding> names(scratchBinding);
 names.push_back(varname);
 if (lexer.current().type == ',')
 {
 nextLexeme();
 parseBindingList(names);
 }
 Location inLocation = lexer.current().location;
 bool hasIn = expectAndConsume(Lexeme::ReservedIn, "for loop");
 TempVector<AstExpr*> values(scratchExpr);
 parseExprList(values);
 Lexeme matchDo = lexer.current();
 bool hasDo = expectAndConsume(Lexeme::ReservedDo, "for loop");
 unsigned int localsBegin = saveLocals();
 functionStack.back().loopDepth++;
 TempVector<AstLocal*> vars(scratchLocal);
 for (size_t i = 0; i < names.size(); ++i)
 vars.push_back(pushLocal(names[i]));
 AstStatBlock* body = parseBlock();
 functionStack.back().loopDepth--;
 restoreLocals(localsBegin);
 Location end = lexer.current().location;
 bool hasEnd = expectMatchEndAndConsume(Lexeme::ReservedEnd, matchDo);
 return allocator.alloc<AstStatForIn>(
 Location(start, end), copy(vars), copy(values), body, hasIn, inLocation, hasDo, matchDo.location, hasEnd);
 }
}
AstExpr* Parser::parseFunctionName(Location start, bool& hasself, AstName& debugname)
{
 if (lexer.current().type == Lexeme::Name)
 debugname = AstName(lexer.current().name);
 AstExpr* expr = parseNameExpr("function name");
 unsigned int recursionCounterOld = recursionCounter;
 while (lexer.current().type == '.')
 {
 Position opPosition = lexer.current().location.begin;
 nextLexeme();
 Name name = parseName("field name");
 debugname = name.name;
 expr = allocator.alloc<AstExprIndexName>(Location(start, name.location), expr, name.name, name.location, opPosition, '.');
 incrementRecursionCounter("function name");
 }
 recursionCounter = recursionCounterOld;
 if (lexer.current().type == ':')
 {
 Position opPosition = lexer.current().location.begin;
 nextLexeme();
 Name name = parseName("method name");
 debugname = name.name;
 expr = allocator.alloc<AstExprIndexName>(Location(start, name.location), expr, name.name, name.location, opPosition, ':');
 hasself = true;
 }
 return expr;
}
AstStat* Parser::parseFunctionStat()
{
 Location start = lexer.current().location;
 Lexeme matchFunction = lexer.current();
 nextLexeme();
 bool hasself = false;
 AstName debugname;
 AstExpr* expr = parseFunctionName(start, hasself, debugname);
 matchRecoveryStopOnToken[Lexeme::ReservedEnd]++;
 AstExprFunction* body = parseFunctionBody(hasself, matchFunction, debugname, nullptr).first;
 matchRecoveryStopOnToken[Lexeme::ReservedEnd]--;
 return allocator.alloc<AstStatFunction>(Location(start, body->location), expr, body);
}
AstStat* Parser::parseLocal()
{
 Location start = lexer.current().location;
 nextLexeme();
 if (lexer.current().type == Lexeme::ReservedFunction)
 {
 Lexeme matchFunction = lexer.current();
 nextLexeme();
 if (matchFunction.location.begin.line == start.begin.line)
 matchFunction.location.begin.column = start.begin.column;
 Name name = parseName("variable name");
 matchRecoveryStopOnToken[Lexeme::ReservedEnd]++;
 auto [body, var] = parseFunctionBody(false, matchFunction, name.name, &name);
 matchRecoveryStopOnToken[Lexeme::ReservedEnd]--;
 Location location{start.begin, body->location.end};
 return allocator.alloc<AstStatLocalFunction>(location, var, body);
 }
 else
 {
 matchRecoveryStopOnToken['=']++;
 TempVector<Binding> names(scratchBinding);
 parseBindingList(names);
 matchRecoveryStopOnToken['=']--;
 TempVector<AstLocal*> vars(scratchLocal);
 TempVector<AstExpr*> values(scratchExpr);
 std::optional<Location> equalsSignLocation;
 if (lexer.current().type == '=')
 {
 equalsSignLocation = lexer.current().location;
 nextLexeme();
 parseExprList(values);
 }
 for (size_t i = 0; i < names.size(); ++i)
 vars.push_back(pushLocal(names[i]));
 Location end = values.empty() ? lexer.previousLocation() : values.back()->location;
 return allocator.alloc<AstStatLocal>(Location(start, end), copy(vars), copy(values), equalsSignLocation);
 }
}
AstStat* Parser::parseReturn()
{
 Location start = lexer.current().location;
 nextLexeme();
 TempVector<AstExpr*> list(scratchExpr);
 if (!blockFollow(lexer.current()) && lexer.current().type != ';')
 parseExprList(list);
 Location end = list.empty() ? start : list.back()->location;
 return allocator.alloc<AstStatReturn>(Location(start, end), copy(list));
}
AstStat* Parser::parseTypeAlias(const Location& start, bool exported)
{
 std::optional<Name> name = parseNameOpt("type name");
 if (!name)
 name = Name(nameError, lexer.current().location);
 auto [generics, genericPacks] = parseGenericTypeList( true);
 expectAndConsume('=', "type alias");
 AstType* type = parseType();
 return allocator.alloc<AstStatTypeAlias>(Location(start, type->location), name->name, name->location, generics, genericPacks, type, exported);
}
AstDeclaredClassProp Parser::parseDeclaredClassMethod()
{
 nextLexeme();
 Location start = lexer.current().location;
 Name fnName = parseName("function name");
 AstArray<AstGenericType> generics;
 AstArray<AstGenericTypePack> genericPacks;
 generics.size = 0;
 generics.data = nullptr;
 genericPacks.size = 0;
 genericPacks.data = nullptr;
 MatchLexeme matchParen = lexer.current();
 expectAndConsume('(', "function parameter list start");
 TempVector<Binding> args(scratchBinding);
 bool vararg = false;
 Location varargLocation;
 AstTypePack* varargAnnotation = nullptr;
 if (lexer.current().type != ')')
 std::tie(vararg, varargLocation, varargAnnotation) = parseBindingList(args, true);
 expectMatchAndConsume(')', matchParen);
 AstTypeList retTypes = parseOptionalReturnType().value_or(AstTypeList{copy<AstType*>(nullptr, 0), nullptr});
 Location end = lexer.current().location;
 TempVector<AstType*> vars(scratchType);
 TempVector<std::optional<AstArgumentName>> varNames(scratchOptArgName);
 if (args.size() == 0 || args[0].name.name != "self" || args[0].annotation != nullptr)
 {
 return AstDeclaredClassProp{
 fnName.name, reportTypeError(Location(start, end), {}, "'self' must be present as the unannotated first parameter"), true};
 }
 for (size_t i = 1; i < args.size(); ++i)
 {
 varNames.push_back(AstArgumentName{args[i].name.name, args[i].name.location});
 if (args[i].annotation)
 vars.push_back(args[i].annotation);
 else
 vars.push_back(reportTypeError(Location(start, end), {}, "All declaration parameters aside from 'self' must be annotated"));
 }
 if (vararg && !varargAnnotation)
 report(start, "All declaration parameters aside from 'self' must be annotated");
 AstType* fnType = allocator.alloc<AstTypeFunction>(
 Location(start, end), generics, genericPacks, AstTypeList{copy(vars), varargAnnotation}, copy(varNames), retTypes);
 return AstDeclaredClassProp{fnName.name, fnType, true};
}
AstStat* Parser::parseDeclaration(const Location& start)
{
 if (lexer.current().type == Lexeme::ReservedFunction)
 {
 nextLexeme();
 Name globalName = parseName("global function name");
 auto [generics, genericPacks] = parseGenericTypeList( false);
 MatchLexeme matchParen = lexer.current();
 expectAndConsume('(', "global function declaration");
 TempVector<Binding> args(scratchBinding);
 bool vararg = false;
 Location varargLocation;
 AstTypePack* varargAnnotation = nullptr;
 if (lexer.current().type != ')')
 std::tie(vararg, varargLocation, varargAnnotation) = parseBindingList(args, true);
 expectMatchAndConsume(')', matchParen);
 AstTypeList retTypes = parseOptionalReturnType().value_or(AstTypeList{copy<AstType*>(nullptr, 0)});
 Location end = lexer.current().location;
 TempVector<AstType*> vars(scratchType);
 TempVector<AstArgumentName> varNames(scratchArgName);
 for (size_t i = 0; i < args.size(); ++i)
 {
 if (!args[i].annotation)
 return reportStatError(Location(start, end), {}, {}, "All declaration parameters must be annotated");
 vars.push_back(args[i].annotation);
 varNames.push_back({args[i].name.name, args[i].name.location});
 }
 if (vararg && !varargAnnotation)
 return reportStatError(Location(start, end), {}, {}, "All declaration parameters must be annotated");
 return allocator.alloc<AstStatDeclareFunction>(
 Location(start, end), globalName.name, generics, genericPacks, AstTypeList{copy(vars), varargAnnotation}, copy(varNames), retTypes);
 }
 else if (AstName(lexer.current().name) == "class")
 {
 nextLexeme();
 Location classStart = lexer.current().location;
 Name className = parseName("class name");
 std::optional<AstName> superName = std::nullopt;
 if (AstName(lexer.current().name) == "extends")
 {
 nextLexeme();
 superName = parseName("superclass name").name;
 }
 TempVector<AstDeclaredClassProp> props(scratchDeclaredClassProps);
 while (lexer.current().type != Lexeme::ReservedEnd)
 {
 if (lexer.current().type == Lexeme::ReservedFunction)
 {
 props.push_back(parseDeclaredClassMethod());
 }
 else if (lexer.current().type == '[')
 {
 const Lexeme begin = lexer.current();
 nextLexeme();
 std::optional<AstArray<char>> chars = parseCharArray();
 expectMatchAndConsume(']', begin);
 expectAndConsume(':', "property type annotation");
 AstType* type = parseType();
 bool containsNull = chars && (strnlen(chars->data, chars->size) < chars->size);
 if (chars && !containsNull)
 props.push_back(AstDeclaredClassProp{AstName(chars->data), type, false});
 else
 report(begin.location, "String literal contains malformed escape sequence");
 }
 else
 {
 Name propName = parseName("property name");
 expectAndConsume(':', "property type annotation");
 AstType* propType = parseType();
 props.push_back(AstDeclaredClassProp{propName.name, propType, false});
 }
 }
 Location classEnd = lexer.current().location;
 nextLexeme();
 return allocator.alloc<AstStatDeclareClass>(Location(classStart, classEnd), className.name, superName, copy(props));
 }
 else if (std::optional<Name> globalName = parseNameOpt("global variable name"))
 {
 expectAndConsume(':', "global variable declaration");
 AstType* type = parseType();
 return allocator.alloc<AstStatDeclareGlobal>(Location(start, type->location), globalName->name, type);
 }
 else
 {
 return reportStatError(start, {}, {}, "declare must be followed by an identifier, 'function', or 'class'");
 }
}
static bool isExprLValue(AstExpr* expr)
{
 return expr->is<AstExprLocal>() || expr->is<AstExprGlobal>() || expr->is<AstExprIndexExpr>() || expr->is<AstExprIndexName>();
}
AstStat* Parser::parseAssignment(AstExpr* initial)
{
 if (!isExprLValue(initial))
 initial = reportExprError(initial->location, copy({initial}), "Assigned expression must be a variable or a field");
 TempVector<AstExpr*> vars(scratchExpr);
 vars.push_back(initial);
 while (lexer.current().type == ',')
 {
 nextLexeme();
 AstExpr* expr = parsePrimaryExpr( true);
 if (!isExprLValue(expr))
 expr = reportExprError(expr->location, copy({expr}), "Assigned expression must be a variable or a field");
 vars.push_back(expr);
 }
 expectAndConsume('=', "assignment");
 TempVector<AstExpr*> values(scratchExprAux);
 parseExprList(values);
 return allocator.alloc<AstStatAssign>(Location(initial->location, values.back()->location), copy(vars), copy(values));
}
AstStat* Parser::parseCompoundAssignment(AstExpr* initial, AstExprBinary::Op op)
{
 if (!isExprLValue(initial))
 {
 initial = reportExprError(initial->location, copy({initial}), "Assigned expression must be a variable or a field");
 }
 nextLexeme();
 AstExpr* value = parseExpr();
 return allocator.alloc<AstStatCompoundAssign>(Location(initial->location, value->location), op, initial, value);
}
std::pair<AstLocal*, AstArray<AstLocal*>> Parser::prepareFunctionArguments(const Location& start, bool hasself, const TempVector<Binding>& args)
{
 AstLocal* self = nullptr;
 if (hasself)
 self = pushLocal(Binding(Name(nameSelf, start), nullptr));
 TempVector<AstLocal*> vars(scratchLocal);
 for (size_t i = 0; i < args.size(); ++i)
 vars.push_back(pushLocal(args[i]));
 return {self, copy(vars)};
}
std::pair<AstExprFunction*, AstLocal*> Parser::parseFunctionBody(
 bool hasself, const Lexeme& matchFunction, const AstName& debugname, const Name* localName)
{
 Location start = matchFunction.location;
 auto [generics, genericPacks] = parseGenericTypeList( false);
 MatchLexeme matchParen = lexer.current();
 expectAndConsume('(', "function");
 TempVector<Binding> args(scratchBinding);
 bool vararg = false;
 Location varargLocation;
 AstTypePack* varargAnnotation = nullptr;
 if (lexer.current().type != ')')
 std::tie(vararg, varargLocation, varargAnnotation) = parseBindingList(args, true);
 std::optional<Location> argLocation;
 if (matchParen.type == Lexeme::Type('(') && lexer.current().type == Lexeme::Type(')'))
 argLocation = Location(matchParen.position, lexer.current().location.end);
 expectMatchAndConsume(')', matchParen, true);
 std::optional<AstTypeList> typelist = parseOptionalReturnType();
 AstLocal* funLocal = nullptr;
 if (localName)
 funLocal = pushLocal(Binding(*localName, nullptr));
 unsigned int localsBegin = saveLocals();
 Function fun;
 fun.vararg = vararg;
 functionStack.emplace_back(fun);
 auto [self, vars] = prepareFunctionArguments(start, hasself, args);
 AstStatBlock* body = parseBlock();
 functionStack.pop_back();
 restoreLocals(localsBegin);
 Location end = lexer.current().location;
 bool hasEnd = expectMatchEndAndConsume(Lexeme::ReservedEnd, matchFunction);
 return {allocator.alloc<AstExprFunction>(Location(start, end), generics, genericPacks, self, vars, vararg, varargLocation, body,
 functionStack.size(), debugname, typelist, varargAnnotation, hasEnd, argLocation),
 funLocal};
}
void Parser::parseExprList(TempVector<AstExpr*>& result)
{
 result.push_back(parseExpr());
 while (lexer.current().type == ',')
 {
 nextLexeme();
 if (lexer.current().type == ')')
 {
 report(lexer.current().location, "Expected expression after ',' but got ')' instead");
 break;
 }
 result.push_back(parseExpr());
 }
}
Parser::Binding Parser::parseBinding()
{
 std::optional<Name> name = parseNameOpt("variable name");
 if (!name)
 name = Name(nameError, lexer.current().location);
 AstType* annotation = parseOptionalType();
 return Binding(*name, annotation);
}
std::tuple<bool, Location, AstTypePack*> Parser::parseBindingList(TempVector<Binding>& result, bool allowDot3)
{
 while (true)
 {
 if (lexer.current().type == Lexeme::Dot3 && allowDot3)
 {
 Location varargLocation = lexer.current().location;
 nextLexeme();
 AstTypePack* tailAnnotation = nullptr;
 if (lexer.current().type == ':')
 {
 nextLexeme();
 tailAnnotation = parseVariadicArgumentTypePack();
 }
 return {true, varargLocation, tailAnnotation};
 }
 result.push_back(parseBinding());
 if (lexer.current().type != ',')
 break;
 nextLexeme();
 }
 return {false, Location(), nullptr};
}
AstType* Parser::parseOptionalType()
{
 if (lexer.current().type == ':')
 {
 nextLexeme();
 return parseType();
 }
 else
 return nullptr;
}
AstTypePack* Parser::parseTypeList(TempVector<AstType*>& result, TempVector<std::optional<AstArgumentName>>& resultNames)
{
 while (true)
 {
 if (shouldParseTypePack(lexer))
 return parseTypePack();
 if (lexer.current().type == Lexeme::Name && lexer.lookahead().type == ':')
 {
 while (resultNames.size() < result.size())
 resultNames.push_back({});
 resultNames.push_back(AstArgumentName{AstName(lexer.current().name), lexer.current().location});
 nextLexeme();
 expectAndConsume(':');
 }
 else if (!resultNames.empty())
 {
 resultNames.push_back({});
 }
 result.push_back(parseType());
 if (lexer.current().type != ',')
 break;
 nextLexeme();
 if (lexer.current().type == ')')
 {
 report(lexer.current().location, "Expected type after ',' but got ')' instead");
 break;
 }
 }
 return nullptr;
}
std::optional<AstTypeList> Parser::parseOptionalReturnType()
{
 if (lexer.current().type == ':' || lexer.current().type == Lexeme::SkinnyArrow)
 {
 if (lexer.current().type == Lexeme::SkinnyArrow)
 report(lexer.current().location, "Function return type annotations are written after ':' instead of '->'");
 nextLexeme();
 unsigned int oldRecursionCount = recursionCounter;
 auto [_location, result] = parseReturnType();
 if (lexer.current().type == ',')
 {
 report(lexer.current().location, "Expected a statement, got ','; did you forget to wrap the list of return types in parentheses?");
 nextLexeme();
 }
 recursionCounter = oldRecursionCount;
 return result;
 }
 return std::nullopt;
}
std::pair<Location, AstTypeList> Parser::parseReturnType()
{
 incrementRecursionCounter("type annotation");
 Lexeme begin = lexer.current();
 if (lexer.current().type != '(')
 {
 if (shouldParseTypePack(lexer))
 {
 AstTypePack* typePack = parseTypePack();
 return {typePack->location, AstTypeList{{}, typePack}};
 }
 else
 {
 AstType* type = parseType();
 return {type->location, AstTypeList{copy(&type, 1), nullptr}};
 }
 }
 nextLexeme();
 Location innerBegin = lexer.current().location;
 matchRecoveryStopOnToken[Lexeme::SkinnyArrow]++;
 TempVector<AstType*> result(scratchType);
 TempVector<std::optional<AstArgumentName>> resultNames(scratchOptArgName);
 AstTypePack* varargAnnotation = nullptr;
 if (lexer.current().type != ')')
 varargAnnotation = parseTypeList(result, resultNames);
 const Location location{begin.location, lexer.current().location};
 expectMatchAndConsume(')', begin, true);
 matchRecoveryStopOnToken[Lexeme::SkinnyArrow]--;
 if (lexer.current().type != Lexeme::SkinnyArrow && resultNames.empty())
 {
 if (result.size() == 1)
 {
 AstType* returnType = parseTypeSuffix(result[0], innerBegin);
 Position endPos = result.size() == 1 ? location.end : returnType->location.end;
 return {Location{location.begin, endPos}, AstTypeList{copy(&returnType, 1), varargAnnotation}};
 }
 return {location, AstTypeList{copy(result), varargAnnotation}};
 }
 AstType* tail = parseFunctionTypeTail(begin, {}, {}, copy(result), copy(resultNames), varargAnnotation);
 return {Location{location, tail->location}, AstTypeList{copy(&tail, 1), varargAnnotation}};
}
AstTableIndexer* Parser::parseTableIndexer()
{
 const Lexeme begin = lexer.current();
 nextLexeme();
 AstType* index = parseType();
 expectMatchAndConsume(']', begin);
 expectAndConsume(':', "table field");
 AstType* result = parseType();
 return allocator.alloc<AstTableIndexer>(AstTableIndexer{index, result, Location(begin.location, result->location)});
}
AstType* Parser::parseTableType()
{
 incrementRecursionCounter("type annotation");
 TempVector<AstTableProp> props(scratchTableTypeProps);
 AstTableIndexer* indexer = nullptr;
 Location start = lexer.current().location;
 MatchLexeme matchBrace = lexer.current();
 expectAndConsume('{', "table type");
 while (lexer.current().type != '}')
 {
 if (lexer.current().type == '[' && (lexer.lookahead().type == Lexeme::RawString || lexer.lookahead().type == Lexeme::QuotedString))
 {
 const Lexeme begin = lexer.current();
 nextLexeme();
 std::optional<AstArray<char>> chars = parseCharArray();
 expectMatchAndConsume(']', begin);
 expectAndConsume(':', "table field");
 AstType* type = parseType();
 bool containsNull = chars && (strnlen(chars->data, chars->size) < chars->size);
 if (chars && !containsNull)
 props.push_back({AstName(chars->data), begin.location, type});
 else
 report(begin.location, "String literal contains malformed escape sequence");
 }
 else if (lexer.current().type == '[')
 {
 if (indexer)
 {
 AstTableIndexer* badIndexer = parseTableIndexer();
 report(badIndexer->location, "Cannot have more than one table indexer");
 }
 else
 {
 indexer = parseTableIndexer();
 }
 }
 else if (props.empty() && !indexer && !(lexer.current().type == Lexeme::Name && lexer.lookahead().type == ':'))
 {
 AstType* type = parseType();
 AstType* index = allocator.alloc<AstTypeReference>(type->location, std::nullopt, nameNumber, std::nullopt, type->location);
 indexer = allocator.alloc<AstTableIndexer>(AstTableIndexer{index, type, type->location});
 break;
 }
 else
 {
 std::optional<Name> name = parseNameOpt("table field");
 if (!name)
 break;
 expectAndConsume(':', "table field");
 AstType* type = parseType();
 props.push_back({name->name, name->location, type});
 }
 if (lexer.current().type == ',' || lexer.current().type == ';')
 {
 nextLexeme();
 }
 else
 {
 if (lexer.current().type != '}')
 break;
 }
 }
 Location end = lexer.current().location;
 if (!expectMatchAndConsume('}', matchBrace))
 end = lexer.previousLocation();
 return allocator.alloc<AstTypeTable>(Location(start, end), copy(props), indexer);
}
AstTypeOrPack Parser::parseFunctionType(bool allowPack)
{
 incrementRecursionCounter("type annotation");
 bool forceFunctionType = lexer.current().type == '<';
 Lexeme begin = lexer.current();
 auto [generics, genericPacks] = parseGenericTypeList( false);
 Lexeme parameterStart = lexer.current();
 expectAndConsume('(', "function parameters");
 matchRecoveryStopOnToken[Lexeme::SkinnyArrow]++;
 TempVector<AstType*> params(scratchType);
 TempVector<std::optional<AstArgumentName>> names(scratchOptArgName);
 AstTypePack* varargAnnotation = nullptr;
 if (lexer.current().type != ')')
 varargAnnotation = parseTypeList(params, names);
 expectMatchAndConsume(')', parameterStart, true);
 matchRecoveryStopOnToken[Lexeme::SkinnyArrow]--;
 AstArray<AstType*> paramTypes = copy(params);
 if (!names.empty())
 forceFunctionType = true;
 bool returnTypeIntroducer = lexer.current().type == Lexeme::SkinnyArrow || lexer.current().type == ':';
 if (params.size() == 1 && !varargAnnotation && !forceFunctionType && !returnTypeIntroducer)
 {
 if (allowPack)
 return {{}, allocator.alloc<AstTypePackExplicit>(begin.location, AstTypeList{paramTypes, nullptr})};
 else
 return {params[0], {}};
 }
 if (!forceFunctionType && !returnTypeIntroducer && allowPack)
 return {{}, allocator.alloc<AstTypePackExplicit>(begin.location, AstTypeList{paramTypes, varargAnnotation})};
 AstArray<std::optional<AstArgumentName>> paramNames = copy(names);
 return {parseFunctionTypeTail(begin, generics, genericPacks, paramTypes, paramNames, varargAnnotation), {}};
}
AstType* Parser::parseFunctionTypeTail(const Lexeme& begin, AstArray<AstGenericType> generics, AstArray<AstGenericTypePack> genericPacks,
 AstArray<AstType*> params, AstArray<std::optional<AstArgumentName>> paramNames, AstTypePack* varargAnnotation)
{
 incrementRecursionCounter("type annotation");
 if (lexer.current().type == ':')
 {
 report(lexer.current().location, "Return types in function type annotations are written after '->' instead of ':'");
 lexer.next();
 }
 else if (lexer.current().type != Lexeme::SkinnyArrow && generics.size == 0 && genericPacks.size == 0 && params.size == 0)
 {
 report(Location(begin.location, lexer.previousLocation()), "Expected '->' after '()' when parsing function type; did you mean 'nil'?");
 return allocator.alloc<AstTypeReference>(begin.location, std::nullopt, nameNil, std::nullopt, begin.location);
 }
 else
 {
 expectAndConsume(Lexeme::SkinnyArrow, "function type");
 }
 auto [endLocation, returnTypeList] = parseReturnType();
 AstTypeList paramTypes = AstTypeList{params, varargAnnotation};
 return allocator.alloc<AstTypeFunction>(Location(begin.location, endLocation), generics, genericPacks, paramTypes, paramNames, returnTypeList);
}
AstType* Parser::parseTypeSuffix(AstType* type, const Location& begin)
{
 TempVector<AstType*> parts(scratchType);
 parts.push_back(type);
 incrementRecursionCounter("type annotation");
 bool isUnion = false;
 bool isIntersection = false;
 Location location = begin;
 while (true)
 {
 Lexeme::Type c = lexer.current().type;
 if (c == '|')
 {
 nextLexeme();
 parts.push_back(parseSimpleType( false).type);
 isUnion = true;
 }
 else if (c == '?')
 {
 Location loc = lexer.current().location;
 nextLexeme();
 parts.push_back(allocator.alloc<AstTypeReference>(loc, std::nullopt, nameNil, std::nullopt, loc));
 isUnion = true;
 }
 else if (c == '&')
 {
 nextLexeme();
 parts.push_back(parseSimpleType( false).type);
 isIntersection = true;
 }
 else if (c == Lexeme::Dot3)
 {
 report(lexer.current().location, "Unexpected '...' after type annotation");
 nextLexeme();
 }
 else
 break;
 }
 if (parts.size() == 1)
 return type;
 if (isUnion && isIntersection)
 {
 return reportTypeError(Location(begin, parts.back()->location), copy(parts),
 "Mixing union and intersection types is not allowed; consider wrapping in parentheses.");
 }
 location.end = parts.back()->location.end;
 if (isUnion)
 return allocator.alloc<AstTypeUnion>(location, copy(parts));
 if (isIntersection)
 return allocator.alloc<AstTypeIntersection>(location, copy(parts));
 LUAU_ASSERT(false);
 ParseError::raise(begin, "Composite type was not an intersection or union.");
}
AstTypeOrPack Parser::parseTypeOrPack()
{
 unsigned int oldRecursionCount = recursionCounter;
 incrementRecursionCounter("type annotation");
 Location begin = lexer.current().location;
 auto [type, typePack] = parseSimpleType( true);
 if (typePack)
 {
 LUAU_ASSERT(!type);
 return {{}, typePack};
 }
 recursionCounter = oldRecursionCount;
 return {parseTypeSuffix(type, begin), {}};
}
AstType* Parser::parseType()
{
 unsigned int oldRecursionCount = recursionCounter;
 incrementRecursionCounter("type annotation");
 Location begin = lexer.current().location;
 AstType* type = parseSimpleType( false).type;
 recursionCounter = oldRecursionCount;
 return parseTypeSuffix(type, begin);
}
AstTypeOrPack Parser::parseSimpleType(bool allowPack)
{
 incrementRecursionCounter("type annotation");
 Location start = lexer.current().location;
 if (lexer.current().type == Lexeme::ReservedNil)
 {
 nextLexeme();
 return {allocator.alloc<AstTypeReference>(start, std::nullopt, nameNil, std::nullopt, start), {}};
 }
 else if (lexer.current().type == Lexeme::ReservedTrue)
 {
 nextLexeme();
 return {allocator.alloc<AstTypeSingletonBool>(start, true)};
 }
 else if (lexer.current().type == Lexeme::ReservedFalse)
 {
 nextLexeme();
 return {allocator.alloc<AstTypeSingletonBool>(start, false)};
 }
 else if (lexer.current().type == Lexeme::RawString || lexer.current().type == Lexeme::QuotedString)
 {
 if (std::optional<AstArray<char>> value = parseCharArray())
 {
 AstArray<char> svalue = *value;
 return {allocator.alloc<AstTypeSingletonString>(start, svalue)};
 }
 else
 return {reportTypeError(start, {}, "String literal contains malformed escape sequence")};
 }
 else if (lexer.current().type == Lexeme::InterpStringBegin || lexer.current().type == Lexeme::InterpStringSimple)
 {
 parseInterpString();
 return {reportTypeError(start, {}, "Interpolated string literals cannot be used as types")};
 }
 else if (lexer.current().type == Lexeme::BrokenString)
 {
 nextLexeme();
 return {reportTypeError(start, {}, "Malformed string")};
 }
 else if (lexer.current().type == Lexeme::Name)
 {
 std::optional<AstName> prefix;
 std::optional<Location> prefixLocation;
 Name name = parseName("type name");
 if (lexer.current().type == '.')
 {
 Position pointPosition = lexer.current().location.begin;
 nextLexeme();
 prefix = name.name;
 prefixLocation = name.location;
 name = parseIndexName("field name", pointPosition);
 }
 else if (lexer.current().type == Lexeme::Dot3)
 {
 report(lexer.current().location, "Unexpected '...' after type name; type pack is not allowed in this context");
 nextLexeme();
 }
 else if (name.name == "typeof")
 {
 Lexeme typeofBegin = lexer.current();
 expectAndConsume('(', "typeof type");
 AstExpr* expr = parseExpr();
 Location end = lexer.current().location;
 expectMatchAndConsume(')', typeofBegin);
 return {allocator.alloc<AstTypeTypeof>(Location(start, end), expr), {}};
 }
 bool hasParameters = false;
 AstArray<AstTypeOrPack> parameters{};
 if (lexer.current().type == '<')
 {
 hasParameters = true;
 parameters = parseTypeParams();
 }
 Location end = lexer.previousLocation();
 return {
 allocator.alloc<AstTypeReference>(Location(start, end), prefix, name.name, prefixLocation, name.location, hasParameters, parameters), {}};
 }
 else if (lexer.current().type == '{')
 {
 return {parseTableType(), {}};
 }
 else if (lexer.current().type == '(' || lexer.current().type == '<')
 {
 return parseFunctionType(allowPack);
 }
 else if (lexer.current().type == Lexeme::ReservedFunction)
 {
 nextLexeme();
 return {reportTypeError(start, {},
 "Using 'function' as a type annotation is not supported, consider replacing with a function type annotation e.g. '(...any) -> "
 "...any'"),
 {}};
 }
 else
 {
 Location astErrorlocation(lexer.previousLocation().end, start.begin);
 Location parseErrorLocation(lexer.previousLocation().end, start.end);
 return {reportMissingTypeError(parseErrorLocation, astErrorlocation, "Expected type, got %s", lexer.current().toString().c_str()), {}};
 }
}
AstTypePack* Parser::parseVariadicArgumentTypePack()
{
 if (lexer.current().type == Lexeme::Name && lexer.lookahead().type == Lexeme::Dot3)
 {
 Name name = parseName("generic name");
 Location end = lexer.current().location;
 expectAndConsume(Lexeme::Dot3, "generic type pack annotation");
 return allocator.alloc<AstTypePackGeneric>(Location(name.location, end), name.name);
 }
 else
 {
 AstType* variadicAnnotation = parseType();
 return allocator.alloc<AstTypePackVariadic>(variadicAnnotation->location, variadicAnnotation);
 }
}
AstTypePack* Parser::parseTypePack()
{
 if (lexer.current().type == Lexeme::Dot3)
 {
 Location start = lexer.current().location;
 nextLexeme();
 AstType* varargTy = parseType();
 return allocator.alloc<AstTypePackVariadic>(Location(start, varargTy->location), varargTy);
 }
 else if (lexer.current().type == Lexeme::Name && lexer.lookahead().type == Lexeme::Dot3)
 {
 Name name = parseName("generic name");
 Location end = lexer.current().location;
 expectAndConsume(Lexeme::Dot3, "generic type pack annotation");
 return allocator.alloc<AstTypePackGeneric>(Location(name.location, end), name.name);
 }
 return nullptr;
}
std::optional<AstExprUnary::Op> Parser::parseUnaryOp(const Lexeme& l)
{
 if (l.type == Lexeme::ReservedNot)
 return AstExprUnary::Not;
 else if (l.type == '-')
 return AstExprUnary::Minus;
 else if (l.type == '#')
 return AstExprUnary::Len;
 else
 return std::nullopt;
}
std::optional<AstExprBinary::Op> Parser::parseBinaryOp(const Lexeme& l)
{
 if (l.type == '+')
 return AstExprBinary::Add;
 else if (l.type == '-')
 return AstExprBinary::Sub;
 else if (l.type == '*')
 return AstExprBinary::Mul;
 else if (l.type == '/')
 return AstExprBinary::Div;
 else if (l.type == '%')
 return AstExprBinary::Mod;
 else if (l.type == '^')
 return AstExprBinary::Pow;
 else if (l.type == Lexeme::Dot2)
 return AstExprBinary::Concat;
 else if (l.type == Lexeme::NotEqual)
 return AstExprBinary::CompareNe;
 else if (l.type == Lexeme::Equal)
 return AstExprBinary::CompareEq;
 else if (l.type == '<')
 return AstExprBinary::CompareLt;
 else if (l.type == Lexeme::LessEqual)
 return AstExprBinary::CompareLe;
 else if (l.type == '>')
 return AstExprBinary::CompareGt;
 else if (l.type == Lexeme::GreaterEqual)
 return AstExprBinary::CompareGe;
 else if (l.type == Lexeme::ReservedAnd)
 return AstExprBinary::And;
 else if (l.type == Lexeme::ReservedOr)
 return AstExprBinary::Or;
 else
 return std::nullopt;
}
std::optional<AstExprBinary::Op> Parser::parseCompoundOp(const Lexeme& l)
{
 if (l.type == Lexeme::AddAssign)
 return AstExprBinary::Add;
 else if (l.type == Lexeme::SubAssign)
 return AstExprBinary::Sub;
 else if (l.type == Lexeme::MulAssign)
 return AstExprBinary::Mul;
 else if (l.type == Lexeme::DivAssign)
 return AstExprBinary::Div;
 else if (l.type == Lexeme::ModAssign)
 return AstExprBinary::Mod;
 else if (l.type == Lexeme::PowAssign)
 return AstExprBinary::Pow;
 else if (l.type == Lexeme::ConcatAssign)
 return AstExprBinary::Concat;
 else
 return std::nullopt;
}
std::optional<AstExprUnary::Op> Parser::checkUnaryConfusables()
{
 const Lexeme& curr = lexer.current();
 if (curr.type != '!')
 return {};
 Location start = curr.location;
 if (curr.type == '!')
 {
 report(start, "Unexpected '!', did you mean 'not'?");
 return AstExprUnary::Not;
 }
 return {};
}
std::optional<AstExprBinary::Op> Parser::checkBinaryConfusables(const BinaryOpPriority binaryPriority[], unsigned int limit)
{
 const Lexeme& curr = lexer.current();
 if (curr.type != '&' && curr.type != '|' && curr.type != '!')
 return {};
 Location start = curr.location;
 Lexeme next = lexer.lookahead();
 if (curr.type == '&' && next.type == '&' && curr.location.end == next.location.begin && binaryPriority[AstExprBinary::And].left > limit)
 {
 nextLexeme();
 report(Location(start, next.location), "Unexpected '&&', did you mean 'and'?");
 return AstExprBinary::And;
 }
 else if (curr.type == '|' && next.type == '|' && curr.location.end == next.location.begin && binaryPriority[AstExprBinary::Or].left > limit)
 {
 nextLexeme();
 report(Location(start, next.location), "Unexpected '||', did you mean 'or'?");
 return AstExprBinary::Or;
 }
 else if (curr.type == '!' && next.type == '=' && curr.location.end == next.location.begin &&
 binaryPriority[AstExprBinary::CompareNe].left > limit)
 {
 nextLexeme();
 report(Location(start, next.location), "Unexpected '!=', did you mean '~='?");
 return AstExprBinary::CompareNe;
 }
 return std::nullopt;
}
AstExpr* Parser::parseExpr(unsigned int limit)
{
 static const BinaryOpPriority binaryPriority[] = {
 {6, 6}, {6, 6}, {7, 7}, {7, 7}, {7, 7},
 {10, 9}, {5, 4}, // power and concat (right associative)
 {3, 3}, {3, 3},
 {3, 3}, {3, 3}, {3, 3}, {3, 3}, // order
 {2, 2}, {1, 1}
 };
 unsigned int recursionCounterOld = recursionCounter;
 incrementRecursionCounter("expression");
 const unsigned int unaryPriority = 8;
 Location start = lexer.current().location;
 AstExpr* expr;
 std::optional<AstExprUnary::Op> uop = parseUnaryOp(lexer.current());
 if (!uop)
 uop = checkUnaryConfusables();
 if (uop)
 {
 nextLexeme();
 AstExpr* subexpr = parseExpr(unaryPriority);
 expr = allocator.alloc<AstExprUnary>(Location(start, subexpr->location), *uop, subexpr);
 }
 else
 {
 expr = parseAssertionExpr();
 }
 std::optional<AstExprBinary::Op> op = parseBinaryOp(lexer.current());
 if (!op)
 op = checkBinaryConfusables(binaryPriority, limit);
 while (op && binaryPriority[*op].left > limit)
 {
 nextLexeme();
 AstExpr* next = parseExpr(binaryPriority[*op].right);
 expr = allocator.alloc<AstExprBinary>(Location(start, next->location), *op, expr, next);
 op = parseBinaryOp(lexer.current());
 if (!op)
 op = checkBinaryConfusables(binaryPriority, limit);
 incrementRecursionCounter("expression");
 }
 recursionCounter = recursionCounterOld;
 return expr;
}
AstExpr* Parser::parseNameExpr(const char* context)
{
 std::optional<Name> name = parseNameOpt(context);
 if (!name)
 return allocator.alloc<AstExprError>(lexer.current().location, copy<AstExpr*>({}), unsigned(parseErrors.size() - 1));
 AstLocal* const* value = localMap.find(name->name);
 if (value && *value)
 {
 AstLocal* local = *value;
 return allocator.alloc<AstExprLocal>(name->location, local, local->functionDepth != functionStack.size() - 1);
 }
 return allocator.alloc<AstExprGlobal>(name->location, name->name);
}
AstExpr* Parser::parsePrefixExpr()
{
 if (lexer.current().type == '(')
 {
 Position start = lexer.current().location.begin;
 MatchLexeme matchParen = lexer.current();
 nextLexeme();
 AstExpr* expr = parseExpr();
 Position end = lexer.current().location.end;
 if (lexer.current().type != ')')
 {
 const char* suggestion = (lexer.current().type == '=') ? "; did you mean to use '{' when defining a table?" : nullptr;
 expectMatchAndConsumeFail(static_cast<Lexeme::Type>(')'), matchParen, suggestion);
 end = lexer.previousLocation().end;
 }
 else
 {
 nextLexeme();
 }
 return allocator.alloc<AstExprGroup>(Location(start, end), expr);
 }
 else
 {
 return parseNameExpr("expression");
 }
}
AstExpr* Parser::parsePrimaryExpr(bool asStatement)
{
 Position start = lexer.current().location.begin;
 AstExpr* expr = parsePrefixExpr();
 unsigned int recursionCounterOld = recursionCounter;
 while (true)
 {
 if (lexer.current().type == '.')
 {
 Position opPosition = lexer.current().location.begin;
 nextLexeme();
 Name index = parseIndexName(nullptr, opPosition);
 expr = allocator.alloc<AstExprIndexName>(Location(start, index.location.end), expr, index.name, index.location, opPosition, '.');
 }
 else if (lexer.current().type == '[')
 {
 MatchLexeme matchBracket = lexer.current();
 nextLexeme();
 AstExpr* index = parseExpr();
 Position end = lexer.current().location.end;
 expectMatchAndConsume(']', matchBracket);
 expr = allocator.alloc<AstExprIndexExpr>(Location(start, end), expr, index);
 }
 else if (lexer.current().type == ':')
 {
 Position opPosition = lexer.current().location.begin;
 nextLexeme();
 Name index = parseIndexName("method name", opPosition);
 AstExpr* func = allocator.alloc<AstExprIndexName>(Location(start, index.location.end), expr, index.name, index.location, opPosition, ':');
 expr = parseFunctionArgs(func, true);
 }
 else if (lexer.current().type == '(')
 {
 if (!asStatement && expr->location.end.line != lexer.current().location.begin.line)
 {
 reportAmbiguousCallError();
 break;
 }
 expr = parseFunctionArgs(expr, false);
 }
 else if (lexer.current().type == '{' || lexer.current().type == Lexeme::RawString || lexer.current().type == Lexeme::QuotedString)
 {
 expr = parseFunctionArgs(expr, false);
 }
 else
 {
 break;
 }
 incrementRecursionCounter("expression");
 }
 recursionCounter = recursionCounterOld;
 return expr;
}
AstExpr* Parser::parseAssertionExpr()
{
 Location start = lexer.current().location;
 AstExpr* expr = parseSimpleExpr();
 if (lexer.current().type == Lexeme::DoubleColon)
 {
 nextLexeme();
 AstType* annotation = parseType();
 return allocator.alloc<AstExprTypeAssertion>(Location(start, annotation->location), expr, annotation);
 }
 else
 return expr;
}
static ConstantNumberParseResult parseInteger(double& result, const char* data, int base)
{
 LUAU_ASSERT(base == 2 || base == 16);
 char* end = nullptr;
 unsigned long long value = strtoull(data, &end, base);
 if (*end != 0)
 return ConstantNumberParseResult::Malformed;
 result = double(value);
 if (value == ULLONG_MAX && errno == ERANGE)
 {
 errno = 0;
 value = strtoull(data, &end, base);
 if (errno == ERANGE)
 return base == 2 ? ConstantNumberParseResult::BinOverflow : ConstantNumberParseResult::HexOverflow;
 }
 return ConstantNumberParseResult::Ok;
}
static ConstantNumberParseResult parseDouble(double& result, const char* data)
{
 if (data[0] == '0' && (data[1] == 'b' || data[1] == 'B') && data[2])
 return parseInteger(result, data + 2, 2);
 if (data[0] == '0' && (data[1] == 'x' || data[1] == 'X') && data[2])
 return parseInteger(result, data, 16);
 char* end = nullptr;
 double value = strtod(data, &end);
 result = value;
 return *end == 0 ? ConstantNumberParseResult::Ok : ConstantNumberParseResult::Malformed;
}
AstExpr* Parser::parseSimpleExpr()
{
 Location start = lexer.current().location;
 if (lexer.current().type == Lexeme::ReservedNil)
 {
 nextLexeme();
 return allocator.alloc<AstExprConstantNil>(start);
 }
 else if (lexer.current().type == Lexeme::ReservedTrue)
 {
 nextLexeme();
 return allocator.alloc<AstExprConstantBool>(start, true);
 }
 else if (lexer.current().type == Lexeme::ReservedFalse)
 {
 nextLexeme();
 return allocator.alloc<AstExprConstantBool>(start, false);
 }
 else if (lexer.current().type == Lexeme::ReservedFunction)
 {
 Lexeme matchFunction = lexer.current();
 nextLexeme();
 return parseFunctionBody(false, matchFunction, AstName(), nullptr).first;
 }
 else if (lexer.current().type == Lexeme::Number)
 {
 return parseNumber();
 }
 else if (lexer.current().type == Lexeme::RawString || lexer.current().type == Lexeme::QuotedString ||
 lexer.current().type == Lexeme::InterpStringSimple)
 {
 return parseString();
 }
 else if (lexer.current().type == Lexeme::InterpStringBegin)
 {
 return parseInterpString();
 }
 else if (lexer.current().type == Lexeme::BrokenString)
 {
 nextLexeme();
 return reportExprError(start, {}, "Malformed string");
 }
 else if (lexer.current().type == Lexeme::BrokenInterpDoubleBrace)
 {
 nextLexeme();
 return reportExprError(start, {}, ERROR_INVALID_INTERP_DOUBLE_BRACE);
 }
 else if (lexer.current().type == Lexeme::Dot3)
 {
 if (functionStack.back().vararg)
 {
 nextLexeme();
 return allocator.alloc<AstExprVarargs>(start);
 }
 else
 {
 nextLexeme();
 return reportExprError(start, {}, "Cannot use '...' outside of a vararg function");
 }
 }
 else if (lexer.current().type == '{')
 {
 return parseTableConstructor();
 }
 else if (lexer.current().type == Lexeme::ReservedIf)
 {
 return parseIfElseExpr();
 }
 else
 {
 return parsePrimaryExpr( false);
 }
}
AstExpr* Parser::parseFunctionArgs(AstExpr* func, bool self)
{
 if (lexer.current().type == '(')
 {
 Position argStart = lexer.current().location.end;
 if (func->location.end.line != lexer.current().location.begin.line)
 reportAmbiguousCallError();
 MatchLexeme matchParen = lexer.current();
 nextLexeme();
 TempVector<AstExpr*> args(scratchExpr);
 if (lexer.current().type != ')')
 parseExprList(args);
 Location end = lexer.current().location;
 Position argEnd = end.end;
 expectMatchAndConsume(')', matchParen);
 return allocator.alloc<AstExprCall>(Location(func->location, end), func, copy(args), self, Location(argStart, argEnd));
 }
 else if (lexer.current().type == '{')
 {
 Position argStart = lexer.current().location.end;
 AstExpr* expr = parseTableConstructor();
 Position argEnd = lexer.previousLocation().end;
 return allocator.alloc<AstExprCall>(Location(func->location, expr->location), func, copy(&expr, 1), self, Location(argStart, argEnd));
 }
 else if (lexer.current().type == Lexeme::RawString || lexer.current().type == Lexeme::QuotedString)
 {
 Location argLocation = lexer.current().location;
 AstExpr* expr = parseString();
 return allocator.alloc<AstExprCall>(Location(func->location, expr->location), func, copy(&expr, 1), self, argLocation);
 }
 else
 {
 return reportFunctionArgsError(func, self);
 }
}
LUAU_NOINLINE AstExpr* Parser::reportFunctionArgsError(AstExpr* func, bool self)
{
 if (self && lexer.current().location.begin.line != func->location.end.line)
 {
 return reportExprError(func->location, copy({func}), "Expected function call arguments after '('");
 }
 else
 {
 return reportExprError(Location(func->location.begin, lexer.current().location.begin), copy({func}),
 "Expected '(', '{' or <string> when parsing function call, got %s", lexer.current().toString().c_str());
 }
}
LUAU_NOINLINE void Parser::reportAmbiguousCallError()
{
 report(lexer.current().location, "Ambiguous syntax: this looks like an argument list for a function call, but could also be a start of "
 "new statement; use ';' to separate statements");
}
AstExpr* Parser::parseTableConstructor()
{
 TempVector<AstExprTable::Item> items(scratchItem);
 Location start = lexer.current().location;
 MatchLexeme matchBrace = lexer.current();
 expectAndConsume('{', "table literal");
 unsigned lastElementIndent = 0;
 while (lexer.current().type != '}')
 {
 lastElementIndent = lexer.current().location.begin.column;
 if (lexer.current().type == '[')
 {
 MatchLexeme matchLocationBracket = lexer.current();
 nextLexeme();
 AstExpr* key = parseExpr();
 expectMatchAndConsume(']', matchLocationBracket);
 expectAndConsume('=', "table field");
 AstExpr* value = parseExpr();
 items.push_back({AstExprTable::Item::General, key, value});
 }
 else if (lexer.current().type == Lexeme::Name && lexer.lookahead().type == '=')
 {
 Name name = parseName("table field");
 expectAndConsume('=', "table field");
 AstArray<char> nameString;
 nameString.data = const_cast<char*>(name.name.value);
 nameString.size = strlen(name.name.value);
 AstExpr* key = allocator.alloc<AstExprConstantString>(name.location, nameString);
 AstExpr* value = parseExpr();
 if (AstExprFunction* func = value->as<AstExprFunction>())
 func->debugname = name.name;
 items.push_back({AstExprTable::Item::Record, key, value});
 }
 else
 {
 AstExpr* expr = parseExpr();
 items.push_back({AstExprTable::Item::List, nullptr, expr});
 }
 if (lexer.current().type == ',' || lexer.current().type == ';')
 {
 nextLexeme();
 }
 else if ((lexer.current().type == '[' || lexer.current().type == Lexeme::Name) && lexer.current().location.begin.column == lastElementIndent)
 {
 report(lexer.current().location, "Expected ',' after table constructor element");
 }
 else if (lexer.current().type != '}')
 {
 break;
 }
 }
 Location end = lexer.current().location;
 if (!expectMatchAndConsume('}', matchBrace))
 end = lexer.previousLocation();
 return allocator.alloc<AstExprTable>(Location(start, end), copy(items));
}
AstExpr* Parser::parseIfElseExpr()
{
 bool hasElse = false;
 Location start = lexer.current().location;
 nextLexeme();
 AstExpr* condition = parseExpr();
 bool hasThen = expectAndConsume(Lexeme::ReservedThen, "if then else expression");
 AstExpr* trueExpr = parseExpr();
 AstExpr* falseExpr = nullptr;
 if (lexer.current().type == Lexeme::ReservedElseif)
 {
 unsigned int oldRecursionCount = recursionCounter;
 incrementRecursionCounter("expression");
 hasElse = true;
 falseExpr = parseIfElseExpr();
 recursionCounter = oldRecursionCount;
 }
 else
 {
 hasElse = expectAndConsume(Lexeme::ReservedElse, "if then else expression");
 falseExpr = parseExpr();
 }
 Location end = falseExpr->location;
 return allocator.alloc<AstExprIfElse>(Location(start, end), condition, hasThen, trueExpr, hasElse, falseExpr);
}
std::optional<Parser::Name> Parser::parseNameOpt(const char* context)
{
 if (lexer.current().type != Lexeme::Name)
 {
 reportNameError(context);
 return {};
 }
 Name result(AstName(lexer.current().name), lexer.current().location);
 nextLexeme();
 return result;
}
Parser::Name Parser::parseName(const char* context)
{
 if (std::optional<Name> name = parseNameOpt(context))
 return *name;
 Location location = lexer.current().location;
 location.end = location.begin;
 return Name(nameError, location);
}
Parser::Name Parser::parseIndexName(const char* context, const Position& previous)
{
 if (std::optional<Name> name = parseNameOpt(context))
 return *name;
 if (lexer.current().type >= Lexeme::Reserved_BEGIN && lexer.current().type < Lexeme::Reserved_END &&
 lexer.current().location.begin.line == previous.line)
 {
 Name result(AstName(lexer.current().name), lexer.current().location);
 nextLexeme();
 return result;
 }
 Location location = lexer.current().location;
 location.end = location.begin;
 return Name(nameError, location);
}
std::pair<AstArray<AstGenericType>, AstArray<AstGenericTypePack>> Parser::parseGenericTypeList(bool withDefaultValues)
{
 TempVector<AstGenericType> names{scratchGenericTypes};
 TempVector<AstGenericTypePack> namePacks{scratchGenericTypePacks};
 if (lexer.current().type == '<')
 {
 Lexeme begin = lexer.current();
 nextLexeme();
 bool seenPack = false;
 bool seenDefault = false;
 while (true)
 {
 Location nameLocation = lexer.current().location;
 AstName name = parseName().name;
 if (lexer.current().type == Lexeme::Dot3 || seenPack)
 {
 seenPack = true;
 if (lexer.current().type != Lexeme::Dot3)
 report(lexer.current().location, "Generic types come before generic type packs");
 else
 nextLexeme();
 if (withDefaultValues && lexer.current().type == '=')
 {
 seenDefault = true;
 nextLexeme();
 if (shouldParseTypePack(lexer))
 {
 AstTypePack* typePack = parseTypePack();
 namePacks.push_back({name, nameLocation, typePack});
 }
 else
 {
 auto [type, typePack] = parseTypeOrPack();
 if (type)
 report(type->location, "Expected type pack after '=', got type");
 namePacks.push_back({name, nameLocation, typePack});
 }
 }
 else
 {
 if (seenDefault)
 report(lexer.current().location, "Expected default type pack after type pack name");
 namePacks.push_back({name, nameLocation, nullptr});
 }
 }
 else
 {
 if (withDefaultValues && lexer.current().type == '=')
 {
 seenDefault = true;
 nextLexeme();
 AstType* defaultType = parseType();
 names.push_back({name, nameLocation, defaultType});
 }
 else
 {
 if (seenDefault)
 report(lexer.current().location, "Expected default type after type name");
 names.push_back({name, nameLocation, nullptr});
 }
 }
 if (lexer.current().type == ',')
 {
 nextLexeme();
 if (lexer.current().type == '>')
 {
 report(lexer.current().location, "Expected type after ',' but got '>' instead");
 break;
 }
 }
 else
 break;
 }
 expectMatchAndConsume('>', begin);
 }
 AstArray<AstGenericType> generics = copy(names);
 AstArray<AstGenericTypePack> genericPacks = copy(namePacks);
 return {generics, genericPacks};
}
AstArray<AstTypeOrPack> Parser::parseTypeParams()
{
 TempVector<AstTypeOrPack> parameters{scratchTypeOrPack};
 if (lexer.current().type == '<')
 {
 Lexeme begin = lexer.current();
 nextLexeme();
 while (true)
 {
 if (shouldParseTypePack(lexer))
 {
 AstTypePack* typePack = parseTypePack();
 parameters.push_back({{}, typePack});
 }
 else if (lexer.current().type == '(')
 {
 auto [type, typePack] = parseTypeOrPack();
 if (typePack)
 parameters.push_back({{}, typePack});
 else
 parameters.push_back({type, {}});
 }
 else if (lexer.current().type == '>' && parameters.empty())
 {
 break;
 }
 else
 {
 parameters.push_back({parseType(), {}});
 }
 if (lexer.current().type == ',')
 nextLexeme();
 else
 break;
 }
 expectMatchAndConsume('>', begin);
 }
 return copy(parameters);
}
std::optional<AstArray<char>> Parser::parseCharArray()
{
 LUAU_ASSERT(lexer.current().type == Lexeme::QuotedString || lexer.current().type == Lexeme::RawString ||
 lexer.current().type == Lexeme::InterpStringSimple);
 scratchData.assign(lexer.current().data, lexer.current().length);
 if (lexer.current().type == Lexeme::QuotedString || lexer.current().type == Lexeme::InterpStringSimple)
 {
 if (!Lexer::fixupQuotedString(scratchData))
 {
 nextLexeme();
 return std::nullopt;
 }
 }
 else
 {
 Lexer::fixupMultilineString(scratchData);
 }
 AstArray<char> value = copy(scratchData);
 nextLexeme();
 return value;
}
AstExpr* Parser::parseString()
{
 Location location = lexer.current().location;
 if (std::optional<AstArray<char>> value = parseCharArray())
 return allocator.alloc<AstExprConstantString>(location, *value);
 else
 return reportExprError(location, {}, "String literal contains malformed escape sequence");
}
AstExpr* Parser::parseInterpString()
{
 TempVector<AstArray<char>> strings(scratchString);
 TempVector<AstExpr*> expressions(scratchExpr);
 Location startLocation = lexer.current().location;
 Location endLocation;
 do
 {
 Lexeme currentLexeme = lexer.current();
 LUAU_ASSERT(currentLexeme.type == Lexeme::InterpStringBegin || currentLexeme.type == Lexeme::InterpStringMid ||
 currentLexeme.type == Lexeme::InterpStringEnd || currentLexeme.type == Lexeme::InterpStringSimple);
 endLocation = currentLexeme.location;
 scratchData.assign(currentLexeme.data, currentLexeme.length);
 if (!Lexer::fixupQuotedString(scratchData))
 {
 nextLexeme();
 return reportExprError(Location{startLocation, endLocation}, {}, "Interpolated string literal contains malformed escape sequence");
 }
 AstArray<char> chars = copy(scratchData);
 nextLexeme();
 strings.push_back(chars);
 if (currentLexeme.type == Lexeme::InterpStringEnd || currentLexeme.type == Lexeme::InterpStringSimple)
 {
 break;
 }
 bool errorWhileChecking = false;
 switch (lexer.current().type)
 {
 case Lexeme::InterpStringMid:
 case Lexeme::InterpStringEnd:
 {
 errorWhileChecking = true;
 nextLexeme();
 expressions.push_back(reportExprError(endLocation, {}, "Malformed interpolated string, expected expression inside '{}'"));
 break;
 }
 case Lexeme::BrokenString:
 {
 errorWhileChecking = true;
 nextLexeme();
 expressions.push_back(reportExprError(endLocation, {}, "Malformed interpolated string, did you forget to add a '`'?"));
 break;
 }
 default:
 expressions.push_back(parseExpr());
 }
 if (errorWhileChecking)
 {
 break;
 }
 switch (lexer.current().type)
 {
 case Lexeme::InterpStringBegin:
 case Lexeme::InterpStringMid:
 case Lexeme::InterpStringEnd:
 break;
 case Lexeme::BrokenInterpDoubleBrace:
 nextLexeme();
 return reportExprError(endLocation, {}, ERROR_INVALID_INTERP_DOUBLE_BRACE);
 case Lexeme::BrokenString:
 nextLexeme();
 return reportExprError(endLocation, {}, "Malformed interpolated string, did you forget to add a '}'?");
 default:
 return reportExprError(endLocation, {}, "Malformed interpolated string, got %s", lexer.current().toString().c_str());
 }
 } while (true);
 AstArray<AstArray<char>> stringsArray = copy(strings);
 AstArray<AstExpr*> expressionsArray = copy(expressions);
 return allocator.alloc<AstExprInterpString>(Location{startLocation, endLocation}, stringsArray, expressionsArray);
}
AstExpr* Parser::parseNumber()
{
 Location start = lexer.current().location;
 scratchData.assign(lexer.current().data, lexer.current().length);
 if (scratchData.find('_') != std::string::npos)
 {
 scratchData.erase(std::remove(scratchData.begin(), scratchData.end(), '_'), scratchData.end());
 }
 double value = 0;
 ConstantNumberParseResult result = parseDouble(value, scratchData.c_str());
 nextLexeme();
 if (result == ConstantNumberParseResult::Malformed)
 return reportExprError(start, {}, "Malformed number");
 return allocator.alloc<AstExprConstantNumber>(start, value, result);
}
AstLocal* Parser::pushLocal(const Binding& binding)
{
 const Name& name = binding.name;
 AstLocal*& local = localMap[name.name];
 local = allocator.alloc<AstLocal>(
 name.name, name.location, local, functionStack.size() - 1, functionStack.back().loopDepth, binding.annotation);
 localStack.push_back(local);
 return local;
}
unsigned int Parser::saveLocals()
{
 return unsigned(localStack.size());
}
void Parser::restoreLocals(unsigned int offset)
{
 for (size_t i = localStack.size(); i > offset; --i)
 {
 AstLocal* l = localStack[i - 1];
 localMap[l->name] = l->shadow;
 }
 localStack.resize(offset);
}
bool Parser::expectAndConsume(char value, const char* context)
{
 return expectAndConsume(static_cast<Lexeme::Type>(static_cast<unsigned char>(value)), context);
}
bool Parser::expectAndConsume(Lexeme::Type type, const char* context)
{
 if (lexer.current().type != type)
 {
 expectAndConsumeFail(type, context);
 if (lexer.lookahead().type == type)
 {
 nextLexeme();
 nextLexeme();
 }
 return false;
 }
 else
 {
 nextLexeme();
 return true;
 }
}
LUAU_NOINLINE void Parser::expectAndConsumeFail(Lexeme::Type type, const char* context)
{
 std::string typeString = Lexeme(Location(Position(0, 0), 0), type).toString();
 std::string currLexemeString = lexer.current().toString();
 if (context)
 report(lexer.current().location, "Expected %s when parsing %s, got %s", typeString.c_str(), context, currLexemeString.c_str());
 else
 report(lexer.current().location, "Expected %s, got %s", typeString.c_str(), currLexemeString.c_str());
}
bool Parser::expectMatchAndConsume(char value, const MatchLexeme& begin, bool searchForMissing)
{
 Lexeme::Type type = static_cast<Lexeme::Type>(static_cast<unsigned char>(value));
 if (lexer.current().type != type)
 {
 expectMatchAndConsumeFail(type, begin);
 return expectMatchAndConsumeRecover(value, begin, searchForMissing);
 }
 else
 {
 nextLexeme();
 return true;
 }
}
LUAU_NOINLINE bool Parser::expectMatchAndConsumeRecover(char value, const MatchLexeme& begin, bool searchForMissing)
{
 Lexeme::Type type = static_cast<Lexeme::Type>(static_cast<unsigned char>(value));
 if (searchForMissing)
 {
 unsigned currentLine = lexer.previousLocation().end.line;
 Lexeme::Type lexemeType = lexer.current().type;
 while (currentLine == lexer.current().location.begin.line && lexemeType != type && matchRecoveryStopOnToken[lexemeType] == 0)
 {
 nextLexeme();
 lexemeType = lexer.current().type;
 }
 if (lexemeType == type)
 {
 nextLexeme();
 return true;
 }
 }
 else
 {
 if (lexer.lookahead().type == type)
 {
 nextLexeme();
 nextLexeme();
 return true;
 }
 }
 return false;
}
LUAU_NOINLINE void Parser::expectMatchAndConsumeFail(Lexeme::Type type, const MatchLexeme& begin, const char* extra)
{
 std::string typeString = Lexeme(Location(Position(0, 0), 0), type).toString();
 std::string matchString = Lexeme(Location(Position(0, 0), 0), begin.type).toString();
 if (lexer.current().location.begin.line == begin.position.line)
 report(lexer.current().location, "Expected %s (to close %s at column %d), got %s%s", typeString.c_str(), matchString.c_str(),
 begin.position.column + 1, lexer.current().toString().c_str(), extra ? extra : "");
 else
 report(lexer.current().location, "Expected %s (to close %s at line %d), got %s%s", typeString.c_str(), matchString.c_str(),
 begin.position.line + 1, lexer.current().toString().c_str(), extra ? extra : "");
}
bool Parser::expectMatchEndAndConsume(Lexeme::Type type, const MatchLexeme& begin)
{
 if (lexer.current().type != type)
 {
 expectMatchEndAndConsumeFail(type, begin);
 if (lexer.lookahead().type == type)
 {
 nextLexeme();
 nextLexeme();
 return true;
 }
 return false;
 }
 else
 {
 if (lexer.current().location.begin.line != begin.position.line && lexer.current().location.begin.column != begin.position.column &&
 endMismatchSuspect.position.line < begin.position.line)
 {
 endMismatchSuspect = begin;
 }
 nextLexeme();
 return true;
 }
}
LUAU_NOINLINE void Parser::expectMatchEndAndConsumeFail(Lexeme::Type type, const MatchLexeme& begin)
{
 if (endMismatchSuspect.type != Lexeme::Eof && endMismatchSuspect.position.line > begin.position.line)
 {
 std::string matchString = Lexeme(Location(Position(0, 0), 0), endMismatchSuspect.type).toString();
 std::string suggestion = format("; did you forget to close %s at line %d?", matchString.c_str(), endMismatchSuspect.position.line + 1);
 expectMatchAndConsumeFail(type, begin, suggestion.c_str());
 }
 else
 {
 expectMatchAndConsumeFail(type, begin);
 }
}
template<typename T>
AstArray<T> Parser::copy(const T* data, size_t size)
{
 AstArray<T> result;
 result.data = size ? static_cast<T*>(allocator.allocate(sizeof(T) * size)) : nullptr;
 result.size = size;
 for (size_t i = 0; i < size; ++i)
 new (result.data + i) T(data[i]);
 return result;
}
template<typename T>
AstArray<T> Parser::copy(const TempVector<T>& data)
{
 return copy(data.empty() ? nullptr : &data[0], data.size());
}
template<typename T>
AstArray<T> Parser::copy(std::initializer_list<T> data)
{
 return copy(data.size() == 0 ? nullptr : data.begin(), data.size());
}
AstArray<char> Parser::copy(const std::string& data)
{
 AstArray<char> result = copy(data.c_str(), data.size() + 1);
 result.size = data.size();
 return result;
}
void Parser::incrementRecursionCounter(const char* context)
{
 recursionCounter++;
 if (recursionCounter > unsigned(FInt::LuauRecursionLimit))
 {
 ParseError::raise(lexer.current().location, "Exceeded allowed recursion depth; simplify your %s to make the code compile", context);
 }
}
void Parser::report(const Location& location, const char* format, va_list args)
{
 if (!parseErrors.empty() && location == parseErrors.back().getLocation())
 return;
 std::string message = vformat(format, args);
 if (FInt::LuauParseErrorLimit == 1)
 throw ParseError(location, message);
 parseErrors.emplace_back(location, message);
 if (parseErrors.size() >= unsigned(FInt::LuauParseErrorLimit))
 ParseError::raise(location, "Reached error limit (%d)", int(FInt::LuauParseErrorLimit));
}
void Parser::report(const Location& location, const char* format, ...)
{
 va_list args;
 va_start(args, format);
 report(location, format, args);
 va_end(args);
}
LUAU_NOINLINE void Parser::reportNameError(const char* context)
{
 if (context)
 report(lexer.current().location, "Expected identifier when parsing %s, got %s", context, lexer.current().toString().c_str());
 else
 report(lexer.current().location, "Expected identifier, got %s", lexer.current().toString().c_str());
}
AstStatError* Parser::reportStatError(
 const Location& location, const AstArray<AstExpr*>& expressions, const AstArray<AstStat*>& statements, const char* format, ...)
{
 va_list args;
 va_start(args, format);
 report(location, format, args);
 va_end(args);
 return allocator.alloc<AstStatError>(location, expressions, statements, unsigned(parseErrors.size() - 1));
}
AstExprError* Parser::reportExprError(const Location& location, const AstArray<AstExpr*>& expressions, const char* format, ...)
{
 va_list args;
 va_start(args, format);
 report(location, format, args);
 va_end(args);
 return allocator.alloc<AstExprError>(location, expressions, unsigned(parseErrors.size() - 1));
}
AstTypeError* Parser::reportTypeError(const Location& location, const AstArray<AstType*>& types, const char* format, ...)
{
 va_list args;
 va_start(args, format);
 report(location, format, args);
 va_end(args);
 return allocator.alloc<AstTypeError>(location, types, false, unsigned(parseErrors.size() - 1));
}
AstTypeError* Parser::reportMissingTypeError(const Location& parseErrorLocation, const Location& astErrorLocation, const char* format, ...)
{
 va_list args;
 va_start(args, format);
 report(parseErrorLocation, format, args);
 va_end(args);
 return allocator.alloc<AstTypeError>(astErrorLocation, AstArray<AstType*>{}, true, unsigned(parseErrors.size() - 1));
}
void Parser::nextLexeme()
{
 Lexeme::Type type = lexer.next( false, true).type;
 while (type == Lexeme::BrokenComment || type == Lexeme::Comment || type == Lexeme::BlockComment)
 {
 const Lexeme& lexeme = lexer.current();
 if (options.captureComments)
 commentLocations.push_back(Comment{lexeme.type, lexeme.location});
 if (lexeme.type == Lexeme::BrokenComment)
 return;
 if (lexeme.type == Lexeme::Comment && lexeme.length && lexeme.data[0] == '!')
 {
 const char* text = lexeme.data;
 unsigned int end = lexeme.length;
 while (end > 0 && isSpace(text[end - 1]))
 --end;
 hotcomments.push_back({hotcommentHeader, lexeme.location, std::string(text + 1, text + end)});
 }
 type = lexer.next( false, false).type;
 }
}
}
namespace Luau
{
void vformatAppend(std::string& ret, const char* fmt, va_list args)
{
 va_list argscopy;
 va_copy(argscopy, args);
#ifdef _MSC_VER
 int actualSize = _vscprintf(fmt, argscopy);
#else
 int actualSize = vsnprintf(NULL, 0, fmt, argscopy);
#endif
 va_end(argscopy);
 if (actualSize <= 0)
 return;
 size_t sz = ret.size();
 ret.resize(sz + actualSize);
 vsnprintf(&ret[0] + sz, actualSize + 1, fmt, args);
}
std::string format(const char* fmt, ...)
{
 std::string result;
 va_list args;
 va_start(args, fmt);
 vformatAppend(result, fmt, args);
 va_end(args);
 return result;
}
void formatAppend(std::string& str, const char* fmt, ...)
{
 va_list args;
 va_start(args, fmt);
 vformatAppend(str, fmt, args);
 va_end(args);
}
std::string vformat(const char* fmt, va_list args)
{
 std::string ret;
 vformatAppend(ret, fmt, args);
 return ret;
}
template<typename String>
static std::string joinImpl(const std::vector<String>& segments, std::string_view delimiter)
{
 if (segments.empty())
 return "";
 size_t len = (segments.size() - 1) * delimiter.size();
 for (const auto& sv : segments)
 len += sv.size();
 std::string result;
 result.resize(len);
 char* dest = const_cast<char*>(result.data());
 auto it = segments.begin();
 memcpy(dest, it->data(), it->size());
 dest += it->size();
 ++it;
 for (; it != segments.end(); ++it)
 {
 memcpy(dest, delimiter.data(), delimiter.size());
 dest += delimiter.size();
 memcpy(dest, it->data(), it->size());
 dest += it->size();
 }
 LUAU_ASSERT(dest == result.data() + len);
 return result;
}
std::string join(const std::vector<std::string_view>& segments, std::string_view delimiter)
{
 return joinImpl(segments, delimiter);
}
std::string join(const std::vector<std::string>& segments, std::string_view delimiter)
{
 return joinImpl(segments, delimiter);
}
std::vector<std::string_view> split(std::string_view s, char delimiter)
{
 std::vector<std::string_view> result;
 while (!s.empty())
 {
 auto index = s.find(delimiter);
 if (index == std::string::npos)
 {
 result.push_back(s);
 break;
 }
 result.push_back(s.substr(0, index));
 s.remove_prefix(index + 1);
 }
 return result;
}
size_t editDistance(std::string_view a, std::string_view b)
{
 while (!a.empty() && !b.empty() && a.front() == b.front())
 {
 a.remove_prefix(1);
 b.remove_prefix(1);
 }
 while (!a.empty() && !b.empty() && a.back() == b.back())
 {
 a.remove_suffix(1);
 b.remove_suffix(1);
 }
 if (a.empty())
 return b.size();
 if (b.empty())
 return a.size();
 size_t maxDistance = a.size() + b.size();
 std::vector<size_t> distances((a.size() + 2) * (b.size() + 2), 0);
 auto getPos = [b](size_t x, size_t y) -> size_t {
 return (x * (b.size() + 2)) + y;
 };
 distances[0] = maxDistance;
 for (size_t x = 0; x <= a.size(); ++x)
 {
 distances[getPos(x + 1, 0)] = maxDistance;
 distances[getPos(x + 1, 1)] = x;
 }
 for (size_t y = 0; y <= b.size(); ++y)
 {
 distances[getPos(0, y + 1)] = maxDistance;
 distances[getPos(1, y + 1)] = y;
 }
 std::array<size_t, 256> seenCharToRow;
 seenCharToRow.fill(0);
 for (size_t x = 1; x <= a.size(); ++x)
 {
 size_t lastMatchedY = 0;
 for (size_t y = 1; y <= b.size(); ++y)
 {
 unsigned char bSeenCharIndex = static_cast<unsigned char>(b[y - 1]);
 size_t x1 = seenCharToRow[bSeenCharIndex];
 size_t y1 = lastMatchedY;
 size_t cost = 1;
 if (a[x - 1] == b[y - 1])
 {
 cost = 0;
 lastMatchedY = y;
 }
 size_t transposition = distances[getPos(x1, y1)] + (x - x1 - 1) + 1 + (y - y1 - 1);
 size_t substitution = distances[getPos(x, y)] + cost;
 size_t insertion = distances[getPos(x, y + 1)] + 1;
 size_t deletion = distances[getPos(x + 1, y)] + 1;
 distances[getPos(x + 1, y + 1)] = std::min(std::min(insertion, deletion), std::min(substitution, transposition));
 }
 unsigned char aSeenCharIndex = static_cast<unsigned char>(a[x - 1]);
 seenCharToRow[aSeenCharIndex] = x;
 }
 return distances[getPos(a.size() + 1, b.size() + 1)];
}
bool startsWith(std::string_view haystack, std::string_view needle)
{
 return haystack.size() >= needle.size() && haystack.substr(0, needle.size()) == needle;
}
bool equalsLower(std::string_view lhs, std::string_view rhs)
{
 if (lhs.size() != rhs.size())
 return false;
 for (size_t i = 0; i < lhs.size(); ++i)
 if (tolower(uint8_t(lhs[i])) != tolower(uint8_t(rhs[i])))
 return false;
 return true;
}
size_t hashRange(const char* data, size_t size)
{
 uint32_t hash = 2166136261;
 for (size_t i = 0; i < size; ++i)
 {
 hash ^= uint8_t(data[i]);
 hash *= 16777619;
 }
 return hash;
}
bool isIdentifier(std::string_view s)
{
 return (s.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_") == std::string::npos);
}
std::string escape(std::string_view s, bool escapeForInterpString)
{
 std::string r;
 r.reserve(s.size() + 50);
 for (uint8_t c : s)
 {
 if (c >= ' ' && c != '\\' && c != '\'' && c != '\"' && c != '`' && c != '{')
 r += c;
 else
 {
 r += '\\';
 if (escapeForInterpString && (c == '`' || c == '{'))
 {
 r += c;
 continue;
 }
 switch (c)
 {
 case '\a':
 r += 'a';
 break;
 case '\b':
 r += 'b';
 break;
 case '\f':
 r += 'f';
 break;
 case '\n':
 r += 'n';
 break;
 case '\r':
 r += 'r';
 break;
 case '\t':
 r += 't';
 break;
 case '\v':
 r += 'v';
 break;
 case '\'':
 r += '\'';
 break;
 case '\"':
 r += '\"';
 break;
 case '\\':
 r += '\\';
 break;
 default:
 Luau::formatAppend(r, "%03u", c);
 }
 }
 }
 return r;
}
}
#include <mutex>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
#ifdef __APPLE__
#endif
LUAU_FASTFLAGVARIABLE(DebugLuauTimeTracing, false)
namespace Luau
{
namespace TimeTrace
{
static double getClockPeriod()
{
#if defined(_WIN32)
 LARGE_INTEGER result = {};
 QueryPerformanceFrequency(&result);
 return 1.0 / double(result.QuadPart);
#elif defined(__APPLE__)
 mach_timebase_info_data_t result = {};
 mach_timebase_info(&result);
 return double(result.numer) / double(result.denom) * 1e-9;
#elif defined(__linux__)
 return 1e-9;
#else
 return 1.0 / double(CLOCKS_PER_SEC);
#endif
}
static double getClockTimestamp()
{
#if defined(_WIN32)
 LARGE_INTEGER result = {};
 QueryPerformanceCounter(&result);
 return double(result.QuadPart);
#elif defined(__APPLE__)
 return double(mach_absolute_time());
#elif defined(__linux__)
 timespec now;
 clock_gettime(CLOCK_MONOTONIC, &now);
 return now.tv_sec * 1e9 + now.tv_nsec;
#else
 return double(clock());
#endif
}
double getClock()
{
 static double period = getClockPeriod();
 static double start = getClockTimestamp();
 return (getClockTimestamp() - start) * period;
}
uint32_t getClockMicroseconds()
{
 static double period = getClockPeriod() * 1e6;
 static double start = getClockTimestamp();
 return uint32_t((getClockTimestamp() - start) * period);
}
}
} // namespace Luau
#if defined(LUAU_ENABLE_TIME_TRACE)
namespace Luau
{
namespace TimeTrace
{
struct GlobalContext
{
 GlobalContext() = default;
 ~GlobalContext()
 {
 for (ThreadContext* context : threads)
 {
 if (!context->events.empty())
 context->flushEvents();
 }
 if (traceFile)
 fclose(traceFile);
 }
 std::mutex mutex;
 std::vector<ThreadContext*> threads;
 uint32_t nextThreadId = 0;
 std::vector<Token> tokens;
 FILE* traceFile = nullptr;
};
GlobalContext& getGlobalContext()
{
 static GlobalContext context;
 return context;
}
uint16_t createToken(GlobalContext& context, const char* name, const char* category)
{
 std::scoped_lock lock(context.mutex);
 LUAU_ASSERT(context.tokens.size() < 64 * 1024);
 context.tokens.push_back({name, category});
 return uint16_t(context.tokens.size() - 1);
}
uint32_t createThread(GlobalContext& context, ThreadContext* threadContext)
{
 std::scoped_lock lock(context.mutex);
 context.threads.push_back(threadContext);
 return ++context.nextThreadId;
}
void releaseThread(GlobalContext& context, ThreadContext* threadContext)
{
 std::scoped_lock lock(context.mutex);
 if (auto it = std::find(context.threads.begin(), context.threads.end(), threadContext); it != context.threads.end())
 context.threads.erase(it);
}
void flushEvents(GlobalContext& context, uint32_t threadId, const std::vector<Event>& events, const std::vector<char>& data)
{
 std::scoped_lock lock(context.mutex);
 if (!context.traceFile)
 {
 context.traceFile = fopen("trace.json", "w");
 if (!context.traceFile)
 return;
 fprintf(context.traceFile, "[\n");
 }
 std::string temp;
 const unsigned tempReserve = 64 * 1024;
 temp.reserve(tempReserve);
 const char* rawData = data.data();
 bool unfinishedEnter = false;
 bool unfinishedArgs = false;
 for (const Event& ev : events)
 {
 switch (ev.type)
 {
 case EventType::Enter:
 {
 if (unfinishedArgs)
 {
 formatAppend(temp, "}");
 unfinishedArgs = false;
 }
 if (unfinishedEnter)
 {
 formatAppend(temp, "},\n");
 unfinishedEnter = false;
 }
 Token& token = context.tokens[ev.token];
 formatAppend(temp, R"({"name": "%s", "cat": "%s", "ph": "B", "ts": %u, "pid": 0, "tid": %u)", token.name, token.category,
 ev.data.microsec, threadId);
 unfinishedEnter = true;
 }
 break;
 case EventType::Leave:
 if (unfinishedArgs)
 {
 formatAppend(temp, "}");
 unfinishedArgs = false;
 }
 if (unfinishedEnter)
 {
 formatAppend(temp, "},\n");
 unfinishedEnter = false;
 }
 formatAppend(temp,
 R"({"ph": "E", "ts": %u, "pid": 0, "tid": %u},)"
 "\n",
 ev.data.microsec, threadId);
 break;
 case EventType::ArgName:
 LUAU_ASSERT(unfinishedEnter);
 if (!unfinishedArgs)
 {
 formatAppend(temp, R"(, "args": { "%s": )", rawData + ev.data.dataPos);
 unfinishedArgs = true;
 }
 else
 {
 formatAppend(temp, R"(, "%s": )", rawData + ev.data.dataPos);
 }
 break;
 case EventType::ArgValue:
 LUAU_ASSERT(unfinishedArgs);
 formatAppend(temp, R"("%s")", rawData + ev.data.dataPos);
 break;
 }
 if (temp.size() > tempReserve - 1024)
 {
 fwrite(temp.data(), 1, temp.size(), context.traceFile);
 temp.clear();
 }
 }
 if (unfinishedArgs)
 {
 formatAppend(temp, "}");
 unfinishedArgs = false;
 }
 if (unfinishedEnter)
 {
 formatAppend(temp, "},\n");
 unfinishedEnter = false;
 }
 fwrite(temp.data(), 1, temp.size(), context.traceFile);
 fflush(context.traceFile);
}
ThreadContext& getThreadContext()
{
 thread_local ThreadContext context;
 return context;
}
uint16_t createScopeData(const char* name, const char* category)
{
 return createToken(Luau::TimeTrace::getGlobalContext(), name, category);
}
}
} // namespace Luau
#endif
namespace Luau
{
class AstNameTable;
}
namespace Luau
{
namespace Compile
{
enum class Global
{
 Default = 0,
 Mutable,
 Written, // written in the code which means we can't reason about the value
};
struct Variable
{
 AstExpr* init = nullptr;
 bool written = false; // is the variable ever assigned to? filled by trackValues
 bool constant = false;
};
void assignMutable(DenseHashMap<AstName, Global>& globals, const AstNameTable& names, const char** mutableGlobals);
void trackValues(DenseHashMap<AstName, Global>& globals, DenseHashMap<AstLocal*, Variable>& variables, AstNode* root);
inline Global getGlobalState(const DenseHashMap<AstName, Global>& globals, AstName name)
{
 const Global* it = globals.find(name);
 return it ? *it : Global::Default;
}
}
} // namespace Luau
namespace Luau
{
namespace Compile
{
struct Constant
{
 enum Type
 {
 Type_Unknown,
 Type_Nil,
 Type_Boolean,
 Type_Number,
 Type_String,
 };
 Type type = Type_Unknown;
 unsigned int stringLength = 0;
 union
 {
 bool valueBoolean;
 double valueNumber;
 const char* valueString = nullptr;
 };
 bool isTruthful() const
 {
 LUAU_ASSERT(type != Type_Unknown);
 return type != Type_Nil && !(type == Type_Boolean && valueBoolean == false);
 }
 AstArray<const char> getString() const
 {
 LUAU_ASSERT(type == Type_String);
 return {valueString, stringLength};
 }
};
void foldConstants(DenseHashMap<AstExpr*, Constant>& constants, DenseHashMap<AstLocal*, Variable>& variables,
 DenseHashMap<AstLocal*, Constant>& locals, const DenseHashMap<AstExprCall*, int>* builtins, AstNode* root);
}
} // namespace Luau
namespace Luau
{
namespace Compile
{
Constant foldBuiltin(int bfid, const Constant* args, size_t count);
}
} // namespace Luau
namespace Luau
{
namespace Compile
{
const double kRadDeg = 3.14159265358979323846 / 180.0;
static Constant cvar()
{
 return Constant();
}
static Constant cbool(bool v)
{
 Constant res = {Constant::Type_Boolean};
 res.valueBoolean = v;
 return res;
}
static Constant cnum(double v)
{
 Constant res = {Constant::Type_Number};
 res.valueNumber = v;
 return res;
}
static Constant cstring(const char* v)
{
 Constant res = {Constant::Type_String};
 res.stringLength = unsigned(strlen(v));
 res.valueString = v;
 return res;
}
static Constant ctype(const Constant& c)
{
 LUAU_ASSERT(c.type != Constant::Type_Unknown);
 switch (c.type)
 {
 case Constant::Type_Nil:
 return cstring("nil");
 case Constant::Type_Boolean:
 return cstring("boolean");
 case Constant::Type_Number:
 return cstring("number");
 case Constant::Type_String:
 return cstring("string");
 default:
 LUAU_ASSERT(!"Unsupported constant type");
 return cvar();
 }
}
static uint32_t bit32(double v)
{
 return uint32_t(int64_t(v));
}
Constant foldBuiltin(int bfid, const Constant* args, size_t count)
{
 switch (bfid)
 {
 case LBF_MATH_ABS:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(fabs(args[0].valueNumber));
 break;
 case LBF_MATH_ACOS:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(acos(args[0].valueNumber));
 break;
 case LBF_MATH_ASIN:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(asin(args[0].valueNumber));
 break;
 case LBF_MATH_ATAN2:
 if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 return cnum(atan2(args[0].valueNumber, args[1].valueNumber));
 break;
 case LBF_MATH_ATAN:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(atan(args[0].valueNumber));
 break;
 case LBF_MATH_CEIL:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(ceil(args[0].valueNumber));
 break;
 case LBF_MATH_COSH:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(cosh(args[0].valueNumber));
 break;
 case LBF_MATH_COS:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(cos(args[0].valueNumber));
 break;
 case LBF_MATH_DEG:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(args[0].valueNumber / kRadDeg);
 break;
 case LBF_MATH_EXP:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(exp(args[0].valueNumber));
 break;
 case LBF_MATH_FLOOR:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(floor(args[0].valueNumber));
 break;
 case LBF_MATH_FMOD:
 if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 return cnum(fmod(args[0].valueNumber, args[1].valueNumber));
 break;
 case LBF_MATH_LDEXP:
 if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 return cnum(ldexp(args[0].valueNumber, int(args[1].valueNumber)));
 break;
 case LBF_MATH_LOG10:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(log10(args[0].valueNumber));
 break;
 case LBF_MATH_LOG:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(log(args[0].valueNumber));
 else if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 {
 if (args[1].valueNumber == 2.0)
 return cnum(log2(args[0].valueNumber));
 else if (args[1].valueNumber == 10.0)
 return cnum(log10(args[0].valueNumber));
 else
 return cnum(log(args[0].valueNumber) / log(args[1].valueNumber));
 }
 break;
 case LBF_MATH_MAX:
 if (count >= 1 && args[0].type == Constant::Type_Number)
 {
 double r = args[0].valueNumber;
 for (size_t i = 1; i < count; ++i)
 {
 if (args[i].type != Constant::Type_Number)
 return cvar();
 double a = args[i].valueNumber;
 r = (a > r) ? a : r;
 }
 return cnum(r);
 }
 break;
 case LBF_MATH_MIN:
 if (count >= 1 && args[0].type == Constant::Type_Number)
 {
 double r = args[0].valueNumber;
 for (size_t i = 1; i < count; ++i)
 {
 if (args[i].type != Constant::Type_Number)
 return cvar();
 double a = args[i].valueNumber;
 r = (a < r) ? a : r;
 }
 return cnum(r);
 }
 break;
 case LBF_MATH_POW:
 if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 return cnum(pow(args[0].valueNumber, args[1].valueNumber));
 break;
 case LBF_MATH_RAD:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(args[0].valueNumber * kRadDeg);
 break;
 case LBF_MATH_SINH:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(sinh(args[0].valueNumber));
 break;
 case LBF_MATH_SIN:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(sin(args[0].valueNumber));
 break;
 case LBF_MATH_SQRT:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(sqrt(args[0].valueNumber));
 break;
 case LBF_MATH_TANH:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(tanh(args[0].valueNumber));
 break;
 case LBF_MATH_TAN:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(tan(args[0].valueNumber));
 break;
 case LBF_BIT32_ARSHIFT:
 if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 {
 uint32_t u = bit32(args[0].valueNumber);
 int s = int(args[1].valueNumber);
 if (unsigned(s) < 32)
 return cnum(double(uint32_t(int32_t(u) >> s)));
 }
 break;
 case LBF_BIT32_BAND:
 if (count >= 1 && args[0].type == Constant::Type_Number)
 {
 uint32_t r = bit32(args[0].valueNumber);
 for (size_t i = 1; i < count; ++i)
 {
 if (args[i].type != Constant::Type_Number)
 return cvar();
 r &= bit32(args[i].valueNumber);
 }
 return cnum(double(r));
 }
 break;
 case LBF_BIT32_BNOT:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(double(uint32_t(~bit32(args[0].valueNumber))));
 break;
 case LBF_BIT32_BOR:
 if (count >= 1 && args[0].type == Constant::Type_Number)
 {
 uint32_t r = bit32(args[0].valueNumber);
 for (size_t i = 1; i < count; ++i)
 {
 if (args[i].type != Constant::Type_Number)
 return cvar();
 r |= bit32(args[i].valueNumber);
 }
 return cnum(double(r));
 }
 break;
 case LBF_BIT32_BXOR:
 if (count >= 1 && args[0].type == Constant::Type_Number)
 {
 uint32_t r = bit32(args[0].valueNumber);
 for (size_t i = 1; i < count; ++i)
 {
 if (args[i].type != Constant::Type_Number)
 return cvar();
 r ^= bit32(args[i].valueNumber);
 }
 return cnum(double(r));
 }
 break;
 case LBF_BIT32_BTEST:
 if (count >= 1 && args[0].type == Constant::Type_Number)
 {
 uint32_t r = bit32(args[0].valueNumber);
 for (size_t i = 1; i < count; ++i)
 {
 if (args[i].type != Constant::Type_Number)
 return cvar();
 r &= bit32(args[i].valueNumber);
 }
 return cbool(r != 0);
 }
 break;
 case LBF_BIT32_EXTRACT:
 if (count >= 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number &&
 (count == 2 || args[2].type == Constant::Type_Number))
 {
 uint32_t u = bit32(args[0].valueNumber);
 int f = int(args[1].valueNumber);
 int w = count == 2 ? 1 : int(args[2].valueNumber);
 if (f >= 0 && w > 0 && f + w <= 32)
 {
 uint32_t m = ~(0xfffffffeu << (w - 1));
 return cnum(double((u >> f) & m));
 }
 }
 break;
 case LBF_BIT32_LROTATE:
 if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 {
 uint32_t u = bit32(args[0].valueNumber);
 int s = int(args[1].valueNumber);
 return cnum(double((u << (s & 31)) | (u >> ((32 - s) & 31))));
 }
 break;
 case LBF_BIT32_LSHIFT:
 if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 {
 uint32_t u = bit32(args[0].valueNumber);
 int s = int(args[1].valueNumber);
 if (unsigned(s) < 32)
 return cnum(double(u << s));
 }
 break;
 case LBF_BIT32_REPLACE:
 if (count >= 3 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number && args[2].type == Constant::Type_Number &&
 (count == 3 || args[3].type == Constant::Type_Number))
 {
 uint32_t n = bit32(args[0].valueNumber);
 uint32_t v = bit32(args[1].valueNumber);
 int f = int(args[2].valueNumber);
 int w = count == 3 ? 1 : int(args[3].valueNumber);
 if (f >= 0 && w > 0 && f + w <= 32)
 {
 uint32_t m = ~(0xfffffffeu << (w - 1));
 return cnum(double((n & ~(m << f)) | ((v & m) << f)));
 }
 }
 break;
 case LBF_BIT32_RROTATE:
 if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 {
 uint32_t u = bit32(args[0].valueNumber);
 int s = int(args[1].valueNumber);
 return cnum(double((u >> (s & 31)) | (u << ((32 - s) & 31))));
 }
 break;
 case LBF_BIT32_RSHIFT:
 if (count == 2 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number)
 {
 uint32_t u = bit32(args[0].valueNumber);
 int s = int(args[1].valueNumber);
 if (unsigned(s) < 32)
 return cnum(double(u >> s));
 }
 break;
 case LBF_TYPE:
 if (count == 1 && args[0].type != Constant::Type_Unknown)
 return ctype(args[0]);
 break;
 case LBF_STRING_BYTE:
 if (count == 1 && args[0].type == Constant::Type_String)
 {
 if (args[0].stringLength > 0)
 return cnum(double(uint8_t(args[0].valueString[0])));
 }
 else if (count == 2 && args[0].type == Constant::Type_String && args[1].type == Constant::Type_Number)
 {
 int i = int(args[1].valueNumber);
 if (i > 0 && unsigned(i) <= args[0].stringLength)
 return cnum(double(uint8_t(args[0].valueString[i - 1])));
 }
 break;
 case LBF_STRING_LEN:
 if (count == 1 && args[0].type == Constant::Type_String)
 return cnum(double(args[0].stringLength));
 break;
 case LBF_TYPEOF:
 if (count == 1 && args[0].type != Constant::Type_Unknown)
 return ctype(args[0]);
 break;
 case LBF_MATH_CLAMP:
 if (count == 3 && args[0].type == Constant::Type_Number && args[1].type == Constant::Type_Number && args[2].type == Constant::Type_Number)
 {
 double min = args[1].valueNumber;
 double max = args[2].valueNumber;
 if (min <= max)
 {
 double v = args[0].valueNumber;
 v = v < min ? min : v;
 v = v > max ? max : v;
 return cnum(v);
 }
 }
 break;
 case LBF_MATH_SIGN:
 if (count == 1 && args[0].type == Constant::Type_Number)
 {
 double v = args[0].valueNumber;
 return cnum(v > 0.0 ? 1.0 : v < 0.0 ? -1.0 : 0.0);
 }
 break;
 case LBF_MATH_ROUND:
 if (count == 1 && args[0].type == Constant::Type_Number)
 return cnum(round(args[0].valueNumber));
 break;
 }
 return cvar();
}
}
} // namespace Luau
namespace Luau
{
struct CompileOptions;
}
namespace Luau
{
namespace Compile
{
struct Builtin
{
 AstName object;
 AstName method;
 bool empty() const
 {
 return object == AstName() && method == AstName();
 }
 bool isGlobal(const char* name) const
 {
 return object == AstName() && method == name;
 }
 bool isMethod(const char* table, const char* name) const
 {
 return object == table && method == name;
 }
};
Builtin getBuiltin(AstExpr* node, const DenseHashMap<AstName, Global>& globals, const DenseHashMap<AstLocal*, Variable>& variables);
void analyzeBuiltins(DenseHashMap<AstExprCall*, int>& result, const DenseHashMap<AstName, Global>& globals,
 const DenseHashMap<AstLocal*, Variable>& variables, const CompileOptions& options, AstNode* root);
struct BuiltinInfo
{
 int params;
 int results;
};
BuiltinInfo getBuiltinInfo(int bfid);
}
} // namespace Luau
namespace Luau
{
class AstNameTable;
struct ParseResult;
class BytecodeBuilder;
class BytecodeEncoder;
struct CompileOptions
{
 int optimizationLevel = 1;
 int debugLevel = 1;
 int coverageLevel = 0;
 const char* vectorLib = nullptr;
 const char* vectorCtor = nullptr;
 const char** mutableGlobals = nullptr;
};
class CompileError : public std::exception
{
public:
 CompileError(const Location& location, const std::string& message);
 virtual ~CompileError() throw();
 virtual const char* what() const throw();
 const Location& getLocation() const;
 static LUAU_NORETURN void raise(const Location& location, const char* format, ...) LUAU_PRINTF_ATTR(2, 3);
private:
 Location location;
 std::string message;
};
void compileOrThrow(BytecodeBuilder& bytecode, const ParseResult& parseResult, const AstNameTable& names, const CompileOptions& options = {});
void compileOrThrow(BytecodeBuilder& bytecode, const std::string& source, const CompileOptions& options = {}, const ParseOptions& parseOptions = {});
std::string compile(
 const std::string& source, const CompileOptions& options = {}, const ParseOptions& parseOptions = {}, BytecodeEncoder* encoder = nullptr);
}
namespace Luau
{
namespace Compile
{
Builtin getBuiltin(AstExpr* node, const DenseHashMap<AstName, Global>& globals, const DenseHashMap<AstLocal*, Variable>& variables)
{
 if (AstExprLocal* expr = node->as<AstExprLocal>())
 {
 const Variable* v = variables.find(expr->local);
 return v && !v->written && v->init ? getBuiltin(v->init, globals, variables) : Builtin();
 }
 else if (AstExprIndexName* expr = node->as<AstExprIndexName>())
 {
 if (AstExprGlobal* object = expr->expr->as<AstExprGlobal>())
 {
 return getGlobalState(globals, object->name) == Global::Default ? Builtin{object->name, expr->index} : Builtin();
 }
 else
 {
 return Builtin();
 }
 }
 else if (AstExprGlobal* expr = node->as<AstExprGlobal>())
 {
 return getGlobalState(globals, expr->name) == Global::Default ? Builtin{AstName(), expr->name} : Builtin();
 }
 else
 {
 return Builtin();
 }
}
static int getBuiltinFunctionId(const Builtin& builtin, const CompileOptions& options)
{
 if (builtin.isGlobal("assert"))
 return LBF_ASSERT;
 if (builtin.isGlobal("type"))
 return LBF_TYPE;
 if (builtin.isGlobal("typeof"))
 return LBF_TYPEOF;
 if (builtin.isGlobal("rawset"))
 return LBF_RAWSET;
 if (builtin.isGlobal("rawget"))
 return LBF_RAWGET;
 if (builtin.isGlobal("rawequal"))
 return LBF_RAWEQUAL;
 if (builtin.isGlobal("rawlen"))
 return LBF_RAWLEN;
 if (builtin.isGlobal("unpack"))
 return LBF_TABLE_UNPACK;
 if (builtin.isGlobal("select"))
 return LBF_SELECT_VARARG;
 if (builtin.isGlobal("getmetatable"))
 return LBF_GETMETATABLE;
 if (builtin.isGlobal("setmetatable"))
 return LBF_SETMETATABLE;
 if (builtin.object == "math")
 {
 if (builtin.method == "abs")
 return LBF_MATH_ABS;
 if (builtin.method == "acos")
 return LBF_MATH_ACOS;
 if (builtin.method == "asin")
 return LBF_MATH_ASIN;
 if (builtin.method == "atan2")
 return LBF_MATH_ATAN2;
 if (builtin.method == "atan")
 return LBF_MATH_ATAN;
 if (builtin.method == "ceil")
 return LBF_MATH_CEIL;
 if (builtin.method == "cosh")
 return LBF_MATH_COSH;
 if (builtin.method == "cos")
 return LBF_MATH_COS;
 if (builtin.method == "deg")
 return LBF_MATH_DEG;
 if (builtin.method == "exp")
 return LBF_MATH_EXP;
 if (builtin.method == "floor")
 return LBF_MATH_FLOOR;
 if (builtin.method == "fmod")
 return LBF_MATH_FMOD;
 if (builtin.method == "frexp")
 return LBF_MATH_FREXP;
 if (builtin.method == "ldexp")
 return LBF_MATH_LDEXP;
 if (builtin.method == "log10")
 return LBF_MATH_LOG10;
 if (builtin.method == "log")
 return LBF_MATH_LOG;
 if (builtin.method == "max")
 return LBF_MATH_MAX;
 if (builtin.method == "min")
 return LBF_MATH_MIN;
 if (builtin.method == "modf")
 return LBF_MATH_MODF;
 if (builtin.method == "pow")
 return LBF_MATH_POW;
 if (builtin.method == "rad")
 return LBF_MATH_RAD;
 if (builtin.method == "sinh")
 return LBF_MATH_SINH;
 if (builtin.method == "sin")
 return LBF_MATH_SIN;
 if (builtin.method == "sqrt")
 return LBF_MATH_SQRT;
 if (builtin.method == "tanh")
 return LBF_MATH_TANH;
 if (builtin.method == "tan")
 return LBF_MATH_TAN;
 if (builtin.method == "clamp")
 return LBF_MATH_CLAMP;
 if (builtin.method == "sign")
 return LBF_MATH_SIGN;
 if (builtin.method == "round")
 return LBF_MATH_ROUND;
 }
 if (builtin.object == "bit32")
 {
 if (builtin.method == "arshift")
 return LBF_BIT32_ARSHIFT;
 if (builtin.method == "band")
 return LBF_BIT32_BAND;
 if (builtin.method == "bnot")
 return LBF_BIT32_BNOT;
 if (builtin.method == "bor")
 return LBF_BIT32_BOR;
 if (builtin.method == "bxor")
 return LBF_BIT32_BXOR;
 if (builtin.method == "btest")
 return LBF_BIT32_BTEST;
 if (builtin.method == "extract")
 return LBF_BIT32_EXTRACT;
 if (builtin.method == "lrotate")
 return LBF_BIT32_LROTATE;
 if (builtin.method == "lshift")
 return LBF_BIT32_LSHIFT;
 if (builtin.method == "replace")
 return LBF_BIT32_REPLACE;
 if (builtin.method == "rrotate")
 return LBF_BIT32_RROTATE;
 if (builtin.method == "rshift")
 return LBF_BIT32_RSHIFT;
 if (builtin.method == "countlz")
 return LBF_BIT32_COUNTLZ;
 if (builtin.method == "countrz")
 return LBF_BIT32_COUNTRZ;
 }
 if (builtin.object == "string")
 {
 if (builtin.method == "byte")
 return LBF_STRING_BYTE;
 if (builtin.method == "char")
 return LBF_STRING_CHAR;
 if (builtin.method == "len")
 return LBF_STRING_LEN;
 if (builtin.method == "sub")
 return LBF_STRING_SUB;
 }
 if (builtin.object == "table")
 {
 if (builtin.method == "insert")
 return LBF_TABLE_INSERT;
 if (builtin.method == "unpack")
 return LBF_TABLE_UNPACK;
 }
 if (options.vectorCtor)
 {
 if (options.vectorLib)
 {
 if (builtin.isMethod(options.vectorLib, options.vectorCtor))
 return LBF_VECTOR;
 }
 else
 {
 if (builtin.isGlobal(options.vectorCtor))
 return LBF_VECTOR;
 }
 }
 return -1;
}
struct BuiltinVisitor : AstVisitor
{
 DenseHashMap<AstExprCall*, int>& result;
 const DenseHashMap<AstName, Global>& globals;
 const DenseHashMap<AstLocal*, Variable>& variables;
 const CompileOptions& options;
 BuiltinVisitor(DenseHashMap<AstExprCall*, int>& result, const DenseHashMap<AstName, Global>& globals,
 const DenseHashMap<AstLocal*, Variable>& variables, const CompileOptions& options)
 : result(result)
 , globals(globals)
 , variables(variables)
 , options(options)
 {
 }
 bool visit(AstExprCall* node) override
 {
 Builtin builtin = node->self ? Builtin() : getBuiltin(node->func, globals, variables);
 if (builtin.empty())
 return true;
 int bfid = getBuiltinFunctionId(builtin, options);
 if (bfid == LBF_SELECT_VARARG && !(node->args.size == 2 && node->args.data[1]->is<AstExprVarargs>()))
 bfid = -1;
 if (bfid >= 0)
 result[node] = bfid;
 return true;
 }
};
void analyzeBuiltins(DenseHashMap<AstExprCall*, int>& result, const DenseHashMap<AstName, Global>& globals,
 const DenseHashMap<AstLocal*, Variable>& variables, const CompileOptions& options, AstNode* root)
{
 BuiltinVisitor visitor{result, globals, variables, options};
 root->visit(&visitor);
}
BuiltinInfo getBuiltinInfo(int bfid)
{
 switch (LuauBuiltinFunction(bfid))
 {
 case LBF_NONE:
 return {-1, -1};
 case LBF_ASSERT:
 return {-1, -1};
 case LBF_MATH_ABS:
 case LBF_MATH_ACOS:
 case LBF_MATH_ASIN:
 return {1, 1};
 case LBF_MATH_ATAN2:
 return {2, 1};
 case LBF_MATH_ATAN:
 case LBF_MATH_CEIL:
 case LBF_MATH_COSH:
 case LBF_MATH_COS:
 case LBF_MATH_DEG:
 case LBF_MATH_EXP:
 case LBF_MATH_FLOOR:
 return {1, 1};
 case LBF_MATH_FMOD:
 return {2, 1};
 case LBF_MATH_FREXP:
 return {1, 2};
 case LBF_MATH_LDEXP:
 return {2, 1};
 case LBF_MATH_LOG10:
 return {1, 1};
 case LBF_MATH_LOG:
 return {-1, 1};
 case LBF_MATH_MAX:
 case LBF_MATH_MIN:
 return {-1, 1};
 case LBF_MATH_MODF:
 return {1, 2};
 case LBF_MATH_POW:
 return {2, 1};
 case LBF_MATH_RAD:
 case LBF_MATH_SINH:
 case LBF_MATH_SIN:
 case LBF_MATH_SQRT:
 case LBF_MATH_TANH:
 case LBF_MATH_TAN:
 return {1, 1};
 case LBF_BIT32_ARSHIFT:
 return {2, 1};
 case LBF_BIT32_BAND:
 return {-1, 1};
 case LBF_BIT32_BNOT:
 return {1, 1};
 case LBF_BIT32_BOR:
 case LBF_BIT32_BXOR:
 case LBF_BIT32_BTEST:
 return {-1, 1};
 case LBF_BIT32_EXTRACT:
 return {-1, 1};
 case LBF_BIT32_LROTATE:
 case LBF_BIT32_LSHIFT:
 return {2, 1};
 case LBF_BIT32_REPLACE:
 return {-1, 1};
 case LBF_BIT32_RROTATE:
 case LBF_BIT32_RSHIFT:
 return {2, 1};
 case LBF_TYPE:
 return {1, 1};
 case LBF_STRING_BYTE:
 return {-1, -1};
 case LBF_STRING_CHAR:
 return {-1, 1};
 case LBF_STRING_LEN:
 return {1, 1};
 case LBF_TYPEOF:
 return {1, 1};
 case LBF_STRING_SUB:
 return {-1, 1};
 case LBF_MATH_CLAMP:
 return {3, 1};
 case LBF_MATH_SIGN:
 case LBF_MATH_ROUND:
 return {1, 1};
 case LBF_RAWSET:
 return {3, 1};
 case LBF_RAWGET:
 case LBF_RAWEQUAL:
 return {2, 1};
 case LBF_TABLE_INSERT:
 return {-1, 0};
 case LBF_TABLE_UNPACK:
 return {-1, -1};
 case LBF_VECTOR:
 return {-1, 1};
 case LBF_BIT32_COUNTLZ:
 case LBF_BIT32_COUNTRZ:
 return {1, 1};
 case LBF_SELECT_VARARG:
 return {-1, -1};
 case LBF_RAWLEN:
 return {1, 1};
 case LBF_BIT32_EXTRACTK:
 return {3, 1};
 case LBF_GETMETATABLE:
 return {1, 1};
 case LBF_SETMETATABLE:
 return {2, 1};
 };
 LUAU_UNREACHABLE();
}
}
} // namespace Luau
namespace Luau
{
class BytecodeEncoder
{
public:
 virtual ~BytecodeEncoder() {}
 virtual uint8_t encodeOp(uint8_t op) = 0;
};
class BytecodeBuilder
{
public:
 struct StringRef
 {
 const char* data = nullptr;
 size_t length = 0;
 bool operator==(const StringRef& other) const;
 };
 struct TableShape
 {
 static const unsigned int kMaxLength = 32;
 int32_t keys[kMaxLength];
 unsigned int length = 0;
 bool operator==(const TableShape& other) const;
 };
 BytecodeBuilder(BytecodeEncoder* encoder = 0);
 uint32_t beginFunction(uint8_t numparams, bool isvararg = false);
 void endFunction(uint8_t maxstacksize, uint8_t numupvalues);
 void setMainFunction(uint32_t fid);
 int32_t addConstantNil();
 int32_t addConstantBoolean(bool value);
 int32_t addConstantNumber(double value);
 int32_t addConstantString(StringRef value);
 int32_t addImport(uint32_t iid);
 int32_t addConstantTable(const TableShape& shape);
 int32_t addConstantClosure(uint32_t fid);
 int16_t addChildFunction(uint32_t fid);
 void emitABC(LuauOpcode op, uint8_t a, uint8_t b, uint8_t c);
 void emitAD(LuauOpcode op, uint8_t a, int16_t d);
 void emitE(LuauOpcode op, int32_t e);
 void emitAux(uint32_t aux);
 size_t emitLabel();
 [[nodiscard]] bool patchJumpD(size_t jumpLabel, size_t targetLabel);
 [[nodiscard]] bool patchSkipC(size_t jumpLabel, size_t targetLabel);
 void foldJumps();
 void expandJumps();
 void setDebugFunctionName(StringRef name);
 void setDebugFunctionLineDefined(int line);
 void setDebugLine(int line);
 void pushDebugLocal(StringRef name, uint8_t reg, uint32_t startpc, uint32_t endpc);
 void pushDebugUpval(StringRef name);
 size_t getInstructionCount() const;
 uint32_t getDebugPC() const;
 void addDebugRemark(const char* format, ...) LUAU_PRINTF_ATTR(2, 3);
 void finalize();
 enum DumpFlags
 {
 Dump_Code = 1 << 0,
 Dump_Lines = 1 << 1,
 Dump_Source = 1 << 2,
 Dump_Locals = 1 << 3,
 Dump_Remarks = 1 << 4,
 };
 void setDumpFlags(uint32_t flags)
 {
 dumpFlags = flags;
 dumpFunctionPtr = &BytecodeBuilder::dumpCurrentFunction;
 }
 void setDumpSource(const std::string& source);
 bool needsDebugRemarks() const
 {
 return (dumpFlags & Dump_Remarks) != 0;
 }
 const std::string& getBytecode() const
 {
 LUAU_ASSERT(!bytecode.empty());
 return bytecode;
 }
 std::string dumpFunction(uint32_t id) const;
 std::string dumpEverything() const;
 std::string dumpSourceRemarks() const;
 void annotateInstruction(std::string& result, uint32_t fid, uint32_t instpos) const;
 static uint32_t getImportId(int32_t id0);
 static uint32_t getImportId(int32_t id0, int32_t id1);
 static uint32_t getImportId(int32_t id0, int32_t id1, int32_t id2);
 static int decomposeImportId(uint32_t ids, int32_t& id0, int32_t& id1, int32_t& id2);
 static uint32_t getStringHash(StringRef key);
 static std::string getError(const std::string& message);
 static uint8_t getVersion();
private:
 struct Constant
 {
 enum Type
 {
 Type_Nil,
 Type_Boolean,
 Type_Number,
 Type_String,
 Type_Import,
 Type_Table,
 Type_Closure,
 };
 Type type;
 union
 {
 bool valueBoolean;
 double valueNumber;
 unsigned int valueString;
 uint32_t valueImport; // 10-10-10-2 encoded import id
 uint32_t valueTable;
 uint32_t valueClosure; // index of function in global list
 };
 };
 struct ConstantKey
 {
 Constant::Type type;
 uint64_t value;
 bool operator==(const ConstantKey& key) const
 {
 return type == key.type && value == key.value;
 }
 };
 struct Function
 {
 std::string data;
 uint8_t maxstacksize = 0;
 uint8_t numparams = 0;
 uint8_t numupvalues = 0;
 bool isvararg = false;
 unsigned int debugname = 0;
 int debuglinedefined = 0;
 std::string dump;
 std::string dumpname;
 std::vector<int> dumpinstoffs;
 };
 struct DebugLocal
 {
 unsigned int name;
 uint8_t reg;
 uint32_t startpc;
 uint32_t endpc;
 };
 struct DebugUpval
 {
 unsigned int name;
 };
 struct Jump
 {
 uint32_t source;
 uint32_t target;
 };
 struct StringRefHash
 {
 size_t operator()(const StringRef& v) const;
 };
 struct ConstantKeyHash
 {
 size_t operator()(const ConstantKey& key) const;
 };
 struct TableShapeHash
 {
 size_t operator()(const TableShape& v) const;
 };
 std::vector<Function> functions;
 uint32_t currentFunction = ~0u;
 uint32_t mainFunction = ~0u;
 std::vector<uint32_t> insns;
 std::vector<int> lines;
 std::vector<Constant> constants;
 std::vector<uint32_t> protos;
 std::vector<Jump> jumps;
 std::vector<TableShape> tableShapes;
 bool hasLongJumps = false;
 DenseHashMap<ConstantKey, int32_t, ConstantKeyHash> constantMap;
 DenseHashMap<TableShape, int32_t, TableShapeHash> tableShapeMap;
 DenseHashMap<uint32_t, int16_t> protoMap;
 int debugLine = 0;
 std::vector<DebugLocal> debugLocals;
 std::vector<DebugUpval> debugUpvals;
 DenseHashMap<StringRef, unsigned int, StringRefHash> stringTable;
 std::vector<StringRef> debugStrings;
 std::vector<std::pair<uint32_t, uint32_t>> debugRemarks;
 std::string debugRemarkBuffer;
 BytecodeEncoder* encoder = nullptr;
 std::string bytecode;
 uint32_t dumpFlags = 0;
 std::vector<std::string> dumpSource;
 std::vector<std::pair<int, std::string>> dumpRemarks;
 std::string (BytecodeBuilder::*dumpFunctionPtr)(std::vector<int>&) const = nullptr;
 void validate() const;
 void validateInstructions() const;
 void validateVariadic() const;
 std::string dumpCurrentFunction(std::vector<int>& dumpinstoffs) const;
 void dumpConstant(std::string& result, int k) const;
 void dumpInstruction(const uint32_t* opcode, std::string& output, int targetLabel) const;
 void writeFunction(std::string& ss, uint32_t id) const;
 void writeLineInfo(std::string& ss) const;
 void writeStringTable(std::string& ss) const;
 int32_t addConstant(const ConstantKey& key, const Constant& value);
 unsigned int addStringTableEntry(StringRef value);
};
}
namespace Luau
{
static_assert(LBC_VERSION_TARGET >= LBC_VERSION_MIN && LBC_VERSION_TARGET <= LBC_VERSION_MAX, "Invalid bytecode version setup");
static_assert(LBC_VERSION_MAX <= 127, "Bytecode version should be 7-bit so that we can extend the serialization to use varint transparently");
static const uint32_t kMaxConstantCount = 1 << 23;
static const uint32_t kMaxClosureCount = 1 << 15;
static const int kMaxJumpDistance = 1 << 23;
static int log2(int v)
{
 LUAU_ASSERT(v);
 int r = 0;
 while (v >= (2 << r))
 r++;
 return r;
}
static void writeByte(std::string& ss, unsigned char value)
{
 ss.append(reinterpret_cast<const char*>(&value), sizeof(value));
}
static void writeInt(std::string& ss, int value)
{
 ss.append(reinterpret_cast<const char*>(&value), sizeof(value));
}
static void writeDouble(std::string& ss, double value)
{
 ss.append(reinterpret_cast<const char*>(&value), sizeof(value));
}
static void writeVarInt(std::string& ss, unsigned int value)
{
 do
 {
 writeByte(ss, (value & 127) | ((value > 127) << 7));
 value >>= 7;
 } while (value);
}
static int getOpLength(LuauOpcode op)
{
 switch (op)
 {
 case LOP_GETGLOBAL:
 case LOP_SETGLOBAL:
 case LOP_GETIMPORT:
 case LOP_GETTABLEKS:
 case LOP_SETTABLEKS:
 case LOP_NAMECALL:
 case LOP_JUMPIFEQ:
 case LOP_JUMPIFLE:
 case LOP_JUMPIFLT:
 case LOP_JUMPIFNOTEQ:
 case LOP_JUMPIFNOTLE:
 case LOP_JUMPIFNOTLT:
 case LOP_NEWTABLE:
 case LOP_SETLIST:
 case LOP_FORGLOOP:
 case LOP_LOADKX:
 case LOP_FASTCALL2:
 case LOP_FASTCALL2K:
 case LOP_JUMPXEQKNIL:
 case LOP_JUMPXEQKB:
 case LOP_JUMPXEQKN:
 case LOP_JUMPXEQKS:
 return 2;
 default:
 return 1;
 }
}
inline bool isJumpD(LuauOpcode op)
{
 switch (op)
 {
 case LOP_JUMP:
 case LOP_JUMPIF:
 case LOP_JUMPIFNOT:
 case LOP_JUMPIFEQ:
 case LOP_JUMPIFLE:
 case LOP_JUMPIFLT:
 case LOP_JUMPIFNOTEQ:
 case LOP_JUMPIFNOTLE:
 case LOP_JUMPIFNOTLT:
 case LOP_FORNPREP:
 case LOP_FORNLOOP:
 case LOP_FORGPREP:
 case LOP_FORGLOOP:
 case LOP_FORGPREP_INEXT:
 case LOP_FORGPREP_NEXT:
 case LOP_JUMPBACK:
 case LOP_JUMPXEQKNIL:
 case LOP_JUMPXEQKB:
 case LOP_JUMPXEQKN:
 case LOP_JUMPXEQKS:
 return true;
 default:
 return false;
 }
}
inline bool isSkipC(LuauOpcode op)
{
 switch (op)
 {
 case LOP_LOADB:
 return true;
 default:
 return false;
 }
}
inline bool isFastCall(LuauOpcode op)
{
 switch (op)
 {
 case LOP_FASTCALL:
 case LOP_FASTCALL1:
 case LOP_FASTCALL2:
 case LOP_FASTCALL2K:
 return true;
 default:
 return false;
 }
}
static int getJumpTarget(uint32_t insn, uint32_t pc)
{
 LuauOpcode op = LuauOpcode(LUAU_INSN_OP(insn));
 if (isJumpD(op))
 return int(pc + LUAU_INSN_D(insn) + 1);
 else if (isFastCall(op))
 return int(pc + LUAU_INSN_C(insn) + 2);
 else if (isSkipC(op) && LUAU_INSN_C(insn))
 return int(pc + LUAU_INSN_C(insn) + 1);
 else if (op == LOP_JUMPX)
 return int(pc + LUAU_INSN_E(insn) + 1);
 else
 return -1;
}
bool BytecodeBuilder::StringRef::operator==(const StringRef& other) const
{
 return (data && other.data) ? (length == other.length && memcmp(data, other.data, length) == 0) : (data == other.data);
}
bool BytecodeBuilder::TableShape::operator==(const TableShape& other) const
{
 return length == other.length && memcmp(keys, other.keys, length * sizeof(keys[0])) == 0;
}
size_t BytecodeBuilder::StringRefHash::operator()(const StringRef& v) const
{
 return hashRange(v.data, v.length);
}
size_t BytecodeBuilder::ConstantKeyHash::operator()(const ConstantKey& key) const
{
 const uint32_t m = 0x5bd1e995;
 uint32_t h1 = uint32_t(key.value);
 uint32_t h2 = uint32_t(key.value >> 32) ^ (key.type * m);
 h1 ^= h2 >> 18;
 h1 *= m;
 h2 ^= h1 >> 22;
 h2 *= m;
 h1 ^= h2 >> 17;
 h1 *= m;
 h2 ^= h1 >> 19;
 h2 *= m;
 return size_t(h2);
}
size_t BytecodeBuilder::TableShapeHash::operator()(const TableShape& v) const
{
 uint32_t hash = 2166136261;
 for (size_t i = 0; i < v.length; ++i)
 {
 hash ^= v.keys[i];
 hash *= 16777619;
 }
 return hash;
}
BytecodeBuilder::BytecodeBuilder(BytecodeEncoder* encoder)
 : constantMap({Constant::Type_Nil, ~0ull})
 , tableShapeMap(TableShape())
 , protoMap(~0u)
 , stringTable({nullptr, 0})
 , encoder(encoder)
{
 LUAU_ASSERT(stringTable.find(StringRef{"", 0}) == nullptr);
 insns.reserve(32);
 lines.reserve(32);
 constants.reserve(16);
 protos.reserve(16);
 functions.reserve(8);
}
uint32_t BytecodeBuilder::beginFunction(uint8_t numparams, bool isvararg)
{
 LUAU_ASSERT(currentFunction == ~0u);
 uint32_t id = uint32_t(functions.size());
 Function func;
 func.numparams = numparams;
 func.isvararg = isvararg;
 functions.push_back(func);
 currentFunction = id;
 hasLongJumps = false;
 debugLine = 0;
 return id;
}
void BytecodeBuilder::endFunction(uint8_t maxstacksize, uint8_t numupvalues)
{
 LUAU_ASSERT(currentFunction != ~0u);
 Function& func = functions[currentFunction];
 func.maxstacksize = maxstacksize;
 func.numupvalues = numupvalues;
#ifdef LUAU_ASSERTENABLED
 validate();
#endif
 func.data.reserve(32 + insns.size() * 7);
 writeFunction(func.data, currentFunction);
 currentFunction = ~0u;
 if (dumpFunctionPtr)
 func.dump = (this->*dumpFunctionPtr)(func.dumpinstoffs);
 insns.clear();
 lines.clear();
 constants.clear();
 protos.clear();
 jumps.clear();
 tableShapes.clear();
 debugLocals.clear();
 debugUpvals.clear();
 constantMap.clear();
 tableShapeMap.clear();
 protoMap.clear();
 debugRemarks.clear();
 debugRemarkBuffer.clear();
}
void BytecodeBuilder::setMainFunction(uint32_t fid)
{
 LUAU_ASSERT(fid < functions.size());
 mainFunction = fid;
}
int32_t BytecodeBuilder::addConstant(const ConstantKey& key, const Constant& value)
{
 if (int32_t* cache = constantMap.find(key))
 return *cache;
 uint32_t id = uint32_t(constants.size());
 if (id >= kMaxConstantCount)
 return -1;
 constantMap[key] = int32_t(id);
 constants.push_back(value);
 return int32_t(id);
}
unsigned int BytecodeBuilder::addStringTableEntry(StringRef value)
{
 unsigned int& index = stringTable[value];
 if (index == 0)
 {
 index = uint32_t(stringTable.size());
 if ((dumpFlags & Dump_Code) != 0)
 debugStrings.push_back(value);
 }
 return index;
}
int32_t BytecodeBuilder::addConstantNil()
{
 Constant c = {Constant::Type_Nil};
 ConstantKey k = {Constant::Type_Nil};
 return addConstant(k, c);
}
int32_t BytecodeBuilder::addConstantBoolean(bool value)
{
 Constant c = {Constant::Type_Boolean};
 c.valueBoolean = value;
 ConstantKey k = {Constant::Type_Boolean, value};
 return addConstant(k, c);
}
int32_t BytecodeBuilder::addConstantNumber(double value)
{
 Constant c = {Constant::Type_Number};
 c.valueNumber = value;
 ConstantKey k = {Constant::Type_Number};
 static_assert(sizeof(k.value) == sizeof(value), "Expecting double to be 64-bit");
 memcpy(&k.value, &value, sizeof(value));
 return addConstant(k, c);
}
int32_t BytecodeBuilder::addConstantString(StringRef value)
{
 unsigned int index = addStringTableEntry(value);
 Constant c = {Constant::Type_String};
 c.valueString = index;
 ConstantKey k = {Constant::Type_String, index};
 return addConstant(k, c);
}
int32_t BytecodeBuilder::addImport(uint32_t iid)
{
 Constant c = {Constant::Type_Import};
 c.valueImport = iid;
 ConstantKey k = {Constant::Type_Import, iid};
 return addConstant(k, c);
}
int32_t BytecodeBuilder::addConstantTable(const TableShape& shape)
{
 if (int32_t* cache = tableShapeMap.find(shape))
 return *cache;
 uint32_t id = uint32_t(constants.size());
 if (id >= kMaxConstantCount)
 return -1;
 Constant value = {Constant::Type_Table};
 value.valueTable = uint32_t(tableShapes.size());
 tableShapeMap[shape] = int32_t(id);
 tableShapes.push_back(shape);
 constants.push_back(value);
 return int32_t(id);
}
int32_t BytecodeBuilder::addConstantClosure(uint32_t fid)
{
 Constant c = {Constant::Type_Closure};
 c.valueClosure = fid;
 ConstantKey k = {Constant::Type_Closure, fid};
 return addConstant(k, c);
}
int16_t BytecodeBuilder::addChildFunction(uint32_t fid)
{
 if (int16_t* cache = protoMap.find(fid))
 return *cache;
 uint32_t id = uint32_t(protos.size());
 if (id >= kMaxClosureCount)
 return -1;
 protoMap[fid] = int16_t(id);
 protos.push_back(fid);
 return int16_t(id);
}
void BytecodeBuilder::emitABC(LuauOpcode op, uint8_t a, uint8_t b, uint8_t c)
{
 uint32_t insn = uint32_t(op) | (a << 8) | (b << 16) | (c << 24);
 insns.push_back(insn);
 lines.push_back(debugLine);
}
void BytecodeBuilder::emitAD(LuauOpcode op, uint8_t a, int16_t d)
{
 uint32_t insn = uint32_t(op) | (a << 8) | (uint16_t(d) << 16);
 insns.push_back(insn);
 lines.push_back(debugLine);
}
void BytecodeBuilder::emitE(LuauOpcode op, int32_t e)
{
 uint32_t insn = uint32_t(op) | (uint32_t(e) << 8);
 insns.push_back(insn);
 lines.push_back(debugLine);
}
void BytecodeBuilder::emitAux(uint32_t aux)
{
 insns.push_back(aux);
 lines.push_back(debugLine);
}
size_t BytecodeBuilder::emitLabel()
{
 return insns.size();
}
bool BytecodeBuilder::patchJumpD(size_t jumpLabel, size_t targetLabel)
{
 LUAU_ASSERT(jumpLabel < insns.size());
 unsigned int jumpInsn = insns[jumpLabel];
 (void)jumpInsn;
 LUAU_ASSERT(isJumpD(LuauOpcode(LUAU_INSN_OP(jumpInsn))));
 LUAU_ASSERT(LUAU_INSN_D(jumpInsn) == 0);
 LUAU_ASSERT(targetLabel <= insns.size());
 int offset = int(targetLabel) - int(jumpLabel) - 1;
 if (int16_t(offset) == offset)
 {
 insns[jumpLabel] |= uint16_t(offset) << 16;
 }
 else if (abs(offset) < kMaxJumpDistance)
 {
 hasLongJumps = true;
 }
 else
 {
 return false;
 }
 jumps.push_back({uint32_t(jumpLabel), uint32_t(targetLabel)});
 return true;
}
bool BytecodeBuilder::patchSkipC(size_t jumpLabel, size_t targetLabel)
{
 LUAU_ASSERT(jumpLabel < insns.size());
 unsigned int jumpInsn = insns[jumpLabel];
 (void)jumpInsn;
 LUAU_ASSERT(isSkipC(LuauOpcode(LUAU_INSN_OP(jumpInsn))) || isFastCall(LuauOpcode(LUAU_INSN_OP(jumpInsn))));
 LUAU_ASSERT(LUAU_INSN_C(jumpInsn) == 0);
 int offset = int(targetLabel) - int(jumpLabel) - 1;
 if (uint8_t(offset) != offset)
 {
 return false;
 }
 insns[jumpLabel] |= offset << 24;
 return true;
}
void BytecodeBuilder::setDebugFunctionName(StringRef name)
{
 unsigned int index = addStringTableEntry(name);
 functions[currentFunction].debugname = index;
 if (dumpFunctionPtr)
 functions[currentFunction].dumpname = std::string(name.data, name.length);
}
void BytecodeBuilder::setDebugFunctionLineDefined(int line)
{
 functions[currentFunction].debuglinedefined = line;
}
void BytecodeBuilder::setDebugLine(int line)
{
 debugLine = line;
}
void BytecodeBuilder::pushDebugLocal(StringRef name, uint8_t reg, uint32_t startpc, uint32_t endpc)
{
 unsigned int index = addStringTableEntry(name);
 DebugLocal local;
 local.name = index;
 local.reg = reg;
 local.startpc = startpc;
 local.endpc = endpc;
 debugLocals.push_back(local);
}
void BytecodeBuilder::pushDebugUpval(StringRef name)
{
 unsigned int index = addStringTableEntry(name);
 DebugUpval upval;
 upval.name = index;
 debugUpvals.push_back(upval);
}
size_t BytecodeBuilder::getInstructionCount() const
{
 return insns.size();
}
uint32_t BytecodeBuilder::getDebugPC() const
{
 return uint32_t(insns.size());
}
void BytecodeBuilder::addDebugRemark(const char* format, ...)
{
 if ((dumpFlags & Dump_Remarks) == 0)
 return;
 size_t offset = debugRemarkBuffer.size();
 va_list args;
 va_start(args, format);
 vformatAppend(debugRemarkBuffer, format, args);
 va_end(args);
 debugRemarkBuffer += '\0';
 debugRemarks.emplace_back(uint32_t(insns.size()), uint32_t(offset));
 dumpRemarks.emplace_back(debugLine, debugRemarkBuffer.c_str() + offset);
}
void BytecodeBuilder::finalize()
{
 LUAU_ASSERT(bytecode.empty());
 size_t capacity = 16;
 for (auto& p : stringTable)
 capacity += p.first.length + 2;
 for (const Function& func : functions)
 capacity += func.data.size();
 bytecode.reserve(capacity);
 uint8_t version = getVersion();
 LUAU_ASSERT(version >= LBC_VERSION_MIN && version <= LBC_VERSION_MAX);
 bytecode = char(version);
 writeStringTable(bytecode);
 writeVarInt(bytecode, uint32_t(functions.size()));
 for (const Function& func : functions)
 bytecode += func.data;
 LUAU_ASSERT(mainFunction < functions.size());
 writeVarInt(bytecode, mainFunction);
}
void BytecodeBuilder::writeFunction(std::string& ss, uint32_t id) const
{
 LUAU_ASSERT(id < functions.size());
 const Function& func = functions[id];
 writeByte(ss, func.maxstacksize);
 writeByte(ss, func.numparams);
 writeByte(ss, func.numupvalues);
 writeByte(ss, func.isvararg);
 writeVarInt(ss, uint32_t(insns.size()));
 for (size_t i = 0; i < insns.size();)
 {
 uint8_t op = LUAU_INSN_OP(insns[i]);
 LUAU_ASSERT(op < LOP__COUNT);
 int oplen = getOpLength(LuauOpcode(op));
 uint8_t openc = encoder ? encoder->encodeOp(op) : op;
 writeInt(ss, openc | (insns[i] & ~0xff));
 for (int j = 1; j < oplen; ++j)
 writeInt(ss, insns[i + j]);
 i += oplen;
 }
 writeVarInt(ss, uint32_t(constants.size()));
 for (const Constant& c : constants)
 {
 switch (c.type)
 {
 case Constant::Type_Nil:
 writeByte(ss, LBC_CONSTANT_NIL);
 break;
 case Constant::Type_Boolean:
 writeByte(ss, LBC_CONSTANT_BOOLEAN);
 writeByte(ss, c.valueBoolean);
 break;
 case Constant::Type_Number:
 writeByte(ss, LBC_CONSTANT_NUMBER);
 writeDouble(ss, c.valueNumber);
 break;
 case Constant::Type_String:
 writeByte(ss, LBC_CONSTANT_STRING);
 writeVarInt(ss, c.valueString);
 break;
 case Constant::Type_Import:
 writeByte(ss, LBC_CONSTANT_IMPORT);
 writeInt(ss, c.valueImport);
 break;
 case Constant::Type_Table:
 {
 const TableShape& shape = tableShapes[c.valueTable];
 writeByte(ss, LBC_CONSTANT_TABLE);
 writeVarInt(ss, uint32_t(shape.length));
 for (unsigned int i = 0; i < shape.length; ++i)
 writeVarInt(ss, shape.keys[i]);
 break;
 }
 case Constant::Type_Closure:
 writeByte(ss, LBC_CONSTANT_CLOSURE);
 writeVarInt(ss, c.valueClosure);
 break;
 default:
 LUAU_ASSERT(!"Unsupported constant type");
 }
 }
 writeVarInt(ss, uint32_t(protos.size()));
 for (uint32_t child : protos)
 writeVarInt(ss, child);
 writeVarInt(ss, func.debuglinedefined);
 writeVarInt(ss, func.debugname);
 bool hasLines = true;
 for (int line : lines)
 if (line == 0)
 {
 hasLines = false;
 break;
 }
 if (hasLines)
 {
 writeByte(ss, 1);
 writeLineInfo(ss);
 }
 else
 {
 writeByte(ss, 0);
 }
 bool hasDebug = !debugLocals.empty() || !debugUpvals.empty();
 if (hasDebug)
 {
 writeByte(ss, 1);
 writeVarInt(ss, uint32_t(debugLocals.size()));
 for (const DebugLocal& l : debugLocals)
 {
 writeVarInt(ss, l.name);
 writeVarInt(ss, l.startpc);
 writeVarInt(ss, l.endpc);
 writeByte(ss, l.reg);
 }
 writeVarInt(ss, uint32_t(debugUpvals.size()));
 for (const DebugUpval& l : debugUpvals)
 {
 writeVarInt(ss, l.name);
 }
 }
 else
 {
 writeByte(ss, 0);
 }
}
void BytecodeBuilder::writeLineInfo(std::string& ss) const
{
 LUAU_ASSERT(!lines.empty());
 int span = 1 << 24;
 for (size_t offset = 0; offset < lines.size(); offset += span)
 {
 size_t next = offset;
 int min = lines[offset];
 int max = lines[offset];
 for (; next < lines.size() && next < offset + span; ++next)
 {
 min = std::min(min, lines[next]);
 max = std::max(max, lines[next]);
 if (max - min > 255)
 break;
 }
 if (next < lines.size() && next - offset < size_t(span))
 {
 span = 1 << log2(int(next - offset));
 }
 }
 int baselineOne = 0;
 std::vector<int> baselineScratch;
 int* baseline = &baselineOne;
 size_t baselineSize = (lines.size() - 1) / span + 1;
 if (baselineSize > 1)
 {
 baselineScratch.resize(baselineSize);
 baseline = baselineScratch.data();
 }
 for (size_t offset = 0; offset < lines.size(); offset += span)
 {
 size_t next = offset;
 int min = lines[offset];
 for (; next < lines.size() && next < offset + span; ++next)
 min = std::min(min, lines[next]);
 baseline[offset / span] = min;
 }
 int logspan = log2(span);
 writeByte(ss, uint8_t(logspan));
 uint8_t lastOffset = 0;
 for (size_t i = 0; i < lines.size(); ++i)
 {
 int delta = lines[i] - baseline[i >> logspan];
 LUAU_ASSERT(delta >= 0 && delta <= 255);
 writeByte(ss, uint8_t(delta) - lastOffset);
 lastOffset = uint8_t(delta);
 }
 int lastLine = 0;
 for (size_t i = 0; i < baselineSize; ++i)
 {
 writeInt(ss, baseline[i] - lastLine);
 lastLine = baseline[i];
 }
}
void BytecodeBuilder::writeStringTable(std::string& ss) const
{
 std::vector<StringRef> strings(stringTable.size());
 for (auto& p : stringTable)
 {
 LUAU_ASSERT(p.second > 0 && p.second <= strings.size());
 strings[p.second - 1] = p.first;
 }
 writeVarInt(ss, uint32_t(strings.size()));
 for (auto& s : strings)
 {
 writeVarInt(ss, uint32_t(s.length));
 ss.append(s.data, s.length);
 }
}
uint32_t BytecodeBuilder::getImportId(int32_t id0)
{
 LUAU_ASSERT(unsigned(id0) < 1024);
 return (1u << 30) | (id0 << 20);
}
uint32_t BytecodeBuilder::getImportId(int32_t id0, int32_t id1)
{
 LUAU_ASSERT(unsigned(id0 | id1) < 1024);
 return (2u << 30) | (id0 << 20) | (id1 << 10);
}
uint32_t BytecodeBuilder::getImportId(int32_t id0, int32_t id1, int32_t id2)
{
 LUAU_ASSERT(unsigned(id0 | id1 | id2) < 1024);
 return (3u << 30) | (id0 << 20) | (id1 << 10) | id2;
}
int BytecodeBuilder::decomposeImportId(uint32_t ids, int32_t& id0, int32_t& id1, int32_t& id2)
{
 int count = ids >> 30;
 id0 = count > 0 ? int(ids >> 20) & 1023 : -1;
 id1 = count > 1 ? int(ids >> 10) & 1023 : -1;
 id2 = count > 2 ? int(ids) & 1023 : -1;
 return count;
}
uint32_t BytecodeBuilder::getStringHash(StringRef key)
{
 const char* str = key.data;
 size_t len = key.length;
 unsigned int h = unsigned(len);
 for (size_t i = len; i > 0; --i)
 h ^= (h << 5) + (h >> 2) + (uint8_t)str[i - 1];
 return h;
}
void BytecodeBuilder::foldJumps()
{
 if (hasLongJumps)
 return;
 for (Jump& jump : jumps)
 {
 uint32_t jumpLabel = jump.source;
 uint32_t jumpInsn = insns[jumpLabel];
 uint32_t targetLabel = jumpLabel + 1 + LUAU_INSN_D(jumpInsn);
 LUAU_ASSERT(targetLabel < insns.size());
 uint32_t targetInsn = insns[targetLabel];
 while (LUAU_INSN_OP(targetInsn) == LOP_JUMP && LUAU_INSN_D(targetInsn) >= 0)
 {
 targetLabel = targetLabel + 1 + LUAU_INSN_D(targetInsn);
 LUAU_ASSERT(targetLabel < insns.size());
 targetInsn = insns[targetLabel];
 }
 int offset = int(targetLabel) - int(jumpLabel) - 1;
 if (LUAU_INSN_OP(jumpInsn) == LOP_JUMP && LUAU_INSN_OP(targetInsn) == LOP_RETURN)
 {
 insns[jumpLabel] = targetInsn;
 lines[jumpLabel] = lines[targetLabel];
 }
 else if (int16_t(offset) == offset)
 {
 insns[jumpLabel] &= 0xffff;
 insns[jumpLabel] |= uint16_t(offset) << 16;
 }
 jump.target = targetLabel;
 }
}
void BytecodeBuilder::expandJumps()
{
 if (!hasLongJumps)
 return;
 const int kMaxJumpDistanceConservative = 32767 / 3;
 std::sort(jumps.begin(), jumps.end(), [](const Jump& lhs, const Jump& rhs) {
 return lhs.source < rhs.source;
 });
 std::vector<uint32_t> remap(insns.size());
 std::vector<uint32_t> newinsns;
 std::vector<int> newlines;
 LUAU_ASSERT(insns.size() == lines.size());
 newinsns.reserve(insns.size());
 newlines.reserve(insns.size());
 size_t currentJump = 0;
 size_t pendingTrampolines = 0;
 for (size_t i = 0; i < insns.size();)
 {
 uint8_t op = LUAU_INSN_OP(insns[i]);
 LUAU_ASSERT(op < LOP__COUNT);
 if (currentJump < jumps.size() && jumps[currentJump].source == i)
 {
 int offset = int(jumps[currentJump].target) - int(jumps[currentJump].source) - 1;
 if (abs(offset) > kMaxJumpDistanceConservative)
 {
 newinsns.push_back(LOP_JUMP | (1 << 16));
 newinsns.push_back(LOP_JUMPX);
 newlines.push_back(lines[i]);
 newlines.push_back(lines[i]);
 pendingTrampolines++;
 }
 currentJump++;
 }
 int oplen = getOpLength(LuauOpcode(op));
 for (int j = 0; j < oplen; ++j)
 {
 remap[i] = uint32_t(newinsns.size());
 newinsns.push_back(insns[i]);
 newlines.push_back(lines[i]);
 i++;
 }
 }
 LUAU_ASSERT(currentJump == jumps.size());
 LUAU_ASSERT(pendingTrampolines > 0);
 for (Jump& jump : jumps)
 {
 int offset = int(jump.target) - int(jump.source) - 1;
 int newoffset = int(remap[jump.target]) - int(remap[jump.source]) - 1;
 if (abs(offset) > kMaxJumpDistanceConservative)
 {
 uint32_t& insnt = newinsns[remap[jump.source] - 1];
 uint32_t& insnj = newinsns[remap[jump.source]];
 LUAU_ASSERT(LUAU_INSN_OP(insnt) == LOP_JUMPX);
 insnt &= 0xff;
 insnt |= uint32_t(newoffset + 1) << 8;
 insnj &= 0xffff;
 insnj |= uint16_t(-2) << 16;
 pendingTrampolines--;
 }
 else
 {
 uint32_t& insn = newinsns[remap[jump.source]];
 LUAU_ASSERT(LUAU_INSN_D(insn) == offset);
 LUAU_ASSERT(int16_t(newoffset) == newoffset);
 insn &= 0xffff;
 insn |= uint16_t(newoffset) << 16;
 }
 }
 LUAU_ASSERT(pendingTrampolines == 0);
 insns.swap(newinsns);
 lines.swap(newlines);
}
std::string BytecodeBuilder::getError(const std::string& message)
{
 std::string result;
 result += char(0);
 result += message;
 return result;
}
uint8_t BytecodeBuilder::getVersion()
{
 return LBC_VERSION_TARGET;
}
#ifdef LUAU_ASSERTENABLED
void BytecodeBuilder::validate() const
{
 validateInstructions();
 validateVariadic();
}
void BytecodeBuilder::validateInstructions() const
{
#define VREG(v) LUAU_ASSERT(unsigned(v) < func.maxstacksize)
#define VREGRANGE(v, count) LUAU_ASSERT(unsigned(v + (count < 0 ? 0 : count)) <= func.maxstacksize)
#define VUPVAL(v) LUAU_ASSERT(unsigned(v) < func.numupvalues)
#define VCONST(v, kind) LUAU_ASSERT(unsigned(v) < constants.size() && constants[v].type == Constant::Type_##kind)
#define VCONSTANY(v) LUAU_ASSERT(unsigned(v) < constants.size())
#define VJUMP(v) LUAU_ASSERT(size_t(i + 1 + v) < insns.size() && insnvalid[i + 1 + v])
 LUAU_ASSERT(currentFunction != ~0u);
 const Function& func = functions[currentFunction];
 std::vector<uint8_t> insnvalid(insns.size(), 0);
 for (size_t i = 0; i < insns.size();)
 {
 uint32_t insn = insns[i];
 LuauOpcode op = LuauOpcode(LUAU_INSN_OP(insn));
 insnvalid[i] = true;
 i += getOpLength(op);
 LUAU_ASSERT(i <= insns.size());
 }
 std::vector<uint8_t> openCaptures;
 for (size_t i = 0; i < insns.size();)
 {
 uint32_t insn = insns[i];
 LuauOpcode op = LuauOpcode(LUAU_INSN_OP(insn));
 switch (op)
 {
 case LOP_LOADNIL:
 VREG(LUAU_INSN_A(insn));
 break;
 case LOP_LOADB:
 VREG(LUAU_INSN_A(insn));
 LUAU_ASSERT(LUAU_INSN_B(insn) == 0 || LUAU_INSN_B(insn) == 1);
 VJUMP(LUAU_INSN_C(insn));
 break;
 case LOP_LOADN:
 VREG(LUAU_INSN_A(insn));
 break;
 case LOP_LOADK:
 VREG(LUAU_INSN_A(insn));
 VCONSTANY(LUAU_INSN_D(insn));
 break;
 case LOP_MOVE:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 break;
 case LOP_GETGLOBAL:
 case LOP_SETGLOBAL:
 VREG(LUAU_INSN_A(insn));
 VCONST(insns[i + 1], String);
 break;
 case LOP_GETUPVAL:
 case LOP_SETUPVAL:
 VREG(LUAU_INSN_A(insn));
 VUPVAL(LUAU_INSN_B(insn));
 break;
 case LOP_CLOSEUPVALS:
 VREG(LUAU_INSN_A(insn));
 while (openCaptures.size() && openCaptures.back() >= LUAU_INSN_A(insn))
 openCaptures.pop_back();
 break;
 case LOP_GETIMPORT:
 VREG(LUAU_INSN_A(insn));
 VCONST(LUAU_INSN_D(insn), Import);
 break;
 case LOP_GETTABLE:
 case LOP_SETTABLE:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 VREG(LUAU_INSN_C(insn));
 break;
 case LOP_GETTABLEKS:
 case LOP_SETTABLEKS:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 VCONST(insns[i + 1], String);
 break;
 case LOP_GETTABLEN:
 case LOP_SETTABLEN:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 break;
 case LOP_NEWCLOSURE:
 {
 VREG(LUAU_INSN_A(insn));
 LUAU_ASSERT(unsigned(LUAU_INSN_D(insn)) < protos.size());
 LUAU_ASSERT(protos[LUAU_INSN_D(insn)] < functions.size());
 unsigned int numupvalues = functions[protos[LUAU_INSN_D(insn)]].numupvalues;
 for (unsigned int j = 0; j < numupvalues; ++j)
 {
 LUAU_ASSERT(i + 1 + j < insns.size());
 uint32_t cinsn = insns[i + 1 + j];
 LUAU_ASSERT(LUAU_INSN_OP(cinsn) == LOP_CAPTURE);
 }
 }
 break;
 case LOP_NAMECALL:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 VCONST(insns[i + 1], String);
 LUAU_ASSERT(LUAU_INSN_OP(insns[i + 2]) == LOP_CALL);
 break;
 case LOP_CALL:
 {
 int nparams = LUAU_INSN_B(insn) - 1;
 int nresults = LUAU_INSN_C(insn) - 1;
 VREG(LUAU_INSN_A(insn));
 VREGRANGE(LUAU_INSN_A(insn) + 1, nparams);
 VREGRANGE(LUAU_INSN_A(insn), nresults); // 1..nresults
 }
 break;
 case LOP_RETURN:
 {
 int nresults = LUAU_INSN_B(insn) - 1;
 VREGRANGE(LUAU_INSN_A(insn), nresults);
 }
 break;
 case LOP_JUMP:
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_JUMPIF:
 case LOP_JUMPIFNOT:
 VREG(LUAU_INSN_A(insn));
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_JUMPIFEQ:
 case LOP_JUMPIFLE:
 case LOP_JUMPIFLT:
 case LOP_JUMPIFNOTEQ:
 case LOP_JUMPIFNOTLE:
 case LOP_JUMPIFNOTLT:
 VREG(LUAU_INSN_A(insn));
 VREG(insns[i + 1]);
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_JUMPXEQKNIL:
 case LOP_JUMPXEQKB:
 VREG(LUAU_INSN_A(insn));
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_JUMPXEQKN:
 VREG(LUAU_INSN_A(insn));
 VCONST(insns[i + 1] & 0xffffff, Number);
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_JUMPXEQKS:
 VREG(LUAU_INSN_A(insn));
 VCONST(insns[i + 1] & 0xffffff, String);
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_ADD:
 case LOP_SUB:
 case LOP_MUL:
 case LOP_DIV:
 case LOP_MOD:
 case LOP_POW:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 VREG(LUAU_INSN_C(insn));
 break;
 case LOP_ADDK:
 case LOP_SUBK:
 case LOP_MULK:
 case LOP_DIVK:
 case LOP_MODK:
 case LOP_POWK:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 VCONST(LUAU_INSN_C(insn), Number);
 break;
 case LOP_AND:
 case LOP_OR:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 VREG(LUAU_INSN_C(insn));
 break;
 case LOP_ANDK:
 case LOP_ORK:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 VCONSTANY(LUAU_INSN_C(insn));
 break;
 case LOP_CONCAT:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 VREG(LUAU_INSN_C(insn));
 LUAU_ASSERT(LUAU_INSN_B(insn) <= LUAU_INSN_C(insn));
 break;
 case LOP_NOT:
 case LOP_MINUS:
 case LOP_LENGTH:
 VREG(LUAU_INSN_A(insn));
 VREG(LUAU_INSN_B(insn));
 break;
 case LOP_NEWTABLE:
 VREG(LUAU_INSN_A(insn));
 break;
 case LOP_DUPTABLE:
 VREG(LUAU_INSN_A(insn));
 VCONST(LUAU_INSN_D(insn), Table);
 break;
 case LOP_SETLIST:
 {
 int count = LUAU_INSN_C(insn) - 1;
 VREG(LUAU_INSN_A(insn));
 VREGRANGE(LUAU_INSN_B(insn), count);
 }
 break;
 case LOP_FORNPREP:
 case LOP_FORNLOOP:
 VREG(LUAU_INSN_A(insn) + 2);
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_FORGPREP:
 VREG(LUAU_INSN_A(insn) + 2 + 1);
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_FORGLOOP:
 VREG(LUAU_INSN_A(insn) + 2 + uint8_t(insns[i + 1]));
 VJUMP(LUAU_INSN_D(insn));
 LUAU_ASSERT(uint8_t(insns[i + 1]) >= 1);
 break;
 case LOP_FORGPREP_INEXT:
 case LOP_FORGPREP_NEXT:
 VREG(LUAU_INSN_A(insn) + 4);
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_GETVARARGS:
 {
 int nresults = LUAU_INSN_B(insn) - 1;
 VREGRANGE(LUAU_INSN_A(insn), nresults);
 }
 break;
 case LOP_DUPCLOSURE:
 {
 VREG(LUAU_INSN_A(insn));
 VCONST(LUAU_INSN_D(insn), Closure);
 unsigned int proto = constants[LUAU_INSN_D(insn)].valueClosure;
 LUAU_ASSERT(proto < functions.size());
 unsigned int numupvalues = functions[proto].numupvalues;
 for (unsigned int j = 0; j < numupvalues; ++j)
 {
 LUAU_ASSERT(i + 1 + j < insns.size());
 uint32_t cinsn = insns[i + 1 + j];
 LUAU_ASSERT(LUAU_INSN_OP(cinsn) == LOP_CAPTURE);
 LUAU_ASSERT(LUAU_INSN_A(cinsn) == LCT_VAL || LUAU_INSN_A(cinsn) == LCT_UPVAL);
 }
 }
 break;
 case LOP_PREPVARARGS:
 LUAU_ASSERT(LUAU_INSN_A(insn) == func.numparams);
 LUAU_ASSERT(func.isvararg);
 break;
 case LOP_BREAK:
 break;
 case LOP_JUMPBACK:
 VJUMP(LUAU_INSN_D(insn));
 break;
 case LOP_LOADKX:
 VREG(LUAU_INSN_A(insn));
 VCONSTANY(insns[i + 1]);
 break;
 case LOP_JUMPX:
 VJUMP(LUAU_INSN_E(insn));
 break;
 case LOP_FASTCALL:
 VJUMP(LUAU_INSN_C(insn));
 LUAU_ASSERT(LUAU_INSN_OP(insns[i + 1 + LUAU_INSN_C(insn)]) == LOP_CALL);
 break;
 case LOP_FASTCALL1:
 VREG(LUAU_INSN_B(insn));
 VJUMP(LUAU_INSN_C(insn));
 LUAU_ASSERT(LUAU_INSN_OP(insns[i + 1 + LUAU_INSN_C(insn)]) == LOP_CALL);
 break;
 case LOP_FASTCALL2:
 VREG(LUAU_INSN_B(insn));
 VJUMP(LUAU_INSN_C(insn));
 LUAU_ASSERT(LUAU_INSN_OP(insns[i + 1 + LUAU_INSN_C(insn)]) == LOP_CALL);
 VREG(insns[i + 1]);
 break;
 case LOP_FASTCALL2K:
 VREG(LUAU_INSN_B(insn));
 VJUMP(LUAU_INSN_C(insn));
 LUAU_ASSERT(LUAU_INSN_OP(insns[i + 1 + LUAU_INSN_C(insn)]) == LOP_CALL);
 VCONSTANY(insns[i + 1]);
 break;
 case LOP_COVERAGE:
 break;
 case LOP_CAPTURE:
 switch (LUAU_INSN_A(insn))
 {
 case LCT_VAL:
 VREG(LUAU_INSN_B(insn));
 break;
 case LCT_REF:
 VREG(LUAU_INSN_B(insn));
 openCaptures.push_back(LUAU_INSN_B(insn));
 break;
 case LCT_UPVAL:
 VUPVAL(LUAU_INSN_B(insn));
 break;
 default:
 LUAU_ASSERT(!"Unsupported capture type");
 }
 break;
 default:
 LUAU_ASSERT(!"Unsupported opcode");
 }
 i += getOpLength(op);
 LUAU_ASSERT(i <= insns.size());
 }
 LUAU_ASSERT(openCaptures.empty());
#undef VREG
#undef VREGEND
#undef VUPVAL
#undef VCONST
#undef VCONSTANY
#undef VJUMP
}
void BytecodeBuilder::validateVariadic() const
{
 bool variadicSeq = false;
 std::vector<uint8_t> insntargets(insns.size(), 0);
 for (size_t i = 0; i < insns.size();)
 {
 uint32_t insn = insns[i];
 LuauOpcode op = LuauOpcode(LUAU_INSN_OP(insn));
 int target = getJumpTarget(insn, uint32_t(i));
 if (target >= 0 && !isFastCall(op))
 {
 LUAU_ASSERT(unsigned(target) < insns.size());
 insntargets[target] = true;
 }
 i += getOpLength(op);
 LUAU_ASSERT(i <= insns.size());
 }
 for (size_t i = 0; i < insns.size();)
 {
 uint32_t insn = insns[i];
 LuauOpcode op = LuauOpcode(LUAU_INSN_OP(insn));
 if (variadicSeq)
 {
 LUAU_ASSERT(!insntargets[i]);
 }
 if (op == LOP_CALL)
 {
 if (LUAU_INSN_B(insn) == 0)
 {
 LUAU_ASSERT(variadicSeq);
 variadicSeq = false;
 }
 else
 {
 LUAU_ASSERT(!variadicSeq);
 }
 if (LUAU_INSN_C(insn) == 0)
 {
 LUAU_ASSERT(!variadicSeq);
 variadicSeq = true;
 }
 }
 else if (op == LOP_GETVARARGS && LUAU_INSN_B(insn) == 0)
 {
 LUAU_ASSERT(!variadicSeq);
 variadicSeq = true;
 }
 else if ((op == LOP_RETURN && LUAU_INSN_B(insn) == 0) || (op == LOP_SETLIST && LUAU_INSN_C(insn) == 0))
 {
 LUAU_ASSERT(variadicSeq);
 variadicSeq = false;
 }
 else if (op == LOP_FASTCALL)
 {
 int callTarget = int(i + LUAU_INSN_C(insn) + 1);
 LUAU_ASSERT(unsigned(callTarget) < insns.size() && LUAU_INSN_OP(insns[callTarget]) == LOP_CALL);
 if (LUAU_INSN_B(insns[callTarget]) == 0)
 {
 LUAU_ASSERT(variadicSeq);
 }
 else
 {
 LUAU_ASSERT(!variadicSeq);
 }
 }
 else if (op == LOP_CLOSEUPVALS || op == LOP_NAMECALL || op == LOP_GETIMPORT || op == LOP_MOVE || op == LOP_GETUPVAL || op == LOP_GETGLOBAL ||
 op == LOP_GETTABLEKS || op == LOP_COVERAGE)
 {
 }
 else
 {
 LUAU_ASSERT(!variadicSeq);
 }
 i += getOpLength(op);
 LUAU_ASSERT(i <= insns.size());
 }
 LUAU_ASSERT(!variadicSeq);
}
#endif
static bool printableStringConstant(const char* str, size_t len)
{
 for (size_t i = 0; i < len; ++i)
 {
 if (unsigned(str[i]) < ' ')
 return false;
 }
 return true;
}
void BytecodeBuilder::dumpConstant(std::string& result, int k) const
{
 LUAU_ASSERT(unsigned(k) < constants.size());
 const Constant& data = constants[k];
 switch (data.type)
 {
 case Constant::Type_Nil:
 formatAppend(result, "nil");
 break;
 case Constant::Type_Boolean:
 formatAppend(result, "%s", data.valueBoolean ? "true" : "false");
 break;
 case Constant::Type_Number:
 formatAppend(result, "%.17g", data.valueNumber);
 break;
 case Constant::Type_String:
 {
 const StringRef& str = debugStrings[data.valueString - 1];
 if (printableStringConstant(str.data, str.length))
 {
 if (str.length < 32)
 formatAppend(result, "'%.*s'", int(str.length), str.data);
 else
 formatAppend(result, "'%.*s'...", 32, str.data);
 }
 break;
 }
 case Constant::Type_Import:
 {
 int id0 = -1, id1 = -1, id2 = -1;
 if (int count = decomposeImportId(data.valueImport, id0, id1, id2))
 {
 {
 const Constant& id = constants[id0];
 LUAU_ASSERT(id.type == Constant::Type_String && id.valueString <= debugStrings.size());
 const StringRef& str = debugStrings[id.valueString - 1];
 formatAppend(result, "%.*s", int(str.length), str.data);
 }
 if (count > 1)
 {
 const Constant& id = constants[id1];
 LUAU_ASSERT(id.type == Constant::Type_String && id.valueString <= debugStrings.size());
 const StringRef& str = debugStrings[id.valueString - 1];
 formatAppend(result, ".%.*s", int(str.length), str.data);
 }
 if (count > 2)
 {
 const Constant& id = constants[id2];
 LUAU_ASSERT(id.type == Constant::Type_String && id.valueString <= debugStrings.size());
 const StringRef& str = debugStrings[id.valueString - 1];
 formatAppend(result, ".%.*s", int(str.length), str.data);
 }
 }
 break;
 }
 case Constant::Type_Table:
 formatAppend(result, "{...}");
 break;
 case Constant::Type_Closure:
 {
 const Function& func = functions[data.valueClosure];
 if (!func.dumpname.empty())
 formatAppend(result, "'%s'", func.dumpname.c_str());
 break;
 }
 }
}
void BytecodeBuilder::dumpInstruction(const uint32_t* code, std::string& result, int targetLabel) const
{
 uint32_t insn = *code++;
 switch (LUAU_INSN_OP(insn))
 {
 case LOP_LOADNIL:
 formatAppend(result, "LOADNIL R%d\n", LUAU_INSN_A(insn));
 break;
 case LOP_LOADB:
 if (LUAU_INSN_C(insn))
 formatAppend(result, "LOADB R%d %d +%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 else
 formatAppend(result, "LOADB R%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
 break;
 case LOP_LOADN:
 formatAppend(result, "LOADN R%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
 break;
 case LOP_LOADK:
 formatAppend(result, "LOADK R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
 dumpConstant(result, LUAU_INSN_D(insn));
 result.append("]\n");
 break;
 case LOP_MOVE:
 formatAppend(result, "MOVE R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
 break;
 case LOP_GETGLOBAL:
 formatAppend(result, "GETGLOBAL R%d K%d [", LUAU_INSN_A(insn), *code);
 dumpConstant(result, *code);
 result.append("]\n");
 code++;
 break;
 case LOP_SETGLOBAL:
 formatAppend(result, "SETGLOBAL R%d K%d [", LUAU_INSN_A(insn), *code);
 dumpConstant(result, *code);
 result.append("]\n");
 code++;
 break;
 case LOP_GETUPVAL:
 formatAppend(result, "GETUPVAL R%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
 break;
 case LOP_SETUPVAL:
 formatAppend(result, "SETUPVAL R%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
 break;
 case LOP_CLOSEUPVALS:
 formatAppend(result, "CLOSEUPVALS R%d\n", LUAU_INSN_A(insn));
 break;
 case LOP_GETIMPORT:
 formatAppend(result, "GETIMPORT R%d %d [", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
 dumpConstant(result, LUAU_INSN_D(insn));
 result.append("]\n");
 code++;
 break;
 case LOP_GETTABLE:
 formatAppend(result, "GETTABLE R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_SETTABLE:
 formatAppend(result, "SETTABLE R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_GETTABLEKS:
 formatAppend(result, "GETTABLEKS R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), *code);
 dumpConstant(result, *code);
 result.append("]\n");
 code++;
 break;
 case LOP_SETTABLEKS:
 formatAppend(result, "SETTABLEKS R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), *code);
 dumpConstant(result, *code);
 result.append("]\n");
 code++;
 break;
 case LOP_GETTABLEN:
 formatAppend(result, "GETTABLEN R%d R%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn) + 1);
 break;
 case LOP_SETTABLEN:
 formatAppend(result, "SETTABLEN R%d R%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn) + 1);
 break;
 case LOP_NEWCLOSURE:
 formatAppend(result, "NEWCLOSURE R%d P%d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
 break;
 case LOP_NAMECALL:
 formatAppend(result, "NAMECALL R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), *code);
 dumpConstant(result, *code);
 result.append("]\n");
 code++;
 break;
 case LOP_CALL:
 formatAppend(result, "CALL R%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn) - 1, LUAU_INSN_C(insn) - 1);
 break;
 case LOP_RETURN:
 formatAppend(result, "RETURN R%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn) - 1);
 break;
 case LOP_JUMP:
 formatAppend(result, "JUMP L%d\n", targetLabel);
 break;
 case LOP_JUMPIF:
 formatAppend(result, "JUMPIF R%d L%d\n", LUAU_INSN_A(insn), targetLabel);
 break;
 case LOP_JUMPIFNOT:
 formatAppend(result, "JUMPIFNOT R%d L%d\n", LUAU_INSN_A(insn), targetLabel);
 break;
 case LOP_JUMPIFEQ:
 formatAppend(result, "JUMPIFEQ R%d R%d L%d\n", LUAU_INSN_A(insn), *code++, targetLabel);
 break;
 case LOP_JUMPIFLE:
 formatAppend(result, "JUMPIFLE R%d R%d L%d\n", LUAU_INSN_A(insn), *code++, targetLabel);
 break;
 case LOP_JUMPIFLT:
 formatAppend(result, "JUMPIFLT R%d R%d L%d\n", LUAU_INSN_A(insn), *code++, targetLabel);
 break;
 case LOP_JUMPIFNOTEQ:
 formatAppend(result, "JUMPIFNOTEQ R%d R%d L%d\n", LUAU_INSN_A(insn), *code++, targetLabel);
 break;
 case LOP_JUMPIFNOTLE:
 formatAppend(result, "JUMPIFNOTLE R%d R%d L%d\n", LUAU_INSN_A(insn), *code++, targetLabel);
 break;
 case LOP_JUMPIFNOTLT:
 formatAppend(result, "JUMPIFNOTLT R%d R%d L%d\n", LUAU_INSN_A(insn), *code++, targetLabel);
 break;
 case LOP_ADD:
 formatAppend(result, "ADD R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_SUB:
 formatAppend(result, "SUB R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_MUL:
 formatAppend(result, "MUL R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_DIV:
 formatAppend(result, "DIV R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_MOD:
 formatAppend(result, "MOD R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_POW:
 formatAppend(result, "POW R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_ADDK:
 formatAppend(result, "ADDK R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 dumpConstant(result, LUAU_INSN_C(insn));
 result.append("]\n");
 break;
 case LOP_SUBK:
 formatAppend(result, "SUBK R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 dumpConstant(result, LUAU_INSN_C(insn));
 result.append("]\n");
 break;
 case LOP_MULK:
 formatAppend(result, "MULK R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 dumpConstant(result, LUAU_INSN_C(insn));
 result.append("]\n");
 break;
 case LOP_DIVK:
 formatAppend(result, "DIVK R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 dumpConstant(result, LUAU_INSN_C(insn));
 result.append("]\n");
 break;
 case LOP_MODK:
 formatAppend(result, "MODK R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 dumpConstant(result, LUAU_INSN_C(insn));
 result.append("]\n");
 break;
 case LOP_POWK:
 formatAppend(result, "POWK R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 dumpConstant(result, LUAU_INSN_C(insn));
 result.append("]\n");
 break;
 case LOP_AND:
 formatAppend(result, "AND R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_OR:
 formatAppend(result, "OR R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_ANDK:
 formatAppend(result, "ANDK R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 dumpConstant(result, LUAU_INSN_C(insn));
 result.append("]\n");
 break;
 case LOP_ORK:
 formatAppend(result, "ORK R%d R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 dumpConstant(result, LUAU_INSN_C(insn));
 result.append("]\n");
 break;
 case LOP_CONCAT:
 formatAppend(result, "CONCAT R%d R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn));
 break;
 case LOP_NOT:
 formatAppend(result, "NOT R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
 break;
 case LOP_MINUS:
 formatAppend(result, "MINUS R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
 break;
 case LOP_LENGTH:
 formatAppend(result, "LENGTH R%d R%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn));
 break;
 case LOP_NEWTABLE:
 formatAppend(result, "NEWTABLE R%d %d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn) == 0 ? 0 : 1 << (LUAU_INSN_B(insn) - 1), *code++);
 break;
 case LOP_DUPTABLE:
 formatAppend(result, "DUPTABLE R%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
 break;
 case LOP_SETLIST:
 formatAppend(result, "SETLIST R%d R%d %d [%d]\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), LUAU_INSN_C(insn) - 1, *code++);
 break;
 case LOP_FORNPREP:
 formatAppend(result, "FORNPREP R%d L%d\n", LUAU_INSN_A(insn), targetLabel);
 break;
 case LOP_FORNLOOP:
 formatAppend(result, "FORNLOOP R%d L%d\n", LUAU_INSN_A(insn), targetLabel);
 break;
 case LOP_FORGPREP:
 formatAppend(result, "FORGPREP R%d L%d\n", LUAU_INSN_A(insn), targetLabel);
 break;
 case LOP_FORGLOOP:
 formatAppend(result, "FORGLOOP R%d L%d %d%s\n", LUAU_INSN_A(insn), targetLabel, uint8_t(*code), int(*code) < 0 ? " [inext]" : "");
 code++;
 break;
 case LOP_FORGPREP_INEXT:
 formatAppend(result, "FORGPREP_INEXT R%d L%d\n", LUAU_INSN_A(insn), targetLabel);
 break;
 case LOP_FORGPREP_NEXT:
 formatAppend(result, "FORGPREP_NEXT R%d L%d\n", LUAU_INSN_A(insn), targetLabel);
 break;
 case LOP_GETVARARGS:
 formatAppend(result, "GETVARARGS R%d %d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn) - 1);
 break;
 case LOP_DUPCLOSURE:
 formatAppend(result, "DUPCLOSURE R%d K%d [", LUAU_INSN_A(insn), LUAU_INSN_D(insn));
 dumpConstant(result, LUAU_INSN_D(insn));
 result.append("]\n");
 break;
 case LOP_BREAK:
 formatAppend(result, "BREAK\n");
 break;
 case LOP_JUMPBACK:
 formatAppend(result, "JUMPBACK L%d\n", targetLabel);
 break;
 case LOP_LOADKX:
 formatAppend(result, "LOADKX R%d K%d [", LUAU_INSN_A(insn), *code);
 dumpConstant(result, *code);
 result.append("]\n");
 code++;
 break;
 case LOP_JUMPX:
 formatAppend(result, "JUMPX L%d\n", targetLabel);
 break;
 case LOP_FASTCALL:
 formatAppend(result, "FASTCALL %d L%d\n", LUAU_INSN_A(insn), targetLabel);
 break;
 case LOP_FASTCALL1:
 formatAppend(result, "FASTCALL1 %d R%d L%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), targetLabel);
 break;
 case LOP_FASTCALL2:
 formatAppend(result, "FASTCALL2 %d R%d R%d L%d\n", LUAU_INSN_A(insn), LUAU_INSN_B(insn), *code, targetLabel);
 code++;
 break;
 case LOP_FASTCALL2K:
 formatAppend(result, "FASTCALL2K %d R%d K%d L%d [", LUAU_INSN_A(insn), LUAU_INSN_B(insn), *code, targetLabel);
 dumpConstant(result, *code);
 result.append("]\n");
 code++;
 break;
 case LOP_COVERAGE:
 formatAppend(result, "COVERAGE\n");
 break;
 case LOP_CAPTURE:
 formatAppend(result, "CAPTURE %s %c%d\n",
 LUAU_INSN_A(insn) == LCT_UPVAL ? "UPVAL"
 : LUAU_INSN_A(insn) == LCT_REF ? "REF"
 : LUAU_INSN_A(insn) == LCT_VAL ? "VAL"
 : "",
 LUAU_INSN_A(insn) == LCT_UPVAL ? 'U' : 'R', LUAU_INSN_B(insn));
 break;
 case LOP_JUMPXEQKNIL:
 formatAppend(result, "JUMPXEQKNIL R%d L%d%s\n", LUAU_INSN_A(insn), targetLabel, *code >> 31 ? " NOT" : "");
 code++;
 break;
 case LOP_JUMPXEQKB:
 formatAppend(result, "JUMPXEQKB R%d %d L%d%s\n", LUAU_INSN_A(insn), *code & 1, targetLabel, *code >> 31 ? " NOT" : "");
 code++;
 break;
 case LOP_JUMPXEQKN:
 formatAppend(result, "JUMPXEQKN R%d K%d L%d%s [", LUAU_INSN_A(insn), *code & 0xffffff, targetLabel, *code >> 31 ? " NOT" : "");
 dumpConstant(result, *code & 0xffffff);
 result.append("]\n");
 code++;
 break;
 case LOP_JUMPXEQKS:
 formatAppend(result, "JUMPXEQKS R%d K%d L%d%s [", LUAU_INSN_A(insn), *code & 0xffffff, targetLabel, *code >> 31 ? " NOT" : "");
 dumpConstant(result, *code & 0xffffff);
 result.append("]\n");
 code++;
 break;
 default:
 LUAU_ASSERT(!"Unsupported opcode");
 }
}
std::string BytecodeBuilder::dumpCurrentFunction(std::vector<int>& dumpinstoffs) const
{
 if ((dumpFlags & Dump_Code) == 0)
 return std::string();
 int lastLine = -1;
 size_t nextRemark = 0;
 std::string result;
 if (dumpFlags & Dump_Locals)
 {
 for (size_t i = 0; i < debugLocals.size(); ++i)
 {
 const DebugLocal& l = debugLocals[i];
 LUAU_ASSERT(l.startpc < l.endpc);
 LUAU_ASSERT(l.startpc < lines.size());
 LUAU_ASSERT(l.endpc <= lines.size());
 formatAppend(result, "local %d: reg %d, start pc %d line %d, end pc %d line %d\n", int(i), l.reg, l.startpc, lines[l.startpc],
 l.endpc - 1, lines[l.endpc - 1]);
 }
 }
 std::vector<int> labels(insns.size(), -1);
 for (size_t i = 0; i < insns.size();)
 {
 int target = getJumpTarget(insns[i], uint32_t(i));
 if (target >= 0)
 {
 LUAU_ASSERT(size_t(target) < insns.size());
 labels[target] = 0;
 }
 i += getOpLength(LuauOpcode(LUAU_INSN_OP(insns[i])));
 LUAU_ASSERT(i <= insns.size());
 }
 int nextLabel = 0;
 for (size_t i = 0; i < labels.size(); ++i)
 if (labels[i] == 0)
 labels[i] = nextLabel++;
 dumpinstoffs.resize(insns.size() + 1, -1);
 for (size_t i = 0; i < insns.size();)
 {
 const uint32_t* code = &insns[i];
 uint8_t op = LUAU_INSN_OP(*code);
 dumpinstoffs[i] = int(result.size());
 if (op == LOP_PREPVARARGS)
 {
 i++;
 continue;
 }
 if (dumpFlags & Dump_Remarks)
 {
 while (nextRemark < debugRemarks.size() && debugRemarks[nextRemark].first == i)
 {
 formatAppend(result, "REMARK %s\n", debugRemarkBuffer.c_str() + debugRemarks[nextRemark].second);
 nextRemark++;
 }
 }
 if (dumpFlags & Dump_Source)
 {
 int line = lines[i];
 if (line > 0 && line != lastLine)
 {
 LUAU_ASSERT(size_t(line - 1) < dumpSource.size());
 formatAppend(result, "%5d: %s\n", line, dumpSource[line - 1].c_str());
 lastLine = line;
 }
 }
 if (dumpFlags & Dump_Lines)
 formatAppend(result, "%d: ", lines[i]);
 if (labels[i] != -1)
 formatAppend(result, "L%d: ", labels[i]);
 int target = getJumpTarget(*code, uint32_t(i));
 dumpInstruction(code, result, target >= 0 ? labels[target] : -1);
 i += getOpLength(LuauOpcode(op));
 LUAU_ASSERT(i <= insns.size());
 }
 dumpinstoffs[insns.size()] = int(result.size());
 return result;
}
void BytecodeBuilder::setDumpSource(const std::string& source)
{
 dumpSource.clear();
 size_t pos = 0;
 while (pos != std::string::npos)
 {
 size_t next = source.find('\n', pos);
 if (next == std::string::npos)
 {
 dumpSource.push_back(source.substr(pos));
 pos = next;
 }
 else
 {
 dumpSource.push_back(source.substr(pos, next - pos));
 pos = next + 1;
 }
 if (!dumpSource.back().empty() && dumpSource.back().back() == '\r')
 dumpSource.back().pop_back();
 }
}
std::string BytecodeBuilder::dumpFunction(uint32_t id) const
{
 LUAU_ASSERT(id < functions.size());
 return functions[id].dump;
}
std::string BytecodeBuilder::dumpEverything() const
{
 std::string result;
 for (size_t i = 0; i < functions.size(); ++i)
 {
 std::string debugname = functions[i].dumpname.empty() ? "??" : functions[i].dumpname;
 formatAppend(result, "Function %d (%s):\n", int(i), debugname.c_str());
 result += functions[i].dump;
 result += "\n";
 }
 return result;
}
std::string BytecodeBuilder::dumpSourceRemarks() const
{
 std::string result;
 size_t nextRemark = 0;
 std::vector<std::pair<int, std::string>> remarks = dumpRemarks;
 std::sort(remarks.begin(), remarks.end());
 for (size_t i = 0; i < dumpSource.size(); ++i)
 {
 const std::string& line = dumpSource[i];
 size_t indent = 0;
 while (indent < line.length() && (line[indent] == ' ' || line[indent] == '\t'))
 indent++;
 while (nextRemark < remarks.size() && remarks[nextRemark].first == int(i + 1))
 {
 formatAppend(result, "%.*s-- remark: %s\n", int(indent), line.c_str(), remarks[nextRemark].second.c_str());
 nextRemark++;
 while (nextRemark < remarks.size() && remarks[nextRemark] == remarks[nextRemark - 1])
 nextRemark++;
 }
 result += line;
 if (i + 1 < dumpSource.size())
 result += '\n';
 }
 return result;
}
void BytecodeBuilder::annotateInstruction(std::string& result, uint32_t fid, uint32_t instpos) const
{
 if ((dumpFlags & Dump_Code) == 0)
 return;
 LUAU_ASSERT(fid < functions.size());
 const Function& function = functions[fid];
 const std::string& dump = function.dump;
 const std::vector<int>& dumpinstoffs = function.dumpinstoffs;
 uint32_t next = instpos + 1;
 LUAU_ASSERT(next < dumpinstoffs.size());
 while (next < dumpinstoffs.size() && dumpinstoffs[next] == -1)
 next++;
 formatAppend(result, "%.*s", dumpinstoffs[next] - dumpinstoffs[instpos], dump.data() + dumpinstoffs[instpos]);
}
}
namespace Luau
{
namespace Compile
{
uint64_t modelCost(AstNode* root, AstLocal* const* vars, size_t varCount, const DenseHashMap<AstExprCall*, int>& builtins);
int computeCost(uint64_t model, const bool* varsConst, size_t varCount);
int getTripCount(double from, double to, double step);
}
} // namespace Luau
namespace Luau
{
namespace Compile
{
struct TableShape
{
 unsigned int arraySize = 0;
 unsigned int hashSize = 0;
};
void predictTableShapes(DenseHashMap<AstExprTable*, TableShape>& shapes, AstNode* root);
}
} // namespace Luau
#include <bitset>
#include <memory>
LUAU_FASTINTVARIABLE(LuauCompileLoopUnrollThreshold, 25)
LUAU_FASTINTVARIABLE(LuauCompileLoopUnrollThresholdMaxBoost, 300)
LUAU_FASTINTVARIABLE(LuauCompileInlineThreshold, 25)
LUAU_FASTINTVARIABLE(LuauCompileInlineThresholdMaxBoost, 300)
LUAU_FASTINTVARIABLE(LuauCompileInlineDepth, 5)
LUAU_FASTFLAGVARIABLE(LuauCompileInlineDefer, false)
namespace Luau
{
using namespace Luau::Compile;
static const uint32_t kMaxRegisterCount = 255;
static const uint32_t kMaxUpvalueCount = 200;
static const uint32_t kMaxLocalCount = 200;
static const uint32_t kMaxInstructionCount = 1'000'000'000;
static const uint8_t kInvalidReg = 255;
CompileError::CompileError(const Location& location, const std::string& message)
 : location(location)
 , message(message)
{
}
CompileError::~CompileError() throw() {}
const char* CompileError::what() const throw()
{
 return message.c_str();
}
const Location& CompileError::getLocation() const
{
 return location;
}
LUAU_NOINLINE void CompileError::raise(const Location& location, const char* format, ...)
{
 va_list args;
 va_start(args, format);
 std::string message = vformat(format, args);
 va_end(args);
 throw CompileError(location, message);
}
static BytecodeBuilder::StringRef sref(AstName name)
{
 LUAU_ASSERT(name.value);
 return {name.value, strlen(name.value)};
}
static BytecodeBuilder::StringRef sref(AstArray<char> data)
{
 LUAU_ASSERT(data.data);
 return {data.data, data.size};
}
static BytecodeBuilder::StringRef sref(AstArray<const char> data)
{
 LUAU_ASSERT(data.data);
 return {data.data, data.size};
}
struct Compiler
{
 struct RegScope;
 Compiler(BytecodeBuilder& bytecode, const CompileOptions& options)
 : bytecode(bytecode)
 , options(options)
 , functions(nullptr)
 , locals(nullptr)
 , globals(AstName())
 , variables(nullptr)
 , constants(nullptr)
 , locstants(nullptr)
 , tableShapes(nullptr)
 , builtins(nullptr)
 {
 localStack.reserve(16);
 upvals.reserve(16);
 }
 int getLocalReg(AstLocal* local)
 {
 Local* l = locals.find(local);
 return l && l->allocated ? l->reg : -1;
 }
 uint8_t getUpval(AstLocal* local)
 {
 for (size_t uid = 0; uid < upvals.size(); ++uid)
 if (upvals[uid] == local)
 return uint8_t(uid);
 if (upvals.size() >= kMaxUpvalueCount)
 CompileError::raise(
 local->location, "Out of upvalue registers when trying to allocate %s: exceeded limit %d", local->name.value, kMaxUpvalueCount);
 Variable* v = variables.find(local);
 if (v && v->written)
 locals[local].captured = true;
 upvals.push_back(local);
 return uint8_t(upvals.size() - 1);
 }
 bool alwaysTerminates(AstStat* node)
 {
 if (AstStatBlock* stat = node->as<AstStatBlock>())
 return stat->body.size > 0 && alwaysTerminates(stat->body.data[stat->body.size - 1]);
 else if (node->is<AstStatReturn>())
 return true;
 else if (node->is<AstStatBreak>() || node->is<AstStatContinue>())
 return true;
 else if (AstStatIf* stat = node->as<AstStatIf>())
 return stat->elsebody && alwaysTerminates(stat->thenbody) && alwaysTerminates(stat->elsebody);
 else
 return false;
 }
 void emitLoadK(uint8_t target, int32_t cid)
 {
 LUAU_ASSERT(cid >= 0);
 if (cid < 32768)
 {
 bytecode.emitAD(LOP_LOADK, target, int16_t(cid));
 }
 else
 {
 bytecode.emitAD(LOP_LOADKX, target, 0);
 bytecode.emitAux(cid);
 }
 }
 AstExprFunction* getFunctionExpr(AstExpr* node)
 {
 if (AstExprLocal* expr = node->as<AstExprLocal>())
 {
 Variable* lv = variables.find(expr->local);
 if (!lv || lv->written || !lv->init)
 return nullptr;
 return getFunctionExpr(lv->init);
 }
 else if (AstExprGroup* expr = node->as<AstExprGroup>())
 return getFunctionExpr(expr->expr);
 else if (AstExprTypeAssertion* expr = node->as<AstExprTypeAssertion>())
 return getFunctionExpr(expr->expr);
 else
 return node->as<AstExprFunction>();
 }
 uint32_t compileFunction(AstExprFunction* func)
 {
 LUAU_TIMETRACE_SCOPE("Compiler::compileFunction", "Compiler");
 if (func->debugname.value)
 LUAU_TIMETRACE_ARGUMENT("name", func->debugname.value);
 LUAU_ASSERT(!functions.contains(func));
 LUAU_ASSERT(regTop == 0 && stackSize == 0 && localStack.empty() && upvals.empty());
 RegScope rs(this);
 bool self = func->self != 0;
 uint32_t fid = bytecode.beginFunction(uint8_t(self + func->args.size), func->vararg);
 setDebugLine(func);
 if (func->vararg)
 bytecode.emitABC(LOP_PREPVARARGS, uint8_t(self + func->args.size), 0, 0);
 uint8_t args = allocReg(func, self + unsigned(func->args.size));
 if (func->self)
 pushLocal(func->self, args);
 for (size_t i = 0; i < func->args.size; ++i)
 pushLocal(func->args.data[i], uint8_t(args + self + i));
 AstStatBlock* stat = func->body;
 for (size_t i = 0; i < stat->body.size; ++i)
 compileStat(stat->body.data[i]);
 if (!alwaysTerminates(stat))
 {
 setDebugLineEnd(stat);
 closeLocals(0);
 bytecode.emitABC(LOP_RETURN, 0, 1, 0);
 }
 if (options.optimizationLevel >= 1 && options.debugLevel >= 2)
 gatherConstUpvals(func);
 bytecode.setDebugFunctionLineDefined(func->location.begin.line + 1);
 if (options.debugLevel >= 1 && func->debugname.value)
 bytecode.setDebugFunctionName(sref(func->debugname));
 if (options.debugLevel >= 2 && !upvals.empty())
 {
 for (AstLocal* l : upvals)
 bytecode.pushDebugUpval(sref(l->name));
 }
 if (options.optimizationLevel >= 1)
 bytecode.foldJumps();
 bytecode.expandJumps();
 popLocals(0);
 if (bytecode.getInstructionCount() > kMaxInstructionCount)
 CompileError::raise(func->location, "Exceeded function instruction limit; split the function into parts to compile");
 bytecode.endFunction(uint8_t(stackSize), uint8_t(upvals.size()));
 Function& f = functions[func];
 f.id = fid;
 f.upvals = upvals;
 if (options.optimizationLevel >= 2 && !func->vararg && !getfenvUsed && !setfenvUsed)
 {
 f.canInline = true;
 f.stackSize = stackSize;
 f.costModel = modelCost(func->body, func->args.data, func->args.size, builtins);
 if (alwaysTerminates(func->body))
 {
 ReturnVisitor returnVisitor(this);
 stat->visit(&returnVisitor);
 f.returnsOne = returnVisitor.returnsOne;
 }
 }
 upvals.clear();
 stackSize = 0;
 return fid;
 }
 bool isExprMultRet(AstExpr* node)
 {
 AstExprCall* expr = node->as<AstExprCall>();
 if (!expr)
 return node->is<AstExprVarargs>();
 if (options.optimizationLevel <= 1)
 return true;
 if (isConstant(expr))
 return false;
 if (options.optimizationLevel >= 2)
 if (int* bfid = builtins.find(expr))
 return getBuiltinInfo(*bfid).results != 1;
 AstExprFunction* func = getFunctionExpr(expr->func);
 Function* fi = func ? functions.find(func) : nullptr;
 if (fi && fi->returnsOne)
 return false;
 return true;
 }
 bool compileExprTempMultRet(AstExpr* node, uint8_t target)
 {
 if (AstExprCall* expr = node->as<AstExprCall>())
 {
 if (options.optimizationLevel >= 2 && !isExprMultRet(node))
 {
 compileExprTemp(node, target);
 return false;
 }
 RegScope rs(this, target);
 compileExprCall(expr, target, 0, true, true);
 return true;
 }
 else if (AstExprVarargs* expr = node->as<AstExprVarargs>())
 {
 RegScope rs(this, target);
 compileExprVarargs(expr, target, 0, true);
 return true;
 }
 else
 {
 compileExprTemp(node, target);
 return false;
 }
 }
 void compileExprTempTop(AstExpr* node, uint8_t target)
 {
 RegScope rs(this, target + 1);
 compileExprTemp(node, target);
 }
 void compileExprVarargs(AstExprVarargs* expr, uint8_t target, uint8_t targetCount, bool multRet = false)
 {
 LUAU_ASSERT(!multRet || unsigned(target + targetCount) == regTop);
 setDebugLine(expr);
 bytecode.emitABC(LOP_GETVARARGS, target, multRet ? 0 : uint8_t(targetCount + 1), 0);
 }
 void compileExprSelectVararg(AstExprCall* expr, uint8_t target, uint8_t targetCount, bool targetTop, bool multRet, uint8_t regs)
 {
 LUAU_ASSERT(targetCount == 1);
 LUAU_ASSERT(!expr->self);
 LUAU_ASSERT(expr->args.size == 2 && expr->args.data[1]->is<AstExprVarargs>());
 AstExpr* arg = expr->args.data[0];
 uint8_t argreg;
 if (int reg = getExprLocalReg(arg); reg >= 0)
 argreg = uint8_t(reg);
 else
 {
 argreg = uint8_t(regs + 1);
 compileExprTempTop(arg, argreg);
 }
 size_t fastcallLabel = bytecode.emitLabel();
 bytecode.emitABC(LOP_FASTCALL1, LBF_SELECT_VARARG, argreg, 0);
 compileExprTemp(expr->func, regs);
 if (argreg != regs + 1)
 bytecode.emitABC(LOP_MOVE, uint8_t(regs + 1), argreg, 0);
 bytecode.emitABC(LOP_GETVARARGS, uint8_t(regs + 2), 0, 0);
 size_t callLabel = bytecode.emitLabel();
 if (!bytecode.patchSkipC(fastcallLabel, callLabel))
 CompileError::raise(expr->func->location, "Exceeded jump distance limit; simplify the code to compile");
 bytecode.emitABC(LOP_CALL, regs, 0, multRet ? 0 : uint8_t(targetCount + 1));
 if (!targetTop)
 {
 for (size_t i = 0; i < targetCount; ++i)
 bytecode.emitABC(LOP_MOVE, uint8_t(target + i), uint8_t(regs + i), 0);
 }
 }
 void compileExprFastcallN(
 AstExprCall* expr, uint8_t target, uint8_t targetCount, bool targetTop, bool multRet, uint8_t regs, int bfid, int bfK = -1)
 {
 LUAU_ASSERT(!expr->self);
 LUAU_ASSERT(expr->args.size >= 1);
 LUAU_ASSERT(expr->args.size <= 2 || (bfid == LBF_BIT32_EXTRACTK && expr->args.size == 3));
 LUAU_ASSERT(bfid == LBF_BIT32_EXTRACTK ? bfK >= 0 : bfK < 0);
 LuauOpcode opc = expr->args.size == 1 ? LOP_FASTCALL1 : (bfK >= 0 || isConstant(expr->args.data[1])) ? LOP_FASTCALL2K : LOP_FASTCALL2;
 uint32_t args[3] = {};
 for (size_t i = 0; i < expr->args.size; ++i)
 {
 if (i > 0 && opc == LOP_FASTCALL2K)
 {
 int32_t cid = getConstantIndex(expr->args.data[i]);
 if (cid < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 args[i] = cid;
 }
 else if (int reg = getExprLocalReg(expr->args.data[i]); reg >= 0)
 {
 args[i] = uint8_t(reg);
 }
 else
 {
 args[i] = uint8_t(regs + 1 + i);
 compileExprTempTop(expr->args.data[i], uint8_t(args[i]));
 }
 }
 size_t fastcallLabel = bytecode.emitLabel();
 bytecode.emitABC(opc, uint8_t(bfid), uint8_t(args[0]), 0);
 if (opc != LOP_FASTCALL1)
 bytecode.emitAux(bfK >= 0 ? bfK : args[1]);
 for (size_t i = 0; i < expr->args.size; ++i)
 {
 if (i > 0 && opc == LOP_FASTCALL2K)
 emitLoadK(uint8_t(regs + 1 + i), args[i]);
 else if (args[i] != regs + 1 + i)
 bytecode.emitABC(LOP_MOVE, uint8_t(regs + 1 + i), uint8_t(args[i]), 0);
 }
 compileExprTemp(expr->func, regs);
 size_t callLabel = bytecode.emitLabel();
 if (!bytecode.patchSkipC(fastcallLabel, callLabel))
 CompileError::raise(expr->func->location, "Exceeded jump distance limit; simplify the code to compile");
 bytecode.emitABC(LOP_CALL, regs, uint8_t(expr->args.size + 1), multRet ? 0 : uint8_t(targetCount + 1));
 if (!targetTop)
 {
 for (size_t i = 0; i < targetCount; ++i)
 bytecode.emitABC(LOP_MOVE, uint8_t(target + i), uint8_t(regs + i), 0);
 }
 }
 bool tryCompileInlinedCall(AstExprCall* expr, AstExprFunction* func, uint8_t target, uint8_t targetCount, bool multRet, int thresholdBase,
 int thresholdMaxBoost, int depthLimit)
 {
 Function* fi = functions.find(func);
 LUAU_ASSERT(fi);
 if (regTop > 128 || fi->stackSize > 32)
 {
 bytecode.addDebugRemark("inlining failed: high register pressure");
 return false;
 }
 if (int(inlineFrames.size()) >= depthLimit)
 {
 bytecode.addDebugRemark("inlining failed: too many inlined frames");
 return false;
 }
 for (InlineFrame& frame : inlineFrames)
 if (frame.func == func)
 {
 bytecode.addDebugRemark("inlining failed: can't inline recursive calls");
 return false;
 }
 if (multRet)
 {
 bytecode.addDebugRemark("inlining failed: can't convert fixed returns to multret");
 return false;
 }
 bool varc[8] = {};
 for (size_t i = 0; i < func->args.size && i < expr->args.size && i < 8; ++i)
 varc[i] = isConstant(expr->args.data[i]);
 if (expr->args.size != 0 && !isExprMultRet(expr->args.data[expr->args.size - 1]))
 for (size_t i = expr->args.size; i < func->args.size && i < 8; ++i)
 varc[i] = true;
 int inlinedCost = computeCost(fi->costModel, varc, std::min(int(func->args.size), 8));
 int baselineCost = computeCost(fi->costModel, nullptr, 0) + 3;
 int inlineProfit = (inlinedCost == 0) ? thresholdMaxBoost : std::min(thresholdMaxBoost, 100 * baselineCost / inlinedCost);
 int threshold = thresholdBase * inlineProfit / 100;
 if (inlinedCost > threshold)
 {
 bytecode.addDebugRemark("inlining failed: too expensive (cost %d, profit %.2fx)", inlinedCost, double(inlineProfit) / 100);
 return false;
 }
 bytecode.addDebugRemark(
 "inlining succeeded (cost %d, profit %.2fx, depth %d)", inlinedCost, double(inlineProfit) / 100, int(inlineFrames.size()));
 compileInlinedCall(expr, func, target, targetCount);
 return true;
 }
 void compileInlinedCall(AstExprCall* expr, AstExprFunction* func, uint8_t target, uint8_t targetCount)
 {
 RegScope rs(this);
 size_t oldLocals = localStack.size();
 std::vector<InlineArg> args;
 if (FFlag::LuauCompileInlineDefer)
 {
 args.reserve(func->args.size);
 }
 else
 {
 inlineFrames.push_back({func, oldLocals, target, targetCount});
 }
 for (size_t i = 0; i < func->args.size; ++i)
 {
 AstLocal* var = func->args.data[i];
 AstExpr* arg = i < expr->args.size ? expr->args.data[i] : nullptr;
 if (i + 1 == expr->args.size && func->args.size > expr->args.size && isExprMultRet(arg))
 {
 unsigned int tail = unsigned(func->args.size - expr->args.size) + 1;
 uint8_t reg = allocReg(arg, tail);
 if (AstExprCall* expr = arg->as<AstExprCall>())
 compileExprCall(expr, reg, tail, true);
 else if (AstExprVarargs* expr = arg->as<AstExprVarargs>())
 compileExprVarargs(expr, reg, tail);
 else
 LUAU_ASSERT(!"Unexpected expression type");
 if (FFlag::LuauCompileInlineDefer)
 {
 for (size_t j = i; j < func->args.size; ++j)
 args.push_back({func->args.data[j], uint8_t(reg + (j - i))});
 }
 else
 {
 for (size_t j = i; j < func->args.size; ++j)
 pushLocal(func->args.data[j], uint8_t(reg + (j - i)));
 }
 break;
 }
 else if (Variable* vv = variables.find(var); vv && vv->written)
 {
 uint8_t reg = allocReg(arg, 1);
 if (arg)
 compileExprTemp(arg, reg);
 else
 bytecode.emitABC(LOP_LOADNIL, reg, 0, 0);
 if (FFlag::LuauCompileInlineDefer)
 args.push_back({var, reg});
 else
 pushLocal(var, reg);
 }
 else if (arg == nullptr)
 {
 if (FFlag::LuauCompileInlineDefer)
 args.push_back({var, kInvalidReg, {Constant::Type_Nil}});
 else
 locstants[var] = {Constant::Type_Nil};
 }
 else if (const Constant* cv = constants.find(arg); cv && cv->type != Constant::Type_Unknown)
 {
 if (FFlag::LuauCompileInlineDefer)
 args.push_back({var, kInvalidReg, *cv});
 else
 locstants[var] = *cv;
 }
 else
 {
 AstExprLocal* le = getExprLocal(arg);
 Variable* lv = le ? variables.find(le->local) : nullptr;
 if (int reg = le ? getExprLocalReg(le) : -1; reg >= 0 && (!lv || !lv->written))
 {
 if (FFlag::LuauCompileInlineDefer)
 args.push_back({var, uint8_t(reg)});
 else
 pushLocal(var, uint8_t(reg));
 }
 else
 {
 uint8_t temp = allocReg(arg, 1);
 compileExprTemp(arg, temp);
 if (FFlag::LuauCompileInlineDefer)
 args.push_back({var, temp});
 else
 pushLocal(var, temp);
 }
 }
 }
 for (size_t i = func->args.size; i < expr->args.size; ++i)
 {
 RegScope rsi(this);
 compileExprAuto(expr->args.data[i], rsi);
 }
 if (FFlag::LuauCompileInlineDefer)
 {
 for (InlineArg& arg : args)
 if (arg.value.type == Constant::Type_Unknown)
 pushLocal(arg.local, arg.reg);
 else
 locstants[arg.local] = arg.value;
 inlineFrames.push_back({func, oldLocals, target, targetCount});
 }
 foldConstants(constants, variables, locstants, builtinsFold, func->body);
 bool usedFallthrough = false;
 for (size_t i = 0; i < func->body->body.size; ++i)
 {
 AstStat* stat = func->body->body.data[i];
 if (AstStatReturn* ret = stat->as<AstStatReturn>())
 {
 compileInlineReturn(ret, true);
 usedFallthrough = true;
 break;
 }
 else
 compileStat(stat);
 }
 if (!usedFallthrough && !alwaysTerminates(func->body))
 {
 for (size_t i = 0; i < targetCount; ++i)
 bytecode.emitABC(LOP_LOADNIL, uint8_t(target + i), 0, 0);
 closeLocals(oldLocals);
 }
 popLocals(oldLocals);
 size_t returnLabel = bytecode.emitLabel();
 patchJumps(expr, inlineFrames.back().returnJumps, returnLabel);
 inlineFrames.pop_back();
 for (size_t i = 0; i < func->args.size; ++i)
 if (Constant* var = locstants.find(func->args.data[i]))
 var->type = Constant::Type_Unknown;
 foldConstants(constants, variables, locstants, builtinsFold, func->body);
 }
 void compileExprCall(AstExprCall* expr, uint8_t target, uint8_t targetCount, bool targetTop = false, bool multRet = false)
 {
 LUAU_ASSERT(!targetTop || unsigned(target + targetCount) == regTop);
 setDebugLine(expr);
 if (options.optimizationLevel >= 2 && !expr->self)
 {
 AstExprFunction* func = getFunctionExpr(expr->func);
 Function* fi = func ? functions.find(func) : nullptr;
 if (fi && fi->canInline &&
 tryCompileInlinedCall(expr, func, target, targetCount, multRet, FInt::LuauCompileInlineThreshold,
 FInt::LuauCompileInlineThresholdMaxBoost, FInt::LuauCompileInlineDepth))
 return;
 if (func && !(fi && fi->canInline))
 {
 if (func->vararg)
 bytecode.addDebugRemark("inlining failed: function is variadic");
 else if (!fi)
 bytecode.addDebugRemark("inlining failed: can't inline recursive calls");
 else if (getfenvUsed || setfenvUsed)
 bytecode.addDebugRemark("inlining failed: module uses getfenv/setfenv");
 }
 }
 RegScope rs(this);
 unsigned int regCount = std::max(unsigned(1 + expr->self + expr->args.size), unsigned(targetCount));
 uint8_t regs = targetTop ? allocReg(expr, regCount - targetCount) - targetCount : allocReg(expr, regCount);
 uint8_t selfreg = 0;
 int bfid = -1;
 if (options.optimizationLevel >= 1 && !expr->self)
 if (const int* id = builtins.find(expr))
 bfid = *id;
 if (bfid >= 0 && bytecode.needsDebugRemarks())
 {
 Builtin builtin = getBuiltin(expr->func, globals, variables);
 bool lastMult = expr->args.size > 0 && isExprMultRet(expr->args.data[expr->args.size - 1]);
 if (builtin.object.value)
 bytecode.addDebugRemark("builtin %s.%s/%d%s", builtin.object.value, builtin.method.value, int(expr->args.size), lastMult ? "+" : "");
 else if (builtin.method.value)
 bytecode.addDebugRemark("builtin %s/%d%s", builtin.method.value, int(expr->args.size), lastMult ? "+" : "");
 }
 if (bfid == LBF_SELECT_VARARG)
 {
 if (multRet == false && targetCount == 1)
 return compileExprSelectVararg(expr, target, targetCount, targetTop, multRet, regs);
 else
 bfid = -1;
 }
 if (bfid == LBF_BIT32_EXTRACT && expr->args.size == 3 && isConstant(expr->args.data[1]) && isConstant(expr->args.data[2]))
 {
 Constant fc = getConstant(expr->args.data[1]);
 Constant wc = getConstant(expr->args.data[2]);
 int fi = fc.type == Constant::Type_Number ? int(fc.valueNumber) : -1;
 int wi = wc.type == Constant::Type_Number ? int(wc.valueNumber) : -1;
 if (fi >= 0 && wi > 0 && fi + wi <= 32)
 {
 int fwp = fi | ((wi - 1) << 5);
 int32_t cid = bytecode.addConstantNumber(fwp);
 if (cid < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 return compileExprFastcallN(expr, target, targetCount, targetTop, multRet, regs, LBF_BIT32_EXTRACTK, cid);
 }
 }
 if (bfid >= 0 && expr->args.size >= 1 && expr->args.size <= 2)
 {
 if (!isExprMultRet(expr->args.data[expr->args.size - 1]))
 return compileExprFastcallN(expr, target, targetCount, targetTop, multRet, regs, bfid);
 else if (options.optimizationLevel >= 2 && int(expr->args.size) == getBuiltinInfo(bfid).params)
 return compileExprFastcallN(expr, target, targetCount, targetTop, multRet, regs, bfid);
 }
 if (expr->self)
 {
 AstExprIndexName* fi = expr->func->as<AstExprIndexName>();
 LUAU_ASSERT(fi);
 if (int reg = getExprLocalReg(fi->expr); reg >= 0)
 {
 selfreg = uint8_t(reg);
 }
 else
 {
 selfreg = regs;
 compileExprTempTop(fi->expr, selfreg);
 }
 }
 else if (bfid < 0)
 {
 compileExprTempTop(expr->func, regs);
 }
 bool multCall = false;
 for (size_t i = 0; i < expr->args.size; ++i)
 if (i + 1 == expr->args.size)
 multCall = compileExprTempMultRet(expr->args.data[i], uint8_t(regs + 1 + expr->self + i));
 else
 compileExprTempTop(expr->args.data[i], uint8_t(regs + 1 + expr->self + i));
 setDebugLineEnd(expr->func);
 if (expr->self)
 {
 AstExprIndexName* fi = expr->func->as<AstExprIndexName>();
 LUAU_ASSERT(fi);
 setDebugLine(fi->indexLocation);
 BytecodeBuilder::StringRef iname = sref(fi->index);
 int32_t cid = bytecode.addConstantString(iname);
 if (cid < 0)
 CompileError::raise(fi->location, "Exceeded constant limit; simplify the code to compile");
 bytecode.emitABC(LOP_NAMECALL, regs, selfreg, uint8_t(BytecodeBuilder::getStringHash(iname)));
 bytecode.emitAux(cid);
 }
 else if (bfid >= 0)
 {
 size_t fastcallLabel = bytecode.emitLabel();
 bytecode.emitABC(LOP_FASTCALL, uint8_t(bfid), 0, 0);
 compileExprTemp(expr->func, regs);
 size_t callLabel = bytecode.emitLabel();
 if (!bytecode.patchSkipC(fastcallLabel, callLabel))
 CompileError::raise(expr->func->location, "Exceeded jump distance limit; simplify the code to compile");
 }
 bytecode.emitABC(LOP_CALL, regs, multCall ? 0 : uint8_t(expr->self + expr->args.size + 1), multRet ? 0 : uint8_t(targetCount + 1));
 if (!targetTop)
 {
 for (size_t i = 0; i < targetCount; ++i)
 bytecode.emitABC(LOP_MOVE, uint8_t(target + i), uint8_t(regs + i), 0);
 }
 }
 bool shouldShareClosure(AstExprFunction* func)
 {
 const Function* f = functions.find(func);
 if (!f)
 return false;
 for (AstLocal* uv : f->upvals)
 {
 Variable* ul = variables.find(uv);
 if (!ul)
 return false;
 if (ul->written)
 return false;
 if (uv->functionDepth != 0 || uv->loopDepth != 0)
 {
 AstExprFunction* uf = ul->init ? ul->init->as<AstExprFunction>() : nullptr;
 if (!uf)
 return false;
 if (uf != func && !shouldShareClosure(uf))
 return false;
 }
 }
 return true;
 }
 void compileExprFunction(AstExprFunction* expr, uint8_t target)
 {
 RegScope rs(this);
 const Function* f = functions.find(expr);
 LUAU_ASSERT(f);
 int16_t pid = bytecode.addChildFunction(f->id);
 if (pid < 0)
 CompileError::raise(expr->location, "Exceeded closure limit; simplify the code to compile");
 captures.clear();
 captures.reserve(f->upvals.size());
 for (AstLocal* uv : f->upvals)
 {
 LUAU_ASSERT(uv->functionDepth < expr->functionDepth);
 if (int reg = getLocalReg(uv); reg >= 0)
 {
 Variable* ul = variables.find(uv);
 bool immutable = !ul || !ul->written;
 captures.push_back({immutable ? LCT_VAL : LCT_REF, uint8_t(reg)});
 }
 else if (const Constant* uc = locstants.find(uv); uc && uc->type != Constant::Type_Unknown)
 {
 uint8_t reg = allocReg(expr, 1);
 compileExprConstant(expr, uc, reg);
 captures.push_back({LCT_VAL, reg});
 }
 else
 {
 LUAU_ASSERT(uv->functionDepth < expr->functionDepth - 1);
 uint8_t uid = getUpval(uv);
 captures.push_back({LCT_UPVAL, uid});
 }
 }
 int16_t shared = -1;
 if (options.optimizationLevel >= 1 && shouldShareClosure(expr) && !setfenvUsed)
 {
 int32_t cid = bytecode.addConstantClosure(f->id);
 if (cid >= 0 && cid < 32768)
 shared = int16_t(cid);
 }
 if (shared < 0)
 bytecode.addDebugRemark("allocation: closure with %d upvalues", int(captures.size()));
 if (shared >= 0)
 bytecode.emitAD(LOP_DUPCLOSURE, target, shared);
 else
 bytecode.emitAD(LOP_NEWCLOSURE, target, pid);
 for (const Capture& c : captures)
 bytecode.emitABC(LOP_CAPTURE, uint8_t(c.type), c.data, 0);
 }
 LuauOpcode getUnaryOp(AstExprUnary::Op op)
 {
 switch (op)
 {
 case AstExprUnary::Not:
 return LOP_NOT;
 case AstExprUnary::Minus:
 return LOP_MINUS;
 case AstExprUnary::Len:
 return LOP_LENGTH;
 default:
 LUAU_ASSERT(!"Unexpected unary operation");
 return LOP_NOP;
 }
 }
 LuauOpcode getBinaryOpArith(AstExprBinary::Op op, bool k = false)
 {
 switch (op)
 {
 case AstExprBinary::Add:
 return k ? LOP_ADDK : LOP_ADD;
 case AstExprBinary::Sub:
 return k ? LOP_SUBK : LOP_SUB;
 case AstExprBinary::Mul:
 return k ? LOP_MULK : LOP_MUL;
 case AstExprBinary::Div:
 return k ? LOP_DIVK : LOP_DIV;
 case AstExprBinary::Mod:
 return k ? LOP_MODK : LOP_MOD;
 case AstExprBinary::Pow:
 return k ? LOP_POWK : LOP_POW;
 default:
 LUAU_ASSERT(!"Unexpected binary operation");
 return LOP_NOP;
 }
 }
 LuauOpcode getJumpOpCompare(AstExprBinary::Op op, bool not_ = false)
 {
 switch (op)
 {
 case AstExprBinary::CompareNe:
 return not_ ? LOP_JUMPIFEQ : LOP_JUMPIFNOTEQ;
 case AstExprBinary::CompareEq:
 return not_ ? LOP_JUMPIFNOTEQ : LOP_JUMPIFEQ;
 case AstExprBinary::CompareLt:
 case AstExprBinary::CompareGt:
 return not_ ? LOP_JUMPIFNOTLT : LOP_JUMPIFLT;
 case AstExprBinary::CompareLe:
 case AstExprBinary::CompareGe:
 return not_ ? LOP_JUMPIFNOTLE : LOP_JUMPIFLE;
 default:
 LUAU_ASSERT(!"Unexpected binary operation");
 return LOP_NOP;
 }
 }
 bool isConstant(AstExpr* node)
 {
 const Constant* cv = constants.find(node);
 return cv && cv->type != Constant::Type_Unknown;
 }
 bool isConstantTrue(AstExpr* node)
 {
 const Constant* cv = constants.find(node);
 return cv && cv->type != Constant::Type_Unknown && cv->isTruthful();
 }
 bool isConstantFalse(AstExpr* node)
 {
 const Constant* cv = constants.find(node);
 return cv && cv->type != Constant::Type_Unknown && !cv->isTruthful();
 }
 Constant getConstant(AstExpr* node)
 {
 const Constant* cv = constants.find(node);
 return cv ? *cv : Constant{Constant::Type_Unknown};
 }
 size_t compileCompareJump(AstExprBinary* expr, bool not_ = false)
 {
 RegScope rs(this);
 bool isEq = (expr->op == AstExprBinary::CompareEq || expr->op == AstExprBinary::CompareNe);
 AstExpr* left = expr->left;
 AstExpr* right = expr->right;
 bool operandIsConstant = isConstant(right);
 if (isEq && !operandIsConstant)
 {
 operandIsConstant = isConstant(left);
 if (operandIsConstant)
 std::swap(left, right);
 }
 uint8_t rl = compileExprAuto(left, rs);
 if (isEq && operandIsConstant)
 {
 const Constant* cv = constants.find(right);
 LUAU_ASSERT(cv && cv->type != Constant::Type_Unknown);
 LuauOpcode opc = LOP_NOP;
 int32_t cid = -1;
 uint32_t flip = (expr->op == AstExprBinary::CompareEq) == not_ ? 0x80000000 : 0;
 switch (cv->type)
 {
 case Constant::Type_Nil:
 opc = LOP_JUMPXEQKNIL;
 cid = 0;
 break;
 case Constant::Type_Boolean:
 opc = LOP_JUMPXEQKB;
 cid = cv->valueBoolean;
 break;
 case Constant::Type_Number:
 opc = LOP_JUMPXEQKN;
 cid = getConstantIndex(right);
 break;
 case Constant::Type_String:
 opc = LOP_JUMPXEQKS;
 cid = getConstantIndex(right);
 break;
 default:
 LUAU_ASSERT(!"Unexpected constant type");
 }
 if (cid < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 size_t jumpLabel = bytecode.emitLabel();
 bytecode.emitAD(opc, rl, 0);
 bytecode.emitAux(cid | flip);
 return jumpLabel;
 }
 else
 {
 LuauOpcode opc = getJumpOpCompare(expr->op, not_);
 uint8_t rr = compileExprAuto(right, rs);
 size_t jumpLabel = bytecode.emitLabel();
 if (expr->op == AstExprBinary::CompareGt || expr->op == AstExprBinary::CompareGe)
 {
 bytecode.emitAD(opc, rr, 0);
 bytecode.emitAux(rl);
 }
 else
 {
 bytecode.emitAD(opc, rl, 0);
 bytecode.emitAux(rr);
 }
 return jumpLabel;
 }
 }
 int32_t getConstantNumber(AstExpr* node)
 {
 const Constant* c = constants.find(node);
 if (c && c->type == Constant::Type_Number)
 {
 int cid = bytecode.addConstantNumber(c->valueNumber);
 if (cid < 0)
 CompileError::raise(node->location, "Exceeded constant limit; simplify the code to compile");
 return cid;
 }
 return -1;
 }
 int32_t getConstantIndex(AstExpr* node)
 {
 const Constant* c = constants.find(node);
 if (!c || c->type == Constant::Type_Unknown)
 return -1;
 int cid = -1;
 switch (c->type)
 {
 case Constant::Type_Nil:
 cid = bytecode.addConstantNil();
 break;
 case Constant::Type_Boolean:
 cid = bytecode.addConstantBoolean(c->valueBoolean);
 break;
 case Constant::Type_Number:
 cid = bytecode.addConstantNumber(c->valueNumber);
 break;
 case Constant::Type_String:
 cid = bytecode.addConstantString(sref(c->getString()));
 break;
 default:
 LUAU_ASSERT(!"Unexpected constant type");
 return -1;
 }
 if (cid < 0)
 CompileError::raise(node->location, "Exceeded constant limit; simplify the code to compile");
 return cid;
 }
 void compileConditionValue(AstExpr* node, const uint8_t* target, std::vector<size_t>& skipJump, bool onlyTruth)
 {
 if (const Constant* cv = constants.find(node); cv && cv->type != Constant::Type_Unknown)
 {
 if (cv->isTruthful() == onlyTruth)
 {
 if (target)
 compileExprTemp(node, *target);
 skipJump.push_back(bytecode.emitLabel());
 bytecode.emitAD(LOP_JUMP, 0, 0);
 }
 return;
 }
 if (AstExprBinary* expr = node->as<AstExprBinary>())
 {
 switch (expr->op)
 {
 case AstExprBinary::And:
 case AstExprBinary::Or:
 {
 if (onlyTruth == (expr->op == AstExprBinary::And))
 {
 std::vector<size_t> elseJump;
 compileConditionValue(expr->left, nullptr, elseJump, !onlyTruth);
 compileConditionValue(expr->right, target, skipJump, onlyTruth);
 size_t elseLabel = bytecode.emitLabel();
 patchJumps(expr, elseJump, elseLabel);
 }
 else
 {
 compileConditionValue(expr->left, target, skipJump, onlyTruth);
 compileConditionValue(expr->right, target, skipJump, onlyTruth);
 }
 return;
 }
 break;
 case AstExprBinary::CompareNe:
 case AstExprBinary::CompareEq:
 case AstExprBinary::CompareLt:
 case AstExprBinary::CompareLe:
 case AstExprBinary::CompareGt:
 case AstExprBinary::CompareGe:
 {
 if (target)
 {
 bytecode.emitABC(LOP_LOADB, *target, onlyTruth ? 1 : 0, 0);
 }
 size_t jumpLabel = compileCompareJump(expr, !onlyTruth);
 skipJump.push_back(jumpLabel);
 return;
 }
 break;
 default:;
 }
 }
 if (AstExprUnary* expr = node->as<AstExprUnary>())
 {
 if (!target && expr->op == AstExprUnary::Not)
 {
 compileConditionValue(expr->expr, target, skipJump, !onlyTruth);
 return;
 }
 }
 if (AstExprGroup* expr = node->as<AstExprGroup>())
 {
 compileConditionValue(expr->expr, target, skipJump, onlyTruth);
 return;
 }
 RegScope rs(this);
 uint8_t reg;
 if (target)
 {
 reg = *target;
 compileExprTemp(node, reg);
 }
 else
 {
 reg = compileExprAuto(node, rs);
 }
 skipJump.push_back(bytecode.emitLabel());
 bytecode.emitAD(onlyTruth ? LOP_JUMPIF : LOP_JUMPIFNOT, reg, 0);
 }
 bool isConditionFast(AstExpr* node)
 {
 const Constant* cv = constants.find(node);
 if (cv && cv->type != Constant::Type_Unknown)
 return true;
 if (AstExprBinary* expr = node->as<AstExprBinary>())
 {
 switch (expr->op)
 {
 case AstExprBinary::And:
 case AstExprBinary::Or:
 return true;
 case AstExprBinary::CompareNe:
 case AstExprBinary::CompareEq:
 case AstExprBinary::CompareLt:
 case AstExprBinary::CompareLe:
 case AstExprBinary::CompareGt:
 case AstExprBinary::CompareGe:
 return true;
 default:
 return false;
 }
 }
 if (AstExprGroup* expr = node->as<AstExprGroup>())
 return isConditionFast(expr->expr);
 return false;
 }
 void compileExprAndOr(AstExprBinary* expr, uint8_t target, bool targetTemp)
 {
 bool and_ = (expr->op == AstExprBinary::And);
 RegScope rs(this);
 if (const Constant* cl = constants.find(expr->left); cl && cl->type != Constant::Type_Unknown)
 {
 compileExpr(and_ == cl->isTruthful() ? expr->right : expr->left, target, targetTemp);
 return;
 }
 if (!isConditionFast(expr->left))
 {
 if (int reg = getExprLocalReg(expr->right); reg >= 0)
 {
 uint8_t lr = compileExprAuto(expr->left, rs);
 uint8_t rr = uint8_t(reg);
 bytecode.emitABC(and_ ? LOP_AND : LOP_OR, target, lr, rr);
 return;
 }
 int32_t cid = getConstantIndex(expr->right);
 if (cid >= 0 && cid <= 255)
 {
 uint8_t lr = compileExprAuto(expr->left, rs);
 bytecode.emitABC(and_ ? LOP_ANDK : LOP_ORK, target, lr, uint8_t(cid));
 return;
 }
 }
 uint8_t reg = targetTemp ? target : allocReg(expr, 1);
 std::vector<size_t> skipJump;
 compileConditionValue(expr->left, &reg, skipJump, !and_);
 compileExprTemp(expr->right, reg);
 size_t moveLabel = bytecode.emitLabel();
 patchJumps(expr, skipJump, moveLabel);
 if (target != reg)
 bytecode.emitABC(LOP_MOVE, target, reg, 0);
 }
 void compileExprUnary(AstExprUnary* expr, uint8_t target)
 {
 RegScope rs(this);
 uint8_t re = compileExprAuto(expr->expr, rs);
 bytecode.emitABC(getUnaryOp(expr->op), target, re, 0);
 }
 static void unrollConcats(std::vector<AstExpr*>& args)
 {
 for (;;)
 {
 AstExprBinary* be = args.back()->as<AstExprBinary>();
 if (!be || be->op != AstExprBinary::Concat)
 break;
 args.back() = be->left;
 args.push_back(be->right);
 }
 }
 void compileExprBinary(AstExprBinary* expr, uint8_t target, bool targetTemp)
 {
 RegScope rs(this);
 switch (expr->op)
 {
 case AstExprBinary::Add:
 case AstExprBinary::Sub:
 case AstExprBinary::Mul:
 case AstExprBinary::Div:
 case AstExprBinary::Mod:
 case AstExprBinary::Pow:
 {
 int32_t rc = getConstantNumber(expr->right);
 if (rc >= 0 && rc <= 255)
 {
 uint8_t rl = compileExprAuto(expr->left, rs);
 bytecode.emitABC(getBinaryOpArith(expr->op, true), target, rl, uint8_t(rc));
 }
 else
 {
 uint8_t rl = compileExprAuto(expr->left, rs);
 uint8_t rr = compileExprAuto(expr->right, rs);
 bytecode.emitABC(getBinaryOpArith(expr->op), target, rl, rr);
 }
 }
 break;
 case AstExprBinary::Concat:
 {
 std::vector<AstExpr*> args = {expr->left, expr->right};
 unrollConcats(args);
 uint8_t regs = allocReg(expr, unsigned(args.size()));
 for (size_t i = 0; i < args.size(); ++i)
 compileExprTemp(args[i], uint8_t(regs + i));
 bytecode.emitABC(LOP_CONCAT, target, regs, uint8_t(regs + args.size() - 1));
 }
 break;
 case AstExprBinary::CompareNe:
 case AstExprBinary::CompareEq:
 case AstExprBinary::CompareLt:
 case AstExprBinary::CompareLe:
 case AstExprBinary::CompareGt:
 case AstExprBinary::CompareGe:
 {
 size_t jumpLabel = compileCompareJump(expr);
 bytecode.emitABC(LOP_LOADB, target, 0, 1);
 size_t thenLabel = bytecode.emitLabel();
 bytecode.emitABC(LOP_LOADB, target, 1, 0);
 patchJump(expr, jumpLabel, thenLabel);
 }
 break;
 case AstExprBinary::And:
 case AstExprBinary::Or:
 {
 compileExprAndOr(expr, target, targetTemp);
 }
 break;
 default:
 LUAU_ASSERT(!"Unexpected binary operation");
 }
 }
 void compileExprIfElse(AstExprIfElse* expr, uint8_t target, bool targetTemp)
 {
 if (isConstant(expr->condition))
 {
 if (isConstantTrue(expr->condition))
 {
 compileExpr(expr->trueExpr, target, targetTemp);
 }
 else
 {
 compileExpr(expr->falseExpr, target, targetTemp);
 }
 }
 else
 {
 std::vector<size_t> elseJump;
 compileConditionValue(expr->condition, nullptr, elseJump, false);
 compileExpr(expr->trueExpr, target, targetTemp);
 size_t thenLabel = bytecode.emitLabel();
 bytecode.emitAD(LOP_JUMP, 0, 0);
 size_t elseLabel = bytecode.emitLabel();
 compileExpr(expr->falseExpr, target, targetTemp);
 size_t endLabel = bytecode.emitLabel();
 patchJumps(expr, elseJump, elseLabel);
 patchJump(expr, thenLabel, endLabel);
 }
 }
 void compileExprInterpString(AstExprInterpString* expr, uint8_t target, bool targetTemp)
 {
 size_t formatCapacity = 0;
 for (AstArray<char> string : expr->strings)
 {
 formatCapacity += string.size + std::count(string.data, string.data + string.size, '%');
 }
 std::string formatString;
 formatString.reserve(formatCapacity);
 size_t stringsLeft = expr->strings.size;
 for (AstArray<char> string : expr->strings)
 {
 if (memchr(string.data, '%', string.size))
 {
 for (size_t characterIndex = 0; characterIndex < string.size; ++characterIndex)
 {
 char character = string.data[characterIndex];
 formatString.push_back(character);
 if (character == '%')
 formatString.push_back('%');
 }
 }
 else
 formatString.append(string.data, string.size);
 stringsLeft--;
 if (stringsLeft > 0)
 formatString += "%*";
 }
 size_t formatStringSize = formatString.size();
 std::unique_ptr<char[]> formatStringPtr(new char[formatStringSize]);
 memcpy(formatStringPtr.get(), formatString.data(), formatStringSize);
 AstArray<char> formatStringArray{formatStringPtr.get(), formatStringSize};
 interpStrings.emplace_back(std::move(formatStringPtr));
 int32_t formatStringIndex = bytecode.addConstantString(sref(formatStringArray));
 if (formatStringIndex < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 RegScope rs(this);
 uint8_t baseReg = allocReg(expr, unsigned(2 + expr->expressions.size));
 emitLoadK(baseReg, formatStringIndex);
 for (size_t index = 0; index < expr->expressions.size; ++index)
 compileExprTempTop(expr->expressions.data[index], uint8_t(baseReg + 2 + index));
 BytecodeBuilder::StringRef formatMethod = sref(AstName("format"));
 int32_t formatMethodIndex = bytecode.addConstantString(formatMethod);
 if (formatMethodIndex < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 bytecode.emitABC(LOP_NAMECALL, baseReg, baseReg, uint8_t(BytecodeBuilder::getStringHash(formatMethod)));
 bytecode.emitAux(formatMethodIndex);
 bytecode.emitABC(LOP_CALL, baseReg, uint8_t(expr->expressions.size + 2), 2);
 bytecode.emitABC(LOP_MOVE, target, baseReg, 0);
 }
 static uint8_t encodeHashSize(unsigned int hashSize)
 {
 size_t hashSizeLog2 = 0;
 while ((1u << hashSizeLog2) < hashSize)
 hashSizeLog2++;
 return hashSize == 0 ? 0 : uint8_t(hashSizeLog2 + 1);
 }
 void compileExprTable(AstExprTable* expr, uint8_t target, bool targetTemp)
 {
 if (expr->items.size == 0)
 {
 TableShape shape = tableShapes[expr];
 bytecode.addDebugRemark("allocation: table hash %d", shape.hashSize);
 bytecode.emitABC(LOP_NEWTABLE, target, encodeHashSize(shape.hashSize), 0);
 bytecode.emitAux(shape.arraySize);
 return;
 }
 unsigned int arraySize = 0;
 unsigned int hashSize = 0;
 unsigned int recordSize = 0;
 unsigned int indexSize = 0;
 for (size_t i = 0; i < expr->items.size; ++i)
 {
 const AstExprTable::Item& item = expr->items.data[i];
 arraySize += (item.kind == AstExprTable::Item::List);
 hashSize += (item.kind != AstExprTable::Item::List);
 recordSize += (item.kind == AstExprTable::Item::Record);
 }
 if (arraySize == 0 && hashSize > 0)
 {
 for (size_t i = 0; i < expr->items.size; ++i)
 {
 const AstExprTable::Item& item = expr->items.data[i];
 LUAU_ASSERT(item.key);
 const Constant* ckey = constants.find(item.key);
 indexSize += (ckey && ckey->type == Constant::Type_Number && ckey->valueNumber == double(indexSize + 1));
 }
 if (hashSize == recordSize + indexSize)
 hashSize = recordSize;
 else
 indexSize = 0;
 }
 int encodedHashSize = encodeHashSize(hashSize);
 RegScope rs(this);
 uint8_t reg = targetTemp ? target : allocReg(expr, 1);
 if (arraySize == 0 && indexSize == 0 && hashSize == recordSize && recordSize >= 1 && recordSize <= BytecodeBuilder::TableShape::kMaxLength)
 {
 BytecodeBuilder::TableShape shape;
 for (size_t i = 0; i < expr->items.size; ++i)
 {
 const AstExprTable::Item& item = expr->items.data[i];
 LUAU_ASSERT(item.kind == AstExprTable::Item::Record);
 AstExprConstantString* ckey = item.key->as<AstExprConstantString>();
 LUAU_ASSERT(ckey);
 int cid = bytecode.addConstantString(sref(ckey->value));
 if (cid < 0)
 CompileError::raise(ckey->location, "Exceeded constant limit; simplify the code to compile");
 LUAU_ASSERT(shape.length < BytecodeBuilder::TableShape::kMaxLength);
 shape.keys[shape.length++] = int16_t(cid);
 }
 int32_t tid = bytecode.addConstantTable(shape);
 if (tid < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 bytecode.addDebugRemark("allocation: table template %d", hashSize);
 if (tid < 32768)
 {
 bytecode.emitAD(LOP_DUPTABLE, reg, int16_t(tid));
 }
 else
 {
 bytecode.emitABC(LOP_NEWTABLE, reg, uint8_t(encodedHashSize), 0);
 bytecode.emitAux(0);
 }
 }
 else
 {
 const AstExprTable::Item* last = expr->items.size > 0 ? &expr->items.data[expr->items.size - 1] : nullptr;
 bool trailingVarargs = last && last->kind == AstExprTable::Item::List && last->value->is<AstExprVarargs>();
 LUAU_ASSERT(!trailingVarargs || arraySize > 0);
 unsigned int arrayAllocation = arraySize - trailingVarargs + indexSize;
 if (hashSize == 0)
 bytecode.addDebugRemark("allocation: table array %d", arrayAllocation);
 else if (arrayAllocation == 0)
 bytecode.addDebugRemark("allocation: table hash %d", hashSize);
 else
 bytecode.addDebugRemark("allocation: table hash %d array %d", hashSize, arrayAllocation);
 bytecode.emitABC(LOP_NEWTABLE, reg, uint8_t(encodedHashSize), 0);
 bytecode.emitAux(arrayAllocation);
 }
 unsigned int arrayChunkSize = std::min(16u, arraySize);
 uint8_t arrayChunkReg = allocReg(expr, arrayChunkSize);
 unsigned int arrayChunkCurrent = 0;
 unsigned int arrayIndex = 1;
 bool multRet = false;
 for (size_t i = 0; i < expr->items.size; ++i)
 {
 const AstExprTable::Item& item = expr->items.data[i];
 AstExpr* key = item.key;
 AstExpr* value = item.value;
 setDebugLine(value);
 if (options.coverageLevel >= 2)
 {
 bytecode.emitABC(LOP_COVERAGE, 0, 0, 0);
 }
 if (arrayChunkCurrent > 0 && (key || arrayChunkCurrent == arrayChunkSize))
 {
 bytecode.emitABC(LOP_SETLIST, reg, arrayChunkReg, uint8_t(arrayChunkCurrent + 1));
 bytecode.emitAux(arrayIndex);
 arrayIndex += arrayChunkCurrent;
 arrayChunkCurrent = 0;
 }
 if (key)
 {
 RegScope rsi(this);
 LValue lv = compileLValueIndex(reg, key, rsi);
 uint8_t rv = compileExprAuto(value, rsi);
 compileAssign(lv, rv);
 }
 else
 {
 uint8_t temp = uint8_t(arrayChunkReg + arrayChunkCurrent);
 if (i + 1 == expr->items.size)
 multRet = compileExprTempMultRet(value, temp);
 else
 compileExprTempTop(value, temp);
 arrayChunkCurrent++;
 }
 }
 if (arrayChunkCurrent)
 {
 bytecode.emitABC(LOP_SETLIST, reg, arrayChunkReg, multRet ? 0 : uint8_t(arrayChunkCurrent + 1));
 bytecode.emitAux(arrayIndex);
 }
 if (target != reg)
 bytecode.emitABC(LOP_MOVE, target, reg, 0);
 }
 bool canImport(AstExprGlobal* expr)
 {
 return options.optimizationLevel >= 1 && getGlobalState(globals, expr->name) != Global::Written;
 }
 bool canImportChain(AstExprGlobal* expr)
 {
 return options.optimizationLevel >= 1 && getGlobalState(globals, expr->name) == Global::Default;
 }
 void compileExprIndexName(AstExprIndexName* expr, uint8_t target)
 {
 setDebugLine(expr);
 AstExprGlobal* importRoot = 0;
 AstExprIndexName* import1 = 0;
 AstExprIndexName* import2 = 0;
 if (AstExprIndexName* index = expr->expr->as<AstExprIndexName>())
 {
 importRoot = index->expr->as<AstExprGlobal>();
 import1 = index;
 import2 = expr;
 }
 else
 {
 importRoot = expr->expr->as<AstExprGlobal>();
 import1 = expr;
 }
 if (importRoot && canImportChain(importRoot))
 {
 int32_t id0 = bytecode.addConstantString(sref(importRoot->name));
 int32_t id1 = bytecode.addConstantString(sref(import1->index));
 int32_t id2 = import2 ? bytecode.addConstantString(sref(import2->index)) : -1;
 if (id0 < 0 || id1 < 0 || (import2 && id2 < 0))
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 if (id0 < 1024 && id1 < 1024 && id2 < 1024)
 {
 uint32_t iid = import2 ? BytecodeBuilder::getImportId(id0, id1, id2) : BytecodeBuilder::getImportId(id0, id1);
 int32_t cid = bytecode.addImport(iid);
 if (cid >= 0 && cid < 32768)
 {
 bytecode.emitAD(LOP_GETIMPORT, target, int16_t(cid));
 bytecode.emitAux(iid);
 return;
 }
 }
 }
 RegScope rs(this);
 uint8_t reg = compileExprAuto(expr->expr, rs);
 setDebugLine(expr->indexLocation);
 BytecodeBuilder::StringRef iname = sref(expr->index);
 int32_t cid = bytecode.addConstantString(iname);
 if (cid < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 bytecode.emitABC(LOP_GETTABLEKS, target, reg, uint8_t(BytecodeBuilder::getStringHash(iname)));
 bytecode.emitAux(cid);
 }
 void compileExprIndexExpr(AstExprIndexExpr* expr, uint8_t target)
 {
 RegScope rs(this);
 Constant cv = getConstant(expr->index);
 if (cv.type == Constant::Type_Number && cv.valueNumber >= 1 && cv.valueNumber <= 256 && double(int(cv.valueNumber)) == cv.valueNumber)
 {
 uint8_t i = uint8_t(int(cv.valueNumber) - 1);
 uint8_t rt = compileExprAuto(expr->expr, rs);
 setDebugLine(expr->index);
 bytecode.emitABC(LOP_GETTABLEN, target, rt, i);
 }
 else if (cv.type == Constant::Type_String)
 {
 BytecodeBuilder::StringRef iname = sref(cv.getString());
 int32_t cid = bytecode.addConstantString(iname);
 if (cid < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 uint8_t rt = compileExprAuto(expr->expr, rs);
 setDebugLine(expr->index);
 bytecode.emitABC(LOP_GETTABLEKS, target, rt, uint8_t(BytecodeBuilder::getStringHash(iname)));
 bytecode.emitAux(cid);
 }
 else
 {
 uint8_t rt = compileExprAuto(expr->expr, rs);
 uint8_t ri = compileExprAuto(expr->index, rs);
 bytecode.emitABC(LOP_GETTABLE, target, rt, ri);
 }
 }
 void compileExprGlobal(AstExprGlobal* expr, uint8_t target)
 {
 if (canImport(expr))
 {
 int32_t id0 = bytecode.addConstantString(sref(expr->name));
 if (id0 < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 if (id0 < 1024)
 {
 uint32_t iid = BytecodeBuilder::getImportId(id0);
 int32_t cid = bytecode.addImport(iid);
 if (cid >= 0 && cid < 32768)
 {
 bytecode.emitAD(LOP_GETIMPORT, target, int16_t(cid));
 bytecode.emitAux(iid);
 return;
 }
 }
 }
 BytecodeBuilder::StringRef gname = sref(expr->name);
 int32_t cid = bytecode.addConstantString(gname);
 if (cid < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 bytecode.emitABC(LOP_GETGLOBAL, target, 0, uint8_t(BytecodeBuilder::getStringHash(gname)));
 bytecode.emitAux(cid);
 }
 void compileExprConstant(AstExpr* node, const Constant* cv, uint8_t target)
 {
 switch (cv->type)
 {
 case Constant::Type_Nil:
 bytecode.emitABC(LOP_LOADNIL, target, 0, 0);
 break;
 case Constant::Type_Boolean:
 bytecode.emitABC(LOP_LOADB, target, cv->valueBoolean, 0);
 break;
 case Constant::Type_Number:
 {
 double d = cv->valueNumber;
 if (d >= std::numeric_limits<int16_t>::min() && d <= std::numeric_limits<int16_t>::max() && double(int16_t(d)) == d &&
 !(d == 0.0 && signbit(d)))
 {
 bytecode.emitAD(LOP_LOADN, target, int16_t(d));
 }
 else
 {
 int32_t cid = bytecode.addConstantNumber(d);
 if (cid < 0)
 CompileError::raise(node->location, "Exceeded constant limit; simplify the code to compile");
 emitLoadK(target, cid);
 }
 }
 break;
 case Constant::Type_String:
 {
 int32_t cid = bytecode.addConstantString(sref(cv->getString()));
 if (cid < 0)
 CompileError::raise(node->location, "Exceeded constant limit; simplify the code to compile");
 emitLoadK(target, cid);
 }
 break;
 default:
 LUAU_ASSERT(!"Unexpected constant type");
 }
 }
 void compileExpr(AstExpr* node, uint8_t target, bool targetTemp = false)
 {
 setDebugLine(node);
 if (options.coverageLevel >= 2 && needsCoverage(node))
 {
 bytecode.emitABC(LOP_COVERAGE, 0, 0, 0);
 }
 if (const Constant* cv = constants.find(node); cv && cv->type != Constant::Type_Unknown)
 {
 compileExprConstant(node, cv, target);
 return;
 }
 if (AstExprGroup* expr = node->as<AstExprGroup>())
 {
 compileExpr(expr->expr, target, targetTemp);
 }
 else if (node->is<AstExprConstantNil>())
 {
 bytecode.emitABC(LOP_LOADNIL, target, 0, 0);
 }
 else if (AstExprConstantBool* expr = node->as<AstExprConstantBool>())
 {
 bytecode.emitABC(LOP_LOADB, target, expr->value, 0);
 }
 else if (AstExprConstantNumber* expr = node->as<AstExprConstantNumber>())
 {
 int32_t cid = bytecode.addConstantNumber(expr->value);
 if (cid < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 emitLoadK(target, cid);
 }
 else if (AstExprConstantString* expr = node->as<AstExprConstantString>())
 {
 int32_t cid = bytecode.addConstantString(sref(expr->value));
 if (cid < 0)
 CompileError::raise(expr->location, "Exceeded constant limit; simplify the code to compile");
 emitLoadK(target, cid);
 }
 else if (AstExprLocal* expr = node->as<AstExprLocal>())
 {
 if (int reg = getExprLocalReg(expr); reg >= 0)
 {
 if (options.optimizationLevel == 0 || target != reg)
 bytecode.emitABC(LOP_MOVE, target, uint8_t(reg), 0);
 }
 else
 {
 LUAU_ASSERT(expr->upvalue);
 uint8_t uid = getUpval(expr->local);
 bytecode.emitABC(LOP_GETUPVAL, target, uid, 0);
 }
 }
 else if (AstExprGlobal* expr = node->as<AstExprGlobal>())
 {
 compileExprGlobal(expr, target);
 }
 else if (AstExprVarargs* expr = node->as<AstExprVarargs>())
 {
 compileExprVarargs(expr, target, 1);
 }
 else if (AstExprCall* expr = node->as<AstExprCall>())
 {
 if (targetTemp && target == regTop - 1)
 compileExprCall(expr, target, 1, true);
 else
 compileExprCall(expr, target, 1);
 }
 else if (AstExprIndexName* expr = node->as<AstExprIndexName>())
 {
 compileExprIndexName(expr, target);
 }
 else if (AstExprIndexExpr* expr = node->as<AstExprIndexExpr>())
 {
 compileExprIndexExpr(expr, target);
 }
 else if (AstExprFunction* expr = node->as<AstExprFunction>())
 {
 compileExprFunction(expr, target);
 }
 else if (AstExprTable* expr = node->as<AstExprTable>())
 {
 compileExprTable(expr, target, targetTemp);
 }
 else if (AstExprUnary* expr = node->as<AstExprUnary>())
 {
 compileExprUnary(expr, target);
 }
 else if (AstExprBinary* expr = node->as<AstExprBinary>())
 {
 compileExprBinary(expr, target, targetTemp);
 }
 else if (AstExprTypeAssertion* expr = node->as<AstExprTypeAssertion>())
 {
 compileExpr(expr->expr, target, targetTemp);
 }
 else if (AstExprIfElse* expr = node->as<AstExprIfElse>())
 {
 compileExprIfElse(expr, target, targetTemp);
 }
 else if (AstExprInterpString* interpString = node->as<AstExprInterpString>())
 {
 compileExprInterpString(interpString, target, targetTemp);
 }
 else
 {
 LUAU_ASSERT(!"Unknown expression type");
 }
 }
 void compileExprTemp(AstExpr* node, uint8_t target)
 {
 return compileExpr(node, target, true);
 }
 uint8_t compileExprAuto(AstExpr* node, RegScope&)
 {
 if (int reg = getExprLocalReg(node); reg >= 0)
 return uint8_t(reg);
 uint8_t reg = allocReg(node, 1);
 compileExprTemp(node, reg);
 return reg;
 }
 void compileExprTempN(AstExpr* node, uint8_t target, uint8_t targetCount, bool targetTop)
 {
 LUAU_ASSERT(!targetTop || unsigned(target + targetCount) == regTop);
 if (AstExprCall* expr = node->as<AstExprCall>())
 {
 compileExprCall(expr, target, targetCount, targetTop);
 }
 else if (AstExprVarargs* expr = node->as<AstExprVarargs>())
 {
 compileExprVarargs(expr, target, targetCount);
 }
 else
 {
 compileExprTemp(node, target);
 for (size_t i = 1; i < targetCount; ++i)
 bytecode.emitABC(LOP_LOADNIL, uint8_t(target + i), 0, 0);
 }
 }
 void compileExprListTemp(const AstArray<AstExpr*>& list, uint8_t target, uint8_t targetCount, bool targetTop)
 {
 LUAU_ASSERT(!targetTop || unsigned(target + targetCount) == regTop);
 if (list.size == targetCount)
 {
 for (size_t i = 0; i < list.size; ++i)
 compileExprTemp(list.data[i], uint8_t(target + i));
 }
 else if (list.size > targetCount)
 {
 for (size_t i = 0; i < targetCount; ++i)
 compileExprTemp(list.data[i], uint8_t(target + i));
 for (size_t i = targetCount; i < list.size; ++i)
 {
 RegScope rsi(this);
 compileExprAuto(list.data[i], rsi);
 }
 }
 else if (list.size > 0)
 {
 for (size_t i = 0; i < list.size - 1; ++i)
 compileExprTemp(list.data[i], uint8_t(target + i));
 compileExprTempN(list.data[list.size - 1], uint8_t(target + list.size - 1), uint8_t(targetCount - (list.size - 1)), targetTop);
 }
 else
 {
 for (size_t i = 0; i < targetCount; ++i)
 bytecode.emitABC(LOP_LOADNIL, uint8_t(target + i), 0, 0);
 }
 }
 struct LValue
 {
 enum Kind
 {
 Kind_Local,
 Kind_Upvalue,
 Kind_Global,
 Kind_IndexName,
 Kind_IndexNumber,
 Kind_IndexExpr,
 };
 Kind kind;
 uint8_t reg;
 uint8_t upval;
 uint8_t index;
 uint8_t number; // index-1 (0-255) in IndexNumber
 BytecodeBuilder::StringRef name;
 Location location;
 };
 LValue compileLValueIndex(uint8_t reg, AstExpr* index, RegScope& rs)
 {
 Constant cv = getConstant(index);
 if (cv.type == Constant::Type_Number && cv.valueNumber >= 1 && cv.valueNumber <= 256 && double(int(cv.valueNumber)) == cv.valueNumber)
 {
 LValue result = {LValue::Kind_IndexNumber};
 result.reg = reg;
 result.number = uint8_t(int(cv.valueNumber) - 1);
 result.location = index->location;
 return result;
 }
 else if (cv.type == Constant::Type_String)
 {
 LValue result = {LValue::Kind_IndexName};
 result.reg = reg;
 result.name = sref(cv.getString());
 result.location = index->location;
 return result;
 }
 else
 {
 LValue result = {LValue::Kind_IndexExpr};
 result.reg = reg;
 result.index = compileExprAuto(index, rs);
 result.location = index->location;
 return result;
 }
 }
 LValue compileLValue(AstExpr* node, RegScope& rs)
 {
 setDebugLine(node);
 if (AstExprLocal* expr = node->as<AstExprLocal>())
 {
 if (int reg = getExprLocalReg(expr); reg >= 0)
 {
 LValue result = {LValue::Kind_Local};
 result.reg = uint8_t(reg);
 result.location = node->location;
 return result;
 }
 else
 {
 LUAU_ASSERT(expr->upvalue);
 LValue result = {LValue::Kind_Upvalue};
 result.upval = getUpval(expr->local);
 result.location = node->location;
 return result;
 }
 }
 else if (AstExprGlobal* expr = node->as<AstExprGlobal>())
 {
 LValue result = {LValue::Kind_Global};
 result.name = sref(expr->name);
 result.location = node->location;
 return result;
 }
 else if (AstExprIndexName* expr = node->as<AstExprIndexName>())
 {
 LValue result = {LValue::Kind_IndexName};
 result.reg = compileExprAuto(expr->expr, rs);
 result.name = sref(expr->index);
 result.location = node->location;
 return result;
 }
 else if (AstExprIndexExpr* expr = node->as<AstExprIndexExpr>())
 {
 uint8_t reg = compileExprAuto(expr->expr, rs);
 return compileLValueIndex(reg, expr->index, rs);
 }
 else
 {
 LUAU_ASSERT(!"Unknown assignment expression");
 return LValue();
 }
 }
 void compileLValueUse(const LValue& lv, uint8_t reg, bool set)
 {
 setDebugLine(lv.location);
 switch (lv.kind)
 {
 case LValue::Kind_Local:
 if (set)
 bytecode.emitABC(LOP_MOVE, lv.reg, reg, 0);
 else
 bytecode.emitABC(LOP_MOVE, reg, lv.reg, 0);
 break;
 case LValue::Kind_Upvalue:
 bytecode.emitABC(set ? LOP_SETUPVAL : LOP_GETUPVAL, reg, lv.upval, 0);
 break;
 case LValue::Kind_Global:
 {
 int32_t cid = bytecode.addConstantString(lv.name);
 if (cid < 0)
 CompileError::raise(lv.location, "Exceeded constant limit; simplify the code to compile");
 bytecode.emitABC(set ? LOP_SETGLOBAL : LOP_GETGLOBAL, reg, 0, uint8_t(BytecodeBuilder::getStringHash(lv.name)));
 bytecode.emitAux(cid);
 }
 break;
 case LValue::Kind_IndexName:
 {
 int32_t cid = bytecode.addConstantString(lv.name);
 if (cid < 0)
 CompileError::raise(lv.location, "Exceeded constant limit; simplify the code to compile");
 bytecode.emitABC(set ? LOP_SETTABLEKS : LOP_GETTABLEKS, reg, lv.reg, uint8_t(BytecodeBuilder::getStringHash(lv.name)));
 bytecode.emitAux(cid);
 }
 break;
 case LValue::Kind_IndexNumber:
 bytecode.emitABC(set ? LOP_SETTABLEN : LOP_GETTABLEN, reg, lv.reg, lv.number);
 break;
 case LValue::Kind_IndexExpr:
 bytecode.emitABC(set ? LOP_SETTABLE : LOP_GETTABLE, reg, lv.reg, lv.index);
 break;
 default:
 LUAU_ASSERT(!"Unknown lvalue kind");
 }
 }
 void compileAssign(const LValue& lv, uint8_t source)
 {
 compileLValueUse(lv, source, true);
 }
 AstExprLocal* getExprLocal(AstExpr* node)
 {
 if (AstExprLocal* expr = node->as<AstExprLocal>())
 return expr;
 else if (AstExprGroup* expr = node->as<AstExprGroup>())
 return getExprLocal(expr->expr);
 else if (AstExprTypeAssertion* expr = node->as<AstExprTypeAssertion>())
 return getExprLocal(expr->expr);
 else
 return nullptr;
 }
 int getExprLocalReg(AstExpr* node)
 {
 if (AstExprLocal* expr = getExprLocal(node))
 {
 Local* l = locals.find(expr->local);
 return l && l->allocated ? l->reg : -1;
 }
 else
 return -1;
 }
 bool isStatBreak(AstStat* node)
 {
 if (AstStatBlock* stat = node->as<AstStatBlock>())
 return stat->body.size == 1 && stat->body.data[0]->is<AstStatBreak>();
 return node->is<AstStatBreak>();
 }
 AstStatContinue* extractStatContinue(AstStatBlock* block)
 {
 if (block->body.size == 1)
 return block->body.data[0]->as<AstStatContinue>();
 else
 return nullptr;
 }
 void compileStatIf(AstStatIf* stat)
 {
 if (isConstantFalse(stat->condition))
 {
 if (stat->elsebody)
 compileStat(stat->elsebody);
 return;
 }
 if (!stat->elsebody && isStatBreak(stat->thenbody) && !areLocalsCaptured(loops.back().localOffset))
 {
 std::vector<size_t> elseJump;
 compileConditionValue(stat->condition, nullptr, elseJump, true);
 for (size_t jump : elseJump)
 loopJumps.push_back({LoopJump::Break, jump});
 return;
 }
 AstStat* continueStatement = extractStatContinue(stat->thenbody);
 if (!stat->elsebody && continueStatement != nullptr && !areLocalsCaptured(loops.back().localOffset))
 {
 if (loops.back().untilCondition)
 validateContinueUntil(continueStatement, loops.back().untilCondition);
 std::vector<size_t> elseJump;
 compileConditionValue(stat->condition, nullptr, elseJump, true);
 for (size_t jump : elseJump)
 loopJumps.push_back({LoopJump::Continue, jump});
 return;
 }
 std::vector<size_t> elseJump;
 compileConditionValue(stat->condition, nullptr, elseJump, false);
 compileStat(stat->thenbody);
 if (stat->elsebody && elseJump.size() > 0)
 {
 if (alwaysTerminates(stat->thenbody))
 {
 size_t elseLabel = bytecode.emitLabel();
 compileStat(stat->elsebody);
 patchJumps(stat, elseJump, elseLabel);
 }
 else
 {
 size_t thenLabel = bytecode.emitLabel();
 bytecode.emitAD(LOP_JUMP, 0, 0);
 size_t elseLabel = bytecode.emitLabel();
 compileStat(stat->elsebody);
 size_t endLabel = bytecode.emitLabel();
 patchJumps(stat, elseJump, elseLabel);
 patchJump(stat, thenLabel, endLabel);
 }
 }
 else
 {
 size_t endLabel = bytecode.emitLabel();
 patchJumps(stat, elseJump, endLabel);
 }
 }
 void compileStatWhile(AstStatWhile* stat)
 {
 if (isConstantFalse(stat->condition))
 return;
 size_t oldJumps = loopJumps.size();
 size_t oldLocals = localStack.size();
 loops.push_back({oldLocals, nullptr});
 size_t loopLabel = bytecode.emitLabel();
 std::vector<size_t> elseJump;
 compileConditionValue(stat->condition, nullptr, elseJump, false);
 compileStat(stat->body);
 size_t contLabel = bytecode.emitLabel();
 size_t backLabel = bytecode.emitLabel();
 setDebugLine(stat->condition);
 bytecode.emitAD(LOP_JUMPBACK, 0, 0);
 size_t endLabel = bytecode.emitLabel();
 patchJump(stat, backLabel, loopLabel);
 patchJumps(stat, elseJump, endLabel);
 patchLoopJumps(stat, oldJumps, endLabel, contLabel);
 loopJumps.resize(oldJumps);
 loops.pop_back();
 }
 void compileStatRepeat(AstStatRepeat* stat)
 {
 size_t oldJumps = loopJumps.size();
 size_t oldLocals = localStack.size();
 loops.push_back({oldLocals, stat->condition});
 size_t loopLabel = bytecode.emitLabel();
 AstStatBlock* body = stat->body;
 RegScope rs(this);
 for (size_t i = 0; i < body->body.size; ++i)
 compileStat(body->body.data[i]);
 size_t contLabel = bytecode.emitLabel();
 size_t endLabel;
 setDebugLine(stat->condition);
 if (isConstantTrue(stat->condition))
 {
 closeLocals(oldLocals);
 endLabel = bytecode.emitLabel();
 }
 else
 {
 std::vector<size_t> skipJump;
 compileConditionValue(stat->condition, nullptr, skipJump, true);
 closeLocals(oldLocals);
 size_t backLabel = bytecode.emitLabel();
 bytecode.emitAD(LOP_JUMPBACK, 0, 0);
 size_t skipLabel = bytecode.emitLabel();
 closeLocals(oldLocals);
 endLabel = bytecode.emitLabel();
 patchJump(stat, backLabel, loopLabel);
 patchJumps(stat, skipJump, skipLabel);
 }
 popLocals(oldLocals);
 patchLoopJumps(stat, oldJumps, endLabel, contLabel);
 loopJumps.resize(oldJumps);
 loops.pop_back();
 }
 void compileInlineReturn(AstStatReturn* stat, bool fallthrough)
 {
 setDebugLine(stat);
 InlineFrame frame = inlineFrames.back();
 compileExprListTemp(stat->list, frame.target, frame.targetCount, false);
 closeLocals(frame.localOffset);
 if (!fallthrough)
 {
 size_t jumpLabel = bytecode.emitLabel();
 bytecode.emitAD(LOP_JUMP, 0, 0);
 inlineFrames.back().returnJumps.push_back(jumpLabel);
 }
 }
 void compileStatReturn(AstStatReturn* stat)
 {
 RegScope rs(this);
 uint8_t temp = 0;
 bool consecutive = false;
 bool multRet = false;
 if (int reg = stat->list.size > 0 ? getExprLocalReg(stat->list.data[0]) : -1; reg >= 0)
 {
 temp = uint8_t(reg);
 consecutive = true;
 for (size_t i = 1; i < stat->list.size; ++i)
 if (getExprLocalReg(stat->list.data[i]) != int(temp + i))
 {
 consecutive = false;
 break;
 }
 }
 if (!consecutive && stat->list.size > 0)
 {
 temp = allocReg(stat, unsigned(stat->list.size));
 for (size_t i = 0; i < stat->list.size; ++i)
 if (i + 1 == stat->list.size)
 multRet = compileExprTempMultRet(stat->list.data[i], uint8_t(temp + i));
 else
 compileExprTempTop(stat->list.data[i], uint8_t(temp + i));
 }
 closeLocals(0);
 bytecode.emitABC(LOP_RETURN, uint8_t(temp), multRet ? 0 : uint8_t(stat->list.size + 1), 0);
 }
 bool areLocalsRedundant(AstStatLocal* stat)
 {
 if (stat->values.size > stat->vars.size)
 return false;
 for (AstLocal* local : stat->vars)
 {
 Variable* v = variables.find(local);
 if (!v || !v->constant)
 return false;
 }
 return true;
 }
 void compileStatLocal(AstStatLocal* stat)
 {
 if (options.optimizationLevel >= 1 && options.debugLevel <= 1 && areLocalsRedundant(stat))
 return;
 if (options.optimizationLevel >= 1 && stat->vars.size == 1 && stat->values.size == 1)
 {
 if (AstExprLocal* re = getExprLocal(stat->values.data[0]))
 {
 Variable* lv = variables.find(stat->vars.data[0]);
 Variable* rv = variables.find(re->local);
 if (int reg = getExprLocalReg(re); reg >= 0 && (!lv || !lv->written) && (!rv || !rv->written))
 {
 pushLocal(stat->vars.data[0], uint8_t(reg));
 return;
 }
 }
 }
 uint8_t vars = allocReg(stat, unsigned(stat->vars.size));
 compileExprListTemp(stat->values, vars, uint8_t(stat->vars.size), true);
 for (size_t i = 0; i < stat->vars.size; ++i)
 pushLocal(stat->vars.data[i], uint8_t(vars + i));
 }
 bool tryCompileUnrolledFor(AstStatFor* stat, int thresholdBase, int thresholdMaxBoost)
 {
 Constant one = {Constant::Type_Number};
 one.valueNumber = 1.0;
 Constant fromc = getConstant(stat->from);
 Constant toc = getConstant(stat->to);
 Constant stepc = stat->step ? getConstant(stat->step) : one;
 int tripCount = (fromc.type == Constant::Type_Number && toc.type == Constant::Type_Number && stepc.type == Constant::Type_Number)
 ? getTripCount(fromc.valueNumber, toc.valueNumber, stepc.valueNumber)
 : -1;
 if (tripCount < 0)
 {
 bytecode.addDebugRemark("loop unroll failed: invalid iteration count");
 return false;
 }
 if (tripCount > thresholdBase)
 {
 bytecode.addDebugRemark("loop unroll failed: too many iterations (%d)", tripCount);
 return false;
 }
 if (Variable* lv = variables.find(stat->var); lv && lv->written)
 {
 bytecode.addDebugRemark("loop unroll failed: mutable loop variable");
 return false;
 }
 AstLocal* var = stat->var;
 uint64_t costModel = modelCost(stat->body, &var, 1, builtins);
 bool varc = true;
 int unrolledCost = computeCost(costModel, &varc, 1) * tripCount;
 int baselineCost = (computeCost(costModel, nullptr, 0) + 1) * tripCount;
 int unrollProfit = (unrolledCost == 0) ? thresholdMaxBoost : std::min(thresholdMaxBoost, 100 * baselineCost / unrolledCost);
 int threshold = thresholdBase * unrollProfit / 100;
 if (unrolledCost > threshold)
 {
 bytecode.addDebugRemark(
 "loop unroll failed: too expensive (iterations %d, cost %d, profit %.2fx)", tripCount, unrolledCost, double(unrollProfit) / 100);
 return false;
 }
 bytecode.addDebugRemark("loop unroll succeeded (iterations %d, cost %d, profit %.2fx)", tripCount, unrolledCost, double(unrollProfit) / 100);
 compileUnrolledFor(stat, tripCount, fromc.valueNumber, stepc.valueNumber);
 return true;
 }
 void compileUnrolledFor(AstStatFor* stat, int tripCount, double from, double step)
 {
 AstLocal* var = stat->var;
 size_t oldLocals = localStack.size();
 size_t oldJumps = loopJumps.size();
 loops.push_back({oldLocals, nullptr});
 for (int iv = 0; iv < tripCount; ++iv)
 {
 locstants[var].type = Constant::Type_Number;
 locstants[var].valueNumber = from + iv * step;
 foldConstants(constants, variables, locstants, builtinsFold, stat);
 size_t iterJumps = loopJumps.size();
 compileStat(stat->body);
 size_t contLabel = bytecode.emitLabel();
 for (size_t i = iterJumps; i < loopJumps.size(); ++i)
 if (loopJumps[i].type == LoopJump::Continue)
 patchJump(stat, loopJumps[i].label, contLabel);
 }
 size_t endLabel = bytecode.emitLabel();
 for (size_t i = oldJumps; i < loopJumps.size(); ++i)
 if (loopJumps[i].type == LoopJump::Break)
 patchJump(stat, loopJumps[i].label, endLabel);
 loopJumps.resize(oldJumps);
 loops.pop_back();
 locstants[var].type = Constant::Type_Unknown;
 foldConstants(constants, variables, locstants, builtinsFold, stat);
 }
 void compileStatFor(AstStatFor* stat)
 {
 RegScope rs(this);
 if (options.optimizationLevel >= 2 && isConstant(stat->to) && isConstant(stat->from) && (!stat->step || isConstant(stat->step)))
 if (tryCompileUnrolledFor(stat, FInt::LuauCompileLoopUnrollThreshold, FInt::LuauCompileLoopUnrollThresholdMaxBoost))
 return;
 size_t oldLocals = localStack.size();
 size_t oldJumps = loopJumps.size();
 loops.push_back({oldLocals, nullptr});
 uint8_t regs = allocReg(stat, 3);
 uint8_t varreg = regs + 2;
 if (Variable* il = variables.find(stat->var); il && il->written)
 varreg = allocReg(stat, 1);
 compileExprTemp(stat->from, uint8_t(regs + 2));
 compileExprTemp(stat->to, uint8_t(regs + 0));
 if (stat->step)
 compileExprTemp(stat->step, uint8_t(regs + 1));
 else
 bytecode.emitABC(LOP_LOADN, uint8_t(regs + 1), 1, 0);
 size_t forLabel = bytecode.emitLabel();
 bytecode.emitAD(LOP_FORNPREP, regs, 0);
 size_t loopLabel = bytecode.emitLabel();
 if (varreg != regs + 2)
 bytecode.emitABC(LOP_MOVE, varreg, regs + 2, 0);
 pushLocal(stat->var, varreg);
 compileStat(stat->body);
 closeLocals(oldLocals);
 popLocals(oldLocals);
 setDebugLine(stat);
 size_t contLabel = bytecode.emitLabel();
 size_t backLabel = bytecode.emitLabel();
 bytecode.emitAD(LOP_FORNLOOP, regs, 0);
 size_t endLabel = bytecode.emitLabel();
 patchJump(stat, forLabel, endLabel);
 patchJump(stat, backLabel, loopLabel);
 patchLoopJumps(stat, oldJumps, endLabel, contLabel);
 loopJumps.resize(oldJumps);
 loops.pop_back();
 }
 void compileStatForIn(AstStatForIn* stat)
 {
 RegScope rs(this);
 size_t oldLocals = localStack.size();
 size_t oldJumps = loopJumps.size();
 loops.push_back({oldLocals, nullptr});
 uint8_t regs = allocReg(stat, 3);
 compileExprListTemp(stat->values, regs, 3, true);
 uint8_t vars = allocReg(stat, std::max(unsigned(stat->vars.size), 2u));
 LUAU_ASSERT(vars == regs + 3);
 LuauOpcode skipOp = LOP_FORGPREP;
 if (options.optimizationLevel >= 1 && stat->vars.size <= 2)
 {
 if (stat->values.size == 1 && stat->values.data[0]->is<AstExprCall>())
 {
 Builtin builtin = getBuiltin(stat->values.data[0]->as<AstExprCall>()->func, globals, variables);
 if (builtin.isGlobal("ipairs")) // for .. in ipairs(t)
 skipOp = LOP_FORGPREP_INEXT;
 else if (builtin.isGlobal("pairs")) // for .. in pairs(t)
 skipOp = LOP_FORGPREP_NEXT;
 }
 else if (stat->values.size == 2)
 {
 Builtin builtin = getBuiltin(stat->values.data[0], globals, variables);
 if (builtin.isGlobal("next")) // for .. in next,t
 skipOp = LOP_FORGPREP_NEXT;
 }
 }
 size_t skipLabel = bytecode.emitLabel();
 bytecode.emitAD(skipOp, regs, 0);
 size_t loopLabel = bytecode.emitLabel();
 for (size_t i = 0; i < stat->vars.size; ++i)
 pushLocal(stat->vars.data[i], uint8_t(vars + i));
 compileStat(stat->body);
 closeLocals(oldLocals);
 popLocals(oldLocals);
 setDebugLine(stat);
 size_t contLabel = bytecode.emitLabel();
 size_t backLabel = bytecode.emitLabel();
 bytecode.emitAD(LOP_FORGLOOP, regs, 0);
 bytecode.emitAux((skipOp == LOP_FORGPREP_INEXT ? 0x80000000 : 0) | uint32_t(stat->vars.size));
 size_t endLabel = bytecode.emitLabel();
 patchJump(stat, skipLabel, backLabel);
 patchJump(stat, backLabel, loopLabel);
 patchLoopJumps(stat, oldJumps, endLabel, contLabel);
 loopJumps.resize(oldJumps);
 loops.pop_back();
 }
 struct Assignment
 {
 LValue lvalue;
 uint8_t conflictReg = kInvalidReg;
 uint8_t valueReg = kInvalidReg;
 };
 void resolveAssignConflicts(AstStat* stat, std::vector<Assignment>& vars, const AstArray<AstExpr*>& values)
 {
 struct Visitor : AstVisitor
 {
 Compiler* self;
 std::bitset<256> conflict;
 std::bitset<256> assigned;
 Visitor(Compiler* self)
 : self(self)
 {
 }
 bool visit(AstExprLocal* node) override
 {
 int reg = self->getLocalReg(node->local);
 if (reg >= 0 && assigned[reg])
 conflict[reg] = true;
 return true;
 }
 };
 Visitor visitor(this);
 for (size_t i = 0; i < vars.size(); ++i)
 {
 const LValue& li = vars[i].lvalue;
 if (li.kind == LValue::Kind_Local)
 {
 if (i < values.size)
 values.data[i]->visit(&visitor);
 visitor.assigned[li.reg] = true;
 }
 }
 for (size_t i = 0; i < vars.size(); ++i)
 {
 const LValue& li = vars[i].lvalue;
 if (li.kind != LValue::Kind_Local && i < values.size)
 values.data[i]->visit(&visitor);
 }
 for (size_t i = vars.size(); i < values.size; ++i)
 values.data[i]->visit(&visitor);
 for (const Assignment& var : vars)
 {
 const LValue& li = var.lvalue;
 if ((li.kind == LValue::Kind_IndexName || li.kind == LValue::Kind_IndexNumber || li.kind == LValue::Kind_IndexExpr) &&
 visitor.assigned[li.reg])
 visitor.conflict[li.reg] = true;
 if (li.kind == LValue::Kind_IndexExpr && visitor.assigned[li.index])
 visitor.conflict[li.index] = true;
 }
 for (Assignment& var : vars)
 {
 const LValue& li = var.lvalue;
 if (li.kind == LValue::Kind_Local && visitor.conflict[li.reg])
 var.conflictReg = allocReg(stat, 1);
 }
 }
 void compileStatAssign(AstStatAssign* stat)
 {
 RegScope rs(this);
 if (stat->vars.size == 1 && stat->values.size == 1)
 {
 LValue var = compileLValue(stat->vars.data[0], rs);
 if (var.kind == LValue::Kind_Local)
 {
 compileExpr(stat->values.data[0], var.reg);
 }
 else
 {
 uint8_t reg = compileExprAuto(stat->values.data[0], rs);
 setDebugLine(stat->vars.data[0]);
 compileAssign(var, reg);
 }
 return;
 }
 std::vector<Assignment> vars(stat->vars.size);
 for (size_t i = 0; i < stat->vars.size; ++i)
 vars[i].lvalue = compileLValue(stat->vars.data[i], rs);
 resolveAssignConflicts(stat, vars, stat->values);
 for (size_t i = 0; i < stat->vars.size && i < stat->values.size; ++i)
 {
 AstExpr* value = stat->values.data[i];
 if (i + 1 == stat->values.size && stat->vars.size > stat->values.size)
 {
 uint8_t rest = uint8_t(stat->vars.size - stat->values.size + 1);
 uint8_t temp = allocReg(stat, rest);
 compileExprTempN(value, temp, rest, true);
 for (size_t j = i; j < stat->vars.size; ++j)
 vars[j].valueReg = uint8_t(temp + (j - i));
 }
 else
 {
 Assignment& var = vars[i];
 if (var.lvalue.kind == LValue::Kind_Local)
 {
 var.valueReg = (var.conflictReg == kInvalidReg) ? var.lvalue.reg : var.conflictReg;
 compileExpr(stat->values.data[i], var.valueReg);
 }
 else
 {
 var.valueReg = compileExprAuto(stat->values.data[i], rs);
 }
 }
 }
 for (size_t i = stat->vars.size; i < stat->values.size; ++i)
 {
 RegScope rsi(this);
 compileExprAuto(stat->values.data[i], rsi);
 }
 for (const Assignment& var : vars)
 {
 LUAU_ASSERT(var.valueReg != kInvalidReg);
 if (var.lvalue.kind != LValue::Kind_Local)
 {
 setDebugLine(var.lvalue.location);
 compileAssign(var.lvalue, var.valueReg);
 }
 }
 for (const Assignment& var : vars)
 {
 if (var.lvalue.kind == LValue::Kind_Local && var.valueReg != var.lvalue.reg)
 bytecode.emitABC(LOP_MOVE, var.lvalue.reg, var.valueReg, 0);
 }
 }
 void compileStatCompoundAssign(AstStatCompoundAssign* stat)
 {
 RegScope rs(this);
 LValue var = compileLValue(stat->var, rs);
 uint8_t target = (var.kind == LValue::Kind_Local) ? var.reg : allocReg(stat, 1);
 switch (stat->op)
 {
 case AstExprBinary::Add:
 case AstExprBinary::Sub:
 case AstExprBinary::Mul:
 case AstExprBinary::Div:
 case AstExprBinary::Mod:
 case AstExprBinary::Pow:
 {
 if (var.kind != LValue::Kind_Local)
 compileLValueUse(var, target, false);
 int32_t rc = getConstantNumber(stat->value);
 if (rc >= 0 && rc <= 255)
 {
 bytecode.emitABC(getBinaryOpArith(stat->op, true), target, target, uint8_t(rc));
 }
 else
 {
 uint8_t rr = compileExprAuto(stat->value, rs);
 bytecode.emitABC(getBinaryOpArith(stat->op), target, target, rr);
 }
 }
 break;
 case AstExprBinary::Concat:
 {
 std::vector<AstExpr*> args = {stat->value};
 unrollConcats(args);
 uint8_t regs = allocReg(stat, unsigned(1 + args.size()));
 compileLValueUse(var, regs, false);
 for (size_t i = 0; i < args.size(); ++i)
 compileExprTemp(args[i], uint8_t(regs + 1 + i));
 bytecode.emitABC(LOP_CONCAT, target, regs, uint8_t(regs + args.size()));
 }
 break;
 default:
 LUAU_ASSERT(!"Unexpected compound assignment operation");
 }
 if (var.kind != LValue::Kind_Local)
 compileAssign(var, target);
 }
 void compileStatFunction(AstStatFunction* stat)
 {
 if (int reg = getExprLocalReg(stat->name); reg >= 0)
 {
 compileExpr(stat->func, uint8_t(reg));
 return;
 }
 RegScope rs(this);
 uint8_t reg = allocReg(stat, 1);
 compileExprTemp(stat->func, reg);
 LValue var = compileLValue(stat->name, rs);
 compileAssign(var, reg);
 }
 void compileStat(AstStat* node)
 {
 setDebugLine(node);
 if (options.coverageLevel >= 1 && needsCoverage(node))
 {
 bytecode.emitABC(LOP_COVERAGE, 0, 0, 0);
 }
 if (AstStatBlock* stat = node->as<AstStatBlock>())
 {
 RegScope rs(this);
 size_t oldLocals = localStack.size();
 for (size_t i = 0; i < stat->body.size; ++i)
 compileStat(stat->body.data[i]);
 closeLocals(oldLocals);
 popLocals(oldLocals);
 }
 else if (AstStatIf* stat = node->as<AstStatIf>())
 {
 compileStatIf(stat);
 }
 else if (AstStatWhile* stat = node->as<AstStatWhile>())
 {
 compileStatWhile(stat);
 }
 else if (AstStatRepeat* stat = node->as<AstStatRepeat>())
 {
 compileStatRepeat(stat);
 }
 else if (node->is<AstStatBreak>())
 {
 LUAU_ASSERT(!loops.empty());
 closeLocals(loops.back().localOffset);
 size_t label = bytecode.emitLabel();
 bytecode.emitAD(LOP_JUMP, 0, 0);
 loopJumps.push_back({LoopJump::Break, label});
 }
 else if (AstStatContinue* stat = node->as<AstStatContinue>())
 {
 LUAU_ASSERT(!loops.empty());
 if (loops.back().untilCondition)
 validateContinueUntil(stat, loops.back().untilCondition);
 closeLocals(loops.back().localOffset);
 size_t label = bytecode.emitLabel();
 bytecode.emitAD(LOP_JUMP, 0, 0);
 loopJumps.push_back({LoopJump::Continue, label});
 }
 else if (AstStatReturn* stat = node->as<AstStatReturn>())
 {
 if (options.optimizationLevel >= 2 && !inlineFrames.empty())
 compileInlineReturn(stat, false);
 else
 compileStatReturn(stat);
 }
 else if (AstStatExpr* stat = node->as<AstStatExpr>())
 {
 if (AstExprCall* expr = stat->expr->as<AstExprCall>())
 {
 uint8_t target = uint8_t(regTop);
 compileExprCall(expr, target, 0);
 }
 else
 {
 RegScope rs(this);
 compileExprAuto(stat->expr, rs);
 }
 }
 else if (AstStatLocal* stat = node->as<AstStatLocal>())
 {
 compileStatLocal(stat);
 }
 else if (AstStatFor* stat = node->as<AstStatFor>())
 {
 compileStatFor(stat);
 }
 else if (AstStatForIn* stat = node->as<AstStatForIn>())
 {
 compileStatForIn(stat);
 }
 else if (AstStatAssign* stat = node->as<AstStatAssign>())
 {
 compileStatAssign(stat);
 }
 else if (AstStatCompoundAssign* stat = node->as<AstStatCompoundAssign>())
 {
 compileStatCompoundAssign(stat);
 }
 else if (AstStatFunction* stat = node->as<AstStatFunction>())
 {
 compileStatFunction(stat);
 }
 else if (AstStatLocalFunction* stat = node->as<AstStatLocalFunction>())
 {
 uint8_t var = allocReg(stat, 1);
 pushLocal(stat->name, var);
 compileExprFunction(stat->func, var);
 Local& l = locals[stat->name];
 l.debugpc = bytecode.getDebugPC();
 }
 else if (node->is<AstStatTypeAlias>())
 {
 }
 else
 {
 LUAU_ASSERT(!"Unknown statement type");
 }
 }
 void validateContinueUntil(AstStat* cont, AstExpr* condition)
 {
 UndefinedLocalVisitor visitor(this);
 condition->visit(&visitor);
 if (visitor.undef)
 CompileError::raise(condition->location,
 "Local %s used in the repeat..until condition is undefined because continue statement on line %d jumps over it",
 visitor.undef->name.value, cont->location.begin.line + 1);
 }
 void gatherConstUpvals(AstExprFunction* func)
 {
 ConstUpvalueVisitor visitor(this);
 func->body->visit(&visitor);
 for (AstLocal* local : visitor.upvals)
 getUpval(local);
 }
 void pushLocal(AstLocal* local, uint8_t reg)
 {
 if (localStack.size() >= kMaxLocalCount)
 CompileError::raise(
 local->location, "Out of local registers when trying to allocate %s: exceeded limit %d", local->name.value, kMaxLocalCount);
 localStack.push_back(local);
 Local& l = locals[local];
 LUAU_ASSERT(!l.allocated);
 l.reg = reg;
 l.allocated = true;
 l.debugpc = bytecode.getDebugPC();
 }
 bool areLocalsCaptured(size_t start)
 {
 LUAU_ASSERT(start <= localStack.size());
 for (size_t i = start; i < localStack.size(); ++i)
 {
 Local* l = locals.find(localStack[i]);
 LUAU_ASSERT(l);
 if (l->captured)
 return true;
 }
 return false;
 }
 void closeLocals(size_t start)
 {
 LUAU_ASSERT(start <= localStack.size());
 bool captured = false;
 uint8_t captureReg = 255;
 for (size_t i = start; i < localStack.size(); ++i)
 {
 Local* l = locals.find(localStack[i]);
 LUAU_ASSERT(l);
 if (l->captured)
 {
 captured = true;
 captureReg = std::min(captureReg, l->reg);
 }
 }
 if (captured)
 {
 bytecode.emitABC(LOP_CLOSEUPVALS, captureReg, 0, 0);
 }
 }
 void popLocals(size_t start)
 {
 LUAU_ASSERT(start <= localStack.size());
 for (size_t i = start; i < localStack.size(); ++i)
 {
 Local* l = locals.find(localStack[i]);
 LUAU_ASSERT(l);
 LUAU_ASSERT(l->allocated);
 l->allocated = false;
 if (options.debugLevel >= 2)
 {
 uint32_t debugpc = bytecode.getDebugPC();
 bytecode.pushDebugLocal(sref(localStack[i]->name), l->reg, l->debugpc, debugpc);
 }
 }
 localStack.resize(start);
 }
 void patchJump(AstNode* node, size_t label, size_t target)
 {
 if (!bytecode.patchJumpD(label, target))
 CompileError::raise(node->location, "Exceeded jump distance limit; simplify the code to compile");
 }
 void patchJumps(AstNode* node, std::vector<size_t>& labels, size_t target)
 {
 for (size_t l : labels)
 patchJump(node, l, target);
 }
 void patchLoopJumps(AstNode* node, size_t oldJumps, size_t endLabel, size_t contLabel)
 {
 LUAU_ASSERT(oldJumps <= loopJumps.size());
 for (size_t i = oldJumps; i < loopJumps.size(); ++i)
 {
 const LoopJump& lj = loopJumps[i];
 switch (lj.type)
 {
 case LoopJump::Break:
 patchJump(node, lj.label, endLabel);
 break;
 case LoopJump::Continue:
 patchJump(node, lj.label, contLabel);
 break;
 default:
 LUAU_ASSERT(!"Unknown loop jump type");
 }
 }
 }
 uint8_t allocReg(AstNode* node, unsigned int count)
 {
 unsigned int top = regTop;
 if (top + count > kMaxRegisterCount)
 CompileError::raise(node->location, "Out of registers when trying to allocate %d registers: exceeded limit %d", count, kMaxRegisterCount);
 regTop += count;
 stackSize = std::max(stackSize, regTop);
 return uint8_t(top);
 }
 void setDebugLine(AstNode* node)
 {
 if (options.debugLevel >= 1)
 bytecode.setDebugLine(node->location.begin.line + 1);
 }
 void setDebugLine(const Location& location)
 {
 if (options.debugLevel >= 1)
 bytecode.setDebugLine(location.begin.line + 1);
 }
 void setDebugLineEnd(AstNode* node)
 {
 if (options.debugLevel >= 1)
 bytecode.setDebugLine(node->location.end.line + 1);
 }
 bool needsCoverage(AstNode* node)
 {
 return !node->is<AstStatBlock>() && !node->is<AstStatTypeAlias>();
 }
 struct FenvVisitor : AstVisitor
 {
 bool& getfenvUsed;
 bool& setfenvUsed;
 FenvVisitor(bool& getfenvUsed, bool& setfenvUsed)
 : getfenvUsed(getfenvUsed)
 , setfenvUsed(setfenvUsed)
 {
 }
 bool visit(AstExprGlobal* node) override
 {
 if (node->name == "getfenv")
 getfenvUsed = true;
 if (node->name == "setfenv")
 setfenvUsed = true;
 return false;
 }
 };
 struct FunctionVisitor : AstVisitor
 {
 Compiler* self;
 std::vector<AstExprFunction*>& functions;
 FunctionVisitor(Compiler* self, std::vector<AstExprFunction*>& functions)
 : self(self)
 , functions(functions)
 {
 functions.reserve(16);
 }
 bool visit(AstExprFunction* node) override
 {
 node->body->visit(this);
 functions.push_back(node);
 return false;
 }
 };
 struct UndefinedLocalVisitor : AstVisitor
 {
 UndefinedLocalVisitor(Compiler* self)
 : self(self)
 , undef(nullptr)
 {
 }
 void check(AstLocal* local)
 {
 Local& l = self->locals[local];
 if (!l.allocated && !undef)
 undef = local;
 }
 bool visit(AstExprLocal* node) override
 {
 if (!node->upvalue)
 check(node->local);
 return false;
 }
 bool visit(AstExprFunction* node) override
 {
 const Function* f = self->functions.find(node);
 LUAU_ASSERT(f);
 for (AstLocal* uv : f->upvals)
 {
 LUAU_ASSERT(uv->functionDepth < node->functionDepth);
 if (uv->functionDepth == node->functionDepth - 1)
 check(uv);
 }
 return false;
 }
 Compiler* self;
 AstLocal* undef;
 };
 struct ConstUpvalueVisitor : AstVisitor
 {
 ConstUpvalueVisitor(Compiler* self)
 : self(self)
 {
 }
 bool visit(AstExprLocal* node) override
 {
 if (node->upvalue && self->isConstant(node))
 {
 upvals.push_back(node->local);
 }
 return false;
 }
 bool visit(AstExprFunction* node) override
 {
 return false;
 }
 Compiler* self;
 std::vector<AstLocal*> upvals;
 };
 struct ReturnVisitor : AstVisitor
 {
 Compiler* self;
 bool returnsOne = true;
 ReturnVisitor(Compiler* self)
 : self(self)
 {
 }
 bool visit(AstExpr* expr) override
 {
 return false;
 }
 bool visit(AstStatReturn* stat) override
 {
 returnsOne &= stat->list.size == 1 && !self->isExprMultRet(stat->list.data[0]);
 return false;
 }
 };
 struct RegScope
 {
 RegScope(Compiler* self)
 : self(self)
 , oldTop(self->regTop)
 {
 }
 RegScope(Compiler* self, unsigned int top)
 : self(self)
 , oldTop(self->regTop)
 {
 LUAU_ASSERT(top <= self->regTop);
 self->regTop = top;
 }
 ~RegScope()
 {
 self->regTop = oldTop;
 }
 Compiler* self;
 unsigned int oldTop;
 };
 struct Function
 {
 uint32_t id;
 std::vector<AstLocal*> upvals;
 uint64_t costModel = 0;
 unsigned int stackSize = 0;
 bool canInline = false;
 bool returnsOne = false;
 };
 struct Local
 {
 uint8_t reg = 0;
 bool allocated = false;
 bool captured = false;
 uint32_t debugpc = 0;
 };
 struct LoopJump
 {
 enum Type
 {
 Break,
 Continue
 };
 Type type;
 size_t label;
 };
 struct Loop
 {
 size_t localOffset;
 AstExpr* untilCondition;
 };
 struct InlineArg
 {
 AstLocal* local;
 uint8_t reg;
 Constant value;
 };
 struct InlineFrame
 {
 AstExprFunction* func;
 size_t localOffset;
 uint8_t target;
 uint8_t targetCount;
 std::vector<size_t> returnJumps;
 };
 struct Capture
 {
 LuauCaptureType type;
 uint8_t data;
 };
 BytecodeBuilder& bytecode;
 CompileOptions options;
 DenseHashMap<AstExprFunction*, Function> functions;
 DenseHashMap<AstLocal*, Local> locals;
 DenseHashMap<AstName, Global> globals;
 DenseHashMap<AstLocal*, Variable> variables;
 DenseHashMap<AstExpr*, Constant> constants;
 DenseHashMap<AstLocal*, Constant> locstants;
 DenseHashMap<AstExprTable*, TableShape> tableShapes;
 DenseHashMap<AstExprCall*, int> builtins;
 const DenseHashMap<AstExprCall*, int>* builtinsFold = nullptr;
 unsigned int regTop = 0;
 unsigned int stackSize = 0;
 bool getfenvUsed = false;
 bool setfenvUsed = false;
 std::vector<AstLocal*> localStack;
 std::vector<AstLocal*> upvals;
 std::vector<LoopJump> loopJumps;
 std::vector<Loop> loops;
 std::vector<InlineFrame> inlineFrames;
 std::vector<Capture> captures;
 std::vector<std::unique_ptr<char[]>> interpStrings;
};
void compileOrThrow(BytecodeBuilder& bytecode, const ParseResult& parseResult, const AstNameTable& names, const CompileOptions& inputOptions)
{
 LUAU_TIMETRACE_SCOPE("compileOrThrow", "Compiler");
 LUAU_ASSERT(parseResult.root);
 LUAU_ASSERT(parseResult.errors.empty());
 CompileOptions options = inputOptions;
 for (const HotComment& hc : parseResult.hotcomments)
 if (hc.header && hc.content.compare(0, 9, "optimize ") == 0)
 options.optimizationLevel = std::max(0, std::min(2, atoi(hc.content.c_str() + 9)));
 AstStatBlock* root = parseResult.root;
 Compiler compiler(bytecode, options);
 assignMutable(compiler.globals, names, options.mutableGlobals);
 trackValues(compiler.globals, compiler.variables, root);
 if (options.optimizationLevel >= 2)
 compiler.builtinsFold = &compiler.builtins;
 if (options.optimizationLevel >= 1)
 {
 analyzeBuiltins(compiler.builtins, compiler.globals, compiler.variables, options, root);
 foldConstants(compiler.constants, compiler.variables, compiler.locstants, compiler.builtinsFold, root);
 predictTableShapes(compiler.tableShapes, root);
 }
 if (options.optimizationLevel >= 1 && (names.get("getfenv").value || names.get("setfenv").value))
 {
 Compiler::FenvVisitor fenvVisitor(compiler.getfenvUsed, compiler.setfenvUsed);
 root->visit(&fenvVisitor);
 }
 std::vector<AstExprFunction*> functions;
 Compiler::FunctionVisitor functionVisitor(&compiler, functions);
 root->visit(&functionVisitor);
 for (AstExprFunction* expr : functions)
 compiler.compileFunction(expr);
 AstExprFunction main(root->location, AstArray<AstGenericType>(), AstArray<AstGenericTypePack>(),
 nullptr, AstArray<AstLocal*>(), true, Luau::Location(), root, 0,
 AstName());
 uint32_t mainid = compiler.compileFunction(&main);
 const Compiler::Function* mainf = compiler.functions.find(&main);
 LUAU_ASSERT(mainf && mainf->upvals.empty());
 bytecode.setMainFunction(mainid);
 bytecode.finalize();
}
void compileOrThrow(BytecodeBuilder& bytecode, const std::string& source, const CompileOptions& options, const ParseOptions& parseOptions)
{
 Allocator allocator;
 AstNameTable names(allocator);
 ParseResult result = Parser::parse(source.c_str(), source.size(), names, allocator, parseOptions);
 if (!result.errors.empty())
 throw ParseErrors(result.errors);
 compileOrThrow(bytecode, result, names, options);
}
std::string compile(const std::string& source, const CompileOptions& options, const ParseOptions& parseOptions, BytecodeEncoder* encoder)
{
 LUAU_TIMETRACE_SCOPE("compile", "Compiler");
 Allocator allocator;
 AstNameTable names(allocator);
 ParseResult result = Parser::parse(source.c_str(), source.size(), names, allocator, parseOptions);
 if (!result.errors.empty())
 {
 const Luau::ParseError& parseError = result.errors.front();
 std::string error = format(":%d: %s", parseError.getLocation().begin.line + 1, parseError.what());
 return BytecodeBuilder::getError(error);
 }
 try
 {
 BytecodeBuilder bcb(encoder);
 compileOrThrow(bcb, result, names, options);
 return bcb.getBytecode();
 }
 catch (CompileError& e)
 {
 std::string error = format(":%d: %s", e.getLocation().begin.line + 1, e.what());
 return BytecodeBuilder::getError(error);
 }
}
}
namespace Luau
{
namespace Compile
{
static bool constantsEqual(const Constant& la, const Constant& ra)
{
 LUAU_ASSERT(la.type != Constant::Type_Unknown && ra.type != Constant::Type_Unknown);
 switch (la.type)
 {
 case Constant::Type_Nil:
 return ra.type == Constant::Type_Nil;
 case Constant::Type_Boolean:
 return ra.type == Constant::Type_Boolean && la.valueBoolean == ra.valueBoolean;
 case Constant::Type_Number:
 return ra.type == Constant::Type_Number && la.valueNumber == ra.valueNumber;
 case Constant::Type_String:
 return ra.type == Constant::Type_String && la.stringLength == ra.stringLength && memcmp(la.valueString, ra.valueString, la.stringLength) == 0;
 default:
 LUAU_ASSERT(!"Unexpected constant type in comparison");
 return false;
 }
}
static void foldUnary(Constant& result, AstExprUnary::Op op, const Constant& arg)
{
 switch (op)
 {
 case AstExprUnary::Not:
 if (arg.type != Constant::Type_Unknown)
 {
 result.type = Constant::Type_Boolean;
 result.valueBoolean = !arg.isTruthful();
 }
 break;
 case AstExprUnary::Minus:
 if (arg.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Number;
 result.valueNumber = -arg.valueNumber;
 }
 break;
 case AstExprUnary::Len:
 if (arg.type == Constant::Type_String)
 {
 result.type = Constant::Type_Number;
 result.valueNumber = double(arg.stringLength);
 }
 break;
 default:
 LUAU_ASSERT(!"Unexpected unary operation");
 }
}
static void foldBinary(Constant& result, AstExprBinary::Op op, const Constant& la, const Constant& ra)
{
 switch (op)
 {
 case AstExprBinary::Add:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Number;
 result.valueNumber = la.valueNumber + ra.valueNumber;
 }
 break;
 case AstExprBinary::Sub:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Number;
 result.valueNumber = la.valueNumber - ra.valueNumber;
 }
 break;
 case AstExprBinary::Mul:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Number;
 result.valueNumber = la.valueNumber * ra.valueNumber;
 }
 break;
 case AstExprBinary::Div:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Number;
 result.valueNumber = la.valueNumber / ra.valueNumber;
 }
 break;
 case AstExprBinary::Mod:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Number;
 result.valueNumber = la.valueNumber - floor(la.valueNumber / ra.valueNumber) * ra.valueNumber;
 }
 break;
 case AstExprBinary::Pow:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Number;
 result.valueNumber = pow(la.valueNumber, ra.valueNumber);
 }
 break;
 case AstExprBinary::Concat:
 break;
 case AstExprBinary::CompareNe:
 if (la.type != Constant::Type_Unknown && ra.type != Constant::Type_Unknown)
 {
 result.type = Constant::Type_Boolean;
 result.valueBoolean = !constantsEqual(la, ra);
 }
 break;
 case AstExprBinary::CompareEq:
 if (la.type != Constant::Type_Unknown && ra.type != Constant::Type_Unknown)
 {
 result.type = Constant::Type_Boolean;
 result.valueBoolean = constantsEqual(la, ra);
 }
 break;
 case AstExprBinary::CompareLt:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Boolean;
 result.valueBoolean = la.valueNumber < ra.valueNumber;
 }
 break;
 case AstExprBinary::CompareLe:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Boolean;
 result.valueBoolean = la.valueNumber <= ra.valueNumber;
 }
 break;
 case AstExprBinary::CompareGt:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Boolean;
 result.valueBoolean = la.valueNumber > ra.valueNumber;
 }
 break;
 case AstExprBinary::CompareGe:
 if (la.type == Constant::Type_Number && ra.type == Constant::Type_Number)
 {
 result.type = Constant::Type_Boolean;
 result.valueBoolean = la.valueNumber >= ra.valueNumber;
 }
 break;
 case AstExprBinary::And:
 if (la.type != Constant::Type_Unknown)
 {
 result = la.isTruthful() ? ra : la;
 }
 break;
 case AstExprBinary::Or:
 if (la.type != Constant::Type_Unknown)
 {
 result = la.isTruthful() ? la : ra;
 }
 break;
 default:
 LUAU_ASSERT(!"Unexpected binary operation");
 }
}
struct ConstantVisitor : AstVisitor
{
 DenseHashMap<AstExpr*, Constant>& constants;
 DenseHashMap<AstLocal*, Variable>& variables;
 DenseHashMap<AstLocal*, Constant>& locals;
 const DenseHashMap<AstExprCall*, int>* builtins;
 bool wasEmpty = false;
 std::vector<Constant> builtinArgs;
 ConstantVisitor(DenseHashMap<AstExpr*, Constant>& constants, DenseHashMap<AstLocal*, Variable>& variables,
 DenseHashMap<AstLocal*, Constant>& locals, const DenseHashMap<AstExprCall*, int>* builtins)
 : constants(constants)
 , variables(variables)
 , locals(locals)
 , builtins(builtins)
 {
 wasEmpty = constants.empty() && locals.empty();
 }
 Constant analyze(AstExpr* node)
 {
 Constant result;
 result.type = Constant::Type_Unknown;
 if (AstExprGroup* expr = node->as<AstExprGroup>())
 {
 result = analyze(expr->expr);
 }
 else if (node->is<AstExprConstantNil>())
 {
 result.type = Constant::Type_Nil;
 }
 else if (AstExprConstantBool* expr = node->as<AstExprConstantBool>())
 {
 result.type = Constant::Type_Boolean;
 result.valueBoolean = expr->value;
 }
 else if (AstExprConstantNumber* expr = node->as<AstExprConstantNumber>())
 {
 result.type = Constant::Type_Number;
 result.valueNumber = expr->value;
 }
 else if (AstExprConstantString* expr = node->as<AstExprConstantString>())
 {
 result.type = Constant::Type_String;
 result.valueString = expr->value.data;
 result.stringLength = unsigned(expr->value.size);
 }
 else if (AstExprLocal* expr = node->as<AstExprLocal>())
 {
 const Constant* l = locals.find(expr->local);
 if (l)
 result = *l;
 }
 else if (node->is<AstExprGlobal>())
 {
 }
 else if (node->is<AstExprVarargs>())
 {
 }
 else if (AstExprCall* expr = node->as<AstExprCall>())
 {
 analyze(expr->func);
 if (const int* bfid = builtins ? builtins->find(expr) : nullptr)
 {
 size_t offset = builtinArgs.size();
 bool canFold = true;
 builtinArgs.reserve(offset + expr->args.size);
 for (size_t i = 0; i < expr->args.size; ++i)
 {
 Constant ac = analyze(expr->args.data[i]);
 if (ac.type == Constant::Type_Unknown)
 canFold = false;
 else
 builtinArgs.push_back(ac);
 }
 if (canFold)
 {
 LUAU_ASSERT(builtinArgs.size() == offset + expr->args.size);
 result = foldBuiltin(*bfid, builtinArgs.data() + offset, expr->args.size);
 }
 builtinArgs.resize(offset);
 }
 else
 {
 for (size_t i = 0; i < expr->args.size; ++i)
 analyze(expr->args.data[i]);
 }
 }
 else if (AstExprIndexName* expr = node->as<AstExprIndexName>())
 {
 analyze(expr->expr);
 }
 else if (AstExprIndexExpr* expr = node->as<AstExprIndexExpr>())
 {
 analyze(expr->expr);
 analyze(expr->index);
 }
 else if (AstExprFunction* expr = node->as<AstExprFunction>())
 {
 expr->body->visit(this);
 }
 else if (AstExprTable* expr = node->as<AstExprTable>())
 {
 for (size_t i = 0; i < expr->items.size; ++i)
 {
 const AstExprTable::Item& item = expr->items.data[i];
 if (item.key)
 analyze(item.key);
 analyze(item.value);
 }
 }
 else if (AstExprUnary* expr = node->as<AstExprUnary>())
 {
 Constant arg = analyze(expr->expr);
 if (arg.type != Constant::Type_Unknown)
 foldUnary(result, expr->op, arg);
 }
 else if (AstExprBinary* expr = node->as<AstExprBinary>())
 {
 Constant la = analyze(expr->left);
 Constant ra = analyze(expr->right);
 if (la.type != Constant::Type_Unknown)
 foldBinary(result, expr->op, la, ra);
 }
 else if (AstExprTypeAssertion* expr = node->as<AstExprTypeAssertion>())
 {
 Constant arg = analyze(expr->expr);
 result = arg;
 }
 else if (AstExprIfElse* expr = node->as<AstExprIfElse>())
 {
 Constant cond = analyze(expr->condition);
 Constant trueExpr = analyze(expr->trueExpr);
 Constant falseExpr = analyze(expr->falseExpr);
 if (cond.type != Constant::Type_Unknown)
 result = cond.isTruthful() ? trueExpr : falseExpr;
 }
 else if (AstExprInterpString* expr = node->as<AstExprInterpString>())
 {
 for (AstExpr* expression : expr->expressions)
 analyze(expression);
 }
 else
 {
 LUAU_ASSERT(!"Unknown expression type");
 }
 recordConstant(constants, node, result);
 return result;
 }
 template<typename T>
 void recordConstant(DenseHashMap<T, Constant>& map, T key, const Constant& value)
 {
 if (value.type != Constant::Type_Unknown)
 map[key] = value;
 else if (wasEmpty)
 ;
 else if (Constant* old = map.find(key))
 old->type = Constant::Type_Unknown;
 }
 void recordValue(AstLocal* local, const Constant& value)
 {
 Variable* v = variables.find(local);
 LUAU_ASSERT(v);
 if (!v->written)
 {
 v->constant = (value.type != Constant::Type_Unknown);
 recordConstant(locals, local, value);
 }
 }
 bool visit(AstExpr* node) override
 {
 analyze(node);
 return false;
 }
 bool visit(AstStatLocal* node) override
 {
 for (size_t i = 0; i < node->vars.size && i < node->values.size; ++i)
 {
 Constant arg = analyze(node->values.data[i]);
 recordValue(node->vars.data[i], arg);
 }
 if (node->vars.size > node->values.size)
 {
 AstExpr* last = node->values.size ? node->values.data[node->values.size - 1] : nullptr;
 bool multRet = last && (last->is<AstExprCall>() || last->is<AstExprVarargs>());
 if (!multRet)
 {
 for (size_t i = node->values.size; i < node->vars.size; ++i)
 {
 Constant nil = {Constant::Type_Nil};
 recordValue(node->vars.data[i], nil);
 }
 }
 }
 else
 {
 for (size_t i = node->vars.size; i < node->values.size; ++i)
 analyze(node->values.data[i]);
 }
 return false;
 }
};
void foldConstants(DenseHashMap<AstExpr*, Constant>& constants, DenseHashMap<AstLocal*, Variable>& variables,
 DenseHashMap<AstLocal*, Constant>& locals, const DenseHashMap<AstExprCall*, int>* builtins, AstNode* root)
{
 ConstantVisitor visitor{constants, variables, locals, builtins};
 root->visit(&visitor);
}
}
} // namespace Luau
namespace Luau
{
namespace Compile
{
inline uint64_t parallelAddSat(uint64_t x, uint64_t y)
{
 uint64_t r = x + y;
 uint64_t s = r & 0x8080808080808080ull;
 return (r ^ s) | (s - (s >> 7));
}
static uint64_t parallelMulSat(uint64_t a, int b)
{
 int bs = (b < 127) ? b : 127;
 uint64_t l = bs * ((a >> 0) & 0x007f007f007f007full);
 uint64_t h = bs * ((a >> 8) & 0x007f007f007f007full);
 uint64_t ls = l + 0x7f807f807f807f80ull;
 uint64_t hs = h + 0x7f807f807f807f80ull;
 uint64_t s = (hs & 0x8000800080008000ull) | ((ls & 0x8000800080008000ull) >> 8);
 uint64_t r = ((h & 0x007f007f007f007full) << 8) | (l & 0x007f007f007f007full);
 return r | (s - (s >> 7));
}
inline bool getNumber(AstExpr* node, double& result)
{
 if (AstExprConstantNumber* ne = node->as<AstExprConstantNumber>())
 {
 result = ne->value;
 return true;
 }
 if (AstExprUnary* ue = node->as<AstExprUnary>(); ue && ue->op == AstExprUnary::Minus)
 if (AstExprConstantNumber* ne = ue->expr->as<AstExprConstantNumber>())
 {
 result = -ne->value;
 return true;
 }
 return false;
}
struct Cost
{
 static const uint64_t kLiteral = ~0ull;
 uint64_t model;
 uint64_t constant;
 Cost(int cost = 0, uint64_t constant = 0)
 : model(cost < 0x7f ? cost : 0x7f)
 , constant(constant)
 {
 }
 Cost operator+(const Cost& other) const
 {
 Cost result;
 result.model = parallelAddSat(model, other.model);
 return result;
 }
 Cost& operator+=(const Cost& other)
 {
 model = parallelAddSat(model, other.model);
 constant = 0;
 return *this;
 }
 Cost operator*(int other) const
 {
 Cost result;
 result.model = parallelMulSat(model, other);
 return result;
 }
 static Cost fold(const Cost& x, const Cost& y)
 {
 uint64_t newmodel = parallelAddSat(x.model, y.model);
 uint64_t newconstant = x.constant & y.constant;
 uint64_t extra = (newconstant == kLiteral) ? 0 : (1 | (0x0101010101010101ull & newconstant));
 Cost result;
 result.model = parallelAddSat(newmodel, extra);
 result.constant = newconstant;
 return result;
 }
};
struct CostVisitor : AstVisitor
{
 const DenseHashMap<AstExprCall*, int>& builtins;
 DenseHashMap<AstLocal*, uint64_t> vars;
 Cost result;
 CostVisitor(const DenseHashMap<AstExprCall*, int>& builtins)
 : builtins(builtins)
 , vars(nullptr)
 {
 }
 Cost model(AstExpr* node)
 {
 if (AstExprGroup* expr = node->as<AstExprGroup>())
 {
 return model(expr->expr);
 }
 else if (node->is<AstExprConstantNil>() || node->is<AstExprConstantBool>() || node->is<AstExprConstantNumber>() ||
 node->is<AstExprConstantString>())
 {
 return Cost(0, Cost::kLiteral);
 }
 else if (AstExprLocal* expr = node->as<AstExprLocal>())
 {
 const uint64_t* i = vars.find(expr->local);
 return Cost(0, i ? *i : 0);
 }
 else if (node->is<AstExprGlobal>())
 {
 return 1;
 }
 else if (node->is<AstExprVarargs>())
 {
 return 3;
 }
 else if (AstExprCall* expr = node->as<AstExprCall>())
 {
 bool builtin = builtins.find(expr) != nullptr;
 bool builtinShort = builtin && expr->args.size <= 2;
 Cost cost = builtin ? 2 : 3;
 if (!builtin)
 cost += model(expr->func);
 for (size_t i = 0; i < expr->args.size; ++i)
 {
 Cost ac = model(expr->args.data[i]);
 cost += ac.model == 0 && !builtinShort ? Cost(1) : ac;
 }
 return cost;
 }
 else if (AstExprIndexName* expr = node->as<AstExprIndexName>())
 {
 return model(expr->expr) + 1;
 }
 else if (AstExprIndexExpr* expr = node->as<AstExprIndexExpr>())
 {
 return model(expr->expr) + model(expr->index) + 1;
 }
 else if (AstExprFunction* expr = node->as<AstExprFunction>())
 {
 return 10;
 }
 else if (AstExprTable* expr = node->as<AstExprTable>())
 {
 Cost cost = 10;
 for (size_t i = 0; i < expr->items.size; ++i)
 {
 const AstExprTable::Item& item = expr->items.data[i];
 if (item.key)
 cost += model(item.key);
 cost += model(item.value);
 cost += 1;
 }
 return cost;
 }
 else if (AstExprUnary* expr = node->as<AstExprUnary>())
 {
 return Cost::fold(model(expr->expr), Cost(0, Cost::kLiteral));
 }
 else if (AstExprBinary* expr = node->as<AstExprBinary>())
 {
 return Cost::fold(model(expr->left), model(expr->right));
 }
 else if (AstExprTypeAssertion* expr = node->as<AstExprTypeAssertion>())
 {
 return model(expr->expr);
 }
 else if (AstExprIfElse* expr = node->as<AstExprIfElse>())
 {
 return model(expr->condition) + model(expr->trueExpr) + model(expr->falseExpr) + 2;
 }
 else if (AstExprInterpString* expr = node->as<AstExprInterpString>())
 {
 Cost cost = 3;
 for (AstExpr* innerExpression : expr->expressions)
 cost += model(innerExpression);
 return cost;
 }
 else
 {
 LUAU_ASSERT(!"Unknown expression type");
 return {};
 }
 }
 void assign(AstExpr* expr)
 {
 if (AstExprLocal* lv = expr->as<AstExprLocal>())
 if (uint64_t* i = vars.find(lv->local))
 *i = 0;
 }
 void loop(AstStatBlock* body, Cost iterCost, int factor = 3)
 {
 Cost before = result;
 result = Cost();
 body->visit(this);
 result = before + (result + iterCost) * factor;
 }
 bool visit(AstExpr* node) override
 {
 result += model(node);
 return false;
 }
 bool visit(AstStatFor* node) override
 {
 result += model(node->from);
 result += model(node->to);
 if (node->step)
 result += model(node->step);
 int tripCount = -1;
 double from, to, step = 1;
 if (getNumber(node->from, from) && getNumber(node->to, to) && (!node->step || getNumber(node->step, step)))
 tripCount = getTripCount(from, to, step);
 loop(node->body, 1, tripCount < 0 ? 3 : tripCount);
 return false;
 }
 bool visit(AstStatForIn* node) override
 {
 for (size_t i = 0; i < node->values.size; ++i)
 result += model(node->values.data[i]);
 loop(node->body, 1);
 return false;
 }
 bool visit(AstStatWhile* node) override
 {
 Cost condition = model(node->condition);
 loop(node->body, condition);
 return false;
 }
 bool visit(AstStatRepeat* node) override
 {
 Cost condition = model(node->condition);
 loop(node->body, condition);
 return false;
 }
 bool visit(AstStat* node) override
 {
 if (node->is<AstStatIf>())
 result += 2;
 else if (node->is<AstStatBreak>() || node->is<AstStatContinue>())
 result += 1;
 return true;
 }
 bool visit(AstStatLocal* node) override
 {
 for (size_t i = 0; i < node->values.size; ++i)
 {
 Cost arg = model(node->values.data[i]);
 if (arg.constant && i < node->vars.size)
 vars[node->vars.data[i]] = arg.constant;
 result += arg;
 }
 return false;
 }
 bool visit(AstStatAssign* node) override
 {
 for (size_t i = 0; i < node->vars.size; ++i)
 assign(node->vars.data[i]);
 return true;
 }
 bool visit(AstStatCompoundAssign* node) override
 {
 assign(node->var);
 result += node->var->is<AstExprLocal>() ? 1 : 2;
 return true;
 }
};
uint64_t modelCost(AstNode* root, AstLocal* const* vars, size_t varCount, const DenseHashMap<AstExprCall*, int>& builtins)
{
 CostVisitor visitor{builtins};
 for (size_t i = 0; i < varCount && i < 7; ++i)
 visitor.vars[vars[i]] = 0xffull << (i * 8 + 8);
 root->visit(&visitor);
 return visitor.result.model;
}
int computeCost(uint64_t model, const bool* varsConst, size_t varCount)
{
 int cost = int(model & 0x7f);
 if (cost == 0x7f)
 return cost;
 for (size_t i = 0; i < varCount && i < 7; ++i)
 cost -= int((model >> (i * 8 + 8)) & 0x7f) * varsConst[i];
 return cost;
}
int getTripCount(double from, double to, double step)
{
 int fromi = (from >= -32767 && from <= 32767 && double(int(from)) == from) ? int(from) : INT_MIN;
 int toi = (to >= -32767 && to <= 32767 && double(int(to)) == to) ? int(to) : INT_MIN;
 int stepi = (step >= -32767 && step <= 32767 && double(int(step)) == step) ? int(step) : INT_MIN;
 if (fromi == INT_MIN || toi == INT_MIN || stepi == INT_MIN || stepi == 0)
 return -1;
 if ((stepi < 0 && toi > fromi) || (stepi > 0 && toi < fromi))
 return 0;
 return (toi - fromi) / stepi + 1;
}
}
} // namespace Luau
namespace Luau
{
namespace Compile
{
static const int kMaxLoopBound = 16;
static AstExprTable* getTableHint(AstExpr* expr)
{
 if (AstExprTable* table = expr->as<AstExprTable>())
 return table;
 if (AstExprCall* call = expr->as<AstExprCall>(); call && !call->self && call->args.size == 2)
 if (AstExprGlobal* func = call->func->as<AstExprGlobal>(); func && func->name == "setmetatable")
 if (AstExprTable* table = call->args.data[0]->as<AstExprTable>())
 return table;
 return nullptr;
}
struct ShapeVisitor : AstVisitor
{
 struct Hasher
 {
 size_t operator()(const std::pair<AstExprTable*, AstName>& p) const
 {
 return DenseHashPointer()(p.first) ^ std::hash<AstName>()(p.second);
 }
 };
 DenseHashMap<AstExprTable*, TableShape>& shapes;
 DenseHashMap<AstLocal*, AstExprTable*> tables;
 DenseHashSet<std::pair<AstExprTable*, AstName>, Hasher> fields;
 DenseHashMap<AstLocal*, unsigned int> loops;
 ShapeVisitor(DenseHashMap<AstExprTable*, TableShape>& shapes)
 : shapes(shapes)
 , tables(nullptr)
 , fields(std::pair<AstExprTable*, AstName>())
 , loops(nullptr)
 {
 }
 void assignField(AstExpr* expr, AstName index)
 {
 if (AstExprLocal* lv = expr->as<AstExprLocal>())
 {
 if (AstExprTable** table = tables.find(lv->local))
 {
 std::pair<AstExprTable*, AstName> field = {*table, index};
 if (!fields.contains(field))
 {
 fields.insert(field);
 shapes[*table].hashSize += 1;
 }
 }
 }
 }
 void assignField(AstExpr* expr, AstExpr* index)
 {
 AstExprLocal* lv = expr->as<AstExprLocal>();
 if (!lv)
 return;
 AstExprTable** table = tables.find(lv->local);
 if (!table)
 return;
 if (AstExprConstantNumber* number = index->as<AstExprConstantNumber>())
 {
 TableShape& shape = shapes[*table];
 if (number->value == double(shape.arraySize + 1))
 shape.arraySize += 1;
 }
 else if (AstExprLocal* iter = index->as<AstExprLocal>())
 {
 if (const unsigned int* bound = loops.find(iter->local))
 {
 TableShape& shape = shapes[*table];
 if (shape.arraySize == 0)
 shape.arraySize = *bound;
 }
 }
 }
 void assign(AstExpr* var)
 {
 if (AstExprIndexName* index = var->as<AstExprIndexName>())
 {
 assignField(index->expr, index->index);
 }
 else if (AstExprIndexExpr* index = var->as<AstExprIndexExpr>())
 {
 assignField(index->expr, index->index);
 }
 }
 bool visit(AstStatLocal* node) override
 {
 if (node->vars.size == 1 && node->values.size == 1)
 if (AstExprTable* table = getTableHint(node->values.data[0]); table && table->items.size == 0)
 tables[node->vars.data[0]] = table;
 return true;
 }
 bool visit(AstStatAssign* node) override
 {
 for (size_t i = 0; i < node->vars.size; ++i)
 assign(node->vars.data[i]);
 for (size_t i = 0; i < node->values.size; ++i)
 node->values.data[i]->visit(this);
 return false;
 }
 bool visit(AstStatFunction* node) override
 {
 assign(node->name);
 node->func->visit(this);
 return false;
 }
 bool visit(AstStatFor* node) override
 {
 AstExprConstantNumber* from = node->from->as<AstExprConstantNumber>();
 AstExprConstantNumber* to = node->to->as<AstExprConstantNumber>();
 if (from && to && from->value == 1.0 && to->value >= 1.0 && to->value <= double(kMaxLoopBound) && !node->step)
 loops[node->var] = unsigned(to->value);
 return true;
 }
};
void predictTableShapes(DenseHashMap<AstExprTable*, TableShape>& shapes, AstNode* root)
{
 ShapeVisitor visitor{shapes};
 root->visit(&visitor);
}
}
} // namespace Luau
namespace Luau
{
namespace Compile
{
struct ValueVisitor : AstVisitor
{
 DenseHashMap<AstName, Global>& globals;
 DenseHashMap<AstLocal*, Variable>& variables;
 ValueVisitor(DenseHashMap<AstName, Global>& globals, DenseHashMap<AstLocal*, Variable>& variables)
 : globals(globals)
 , variables(variables)
 {
 }
 void assign(AstExpr* var)
 {
 if (AstExprLocal* lv = var->as<AstExprLocal>())
 {
 variables[lv->local].written = true;
 }
 else if (AstExprGlobal* gv = var->as<AstExprGlobal>())
 {
 globals[gv->name] = Global::Written;
 }
 else
 {
 var->visit(this);
 }
 }
 bool visit(AstStatLocal* node) override
 {
 for (size_t i = 0; i < node->vars.size && i < node->values.size; ++i)
 variables[node->vars.data[i]].init = node->values.data[i];
 for (size_t i = node->values.size; i < node->vars.size; ++i)
 variables[node->vars.data[i]].init = nullptr;
 return true;
 }
 bool visit(AstStatAssign* node) override
 {
 for (size_t i = 0; i < node->vars.size; ++i)
 assign(node->vars.data[i]);
 for (size_t i = 0; i < node->values.size; ++i)
 node->values.data[i]->visit(this);
 return false;
 }
 bool visit(AstStatCompoundAssign* node) override
 {
 assign(node->var);
 node->value->visit(this);
 return false;
 }
 bool visit(AstStatLocalFunction* node) override
 {
 variables[node->name].init = node->func;
 return true;
 }
 bool visit(AstStatFunction* node) override
 {
 assign(node->name);
 node->func->visit(this);
 return false;
 }
};
void assignMutable(DenseHashMap<AstName, Global>& globals, const AstNameTable& names, const char** mutableGlobals)
{
 if (AstName name = names.get("_G"); name.value)
 globals[name] = Global::Mutable;
 if (mutableGlobals)
 for (const char** ptr = mutableGlobals; *ptr; ++ptr)
 if (AstName name = names.get(*ptr); name.value)
 globals[name] = Global::Mutable;
}
void trackValues(DenseHashMap<AstName, Global>& globals, DenseHashMap<AstLocal*, Variable>& variables, AstNode* root)
{
 ValueVisitor visitor{globals, variables};
 root->visit(&visitor);
}
}
} // namespace Luau
char* luau_compile(const char* source, size_t size, lua_CompileOptions* options, size_t* outsize)
{
 LUAU_ASSERT(outsize);
 Luau::CompileOptions opts;
 if (options)
 {
 static_assert(sizeof(lua_CompileOptions) == sizeof(Luau::CompileOptions), "C and C++ interface must match");
 memcpy(static_cast<void*>(&opts), options, sizeof(opts));
 }
 std::string result = compile(std::string(source, size), opts);
 char* copy = static_cast<char*>(malloc(result.size()));
 if (!copy)
 return nullptr;
 memcpy(copy, result.data(), result.size());
 *outsize = result.size();
 return copy;
}
#endif