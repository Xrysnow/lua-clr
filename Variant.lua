---@class clr.Variant
local M = {}
local ffi = require('ffi')
local lib = require('lib')
local vt = require('variant_type')

local _type = ffi.metatype(ffi.typeof('_variant_t'), M)
local _runtime_type = {}

local mt = { __call = function(_, ...)
    local ret = ffi.new(_type)
    lib.variant_init(ret)
    ret = ffi.gc(ret, function(this)
        _runtime_type[tostring(this)] = nil
        lib.variant_init(this)
    end)
    if ret.__ctor then
        ret:__ctor(...)
    end
    return ret
end }
setmetatable(M, mt)

function M:__ctor(val)
    self:set(val)
end

function M:__index(k)
    if M[k] then
        return M[k]
    end
    local cls = _runtime_type[tostring(self)]
    if cls and cls:hasInstanceMember(k) then
        return cls:getInstanceMember(self, k)
    end
end

function M:__newindex(k, v)
    local cls = _runtime_type[tostring(self)]
    if cls and cls:hasInstanceMember(k) then
        return cls:setInstanceMember(self, k, v)
    end
end

function M:clone()
    return M(self)
end

function M:getType()
    return lib.variant_get_type(self)
end

function M:getTypeName()
    local ty = self:getType()
    if ty == vt.VARENUM.VT_UNKNOWN then
        local rtti = ffi.string(lib.variant_get_unknown_rtti(self))
        return 'VT_UNKNOWN<' .. rtti .. '>'
    else
        return vt.getTypeName(ty)
    end
end

function M:setEmpty()
    lib.variant_init(self)
    return self
end

function M:isEmpty()
    return self:getType() == 0
end

function M:setBool(val)
    lib.variant_set_bool(self, val)
    return self
end

function M:getBool()
    return lib.variant_get_bool(self)
end

function M:setString(val)
    lib.variant_set_string(self, val)
    return self
end

function M:getString()
    return ffi.string(lib.variant_get_string(self))
end

function M:setNumber(val, type)
    type = type or 'double'
    lib['variant_set_' .. type](self, val)
    return self
end

function M:getNumber()
    return tonumber(lib.variant_get_double(self))
end

function M:get()
    local t = self:getType()
    if vt.isNumber(t) then
        return self:getNumber()
    elseif vt.isString(t) then
        return self:getString()
    elseif vt.isBool(t) then
        return self:getBool()
    else
        return self
    end
end

function M:set(val, type_)
    local t = type(val)
    if t == 'number' then
        self:setNumber(val, type_)
    elseif t == 'string' then
        self:setString(val)
    elseif t == 'boolean' then
        self:setBool(val)
    elseif t == 'nil' then
        self:setEmpty()
    elseif t == 'cdata' then
        lib.variant_copy(self, val)
    else
        error(string.format('invalid param type: %s', t))
    end
    return self
end

function M:_setRuntimeType(cls)
    _runtime_type[tostring(self)] = cls
end

function M:_getRuntimeType()
    return _runtime_type[tostring(self)]
end

return M
