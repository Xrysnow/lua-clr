#pragma once
#include "common.h"
#include "ClrClass.h"

class ClrAssembly
{
	mscorlib::_AssemblyPtr p_;

	HRESULT _hr;
	std::string _lastError;

	std::unordered_map<std::string, std::shared_ptr<ClrClass>> _clss;
public:

	ClrAssembly(mscorlib::_AssemblyPtr p);
	bool initClass();

	ClrClass* getClass(const std::string& className);
	int32_t getClasses(ClrClass** ret);
	void clearClass();

	const char* getLastError() const;
};
