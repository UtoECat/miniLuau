-- This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
print("testing native code generation")

assert((function(x, y)
  -- trigger a linear sequence
  local t1 = x + 2
  local t2 = x - 7

  local a = x * 10
  local b = a + 1
  a = y -- update 'a' version
  local t = {} -- call to allocate table forces a spill
  local c = x * 10
  return c, b, t, t1, t2
end)(5, 10) == 50)

return('OK')
