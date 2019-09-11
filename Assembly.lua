---@class clr.Assembly
local M = {}
local ffi = require('ffi')
local lib = require('lib')
local var = require('Variant')

local mt = { __call = function(_, ...)
    local ret = {}
    setmetatable(ret, { __index = M })
    ret:__ctor(...)
    return ret
end }
setmetatable(M, mt)

function M:__ctor(a, domain)
    assert(a and not ffi.isnullptr(a))
    self._a = a
    self._domain = domain
    if not lib.assembly_init_class(self._a) then
        error(self:getLastError())
    end
    self._cs = self:_getClasses()
    self._ns = self:_makeNameSpace()
end

---@return clr.Class
function M:getClass(className)
    return self._cs[className]
end

function M:getLastError()
    return ffi.string(lib.assembly_get_last_error(self._a))
end

function M:getIndexer(root)
    local index
    index = function(t, k)
        local ns = t['.ns']
        if ns[k] then
            -- nested
            local new_t = { ['.ns'] = ns[k] }
            local mt_ = { __index = index }
            if ns[k][1] then
                -- class
                local cls = ns[k][1]
                mt_.__call = function(t_, ...)
                    return cls:createInstance(...)
                end
            end
            return setmetatable(new_t, mt_)
        elseif ns[1] then
            -- class
            local cls = ns[1]
            if cls:hasStaticMember(k) then
                return cls:getStaticMember(k)
            end
        end
    end
    return setmetatable({ ['.ns'] = root or self._ns }, { __index = index })
end

--

function M:_getClasses()
    local n = lib.assembly_get_class(self._a, nil)
    if n == 0 then
        return {}
    end
    local clss = ffi.new(string.format('ClrClass*[%d]', n))
    lib.assembly_get_class(self._a, clss)
    local ret = {}
    for i = 1, n do
        local c = require('Class')(clss[i - 1], self)
        local name = c:getFullName()
        if ret[name] then
            error('same full name ' .. name)
        end
        ret[name] = c
    end
    return ret
end

local insert = table.insert
local sfind = string.find
local function string_split(s, sep)
    local ret = {}
    if not sep or sep == '' then
        local len = #s
        for i = 1, len do
            insert(ret, s:sub(i, i))
        end
    else
        while true do
            local p = sfind(s, sep)
            if not p then
                insert(ret, s)
                break
            end
            local ss = s:sub(1, p - 1)
            insert(ret, ss)
            s = s:sub(p + 1, #s)
        end
    end
    return ret
end

function M:_makeNameSpace()
    local ns = {}
    for k, v in pairs(self._cs) do
        --print('-', k)
        local s = string_split(k, '%.')
        if #s == 0 then
            error('invalid name')
        else
            local cur_ns = ns
            for i = 1, #s do
                local si = s[i]
                assert(si ~= '', k)
                if i < #s then
                    if not cur_ns[si] then
                        cur_ns[si] = {}
                    end
                    cur_ns = cur_ns[si]
                else
                    local ss = string_split(si, '+')
                    if #ss == 0 then
                        error('invalid name')
                    else
                        -- nested class
                        for j = 1, #ss do
                            local ssj = ss[j]
                            if not cur_ns[ssj] then
                                cur_ns[ssj] = {}
                            end
                            if j < #ss then
                                cur_ns = cur_ns[ssj]
                            else
                                cur_ns[ssj][1] = v
                            end
                        end
                    end
                end
            end
        end
    end
    return ns
end

return M
