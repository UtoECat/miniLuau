local tests = {
"test_BinaryTree.lua",
--"test_GC_Boehm_Trees.lua",
"test_GC_hashtable_Keyval.lua",
"test_GC_Tree_Pruning_Eager.lua",
"test_GC_Tree_Pruning_Gen.lua",
"test_GC_Tree_Pruning_Lazy.lua",
"test_LargeTableCtor_array.lua",
"test_LargeTableCtor_hash.lua",
"test_LB_mandel.lua",
"test_Pcall_pcall_yield.lua",
"test_SunSpider_3d-raytrace.lua",
"test_TableCreate_nil.lua",
"test_TableCreate_number.lua",
"test_TableCreate_zerofill.lua",
"test_TableMarshal_select.lua",
"test_TableMarshal_table_pack.lua",
"test_TableMarshal_varargs.lua",
}

local clock = clock or os.clock

local bench = {}
function bench.runCode(func, name)
	local old = clock()
	func()
	print(name, "elapsed time = ", clock() - old)
end

local loadfile = loadfile or error("lloadfile required")

local require = require

if package then
	package.loaded["bench_support"] = bench
else
	require = function()
		return bench
	end
end

local typeof = typeof or type

local mt = {__index=_G}
local osemu = {
	time = function() return math.abs(clock()) end,
	timediff = function(a, b) return a-b end,
	clock = clock
}

assert(table.create)

local old = clock()
for _,item in pairs(tests) do
	local safe = {}
	safe._G = safe
	safe.require = require
	safe.os = osemu
	safe.typeof = typeof
	setmetatable(safe, mt)
	local f, err = loadfile('bench/gc/'..item, 't', safe)
	assert(f, err)
	f()
end
print("TOTAL : ", clock() - old)
