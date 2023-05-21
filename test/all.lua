assert(loadfile, "no loadfile? bad...")
assert(setfenv, "how dare you???")

local tests_files = {"apicalls.lua",
"assert.lua",
"attrib.lua",
"basic.lua",
"bitwise.lua",
"calls.lua",
"clear.lua",
"closure.lua",
"constructs.lua",
"coroutine.lua",
"coverage.lua",
"datetime.lua",
"debugger.lua",
"debug.lua",
"errors.lua",
"events.lua",
"exceptions.lua",
"gc.lua",
"ifelseexpr.lua",
"interrupt.lua",
"iter.lua",
"literals.lua",
"locals.lua",
"math.lua",
"move.lua",
"ndebug_upvalues.lua",
"pcall.lua",
"pm.lua",
"safeenv.lua",
"sort.lua",
"strconv.lua",
"stringinterp.lua",
"strings.lua",
"tables.lua",
"tmerror.lua",
"tpack.lua",
"types.lua",
"userdata.lua",
"utf8.lua",
"vararg.lua",
"vector.lua"}

local safeenv = {}
setmetatable(safeenv, {__index=_G})
safeenv._G = safeenv

local a, b = loadfile("all.lua")
assert(type(a) == "function", "bad loadfile")
assert(not b, "bad loadfile")

local a, b = loadfile("BADFILE100%")
assert(not a, "bad loadfile with invalud file in input")
assert(type(b) == "string")

local function test2()
	print(debug.traceback("stack traceback : ", 1))
end

test2()

for _, name in tests_files do
	local fun, err = loadfile(name)
	if not fun then
		error(err)
	end
	setfenv(fun, safeenv)
	local ok
	ok, err = xpcall(fun, debug.traceback)
	if not ok then
		print(`test {name} was not passed :\n{err})`)
		break;
	end
end
