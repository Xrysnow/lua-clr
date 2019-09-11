#pragma once
#include "common.h"

class ClrClass;
class ClrAssembly;
class ClrDomain;
class _variant_t;

#ifdef __cplusplus
extern "C" {
#endif
	CLRHELPER_API ClrDomain* load_domain(const char* name, const char* clrVersion);
	CLRHELPER_API ClrDomain* get_domain(const char* name);
	CLRHELPER_API void unload_domain(const char* name);
	CLRHELPER_API void clear_domain();
	CLRHELPER_API const char* get_last_error();

	CLRHELPER_API ClrAssembly* domain_load_assembly(ClrDomain* self, const char* assemblyName, uint8_t* mod, size_t modSize);
	CLRHELPER_API ClrAssembly* domain_get_assembly(ClrDomain* self, const char* assemblyName);
	CLRHELPER_API void domain_unload_assembly(ClrDomain* self, const char* assemblyName);
	CLRHELPER_API const char* domain_get_last_error(ClrDomain* self);

	CLRHELPER_API bool assembly_init_class(ClrAssembly* self);
	CLRHELPER_API int32_t assembly_get_class(ClrAssembly* self, ClrClass** ret);
	CLRHELPER_API void assembly_clear_class(ClrAssembly* self);
	CLRHELPER_API const char* assembly_get_last_error(ClrAssembly* self);

	//

	CLRHELPER_API void class_get_type_info(ClrClass* self,
		int32_t* infoCount, const char** infoName, _variant_t* info);
	CLRHELPER_API int32_t class_get_method_info(ClrClass* self,
		int32_t* infoCount, const char** infoName, _variant_t** info);
	CLRHELPER_API int32_t class_get_field_info(ClrClass* self,
		int32_t* infoCount, const char** infoName, _variant_t** info);
	CLRHELPER_API int32_t class_get_property_info(ClrClass* self,
		int32_t* infoCount, const char** infoName, _variant_t** info);

	CLRHELPER_API bool class_create_instance(ClrClass* self,
		_variant_t** argv, int32_t argc, _variant_t* ret);

	CLRHELPER_API bool class_invoke_static_method(ClrClass* self,
		const char* methodName,
		_variant_t** argv, int32_t argc, _variant_t* ret);
	CLRHELPER_API bool class_invoke_method(ClrClass* self,
		_variant_t* instance, const char* methodName,
		_variant_t** argv, int32_t argc, _variant_t* ret);

	CLRHELPER_API bool class_get_static_field(ClrClass* self,
		const char* fieldName, _variant_t* ret);
	CLRHELPER_API bool class_get_field(ClrClass* self, _variant_t* instance,
		const char* fieldName, _variant_t* ret);
	CLRHELPER_API bool class_set_static_field(ClrClass* self,
		const char* fieldName, _variant_t* arg);
	CLRHELPER_API bool class_set_field(ClrClass* self, _variant_t* instance,
		const char* fieldName, _variant_t* arg);

	CLRHELPER_API bool class_get_static_property(ClrClass* self,
		const char* propertyName, _variant_t* ret);
	CLRHELPER_API bool class_get_property(ClrClass* self, _variant_t* instance,
		const char* propertyName, _variant_t* ret);
	CLRHELPER_API bool class_set_static_property(ClrClass* self,
		const char* propertyName, _variant_t* arg);
	CLRHELPER_API bool class_set_property(ClrClass* self, _variant_t* instance,
		const char* propertyName, _variant_t* arg);

	CLRHELPER_API const char* class_get_last_error(ClrClass* self);

	//

#define DECL_VAR(T_)\
	CLRHELPER_API void variant_set_##T_(_variant_t* self, T_ v);\
	CLRHELPER_API T_ variant_get_##T_(_variant_t* self);

	CLRHELPER_API int32_t variant_size();
	CLRHELPER_API void variant_init(_variant_t* v);
	CLRHELPER_API void variant_copy(_variant_t* dst, _variant_t* src);
	CLRHELPER_API int32_t variant_get_type(_variant_t* self);

	DECL_VAR(char);
	DECL_VAR(int8_t);
	DECL_VAR(uint8_t);
	DECL_VAR(int16_t);
	DECL_VAR(uint16_t);
	DECL_VAR(int32_t);
	DECL_VAR(uint32_t);
	DECL_VAR(int64_t);
	DECL_VAR(uint64_t);
	DECL_VAR(float);
	DECL_VAR(double);
	DECL_VAR(bool);
#undef DECL_VAR

	CLRHELPER_API void variant_set_string(_variant_t* self, const char* v);
	CLRHELPER_API const char* variant_get_string(_variant_t* self);

	CLRHELPER_API const char* variant_get_unknown_rtti(_variant_t* self);
	CLRHELPER_API void* variant_get_byref(_variant_t* self);

#ifdef __cplusplus
}
#endif
