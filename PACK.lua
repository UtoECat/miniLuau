
inspect = dofile('../inspect.lua')

print([[
Luau source packer v2.0.
Copyright (C) UtECat 2023-2024.
MIT License.
NO ANY WARRIANTY!
]])

local START_TIME = os.time()

-- 
-- UTILITIES 
--

gen = {}

local function pesc(s)
  return string.gsub(s, '([%"%$])', '\\%1')
end

local function trimspaces(s)
  local n = s:find"%S"
  return n and s:match(".*%S", n) or ""
end

-- RAW list all directories
function gen.dirs(path)
  local t = {}
  path = trimspaces(path)
  local p = io.popen(string.format('find "%s" -type d', pesc(path)), 'r')
  assert(p, "can't execute find command!")
	for c in p:lines() do
    c = trimspaces(c)
		if c ~= path then
			t[#t+1] = c
		end
	end
	p:close()
  return t
end

function gen.pathinfo(filePath)
    filePath = trimspaces(filePath)
    local pathParts = {}
    local fileNameWithExt = filePath:match("([^/\\]+)$")
    local fileNameWithoutExt = fileNameWithExt:match("(.+)%..+$") or fileNameWithExt
    local fileExtension = fileNameWithExt:match("%.([^%.]+)$") or ""
    local pathWithoutFileName = filePath:match("^(.*[\\/])") or ""
    pathParts.ext = fileExtension
    pathParts.filename = fileNameWithoutExt
    pathParts.fullname = fileNameWithExt
    -- fullpath is the original path
    pathParts.path = pathWithoutFileName
    pathParts.fullpath = filePath
    return pathParts
end

function gen.files(path)
  local c = {}
  local h = {}
  path = trimspaces(path)
  local p = io.popen(string.format('find "%s" -maxdepth 1 -type f -name \'*.cpp\' -or -name \'*.h\' -or -name \'*.hpp\' ', pesc(path)), 'r')
  assert(p, "can't execute find command!")
	for n in p:lines() do
    local info = gen.pathinfo(n)
    n = info.fullpath
    local ext = info.ext:lower()
    if ext == "cpp" or ext == "c" then
		  c[#c+1] = n
    else
      h[#h+1] = n
    end
	end
	p:close()
  return c, h
end


local SEP = "/"

function gen.splitpath(path)
    local components = {}
    -- Split the path by the separator and trim each component
    for component in string.gmatch(path, "([^/\\]+)") do
        -- Trim leading and trailing separators
        local trimmed = component:gsub("^[%./\\]+", ""):gsub("[/\\]+$", "")
        if trimmed ~= "" then
            table.insert(components, trimmed)
        end
    end
    return components
end

function gen.conpath(...)
  local t = {}
  local n = select('#', ...)

  for i = 1, n do
    local s = select(i, ...)
    t[#t+1] = tostring(s):gsub("[/\\]+$", ""):gsub("^[%./\\]+", "")
  end
  return table.concat(t, "/")
end

function gen.pathset(Path)
    local paths = {}
    Path = gen.conpath(Path)

    -- Split the file path into components
    local components = {}
    for component in string.gmatch(Path, "([^/]+)") do
        table.insert(components, component)
    end

    -- Build all possible paths
    for i = 1, #components do
        -- Join the remaining components to form the new path
        local newPath = table.concat(components, "/", i)
        table.insert(paths, newPath)
    end
    return paths
end

function table.merge(a, b)
  for k, v in pairs(a) do
    if not b[k] then
      b[k] = v
    end
  end
  return b
end

function table.arr_to_set(t, s)
  s = s or {}
  for k, v in pairs(t) do
    s[v] = k
  end
  return s
end

function table.set_to_arr(t, a)
  a = a or {}
  for k, _ in pairs(t) do
    a[#a+1] = k
  end
  return a
end

function table.combine_arr_from_set(a, b)
  local set = table.arr_to_set(b)
  for k, _ in pairs(a) do
    if not set[k] then
      b[#b+1] = k
    end
  end
  return b
end

function table.override(a, b)
  for k, v in pairs(a) do
    b[k] = v
  end
  return b
end

if not table.clone then
  table.clone = function (a)
    return table.override(a, {})
  end
end

function table.set_append(s, k, v)
  local t = s[k] or {}
  s[k] = t
  table.merge(v, t)
end


---
--- CONFIGURE
---

-- CONFIGURE : list of modules
-- dependencies syntax
-- name - neme odf the directory we will dig for code and headers
-- out - ouput name of amalgamated files without extensions
-- deps - list of dependencies.
-- all dependencies are resolved propely in order + all #includes are replaced prioperly
-- dirs - list of source diectories

local modules = {
  {
    name = "Common",
    dirs = {"include/Luau"},
    out = "luau_common",
    deps = {}
  },
  {
    name = "Ast",
    dirs = {"include/Luau", "include", "src"},
    out = "luau_ast",
    deps = {"Common"},
  },
  {
    name = "Compiler",
    dirs = {"include/Luau", "include", "src"},
    out = "luau_compiler",
    deps = {"Ast"}
  },
  {
    name = "Config",
    out = "luau_config",
    dirs = {"include/Luau", "include", "src"},
    deps = {"Ast"}
  },
  --[[{
    name = "Analysis",
    out = "luau_analysis",
    dirs = {"include/Luau", "include", "src"},
    deps = {"Ast", "Config"}
  },]]
  {
    name = "CodeGen",
    out = "luau_codegen",
    dirs = {"include/Luau", "include", "src"},
    deps = {"Common", "VM"}
  },
  {
    name = "VM",
    out = "luau_vm",
    dirs = {"src", "include"},
    deps = {"Common"}
  }
}

-- set of result filenames we NOT allowed to inline!
local resultset = {

}

-- CONFIGURE : condition on which header goes into internal header, instead of global exposed one!
local function is_internal_header(mod, path)
  if mod.internal then return true end
  local arr = gen.splitpath(path)
  if arr[2] == "src" then
    return true
  end
  if arr[2] == "include" and arr[3] == "Luau" then
    return true
  end
  return false
end

-- 
-- SEARCHING ALGORITHM 
--

local function recursive_set_append(t, origin, curr)
  assert(t and origin and curr)
  for _, k in pairs(curr) do
    if not t[k] then
      t[k] = assert(origin[k], tostring(k).." not exists!").deps
      assert(t[k], tostring(k))
      recursive_set_append(t, origin, t[k]) -- append values of thos set
    end
  end
  return curr
end

local function process_modules()
  local res = {}
  for k, v in pairs(modules) do
    if res[v.name] then error('module' .. v.name .. 'already exists') end
    res[v.name] = v
  end
  modules = res
  for _, v in pairs(modules) do
    local set = {}
    recursive_set_append(set, modules, v.deps)
    table.combine_arr_from_set(set, v.deps)

    v.sources = {}
    v.headers = {}
    for _, path in pairs(v.dirs) do
      local c, h = gen.files(gen.conpath(v.name, path))
      for _, item in pairs(c) do
        v.sources[#v.sources+1] = item
      end
      for _, item in pairs(h) do
        v.headers[#v.headers+1] = item
      end
    end
  end
end


local function genincludes(v)
    local set = {}
    for _, path in pairs(v.headers) do
      local outname = v.out .. (is_internal_header(v, path) and '_int.hpp' or '.hpp')
      resultset[outname] = true -- ensure it's added
      for _, item in pairs(gen.pathset(path)) do
        set[item] = {common = outname, real = path}
      end
    end
  return set
end

function gen.find_header(curr, name)
  local deps = curr.deps

  -- try our own headers first
  local myset = {}
  table.merge(genincludes(curr), myset)
  if myset[name] then
    print("FOUND IN OUR HEADERS")
    return myset[name].real
  end

  -- then try all dependencies
  for _, _n in pairs(deps) do
    local set = {}
    table.merge(genincludes(modules[_n]), set)
    if set[name] then
      print("FOUND IN EXTERNAL HEADERS")
      return tostring(set[name].common)
    end
  end

  --print('not found '..name..' !')
  return nil
end

process_modules()
--print(inspect(modules))

--local try = {"Luau/Bytecode.h", "Luau/Location.h", "BytecodeSummary.h"}

--for _,v in pairs(try) do
--  print(inspect(gen.find_header(modules["Ast"], v)))
--end
MOD = modules



local CTX = {
  included = {}, -- inclusion cache
  buffer = {}, -- current file buffer
  module = nil,
  incl_stack = {}
}


function gen.readfile(n)
  if not n then return n end
	local f, err = io.open(n, 'r')
	if not f then
    print(n, err)
		return nil
	end
	local str = f:read('a')
	f:close()
	if not str then return nil end
	return str
end

function CTX.remember(name, realname, ok)
  if not CTX.included[realname] then
    CTX.included[realname] = {ok = ok, count = 0}
  end
  local o = CTX.included[realname]
  o.count = o.count + 1

  if not CTX.included[name] then
    CTX.included[name] = {ref=o}
  end
end

local function format_string(str)
  str = str:gsub("\t", "  ") -- remove tabs
  str = str:gsub("#pragma +once", "\n") -- remove pragmas
	str = str:gsub("\\ -\n", "") -- remove continue line
  return str
end

local INCLUDE_PATTERN = '\n?%s*#%s*include%s+["<](.-)[">]'
-- recursive :D
function CTX.includefile(_oname)
	local oname = gen.pathinfo(_oname).fullpath
	-- get only filename without path separator at the beginning

  if CTX.included[oname] == nil then
		--print("Including", oname, "!") -- new short name 
    -- search for realname
    local realname = gen.find_header(CTX.module, oname)
		print("Including", oname, ">>", realname, "!")

    -- error condition 1
    if not realname then
      print("header not found " .. oname)
      CTX.included[oname] = {ok = false, count = 1}
      return "\n// @@@ PACK.lua : not found, likely and std header\n#include <"..oname..">\n" -- ok
    end

    -- error condition 2
    if resultset[realname] then
      if CTX.included[realname] then
        CTX.remember(oname, realname, true) -- good
        return "\n// DONE : was aleready included <"..oname..">\n" -- ok
      end
      print("header ignored (it's from result set):" ..realname)
      CTX.remember(oname, realname, true) -- good
      return "\n#include \""..realname.."\"\n", false -- local
    end

    if CTX.included[realname] == nil then
      -- include file!
      local txt = gen.readfile(realname)
        assert(type(txt) == "string")
        txt = "\n"..format_string(txt).."\n"

        CTX.remember(oname, realname, true) -- good
        -- parse included file for more inclusions
        -- push
        CTX.incl_stack[#CTX.incl_stack+1] = oname
        --print('debug: ', inspect(CTX.incl_stack))
        local str = txt
        str = str:gsub(INCLUDE_PATTERN, CTX.includefile)

        --print("TEST : ", str:find(INCLUDE_PATTERN))
        local _tmp = {}
        for match in string.gmatch(str, INCLUDE_PATTERN) do
          _tmp[#_tmp+1] = match
        end
        if #_tmp > 0 then
          print("@@> DEBUG : leftover : ", table.concat(_tmp, ', '))
        end
        
        -- pop
        CTX.incl_stack[#CTX.incl_stack] = nil
			  return "// @@@ PACK.lua : done, inlined <"..realname..">\n\n"..str
    else -- found
      CTX.remember(oname, realname, true) -- good
      return "\n// DONE : was aleready inlined <"..oname..">\n" -- ok
    end
    print("@@!! FAILED ", oname, realname)
  end
	if CTX.included[oname].ref then
		return '\n// @@@@@ PACK.LUA : unknown was already included! <'..oname..'>\n' -- already included
	end
  if CTX.included[oname].count then
    CTX.included[oname].count = CTX.included[oname].count + 1
		return '\n// @@@@@ PACK.LUA : was already included! <'..oname..'>\n' -- already included
	end
  error('how?')
end


-- append source file with headers :)
function CTX.appendfile(oname)
	-- get only filename without path separator at the beginning
	local name = gen.pathinfo(oname).fullpath
		print("Source ", name, oname, "!")
		local txt = gen.readfile(name)
    assert(type(txt) == "string")
    txt = "\n"..format_string(txt).."\n"

			-- parse included file for more inclusions
      CTX.incl_stack[#CTX.incl_stack+1] = name
      print('debug: ', inspect(CTX.incl_stack))

			local str = txt --('#line __LINE__ "%s"\n%s'):format(name, txt)
			str = str:gsub(INCLUDE_PATTERN, CTX.includefile)
      -- pop
      CTX.incl_stack[#CTX.incl_stack] = nil
			str = str--..('#line __LINE__ "%s"\n'):format(curr)
			return str
end


---
--- HEY HEY HEY! Interruprion by static function/structure duplications founder!
---
local function find_sub(s, v, i ,e)
  return string.find(s:sub(1, e), v, i)
end

local function split(s, off)
    local res = {}
    local start = 1

    while true do
        local startPos, endPos = string.find(s, "%w+", start)
        if not startPos then break end
        table.insert(res, {s=string.sub(s, startPos, endPos), a=startPos+off, b=endPos+off})
        start = endPos + 1
    end

    local res_2 = {}
    for i = #res, 1, -1 do
      res_2[#res_2+1] = res[i]
    end
    return res_2
end

local kwords = {
  template = 1, class = 1, struct = 1, static =1
}

--[[
local function find_revend(s, off)
  local res = {}
  local i = #s

  local mode = 0
  local tmp = ""
  local tcnt = 0

  local print = function() end

  while i >= 1 do
    local c = string.sub(s, i, i)
    if c == ';' then
      break -- exit
    end
    if c == ' ' or c == '\n' then
      print("space")
      if mode == 2 then
        mode = 0
        if kwords[tmp] then
          print("ret word ", tmp)
          return i + 1 + off -- done
        end
      end
      -- skip
    elseif mode == 0 then
      if c == ')' or c == '>' or c == '}' then
        mode = 1
        tcnt = 1
        print("mode br start", c)
      end
      if c:find('%w') then
        mode = 2
        tmp = c
        print("mode word start", c)
      end
    elseif mode == 1 then
      if c == ')' or c == '>' or c == '}' then
        tcnt = tcnt + 1
      elseif c == '(' or c == '<' or c == '{' then
        tcnt = tcnt - 1
        if tcnt <= 0 then
          mode = 0 -- rollback
          print("mode br end", c)
        end
      end
    elseif mode == 2 then
      if c:find('%w') then
        tmp = c .. tmp
        print("char word ", tmp)
      else
        mode = 0
        if c == ')' or c == '>' or c == '}' then
          mode = 1
          tcnt = 1
          print("mode br start", c)
        end
        if kwords[tmp] then
          print("ret word ", tmp)
          return i + 1 + off -- done
        end
        print("end word ", tmp)
      end
    end
    i = i - 1
  end

  return nil
end

print(find_revend(
  "asys template <class Iterator> void insert(Iterator begin, Iterator end) ", 0
))

print(find_revend(
  "asys template void class insert", 0
))

--os.exit()

local function findbraces(input)
  local results = {}
  local stack = {}
  local old = 1

  for i = 1, #input do
      local char = input:sub(i, i)

      if char == "{" then
          -- Push the index of the opening brace onto the stack
          table.insert(stack, {old, i})
          old = i + 1
      elseif char == "}" then
          -- If there's a matching opening brace, pop from the stack
          if #stack > 0 then
              local openIndex = table.remove(stack)
              old = openIndex[1]
              openIndex = openIndex[2]
              assert(old <= openIndex)

              --[=[
              -- Store the pair of indices (opening, closing)
              local tfnd = split(input:sub(old, openIndex), old-1)
              local fnd = nil

              for _, value in pairs(tfnd) do
                fnd = value.a
                --print(value.a, value.b, openIndex, input:sub(value.a, value.b), value.s)
                --assert(input:sub(value.a, value.b) == value.s)
                if kwords[value.s] and (openIndex - value.a) < 320 then
                  break
                else
                  fnd = nil
                end
              end
              ]=]

              --local sss = input:sub(old, openIndex-1)
              --if sss:find("isMetamethod") then
              --  print(fnd, sss, openIndex, i)
              --end
              local fnd = find_revend(input:sub(old, openIndex), old-1)

              if fnd then
                table.insert(results, {fnd, openIndex, i})
              end
          end
          old = i + 1
      end
  end

  return results
end


local function str_remove(input, startIndex, endIndex)
  -- Ensure indices are within the bounds of the string
  if startIndex < 1 or endIndex > #input or startIndex > endIndex then
      return input, "Invalid indices"
  end

  -- Concatenate the parts before and after the specified section
  local before = input:sub(1, startIndex - 1)  -- Part before the section
  local after = input:sub(endIndex + 1)         -- Part after the section

  return before .. after  -- Return the new string
end

function rangesInterfere(range1, range2)
  local start1, end1 = range1[1], range1[2]
  local start2, end2 = range2[1], range2[2]

  -- Check if the ranges overlap
  return (start1 <= end2) and (start2 <= end1)
end

-- removes duplicate similar blocks in strings
local function remove_duplicate_blocks(str)
  print("===============================================================")
  local t = findbraces(str)
  if not t then
    return str -- whatever
  end
  local set = {}
  local dupl = {}

  for _, v in pairs(t) do
    local k = v.k or (str:sub(v[1], v[3]):gsub("%s+", " "))
    v.k = k
    --print(v.k)

    if set[k] and str:sub(v[2], v[3]):find(';') then -- already exists somewhere
      local ok = true
    
      print(v)
      for j = 1, #dupl do
        local w = dupl[j]
        print(w[1] <= v[3], v[3] <= w[1], w[1], w[3], v[1], v[3])
        if w[1] <= v[3] and v[3] <= w[1] then
          ok = false
          --print("is inside ", inspect(v, w))
          break
        elseif v[1] <= w[3] and w[3] <= v[1]  then
          --print("is innter object ", inspect(w, v))
          table.remove(dupl, j) -- hehe
          break
        end
      end
    
      if ok then
        print('added')
        dupl[#dupl + 1] = v -- add duplicate
      end
    else
      --print('asus')
      set[k] = v
    end
  end

  for i = #dupl, 1, -1 do
    local v = dupl[i]
    print(v.k, v[1], str:sub(v[1], v[3]))
    str = str_remove(str, v[1], v[3])
  end
  print("===============================================================")
  return str
end
]]


---
--- Back to packing
---

-- here all magic is packed together

-- finally outputs source file
function CTX.donefile(filename, append, prepend)
	local str = table.concat(CTX.buffer, '\n')
	CTX.buffer = {}
	collectgarbage()

  local doit = false

	--str = str:gsub("\n?%s*#%s*define[ \t]+([_%w]*)[ \t]?(.-)\n", unused_check)

  if doit then
	  str = str:gsub('(\n[^/"]-)//.-\n', "%1\n") -- remove C++ comments
	  str = str:gsub('\n%s*//.-\n', "\n") -- remove C++ comments 2
    str = str:gsub("/%*.-%*/", "") -- remove /* */ commentaries
  end
  if doit then
    str = str:gsub(" +", " ") -- remove redundant spaces 
	  str = str:gsub(" \n", "\n") -- remove space + newline
	  --str = str:gsub("\n\n+", "\n") -- remove extra newlines...
  end

  -- v2
  str = str:gsub("[\r\t]", "  ") -- replace tabs with spaces
  str = str:gsub("\n[ ]+\n", "\n\n") -- remove staces within newlines
  str = str:gsub("\n\n+\n", "\n\n") -- remove extra newlines

  --str = remove_duplicate_blocks(str)
	str = (append or "") .. "\n//only once\n#pragma once\n" .. str .. (prepend or "") -- hehe

	-- rollback included cache :
	-- we want to reinclude cpp STL and cstdlib headers again
	-- in new files, but don't want to do this for Luau sources.
	for k, v in pairs(CTX.included) do
    local o = v.ref and v.ref or v
		if v.ok and v.count then
			v.count = 1
		else
			CTX.included[k] = nil -- Std headers (all unfounded)
		end
	end

  str = "/* @@@@@ PACK.LUA : THIS FILE WAS AUTOGENERATED USING PACK.lua v.2.0!\n * @@@@@ SEE https://github.com/UtoECat/miniLuau/blob/main/PACK.lua FOR DETAILS\n */\n" .. str

  -- change dest path
  filename = "../" .. filename

	local f = io.open(filename, "w")
  assert(f)
	f:write(str)
	f:flush()
	f:close()

  local cmd = "clang-format --style=LLVM -i " .. pesc(filename)
  --print("@!! calling clang-format", cmd)
  --print(os.execute(cmd))

  cmd = "g++ -fsyntax-only -I. -c ".. pesc(filename)
  print("@!! calling gcc-syntx-check", cmd)
  local stat, _, ok = os.execute(cmd)
  print(stat, ok)
  assert(stat and ok == 0)

	print("File "..filename.." is done!")
end

function CTX.appendFiles(t)
	for _,v in pairs(t) do
	CTX.buffer[#CTX.buffer+1] = CTX.appendfile(v)
	end
end

function CTX.includeFiles(t)
  print(inspect(t))
	for _,v in pairs(t) do
	  CTX.buffer[#CTX.buffer+1] = CTX.includefile(v)
	end
end

-- reset and setup new module!
function CTX.start(modname)
  CTX.incl_stack = {}
  CTX.buffer = {}
  CTX.included = {}
  CTX.module = assert(modules[modname])
end


-- luau copyright
local COPYRIGHT = [[/*
 * Luau programming language.
 * MIT License
 *
 * Copyright (c) 2019-2024 Roblox Corporation
 * Copyright (c) 1994–2019 Lua.org, PUC-Rio.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
]]


--- 
--- FINAL SEQUENCE
--- 

local function pack_module(mod)
  print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@")
  print("@@@ MODULE = ", inspect(mod))
  print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@")
  CTX.start(mod.name)

  resultset[CTX.module.out .. '.hpp'] = true
  resultset[CTX.module.out .. '_int.hpp'] = true

  -- do external headers
  local has_glob = false
  print("@@@ packing external header file")
  for _,v in pairs(CTX.module.headers) do
    if not is_internal_header(CTX.module, v) then
	    CTX.buffer[#CTX.buffer+1] = CTX.includefile(v)
    end
	end
  if #CTX.buffer > 0 then -- OK
     has_glob = true
    CTX.donefile(CTX.module.out .. '.hpp', COPYRIGHT)
  end
  --print(inspect(CTX.included))

  local has_int = false
  -- do internal headers
  print("@@@ packing internal header file")
  for _,v in pairs(CTX.module.headers) do
    if is_internal_header(CTX.module, v) then
	    CTX.buffer[#CTX.buffer+1] = CTX.includefile(v)
    end
	end
  if #CTX.buffer > 0 then -- OK
    has_int = true
    CTX.donefile(CTX.module.out .. '_int.hpp', COPYRIGHT .. (has_glob and ("\n#include\""..CTX.module.out .. '.hpp'.."\"\n") or ""))
  end
  --print(inspect(CTX.included))

  -- do sources
  print("@@@ packing c++ source file")
  for _,v in pairs(CTX.module.sources) do
	  CTX.buffer[#CTX.buffer+1] = CTX.appendfile(v)
	end
  if #CTX.buffer > 0 then -- OK
    local include_file = (has_int and ("\n#include\""..CTX.module.out .. '_int.hpp'.."\"\n") or nil) 
    include_file = include_file or (has_glob and ("\n#include\""..CTX.module.out .. '.hpp'.."\"\n") or nil)

    CTX.donefile(CTX.module.out .. '.cpp',  COPYRIGHT .. (include_file) and (include_file) or "")
  end
  --print(inspect(CTX.included))
end

-- tree traverse with dependencies first
local done_modules = {}

local function traverse_module(mod)
  if done_modules[mod] then
    return
  end

  -- mark to prevent recursion
  done_modules[mod] = 0

  -- pack dependencies first
  for _, sub in pairs(mod.deps) do
    local submod = modules[sub]
    if not done_modules[submod] then
      traverse_module(submod)
    end
  end

  -- now pack us
  done_modules[mod] = 1
  pack_module(mod)
end

for _, mod in pairs(modules) do
  traverse_module(mod) -- pack all with correct dependency order
end

print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@")
print("@@@@@ DONE! TOTAL TIME (sec) : ", os.time() - START_TIME)
print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@")

--doit(modules["Common"])
--doit(modules["Ast"])
--doit(modules["Config"])


--[==[

-- do luau.h at first
module_dir = "VM"
includeFiles(lsdir('VM/include/'))
includeFiles(lsdir('Compiler/include/'))
includeFiles(lsdir('CodeGen/include/'))
donefile('luau.hpp', COPYRIGHT..[[
// Comment this out to not build AST and Compiler
#define LUAU_ENABLE_COMPILER 1
#define LUAU_ENABLE_CODEGEN 1
]])

-- then do luau.c with optional Compiler + AST
module_dir = common_dir
includeFiles(lsdir('Common/include/Luau'))

module_dir = "VM"
includeFiles(lsdir('VM/src'))

-- optional 
buf[#buf+1] = "#ifdef LUAU_ENABLE_COMPILER"
-- issues with macros names collisions
buf[#buf+1] = "#undef upvalue"

-- Ast
module_dir = "Ast"
includeFiles(lsdir('Ast/src'))

-- Compiler
module_dir = "Compiler"
includeFiles(lsdir('Compiler/src'))

-- optional 
buf[#buf+1] = "#ifdef LUAU_ENABLE_CODEGEN"

-- issues with macros names collisions
buf[#buf+1] = "#undef VM_INTERRUPT"
-- issue with log2 being not defined (TODO: REMOVE)
buf[#buf+1] = "#undef log2"
buf[#buf+1] = [[
double log2_custom( double n )  
{  
    // log(n)/log(2) is log2.  
    return log( n ) / log( 2 );  
}
]]
buf[#buf+1] = "#define log2 log2_custom"
-- issues with variable names
buf[#buf+1] = "#undef kPageSize"
buf[#buf+1] = "#define kPageSize kPageSize_2"

module_dir = "CodeGen"

-- ~~TODO FIXME~~ hasTypedParameters function is duplicated, so here we do some shit
-- it would be better to write an issue at some point...
buf[#buf+1] = includefile('CodeGen/src/IrBuilder.cpp')
buf[#buf+1] = "#undef hasTypedParameters"
buf[#buf+1] = "#define hasTypedParameters hasTypedParameters_2"

-- OH NO, IT GETS WORSE...
-- PRAY TO GODS THAT NEW LUAU RELEASE WILL NOT ADD MORE STATIC VARS IN SAME NAMESPACE WITH SAME NAME...
codeden_defs = {
  'kCodeEntryInsn',
  'logPerfFunction',
  'onCloseState',
  'onDestroyFunction',
  'onEnter',
  'onEnterDisabled',
  'getMemorySize',
  'createNativeFunction'
}

print('=========================================')

-- redefine
for _, v in pairs(codeden_defs) do
  buf[#buf+1] = string.format("#define %s %s_2", v, v)
end

buf[#buf+1] = includefile('CodeGen/src/CodeGenContext.cpp')

-- undef
for _, v in pairs(codeden_defs) do
  buf[#buf+1] = string.format("#undef %s", v)
end
print('=========================================')


includeFiles(lsdir('CodeGen/src'))

-- endif
buf[#buf+1] = "#endif"
buf[#buf+1] = "#endif"

-- manually insert "#include luau.h" here.
donefile('luau.cpp', COPYRIGHT..[[
#include "luau.hpp"
// hack around some variable collision issues, redefined later
#define kPageSize kPageSize
#define hasTypedParameters hasTypedParameters
]])
]==]
