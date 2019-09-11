#pragma once
#include "common.h"
#include <set>

class ClrClass
{
	mscorlib::_TypePtr _type;
	std::vector<mscorlib::_MethodInfoPtr> methodInfos;
	std::vector<mscorlib::_FieldInfoPtr> filedInfos;
	std::vector<mscorlib::_PropertyInfoPtr> propertyInfos;

	HRESULT _hr;
	std::string _lastError;

	bool initMethods();
	bool initFields();
	bool initProperties();

public:
	ClrClass(mscorlib::_TypePtr pt);

	mscorlib::_TypePtr getType();

	static void getTypeInfo(mscorlib::_TypePtr pt, 
		int32_t* infoCount, const char** infoName, _variant_t* info);
	void getTypeInfo(int32_t* infoCount, const char** infoName, _variant_t* info);

	int32_t getMethodInfo(
		int32_t* infoCount, const char** infoName, _variant_t** info);
	int32_t getFieldInfo(
		int32_t* infoCount, const char** infoName, _variant_t** info);
	int32_t getPropertyInfo(
		int32_t* infoCount, const char** infoName, _variant_t** info);

	bool createInstance(_variant_t** argv, int32_t argc, _variant_t* ret);

	bool invokeStaticMethod(const std::string& methodName,
		_variant_t** argv, int32_t argc, _variant_t* ret);
	bool invokeMethod(_variant_t* instance, const std::string& methodName,
		_variant_t** argv, int32_t argc, _variant_t* ret);

	bool getStaticField(
		const std::string& fieldName, _variant_t* ret);
	bool getField(_variant_t* instance,
		const std::string& fieldName, _variant_t* ret);
	bool setStaticField(
		const std::string& fieldName, _variant_t* arg);
	bool setField(_variant_t* instance,
		const std::string& fieldName, _variant_t* arg);

	bool getStaticProperty(
		const std::string& propertyName, _variant_t* ret);
	bool getProperty(_variant_t* instance,
		const std::string& propertyName, _variant_t* ret);
	bool setStaticProperty(
		const std::string& propertyName, _variant_t* arg);
	bool setProperty(_variant_t* instance,
		const std::string& propertyName, _variant_t* arg);

	bool isInstanceOf(_variant_t* arg);

	const char* getLastError() const;
};
