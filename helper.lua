local ffi = require('ffi')

ffi.nullptr = ffi.cast('void*', 0)

function ffi.isnullptr(p)
    return not (ffi.cast('void*', p) > nil)
end

local typeof = ffi.typeof
local cast = ffi.cast
local _ptr_t = typeof('void*')

function ffi.convert_ptr(userdata, cdata_ptr_t)
    return cast(cdata_ptr_t or _ptr_t, userdata)
end

local M = {}

function M.scope(...)
    local asm = { ... }
    if #asm == 0 then
        asm = { 'mscorlib.dll' }
    end
    for i = 1, #asm do
        if type(asm[i]) == 'string' then
            asm[i] = M.assembly(asm[i])
        end
    end
    local indexers = {}
    for i, v in ipairs(asm) do
        indexers[i] = v:getIndexer()
    end
    local index = function(_, k)
        for i, v in ipairs(indexers) do
            if v[k] then
                return v[k]
            end
        end
        return _G[k]
    end
    local using = function(t)
        assert(t, 'invalid namespace')
        table.insert(indexers, t)
    end
    return function(f)
        setfenv(f, setmetatable({ using = using }, { __index = index }))
        f()
    end
end

local d
local ass = {}

function M.assembly(name)
    d = d or require('Domain')()
    if not ass[name] then
        ass[name] = d:getAssembly(name)
    end
    return ass[name]
end

ffi.cdef [[
typedef struct FILE_ { void* _Placeholder; } FILE;

FILE *fopen(const char *filename, const char *mode);
int fseek(FILE *stream, long int offset, int whence);
long int ftell(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
int fclose(FILE *stream);
]]

function M.loadfile(path)
    local fp = ffi.C.fopen(path, 'rb')
    if not ffi.isnullptr(fp) then
        ffi.C.fseek(fp, 0, 2)
        local size = ffi.C.ftell(fp)
        ffi.C.fseek(fp, 0, 0)
        local buf = ffi.typeof('uint8_t[$]', size)()
        local read = ffi.C.fread(buf, 1, size, fp)
        ffi.C.fclose(fp)
        if read == size then
            return buf, size
        end
    end
end

return M
