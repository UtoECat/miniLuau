inspect = require('inspect')

---
--- HEY HEY HEY! Interruprion by static function/structure duplications founder!
---

local kwords = {
  template = 1, class = 1, struct = 1, static =1
}

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

              --[[
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
              ]]

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

local f = io.open("luau_analysis.cpp", "r")
assert(f)
local s = f:read("a")
f:close()
remove_duplicate_blocks(s)