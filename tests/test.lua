-- comment 1
-- comment 2
--[[
-- mulitline comment
--]]

require()

print ("small test suit :p")
print ("_G TABLE CONTENT :")
for k,v in _G do
	print('>> ' .. tostring(k) .. ' = ' .. tostring(v))
end

local oldG = _G

print("test loadstring() to custom enviroment")
local safe = {}
setmetatable(safe, {__index=_G})
safe._G = safe
safe.safe = safe

local a, err = loadstring([[return function () 
	return test + 1
end]], 'adefinition', safe)
if not a then
	error(err)
end
a = a()

print(getfenv(a), _G, safe)
assert(getfenv(a) == safe);
safe.test = 10
print(a());

print("test default enviroment for loadstring()")
local c, err = loadstring([[return _G]])
print(getfenv(c), _G, safe, c())
assert(c, err)
assert(getfenv(c) == c(), "_G is ill'formed")
assert(c() ~= _G, "no sandboxing for loaded scripts!!!")

print("test dostring() enviroment")
assert(dostring("return _G") ~= _G, "_G is ill-formrd")
print(dostring("return _G", "hehe", safe), safe, _G)
assert(dostring("return _G", "hehe", safe) == safe, "bad enviroment")
print(dostring("return 1, 2, 3", "hehe", safe))
print(dostring("return 1", "hehe", safe))
assert(dostring("return _G", "hehe", _G) == _G, "_G should not be sandboxed here")

print("is enviroment changed in main thread? (must not)")
assert(oldG == _G)

print("If you want to run test suit, cd into ./test and")
print("run : `../luau-test all.lua`")
