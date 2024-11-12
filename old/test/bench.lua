local tests = {
"ack.lua", 
"binary-trees.lua", 
"fannkuch-redux.lua", 
"fixpoint-fact.lua", 
"heapsort.lua", 
"mandel.lua", 
"queen.lua", 
"scimark.lua", 
"spectral-norm.lua"
}

if not typeof then
	typeof = type
end

if jit then
	jit = nil
end

local clock = clock or os.clock

local bench = {}
function bench.runCode(func, name)
	local old = clock()
	func()
	print(name, "elapsed time = ", clock() - old)
end

local require = function()
	return bench
end

local mt = {__index=_G}
local osemu = {
	time = function() return math.abs(clock()) end,
	timediff = function(a, b) return a-b end
}

local old = clock()
for _,item in pairs(tests) do
	local safe = {}
	safe._G = safe
	safe.require = require
	safe.typeof = typeof
	safe.os = osemu
	setmetatable(safe, mt)
	local f, err = loadfile('bench/'..item, 't', safe)
	assert(f, err)
	f()
end
print("TOTAL : ", clock() - old)
