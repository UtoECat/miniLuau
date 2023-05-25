local function encode(val, stack, space) 
	local t = typeof(val)
	stack = stack or {}
	space = space or ""
	if stack[val] then
		return stack[val]
	end

	if t == "table" then
		stack[val] = tostring(val)
		local buff = {'{\n'}
		for k, v in val do
			buff[#buff+1] = space..' ['..encode(k, stack, space..' ')..']'
			buff[#buff+1] = ' = '
			buff[#buff+1] = encode(v, stack, space..' ')
			buff[#buff+1] = "\n"
		end
		buff[#buff+1] = space.."}"
		return table.concat(buff)
	elseif t == "string" then
		return string.format('%q', val)
	elseif t == "function" then
		local name, nargs, isvar, line = debug.info(val, "nal")
		stack[val] = name.."()"

		local function build_args(n, va)
			local str = ""
			local ch  = string.byte('a')
			for i = 1, n do
				str = str .. string.char(ch + i) .. ', '
			end
			if va then
				str = str .. ', ...'
			end
			return str
		end

		return string.format("%s(%s) : %i", name, 
			build_args(nargs, isvar), line)
	else
		return tostring(val)
	end
end

print(encode(unsafe))
--error("fuck")
