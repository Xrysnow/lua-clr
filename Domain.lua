---@class clr.Domain
local M = {}
local ffi = require('ffi')
local lib = require('lib')

local mt = { __call = function(_, ...)
    local ret = {}
    setmetatable(ret, { __index = M })
    ret:__ctor(...)
    ret['.gc_proxy'] = ffi.gc(ffi.new('int[1]'), function()
        ret:dtor()
    end)
    return ret
end }
setmetatable(M, mt)

function M:__ctor(name, clrVersion)
    name = name or ''
    clrVersion = clrVersion or ''
    local d = lib.load_domain(name, clrVersion)
    if ffi.isnullptr(d) then
        error(ffi.string(lib.get_last_error()))
    end
    self._d = d
    self._name = name
    self._as = {}
end

function M:getAssembly(assemblyName)
    if self._as[assemblyName] then
        return self._as[assemblyName]
    end
    local a = lib.domain_load_assembly(self._d, assemblyName, nil, 0)
    if ffi.isnullptr(a) then
        -- load from file
        local buf, size = require('helper').loadfile(assemblyName)
        if buf then
            a = lib.domain_load_assembly(self._d, assemblyName, buf, size)
        end
    end
    if ffi.isnullptr(a) then
        error(self:getLastError())
    end
    local ret = require('Assembly')(a, self)
    self._as[assemblyName] = ret
    return ret
end

function M:removeAssembly(assemblyName)
    if self._as[assemblyName] then
        lib.domain_unload_assembly(self._d, assemblyName)
        self._as[assemblyName] = nil
    end
end

function M:getLastError()
    return ffi.string(lib.domain_get_last_error(self._d))
end

function M:dtor()
    lib.unload_domain(self._name)
    self._d = nil
    self._as = {}
    local err = ffi.string(lib.get_last_error())
    if err ~= '' then
        print(err)
    end
end

return M
