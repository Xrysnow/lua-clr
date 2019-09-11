#include "stdafx.h"
#include "ClrClass.h"

using namespace mscorlib;

#define CATCH_COM_ERR catch(const _com_error& e) { CERR_E(""); _hr = e.Error(); return false; }

LONG GetSafeArrayLen(LPSAFEARRAY psa)
{
	LONG ubound = 0;
	SafeArrayGetUBound(psa, 1, &ubound);
	return ubound + 1;
}

bool ClrClass::initMethods()
{
	try
	{
		LPSAFEARRAY arr = _type->GetMethods_2();
		LONG count = GetSafeArrayLen(arr);
		for (long i = 0; i < count; ++i)
		{
			IUnknown* v = nullptr;
			if (SUCCEEDED(SafeArrayGetElement(arr, &i, &v)))
			{
				_MethodInfoPtr method = v;
				methodInfos.push_back(method);
			}
		}
		SafeArrayDestroy(arr);
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::initFields()
{
	try
	{
		LPSAFEARRAY arr = _type->GetFields_2();
		LONG count = GetSafeArrayLen(arr);
		for (long i = 0; i < count; ++i)
		{
			IUnknown* v = nullptr;
			if (SUCCEEDED(SafeArrayGetElement(arr, &i, &v)))
			{
				_FieldInfoPtr filed = v;
				filedInfos.push_back(filed);
			}
		}
		SafeArrayDestroy(arr);
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::initProperties()
{
	try
	{
		LPSAFEARRAY arr = _type->GetProperties_2();
		LONG count = GetSafeArrayLen(arr);
		for (long i = 0; i < count; ++i)
		{
			IUnknown* v = nullptr;
			if (SUCCEEDED(SafeArrayGetElement(arr, &i, &v)))
			{
				_PropertyInfoPtr filed = v;
				propertyInfos.push_back(filed);
			}
		}
		SafeArrayDestroy(arr);
	}
	CATCH_COM_ERR;
	return true;
}

ClrClass::ClrClass(_TypePtr pt): _type(pt)
{
	initMethods();
	initFields();
	initProperties();
}

_TypePtr ClrClass::getType()
{
	return _type;
}

void ClrClass::getTypeInfo(_TypePtr pt, int32_t* infoCount, const char** infoName, _variant_t* info)
{
	static const char* InfoNames[] = {
		"ToString",
		"FullName",
		"Namespace",
		"AssemblyQualifiedName",
		"MemberType",
		"Attributes",
		"IsNestedPublic",
		"IsNestedPrivate",
		"IsNestedFamily",
		"IsNestedAssembly",
		"IsNestedFamANDAssem",
		"IsAutoLayout",
		"IsLayoutSequential",
		"IsExplicitLayout",
		"IsClass",
		"IsInterface",
		"IsValueType",
		"IsNestedFamORAssem",
		"IsAbstract",
		"IsSealed",
		"IsEnum",
		"IsSpecialName",
		"IsImport",
		"IsSerializable",
		"IsAnsiClass",
		"IsUnicodeClass",
		"IsAutoClass",
		"IsArray",
		"IsByRef",
		"IsPointer",
		"IsPrimitive",
		"IsCOMObject",
		"HasElementType",
		"IsContextful",
		"IsMarshalByRef",
		"IsNotPublic",
		"IsPublic",
		"name",
		"Type",
	};
	static const auto InfoCount = sizeof(InfoNames) / sizeof(const char*);
	if (infoCount)
		*infoCount = InfoCount;
	if (infoName)
	{
		for (auto i = 0u; i < InfoCount; ++i)
			infoName[i] = InfoNames[i];
	}
	if (!info)
		return;
	info[0] = pt->ToString;
	info[1] = pt->FullName;
	info[2] = pt->Namespace;
	info[3] = pt->AssemblyQualifiedName;
	info[4] = pt->MemberType;
	info[5] = pt->Attributes;
	info[6] = (bool)pt->IsNestedPublic;
	info[7] = (bool)pt->IsNestedPrivate;
	info[8] = (bool)pt->IsNestedFamily;
	info[9] = (bool)pt->IsNestedAssembly;
	info[10] = (bool)pt->IsNestedFamANDAssem;
	info[11] = (bool)pt->IsAutoLayout;
	info[12] = (bool)pt->IsLayoutSequential;
	info[13] = (bool)pt->IsExplicitLayout;
	info[14] = (bool)pt->IsClass;
	info[15] = (bool)pt->IsInterface;
	info[16] = (bool)pt->IsValueType;
	info[17] = (bool)pt->IsNestedFamORAssem;
	info[18] = (bool)pt->IsAbstract;
	info[19] = (bool)pt->IsSealed;
	info[20] = (bool)pt->IsEnum;
	info[21] = (bool)pt->IsSpecialName;
	info[22] = (bool)pt->IsImport;
	info[23] = (bool)pt->IsSerializable;
	info[24] = (bool)pt->IsAnsiClass;
	info[25] = (bool)pt->IsUnicodeClass;
	info[26] = (bool)pt->IsAutoClass;
	info[27] = (bool)pt->IsArray;
	info[28] = (bool)pt->IsByRef;
	info[29] = (bool)pt->IsPointer;
	info[30] = (bool)pt->IsPrimitive;
	info[31] = (bool)pt->IsCOMObject;
	info[32] = (bool)pt->HasElementType;
	info[33] = (bool)pt->IsContextful;
	info[34] = (bool)pt->IsMarshalByRef;
	info[35] = (bool)pt->IsNotPublic;
	info[36] = (bool)pt->IsPublic;
	info[37] = pt->name;
	info[38] = (IUnknown*)pt;
}

void ClrClass::getTypeInfo(int32_t* infoCount, const char** infoName, _variant_t* info)
{
	getTypeInfo(_type, infoCount, infoName, info);
}

int32_t ClrClass::getMethodInfo(int32_t* infoCount, const char** infoName, _variant_t** info)
{
	static const char* InfoNames[] = {
		"ToString",
		"IsPublic",
		"IsPrivate",
		"IsFamily",
		"IsAssembly",
		"IsStatic",
		"IsFinal",
		"IsVirtual",
		"IsHideBySig",
		"IsAbstract",
		"IsSpecialName",
		"IsConstructor",
		"name",
		"MethodInfo",
		"ReturnType",
	};
	static const auto InfoCount = sizeof(InfoNames) / sizeof(const char*);
	if (infoCount)
		*infoCount = InfoCount;
	if (infoName)
	{
		for (auto i = 0u; i < InfoCount; ++i)
			infoName[i] = InfoNames[i];
	}
	auto i = 0u;
	for (auto& m : methodInfos)
	{
		if (info)
		{
			info[i][0] = m->ToString;
			info[i][1] = (bool)m->IsPublic;
			info[i][2] = (bool)m->IsPrivate;
			info[i][3] = (bool)m->IsFamily;
			info[i][4] = (bool)m->IsAssembly;
			info[i][5] = (bool)m->IsStatic;
			info[i][6] = (bool)m->IsFinal;
			info[i][7] = (bool)m->IsVirtual;
			info[i][8] = (bool)m->IsHideBySig;
			info[i][9] = (bool)m->IsAbstract;
			info[i][10] = (bool)m->IsSpecialName;
			info[i][11] = (bool)m->IsConstructor;
			info[i][12] = m->name;
			info[i][13] = (IUnknown*)m;
			info[i][14] = (IUnknown*)m->GetreturnType();
		}
		++i;
	}
	return methodInfos.size();
}

int32_t ClrClass::getFieldInfo(int32_t* infoCount, const char** infoName, _variant_t** info)
{
	static const char* InfoNames[] = {
		"ToString",
		"MemberType",
		"Attributes",
		"IsSpecialName",
		"IsPinvokeImpl",
		"IsPublic",
		"IsPrivate",
		"IsFamily",
		"IsAssembly",
		"IsFamilyAndAssembly",
		"IsFamilyOrAssembly",
		"IsStatic",
		"IsInitOnly",
		"IsLiteral",
		"name",
		"FieldInfo",
	};
	static const auto InfoCount = sizeof(InfoNames) / sizeof(const char*);
	if (infoCount)
		*infoCount = InfoCount;
	if (infoName)
	{
		for (auto i = 0u; i < InfoCount; ++i)
			infoName[i] = InfoNames[i];
	}
	auto i = 0u;
	for (auto& it : filedInfos)
	{
		if (info)
		{
			info[i][0] = it->ToString;
			info[i][1] = it->MemberType;
			info[i][2] = it->Attributes;
			info[i][3] = (bool)it->IsSpecialName;
			info[i][4] = (bool)it->IsPinvokeImpl;
			info[i][5] = (bool)it->IsPublic;
			info[i][6] = (bool)it->IsPrivate;
			info[i][7] = (bool)it->IsFamily;
			info[i][8] = (bool)it->IsAssembly;
			info[i][9] = (bool)it->IsFamilyAndAssembly;
			info[i][10] = (bool)it->IsFamilyOrAssembly;
			info[i][11] = (bool)it->IsStatic;
			info[i][12] = (bool)it->IsInitOnly;
			info[i][13] = (bool)it->IsLiteral;
			info[i][14] = it->name;
			info[i][15] = (IUnknown*)it;
		}
		++i;
	}
	return filedInfos.size();
}

int32_t ClrClass::getPropertyInfo(int32_t* infoCount, const char** infoName, _variant_t** info)
{
	static const char* InfoNames[] = {
		"ToString",
		"MemberType",
		"Attributes",
		"CanRead",
		"CanWrite",
		"IsSpecialName",
		"name",
		"PropertyInfo",
	};
	static const auto InfoCount = sizeof(InfoNames) / sizeof(const char*);
	if (infoCount)
		*infoCount = InfoCount;
	if (infoName)
	{
		for (auto i = 0u; i < InfoCount; ++i)
			infoName[i] = InfoNames[i];
	}
	auto i = 0u;
	for (auto& it : propertyInfos)
	{
		if (info)
		{
			info[i][0] = it->ToString;
			info[i][1] = it->MemberType;
			info[i][2] = it->Attributes;
			info[i][3] = (bool)it->CanRead;
			info[i][4] = (bool)it->CanWrite;
			info[i][5] = (bool)it->IsSpecialName;
			info[i][6] = it->name;
			info[i][7] = (IUnknown*)it;
		}
		++i;
	}
	return propertyInfos.size();
}

bool ClrClass::createInstance(_variant_t** argv, int32_t argc, _variant_t* ret)
{
	helper::SafeArrayVector arr(VT_VARIANT, argc);
	_hr = arr.putVariants(argv, argc);
	if (FAILED(_hr))
	{
		CERR("Failed to add element to safe array");
		return false;
	}
	_variant_t empty;
	if (!ret)
		ret = &empty;
	try
	{
		*ret = _type->GetAssembly()->CreateInstance_3(_type->FullName, false,
			(BindingFlags)(BindingFlags_CreateInstance),
			nullptr, arr.getArray(), nullptr, nullptr
		);
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::invokeStaticMethod(const std::string& methodName, _variant_t** argv, int32_t argc, _variant_t* ret)
{
	_bstr_t name = helper::MultiByteToBSTR(methodName);
	helper::SafeArrayVector arr(VT_VARIANT, argc);
	_hr = arr.putVariants(argv, argc);
	if (FAILED(_hr))
	{
		CERR("Failed to add element to safe array");
		return false;
	}
	_variant_t empty;
	if (!ret)
		ret = &empty;
	try
	{
		*ret = _type->InvokeMember_3(name, static_fn_call, nullptr, empty, arr.getArray());
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::invokeMethod(_variant_t* instance, const std::string& methodName,
	_variant_t** argv, int32_t argc, _variant_t* ret)
{
	if(!instance)
		return false;
	_bstr_t name = helper::MultiByteToBSTR(methodName);
	helper::SafeArrayVector arr(VT_VARIANT, argc);
	_hr = arr.putVariants(argv, argc);
	if (FAILED(_hr))
	{
		CERR("Failed to add element to safe array");
		return false;
	}
	_variant_t empty;
	if (!ret)
		ret = &empty;
	try
	{
		*ret = _type->InvokeMember_3(name, mem_fn_call, nullptr, *instance, arr.getArray());
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::getStaticField(const std::string& fieldName, _variant_t* ret)
{
	_bstr_t name = helper::MultiByteToBSTR(fieldName);
	_variant_t empty;
	if (!ret)
		ret = &empty;
	try
	{
		*ret = _type->InvokeMember_3(name,
			(BindingFlags)(BindingFlags_GetField | BindingFlags_Public | BindingFlags_Static),
			nullptr, empty, nullptr);
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::getField(_variant_t* instance, const std::string& fieldName, _variant_t* ret)
{
	if (!instance)
		return false;
	_bstr_t name = helper::MultiByteToBSTR(fieldName);
	_variant_t empty;
	if (!ret)
		ret = &empty;
	try
	{
		*ret = _type->InvokeMember_3(name,
			(BindingFlags)(BindingFlags_GetField | BindingFlags_Public | BindingFlags_Instance),
			nullptr, *instance, nullptr);
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::setStaticField(const std::string& fieldName, _variant_t* arg)
{
	_bstr_t name = helper::MultiByteToBSTR(fieldName);
	_variant_t empty;
	if (!arg)
		arg = &empty;
	helper::SafeArrayVector arr(VT_VARIANT, 1);
	_hr = arr.putVariants(&arg, 1);
	if (FAILED(_hr))
	{
		CERR("Failed to add element to safe array");
		return false;
	}
	try
	{
		_type->InvokeMember_3(name,
			(BindingFlags)(BindingFlags_SetField | BindingFlags_Public | BindingFlags_Static),
			nullptr, empty, arr.getArray());
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::setField(_variant_t* instance, const std::string& fieldName, _variant_t* arg)
{
	if (!instance)
		return false;
	_bstr_t name = helper::MultiByteToBSTR(fieldName);
	_variant_t empty;
	if (!arg)
		arg = &empty;
	helper::SafeArrayVector arr(VT_VARIANT, 1);
	_hr = arr.putVariants(&arg, 1);
	if (FAILED(_hr))
	{
		CERR("Failed to add element to safe array");
		return false;
	}
	try
	{
		_type->InvokeMember_3(name,
			(BindingFlags)(BindingFlags_SetField | BindingFlags_Public | BindingFlags_Instance),
			nullptr, *instance, arr.getArray());
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::getStaticProperty(const std::string& propertyName, _variant_t* ret)
{
	_bstr_t name = helper::MultiByteToBSTR(propertyName);
	_variant_t empty;
	if (!ret)
		ret = &empty;
	try
	{
		*ret = _type->InvokeMember_3(name,
			(BindingFlags)(BindingFlags_GetProperty | BindingFlags_Public | BindingFlags_Static),
			nullptr, empty, nullptr);
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::getProperty(_variant_t* instance, const std::string& propertyName, _variant_t* ret)
{
	if (!instance)
		return false;
	_bstr_t name = helper::MultiByteToBSTR(propertyName);
	_variant_t empty;
	if (!ret)
		ret = &empty;
	try
	{
		*ret = _type->InvokeMember_3(name,
			(BindingFlags)(BindingFlags_GetProperty | BindingFlags_Public | BindingFlags_Instance),
			nullptr, *instance, nullptr);
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::setStaticProperty(const std::string& propertyName, _variant_t* arg)
{
	_bstr_t name = helper::MultiByteToBSTR(propertyName);
	_variant_t empty;
	if (!arg)
		arg = &empty;
	helper::SafeArrayVector arr(VT_VARIANT, 1);
	_hr = arr.putVariants(&arg, 1);
	if (FAILED(_hr))
	{
		CERR("Failed to add element to safe array");
		return false;
	}
	try
	{
		_type->InvokeMember_3(name,
			(BindingFlags)(BindingFlags_SetProperty | BindingFlags_Public | BindingFlags_Static),
			nullptr, empty, arr.getArray());
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::setProperty(_variant_t* instance, const std::string& propertyName, _variant_t* arg)
{
	if (!instance)
		return false;
	_bstr_t name = helper::MultiByteToBSTR(propertyName);
	_variant_t empty;
	if (!arg)
		arg = &empty;
	helper::SafeArrayVector arr(VT_VARIANT, 1);
	_hr = arr.putVariants(&arg, 1);
	if (FAILED(_hr))
	{
		CERR("Failed to add element to safe array");
		return false;
	}
	try
	{
		_type->InvokeMember_3(name,
			(BindingFlags)(BindingFlags_SetProperty | BindingFlags_Public | BindingFlags_Instance),
			nullptr, *instance, arr.getArray());
	}
	CATCH_COM_ERR;
	return true;
}

bool ClrClass::isInstanceOf(_variant_t* arg)
{
	if (!arg) return false;
	try
	{
		return _type->IsInstanceOfType(*arg);
	}
	CATCH_COM_ERR;
}

const char* ClrClass::getLastError() const
{
	return _lastError.c_str();
}
