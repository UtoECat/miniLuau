print([[
Luau source packer.
Copyright (C) UtECat 2023-2024.
MIT License.
NO ANY WARRIANTY!
]])

-- modules directories to pack
local modules = {
	"Ast", 
	"Compiler",
	"CodeGen", -- let's go
	"VM"
}

-- directory of currently packed module.
local module_dir = nil

-- common directory
local common_dir = "Common/include/Luau"

-- subdirs in every module
local subdirs_name = {"include", "include/Luau", "src"}

-- PATH_SEP
local SEP = '/'

local function checkfile(n)
	local f = io.open(n, 'r')
	if f then 
		f:close()
		return n
	end
	return nil
end

local function searchfile(n)
	local tmp

	-- check in module sources at first :
	tmp = checkfile(module_dir..SEP..subdirs_name[3]..SEP..n)
	if tmp then return tmp end

	-- else check in common dir (no src dir there)
	tmp = checkfile(common_dir..SEP..n)
	if tmp then return tmp end

	-- else check in shared modules dirs (no src dir there)
	for _, mod in pairs(modules) do
		tmp = checkfile(mod..SEP..subdirs_name[1]..SEP..n)
		if tmp then return tmp end
		-- and in Luau subfolder too
		tmp = checkfile(mod..SEP..subdirs_name[2]..SEP..n)
		if tmp then return tmp end
	end

	-- not found
	return nil
end

local function lsdir(path)
	local t = {}
	local p = io.popen(string.format("ls '%s' --file-type", path), 'r')
	for c in p:lines() do
		if not string.find(c, '/') then
			t[#t+1] = c
		end
	end
	p:close()
	return t
end

local function readfile(n)
	n = searchfile(n) or n
	local f, err = io.open(n, 'r')
	if not f then
		print("Can't open file " ..n.. " : " .. err)
		return "#include <"..n..">\n", false
	end
	local str = f:read('a')
	f:close()
	if not str then return nil, false end
	return str, true
end

-- anti-infinite recursion + inclusion cache.
local included = {}
local INCLUDE_PATTERN = '\n?%s*#%s*include%s+["<](.-)[">]'

local curr = ""

-- recursive :D
local function includefile(oname)
	-- get only filename without path separator at the beginning
	local _, _, name = oname:find("/?([%w_.,]*)$")
	if not included[name] then
		print("Including", name, "!")
		local txt, stat = readfile(oname)
		included[name] = 0
		if stat then
			-- parse included file for more inclusions
      print('debug: ', name, curr)
			local str = ('#line __LINE__ "%s"\n%s'):format(name, txt)
			local old = curr
			curr = name
			str = str:gsub(INCLUDE_PATTERN, includefile)
			curr = old
			str = str..('#line __LINE__ "%s"\n'):format(curr)
			return str
		else
			included[name] = -1;
			return "\n"..txt -- include error :(
		end	
	elseif included[name] > 0 then
		return '\n' -- already included
	end
	if included[name] >= 0 then included[name] = included[name] + 1 end
	return '//included "'..name..'" \n' -- i don't know why it repeats here
	-- btw it works, so i leave it alone
end

-- here all magic is packed together
local buf = {} -- buffer for output file sources

-- finally outputs source file
local function donefile(file, append, prepend)
	local str = table.concat(buf, '\n')
	buf = {}
	collectgarbage()
	
	--str = str:gsub("\n?%s*#%s*define[ \t]+([_%w]*)[ \t]?(.-)\n", unused_check)
	str = str:gsub("#pragma%s-once", "\n") -- remove pragmas
	str = str:gsub("\\%s-\n", "") -- remove continue line
	str = str:gsub('(\n[^/"]-)//.-\n', "%1\n") -- remove C++ comments
	str = str:gsub('\n%s*//.-\n', "\n") -- remove C++ comments 2
	str = str:gsub("/%*.-%*/", "") -- remove /* */ commentaries
	str = str:gsub("\t", " ") -- remove tabs
	str = str:gsub(" +", " ") -- remove ALL spaces ._.
	str = str:gsub(" \n", "\n") -- remove space + newline
	str = str:gsub("\n\n+", "\n") -- remove extra newlines...
	str = (append or "") .. str .. (prepend or "") -- hehe

	-- rollback included cache :
	-- we want to reinclude cpp STL and cstdlib headers again
	-- in new files, but don't want to do this for Luau sources.
	for k, v in pairs(included) do
		if v > 0 then
			included[k] = 1 -- Luau
		elseif v == -1 then
			included[k] = nil -- Std headers (all unfounded)
		end
	end

	local f = io.open(file, "w")
	f:write(str)
	f:flush()
	f:close()
	print("File "..file.." is done!")
end

local function includeFiles(t)
	for _,v in pairs(t) do
		buf[#buf+1] = includefile(v)
	end
end

-- luau copyright
local COPYRIGHT = [[/*
Luau programming language. (Packed version using PACK.LUA)
MIT License

Copyright (c) 2019-2024 Roblox Corporation
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
]]

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

