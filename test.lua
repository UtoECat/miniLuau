inspect = require('inspect')


---
--- HEY HEY HEY! Interruprion by static function/structure duplications founder!
---

local kwords = {
  template = 1, class = 1, struct = 1, static =1
}

-- removes duplicate similar blocks in strings
local function remove_duplicate_blocks(str)
  local replace_duplicates = {
    ["static bool isMetamethod"] = "static bool isMetamethod_duplicate(){}",
    ["struct FreeTypeSearcher : TypeVisitor"] = "struct FreeTypeSearcher_duplicate : TypeVisitor{}",
    --["struct StackPusher\n{"] = "struct StackPusher_dupl {}"
  },
  print("===============================================================")
  for patt, replwith in pairs(replace_duplicates) do
    local print = function(...) end
    local newstart, newpos = str:find(patt, 1, true)
    while newpos and newpos <= #str do
      print("@@@@@@@@@ ATT ", str:sub(newpos, newpos + 32))
      newstart, newpos = str:find(patt, newpos, true) -- find SECOND and other usages
      if not newpos then
        break -- end
      end
      print("found at ", newstart)

      local dstack = {} -- stack of brackets
      local comm = 0
      local stackres = {}
      while newpos <= #str do
        local c = str:sub(newpos, newpos)
        if c == " " or c == '\n' then
          if c == "\n" and comm == 2 then
            comm = 0 -- reset one-line comment
            print("comment done")
          end
          -- continue
        elseif comm > 1 then -- real comments
          if comm == 2 then -- one-line
            -- continue
          elseif comm == 3 then
            if c == '*' then
              comm = 4
            end
          elseif comm == 4 then
            if c == '/' then
              comm = 0 -- stop comments
              print('multiline comment done')
            end
          end
        elseif c == '/' then
          comm = comm + 1 -- commnents
        elseif comm == 1 and c == '/' then
          comm = 2 -- one-line
          print('one line')
        elseif comm == 1 and c == '*' then
          comm = 3 -- multi-line
          print('miltiline')
        elseif c == '(' or c == '{' or c == '<' then
          stackres[#stackres+1] = c
          dstack[#dstack+1] = c
          comm = 0
          print('bracket open ', #dstack, c)
        elseif #dstack > 0 and (c == ')' or c == '}' or c == '>') then
          print('bracket close ', #dstack, c)
          stackres[#stackres+1] = c
          comm = 0
          
          if c ~= '>' then
            while dstack[#dstack] == '<' do
              dstack[#dstack] = nil
              print('==>> actual close ', #dstack)
            end

            dstack[#dstack] = nil -- pop us
          elseif dstack[#dstack] == '<' then -- pop one
            dstack[#dstack] = nil
          end

          if #dstack == 0 and c == '}' then
            print("DONE via ", c)
            break -- done
          end
        elseif c == ';' then
          stackres[#stackres+1] = c
          -- pop all shit
          while dstack[#dstack] == '<' do
            dstack[#dstack] = nil 
          end
          comm = 0
          if #dstack == 0 then
            print("DONE via ", ';')
             break -- done
          end
        end
        newpos = newpos + 1
      end

      local a, b = str:sub(1, newstart-1), str:sub(newpos+1, #str)
      print("replaced duplicate of", patt, "at", newpos)
      --print(a, "\n===========\n")
      --print(b, "\n==========\n")
      --print("ORID:\n", str)
      str = a .. replwith .. b
      newpos = newstart + #replwith - 1
      --os.exit()
    end
  end
  print("===============================================================")
  return str
end


local f = io.open("pack-out/luau_analysis.cpp", "r")
assert(f)
local s = [[ 
assert 
blocks 
s
cscscsscsc

static bool isMetamethod {
}

struct FreeTypeSearcher : TypeVisitor {
}

sus 


real sus 

struct FreeTypeSearcher : TypeVisitor {
template<Fuck 1, Fuck 2, Fuck you = [](){saysy;}>
  static int fuckyourmam(int a /*fuck*/ = {{("fuck")}}) {
    print("fuck", i > 0,  i < 0);
  }
}

sus x2 // aaaaa

struct FreeTypeSearcher = bogus

static bool isMetamethod () {
}

a

struct FreeTypeSearcher : TypeVisitor {
    NotNull<Scope> scope;

    explicit FreeTypeSearcher(NotNull<Scope> scope)
        : TypeVisitor(/*skipBoundTypes*/ true)
        , scope(scope)
    {
    }

    enum Polarity
    {
        Positive,
        Negative,
        Both,
    };

    Polarity polarity = Positive;

    void flip()
    {
        switch (polarity)
        {
        case Positive:
            polarity = Negative;
            break;
        case Negative:
            polarity = Positive;
            break;
        case Both:
            break;
        }
    }

    DenseHashSet<const void*> seenPositive{nullptr};
    DenseHashSet<const void*> seenNegative{nullptr};

    bool seenWithPolarity(const void* ty)
    {
        switch (polarity)
        {
        case Positive:
        {
            if (seenPositive.contains(ty))
                return true;

            seenPositive.insert(ty);
            return false;
        }
        case Negative:
        {
            if (seenNegative.contains(ty))
                return true;

            seenNegative.insert(ty);
            return false;
        }
        case Both:
        {
            if (seenPositive.contains(ty) && seenNegative.contains(ty))
                return true;

            seenPositive.insert(ty);
            seenNegative.insert(ty);
            return false;
        }
        }

        return false;
    }

    // The keys in these maps are either TypeIds or TypePackIds. It's safe to
    // mix them because we only use these pointers as unique keys.  We never
    // indirect them.
    DenseHashMap<const void*, size_t> negativeTypes{0};
    DenseHashMap<const void*, size_t> positiveTypes{0};

    bool visit(TypeId ty) override
    {
        if (seenWithPolarity(ty))
            return false;

        LUAU_ASSERT(ty);
        return true;
    }

    bool visit(TypeId ty, const FreeType& ft) override
    {
        if (seenWithPolarity(ty))
            return false;

        if (!subsumes(scope, ft.scope))
            return true;

        switch (polarity)
        {
        case Positive:
            positiveTypes[ty]++;
            break;
        case Negative:
            negativeTypes[ty]++;
            break;
        case Both:
            positiveTypes[ty]++;
            negativeTypes[ty]++;
            break;
        }

        return true;
    }

    bool visit(TypeId ty, const TableType& tt) override
    {
        if (seenWithPolarity(ty))
            return false;

        if ((tt.state == TableState::Free || tt.state == TableState::Unsealed) && subsumes(scope, tt.scope))
        {
            switch (polarity)
            {
            case Positive:
                positiveTypes[ty]++;
                break;
            case Negative:
                negativeTypes[ty]++;
                break;
            case Both:
                positiveTypes[ty]++;
                negativeTypes[ty]++;
                break;
            }
        }

        for (const auto& [_name, prop] : tt.props)
        {
            if (prop.isReadOnly())
                traverse(*prop.readTy);
            else
            {
                LUAU_ASSERT(prop.isShared());

                Polarity p = polarity;
                polarity = Both;
                traverse(prop.type());
                polarity = p;
            }
        }

        if (tt.indexer)
        {
            traverse(tt.indexer->indexType);
            traverse(tt.indexer->indexResultType);
        }

        return false;
    }

    bool visit(TypeId ty, const FunctionType& ft) override
    {
        if (seenWithPolarity(ty))
            return false;

        flip();
        traverse(ft.argTypes);
        flip();

        traverse(ft.retTypes);

        return false;
    }

    bool visit(TypeId, const ClassType&) override
    {
        return false;
    }

    bool visit(TypePackId tp, const FreeTypePack& ftp) override
    {
        if (seenWithPolarity(tp))
            return false;

        if (!subsumes(scope, ftp.scope))
            return true;

        switch (polarity)
        {
        case Positive:
            positiveTypes[tp]++;
            break;
        case Negative:
            negativeTypes[tp]++;
            break;
        case Both:
            positiveTypes[tp]++;
            negativeTypes[tp]++;
            break;
        }

        return true;
    }
};


int A = isMetamethod(); fuck
]]--f:read("a")
print(s)
print("::::==========::::")
f:close()
print(remove_duplicate_blocks(s))