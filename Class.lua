---@class clr.Class
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

function M:__ctor(c, assembly)
    assert(c and not ffi.isnullptr(c))
    self._c = c
    self._assembly = assembly
    self._info = self:_getTypeInfo()
    self._minfo = self:_getMethodInfo()
    self._finfo = self:_getFieldInfo()
    self._pinfo = self:_getPropertyInfo()
    self._meminfo = self:_getMemberInfo()
end

local function packArgs(...)
    local args = { ... }
    local argc = select('#', ...)
    local argv
    if argc > 0 then
        argv = ffi.new(string.format('_variant_t*[%d]', argc))
        for i = 1, argc do
            if type(args[i]) == 'cdata' then
                argv[i - 1] = args[i]
            else
                argv[i - 1] = var(args[i])
            end
        end
    end
    return argv, argc
end

---@return clr.Variant
function M:createInstance(...)
    local argv, argc = packArgs(...)
    local ret = var()
    local ok = lib.class_create_instance(self._c, argv, argc, ret)
    if not ok then
        error(self:getLastError())
    end
    ret:_setRuntimeType(self)
    return ret
end

---@return clr.Variant
function M:invokeMethod(instance, methodName, ...)
    local argv, argc = packArgs(...)
    local ret = var()
    local ok
    if instance ~= nil then
        ok = lib.class_invoke_method(self._c, instance, methodName, argv, argc, ret)
    else
        ok = lib.class_invoke_static_method(self._c, methodName, argv, argc, ret)
    end
    if not ok then
        error(self:getLastError())
    end
    --TODO: set runtime type
    return ret
end

---@return clr.Variant
function M:getField(instance, fieldName)
    return self:_getFP(instance, fieldName)
end

function M:setField(instance, fieldName, arg)
    self:_setFP(instance, fieldName, arg)
end

---@return clr.Variant
function M:getProperty(instance, propertyName)
    return self:_getFP(instance, propertyName, true)
end

function M:setProperty(instance, propertyName, arg)
    self:_setFP(instance, propertyName, arg, true)
end

function M:hasStaticMember(name)
    return self._meminfo[name] and self._meminfo[name].static
end

--TODO: set runtime type
function M:getStaticMember(name)
    local inf = self._meminfo[name]
    if not inf or not inf.static then
        return
    end
    if inf.is_field then
        return self:getField(nil, name)
    elseif inf.is_property then
        if not inf.read then
            error('attempt to read a not readable property')
        end
        return self:getProperty(nil, name)
    elseif inf.is_method then
        local f = function(...)
            return self:invokeMethod(nil, name, ...)
        end
        return f
    end
end

function M:setStaticMember(name, arg)
    local inf = self._meminfo[name]
    if not inf or not inf.static then
        return
    end
    if inf.is_field then
        self:setField(nil, name, arg)
    elseif inf.is_property then
        if not inf.write then
            error('attempt to write a not writeable property')
        end
        self:setProperty(nil, name, arg)
    end
end

function M:hasInstanceMember(name)
    return self._meminfo[name] and not self._meminfo[name].static
end

--TODO: set runtime type
function M:getInstanceMember(instance, name)
    local inf = self._meminfo[name]
    if not inf or inf.static then
        return
    end
    if inf.is_field then
        return self:getField(instance, name)
    elseif inf.is_property then
        if not inf.read then
            error('attempt to read a not readable property')
        end
        return self:getProperty(instance, name)
    elseif inf.is_method then
        local f = function(...)
            return self:invokeMethod(instance, name, ...)
        end
        return f
    end
end

function M:setInstanceMember(instance, name, arg)
    local inf = self._meminfo[name]
    if not inf or inf.static then
        return
    end
    if inf.is_field then
        self:setField(instance, name, arg)
    elseif inf.is_property then
        if not inf.write then
            error('attempt to write a not writeable property')
        end
        self:setProperty(instance, name, arg)
    end
end

function M:getName()
    return self._info.name
end

function M:getFullName()
    return self._info.FullName
end

function M:getNamespace()
    return self._info.Namespace
end

function M:getLastError()
    return ffi.string(lib.class_get_last_error(self._c))
end

--

function M:_getTypeInfo()
    local infoCount = ffi.new('int32_t[1]')
    lib.class_get_type_info(self._c, infoCount, nil, nil)
    infoCount = tonumber(infoCount[0])
    local infoName = ffi.new(string.format('const char*[%d]', infoCount))
    local info = ffi.new(string.format('_variant_t[%d]', infoCount))
    lib.class_get_type_info(self._c, nil, infoName, info)
    local ret = {}
    for i = 1, infoCount do
        local v = info[i - 1]
        local iname = ffi.string(infoName[i - 1])
        ret[iname] = v:get()
    end
    return ret
end

function M:_getMethodInfo()
    return self:_getInfo(lib.class_get_method_info)
end

function M:_getFieldInfo()
    return self:_getInfo(lib.class_get_field_info)
end

function M:_getPropertyInfo()
    return self:_getInfo(lib.class_get_property_info)
end

function M:_getInfo(f)
    local infoCount = ffi.new('int32_t[1]')
    local n = f(self._c, infoCount, nil, nil)
    n = tonumber(n)
    infoCount = tonumber(infoCount[0])
    local infoName = ffi.new(string.format('const char*[%d]', infoCount))
    local info = ffi.new(string.format('_variant_t*[%d]', n))
    local _infos = {}
    for i = 1, n do
        -- keep alive
        _infos[i] = ffi.new(string.format('_variant_t[%d]', infoCount))
        info[i - 1] = _infos[i]
    end
    f(self._c, nil, infoName, info)
    local infoName_ = {}
    local ret = {}
    for i = 1, infoCount do
        infoName_[i] = ffi.string(infoName[i - 1])
    end
    for i = 1, n do
        local inf = {}
        ret[i] = inf
        for j = 1, infoCount do
            local v = info[i - 1][j - 1]
            inf[infoName_[j]] = v:get()
        end
    end
    return ret
end

function M:_getMemberInfo()
    local minfo = self._minfo
    local finfo = self._finfo
    local pinfo = self._pinfo
    local m = {}
    for i, v in ipairs(minfo) do
        if v.IsPublic then
            if not m[v.name] then
                m[v.name] = {}
            end
            local t = m[v.name]
            t.is_method = true
            t.static = v.IsStatic
        end
    end
    for i, v in ipairs(finfo) do
        if v.IsPublic then
            if not m[v.name] then
                m[v.name] = {}
            end
            local t = m[v.name]
            t.is_field = true
            t.static = v.IsStatic
        end
    end
    for i, v in ipairs(pinfo) do
        if not m[v.name] then
            m[v.name] = {}
        end
        local t = m[v.name]
        t.is_property = true
        t.read = v.CanRead
        t.write = v.CanWrite
    end
    return m
end

function M:_getFP(ins, name, isProp)
    local ret = var()
    local ok
    if ins ~= nil then
        if isProp then
            ok = lib.class_get_property(self._c, ins, name, ret)
        else
            ok = lib.class_get_field(self._c, ins, name, ret)
        end
    else
        if isProp then
            ok = lib.class_get_static_property(self._c, name, ret)
        else
            ok = lib.class_get_static_field(self._c, name, ret)
        end
    end
    if not ok then
        error(self:getLastError())
    end
    return ret
end

function M:_setFP(ins, name, arg, isProp)
    arg = var(arg)
    local ok
    if ins ~= nil then
        if isProp then
            ok = lib.class_set_property(self._c, ins, name, arg)
        else
            ok = lib.class_set_field(self._c, ins, name, arg)
        end
    else
        if isProp then
            ok = lib.class_set_static_property(self._c, name, arg)
        else
            ok = lib.class_set_static_field(self._c, name, arg)
        end
    end
    if not ok then
        error(self:getLastError())
    end
end

return M
