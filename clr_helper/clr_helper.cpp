#include "stdafx.h"
#include "common.h"
#include "clr_helper.h"
#include "ClrDomain.h"

using namespace mscorlib;

static std::unordered_map<std::string, std::shared_ptr<ClrDomain>> dms;
static std::string temp_error_string;

ClrDomain* load_domain(const char* name, const char* clrVersion)
{
	const auto it = dms.find(name);
	if (it != dms.end())
		return it->second.get();
	auto d = std::make_shared<ClrDomain>();
	if (!d->init(clrVersion))
	{
		temp_error_string = d->getLastError();
		return nullptr;
	}
	dms[name] = d;
	return d.get();
}

ClrDomain* get_domain(const char* name)
{
	const auto it = dms.find(name);
	if (it != dms.end())
		return it->second.get();
	return nullptr;
}

void unload_domain(const char* name)
{
	dms.erase(name);
}

void clear_domain()
{
	dms.clear();
}

const char* get_last_error()
{
	return temp_error_string.c_str();
}

ClrAssembly* domain_load_assembly(ClrDomain* self, const char* assemblyName, uint8_t* mod, size_t modSize)
{
	if(!self) return nullptr;
	return self->loadAssembly(assemblyName, mod, modSize);
}

ClrAssembly* domain_get_assembly(ClrDomain* self, const char* assemblyName)
{
	if (!self) return nullptr;
	return self->getAssembly(assemblyName);
}

void domain_unload_assembly(ClrDomain* self, const char* assemblyName)
{
	if (!self) return;
	self->unloadAssembly(assemblyName);
}

const char* domain_get_last_error(ClrDomain* self)
{
	if (!self) return "";
	return self->getLastError();
}

bool assembly_init_class(ClrAssembly* self)
{
	if (!self) return false;
	return self->initClass();
}

int32_t assembly_get_class(ClrAssembly* self, ClrClass** ret)
{
	if (!self) return 0;
	return self->getClasses(ret);
}

void assembly_clear_class(ClrAssembly* self)
{
	if (!self) return;
	self->clearClass();
}

const char* assembly_get_last_error(ClrAssembly* self)
{
	if (!self) return "";
	return self->getLastError();
}

void class_get_type_info(ClrClass* self, int32_t* infoCount, const char** infoName,
	_variant_t* info)
{
	if (!self) return;
	self->getTypeInfo(infoCount, infoName, info);
}

int32_t class_get_method_info(ClrClass* self, int32_t* infoCount, const char** infoName,
	_variant_t** info)
{
	if (!self) return 0;
	return self->getMethodInfo(infoCount, infoName, info);
}

int32_t class_get_field_info(ClrClass* self, int32_t* infoCount, const char** infoName,
	_variant_t** info)
{
	if (!self) return 0;
	return self->getFieldInfo(infoCount, infoName, info);
}

int32_t class_get_property_info(ClrClass* self, int32_t* infoCount, const char** infoName,
	_variant_t** info)
{
	if (!self) return 0;
	return self->getPropertyInfo(infoCount, infoName, info);
}

bool class_create_instance(ClrClass* self, _variant_t** argv, int32_t argc, _variant_t* ret)
{
	if (!self) return false;
	return self->createInstance(argv, argc, ret);
}

bool class_invoke_static_method(ClrClass* self, const char* methodName, _variant_t** argv, int32_t argc,
	_variant_t* ret)
{
	if (!self || !methodName) return false;
	return self->invokeStaticMethod(methodName, argv, argc, ret);
}

bool class_invoke_method(ClrClass* self, _variant_t* instance, const char* methodName,
	_variant_t** argv, int32_t argc, _variant_t* ret)
{
	if (!self || !methodName) return false;
	return self->invokeMethod(instance, methodName, argv, argc, ret);
}
bool class_get_static_field(ClrClass* self, const char* fieldName, _variant_t* ret)
{
	if (!self || !fieldName) return false;
	return self->getStaticField(fieldName, ret);
}

bool class_get_field(ClrClass* self, _variant_t* instance, const char* fieldName, _variant_t* ret)
{
	if (!self || !fieldName) return false;
	return self->getField(instance, fieldName, ret);
}

bool class_set_static_field(ClrClass* self, const char* fieldName, _variant_t* arg)
{
	if (!self || !fieldName) return false;
	return self->setStaticField(fieldName, arg);
}

bool class_set_field(ClrClass* self, _variant_t* instance, const char* fieldName, _variant_t* arg)
{
	if (!self || !fieldName) return false;
	return self->setField(instance, fieldName, arg);
}

bool class_get_static_property(ClrClass* self, const char* propertyName, _variant_t* ret)
{
	if (!self || !propertyName) return false;
	return self->getStaticProperty(propertyName, ret);
}

bool class_get_property(ClrClass* self, _variant_t* instance, const char* propertyName, _variant_t* ret)
{
	if (!self || !propertyName) return false;
	return self->getProperty(instance, propertyName, ret);
}

bool class_set_static_property(ClrClass* self, const char* propertyName, _variant_t* arg)
{
	if (!self || !propertyName) return false;
	return self->setStaticProperty(propertyName, arg);
}

bool class_set_property(ClrClass* self, _variant_t* instance, const char* propertyName, _variant_t* arg)
{
	if (!self || !propertyName) return false;
	return self->setProperty(instance, propertyName, arg);
}

const char* class_get_last_error(ClrClass* self)
{
	if (!self) return "";
	return self->getLastError();
}

int32_t variant_size()
{
	return sizeof(_variant_t);
}

void variant_init(_variant_t* v)
{
	if (v) *v = _variant_t();
}

void variant_copy(_variant_t* dst, _variant_t* src)
{
	if (dst&&src&&dst != src)
		*dst = *src;
}

int32_t variant_get_type(_variant_t* self)
{
	if (!self) return 0;
	return self->vt;
}

#define DEF_VAR(T_)\
	void variant_set_##T_(_variant_t* self, T_ arg){\
		if (!self) return;\
		*self = arg; }\
	T_ variant_get_##T_(_variant_t* self) {\
		if (!self) return T_{};\
		return *self; }

DEF_VAR(char);
void variant_set_int8_t(_variant_t* self, int8_t arg) {
	if (!self) return; *self = arg;
}
int8_t variant_get_int8_t(_variant_t* self) {
	if (!self) return int8_t{}; return (char)*self;
}
//DEF_VAR(int8_t);
DEF_VAR(uint8_t);
DEF_VAR(int16_t);
DEF_VAR(uint16_t);
DEF_VAR(int32_t);
DEF_VAR(uint32_t);
DEF_VAR(int64_t);
DEF_VAR(uint64_t);
DEF_VAR(float);
DEF_VAR(double);
DEF_VAR(bool);


void variant_set_string(_variant_t* self, const char* v)
{
	if (!self || !v) return;
	self->SetString(v);
}

static std::string tmp_var_get_string;
const char* variant_get_string(_variant_t* self)
{
	if (!self || self->vt != VT_BSTR) return "";
	const _bstr_t str(*self);
	const char* p = str;
	tmp_var_get_string = p ? p : "";
	return tmp_var_get_string.c_str();
}

void* variant_get_byref(_variant_t* self)
{
	if (!self || self->vt != VT_BYREF) return nullptr;
	return self->byref;
}

static std::string tmp_unknown_rtti;
const char* variant_get_unknown_rtti(_variant_t* self)
{
	if (!self || self->vt != VT_UNKNOWN) return "";
	tmp_unknown_rtti = typeid(self->punkVal).name();
	return tmp_unknown_rtti.c_str();
}
