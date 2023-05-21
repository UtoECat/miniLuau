-- comment 1
-- comment 2
--[[
-- mulitline comment
--]]

print ("small test suit :p")
print ("_G TABLE CONTENT :")
for k,v in _G do
	print('>> ' .. tostring(k) .. ' = ' .. tostring(v))
end


local safe = {}
setmetatable(safe, {__index=_G})
safe._G = safre

local a, err = loadstring([[return function () 
	return test + 1
end]], 'adefinition', safe)
if not a then
	error(err)
end
a = a()

assert(getfenv(a) == safe);
safe.test = 10
print(a());

print("If you want to run test suit, cd into ./test and")
print("run : `../luau-test all.lua`")
