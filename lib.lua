local ffi = require('ffi')

ffi.cdef [[
typedef struct ClrClass_ {} ClrClass;
typedef struct ClrAssembly_ {} ClrAssembly;
typedef struct ClrDomain_ {} ClrDomain;

ClrDomain* load_domain(const char* name, const char* clrVersion);
ClrDomain* get_domain(const char* name);
void unload_domain(const char* name);
void clear_domain();
const char* get_last_error();

int32_t variant_size();
]]

local lib = ffi.load('clr_helper.dll')

local _sz = tonumber(lib.variant_size())
local _def = [[
typedef struct __variant_t { uint8_t __[%d]; } _variant_t;
]]
_def = string.format(_def, _sz)
ffi.cdef(_def)
assert(ffi.sizeof(ffi.typeof('_variant_t')) == _sz)

ffi.cdef [[
void variant_init(_variant_t* v);
void variant_copy(_variant_t* dst, _variant_t* src);
int32_t variant_get_type(_variant_t* self);

void variant_set_string(_variant_t* self, const char* v);
const char* variant_get_string(_variant_t* self);
const char* variant_get_unknown_rtti(_variant_t* self);
void* variant_get_byref(_variant_t* self);

char variant_get_char(_variant_t* self);
int8_t variant_get_int8_t(_variant_t* self);
uint8_t variant_get_uint8_t(_variant_t* self);
int16_t variant_get_int16_t(_variant_t* self);
uint16_t variant_get_uint16_t(_variant_t* self);
int32_t variant_get_int32_t(_variant_t* self);
uint32_t variant_get_uint32_t(_variant_t* self);
int64_t variant_get_int64_t(_variant_t* self);
uint64_t variant_get_uint64_t(_variant_t* self);
float variant_get_float(_variant_t* self);
double variant_get_double(_variant_t* self);
bool variant_get_bool(_variant_t* self);

void variant_set_char(_variant_t* self, char v);
void variant_set_int8_t(_variant_t* self, int8_t v);
void variant_set_uint8_t(_variant_t* self, uint8_t v);
void variant_set_int16_t(_variant_t* self, int16_t v);
void variant_set_uint16_t(_variant_t* self, uint16_t v);
void variant_set_int32_t(_variant_t* self, int32_t v);
void variant_set_uint32_t(_variant_t* self, uint32_t v);
void variant_set_int64_t(_variant_t* self, int64_t v);
void variant_set_uint64_t(_variant_t* self, uint64_t v);
void variant_set_float(_variant_t* self, float v);
void variant_set_double(_variant_t* self, double v);
void variant_set_bool(_variant_t* self, bool v);

//

ClrAssembly* domain_load_assembly(ClrDomain* self, const char* assemblyName, uint8_t* mod, size_t modSize);
ClrAssembly* domain_get_assembly(ClrDomain* self, const char* assemblyName);
void domain_unload_assembly(ClrDomain* self, const char* assemblyName);
const char* domain_get_last_error(ClrDomain* self);

//

bool assembly_init_class(ClrAssembly* self);
int32_t assembly_get_class(ClrAssembly* self, ClrClass** ret);
void assembly_clear_class(ClrAssembly* self);
const char* assembly_get_last_error(ClrAssembly* self);

//

void class_get_type_info(ClrClass* self,
	int32_t* infoCount, const char** infoName, _variant_t* info);
int32_t class_get_method_info(ClrClass* self,
	int32_t* infoCount, const char** infoName, _variant_t** info);
int32_t class_get_field_info(ClrClass* self,
	int32_t* infoCount, const char** infoName, _variant_t** info);
int32_t class_get_property_info(ClrClass* self,
	int32_t* infoCount, const char** infoName, _variant_t** info);

bool class_create_instance(ClrClass* self,
	_variant_t** argv, int32_t argc, _variant_t* ret);

bool class_invoke_static_method(ClrClass* self,
	const char* methodName,
	_variant_t** argv, int32_t argc, _variant_t* ret);
bool class_invoke_method(ClrClass* self,
	_variant_t* instance, const char* methodName,
	_variant_t** argv, int32_t argc, _variant_t* ret);

bool class_get_static_field(ClrClass* self,
	const char* fieldName, _variant_t* ret);
bool class_get_field(ClrClass* self, _variant_t* instance,
	const char* fieldName, _variant_t* ret);
bool class_set_static_field(ClrClass* self,
	const char* fieldName, _variant_t* arg);
bool class_set_field(ClrClass* self, _variant_t* instance,
	const char* fieldName, _variant_t* arg);

bool class_get_static_property(ClrClass* self,
	const char* propertyName, _variant_t* ret);
bool class_get_property(ClrClass* self, _variant_t* instance,
	const char* propertyName, _variant_t* ret);
bool class_set_static_property(ClrClass* self,
	const char* propertyName, _variant_t* arg);
bool class_set_property(ClrClass* self, _variant_t* instance,
	const char* propertyName, _variant_t* arg);

const char* class_get_last_error(ClrClass* self);

]]

return lib
