#include "stdafx.h"
#include "ClrAssembly.h"

using namespace mscorlib;

ClrAssembly::ClrAssembly(_AssemblyPtr p): p_(p)
{
}

bool ClrAssembly::initClass()
{
	_TypePtr pClsType = nullptr;
	_TypePtr* pTypes = nullptr;
	BSTR pName = nullptr;
	SAFEARRAY* pArray = nullptr;
	long lower_bound = 0;
	long upper_bound = 0;
	try
	{
		pArray = p_->GetTypes();
	}
	catch (const _com_error& e)
	{
		CERR_E("Failed to get types");
		_hr = e.Error();
		return false;
	}
	if (nullptr == pArray)
	{
		CERR("Failed to get types");
		return false;
	}
	SafeArrayGetLBound(pArray, 1, &lower_bound);
	SafeArrayGetUBound(pArray, 1, &upper_bound);
	SafeArrayAccessData(pArray, (void**)&pTypes);
	auto elem_count = upper_bound - lower_bound + 1;
	for (auto i = 0; i < elem_count; ++i)
	{
		pClsType = pTypes[i];
		if (FAILED((_hr = pClsType->get_FullName(&pName))) || !pName)
		{
			CERR("Failed to get types");
			break;
		}
		int attr = pClsType->Attributes;
		attr = attr & TypeAttributes_VisibilityMask;
		if (attr == TypeAttributes_NotPublic || attr == TypeAttributes_NestedPrivate)
			continue;
		std::string fullName = helper::WideCharToMultiByte_UTF8(pName);
		_clss[fullName] = std::make_shared<ClrClass>(pClsType);
	}
	SafeArrayUnaccessData(pArray);
	return true;
}

ClrClass* ClrAssembly::getClass(const std::string& className)
{
	const auto it = _clss.find(className);
	if (it != _clss.end())
		return it->second.get();
	return nullptr;
}

int32_t ClrAssembly::getClasses(ClrClass** ret)
{
	if (ret)
	{
		auto i = 0u;
		for (auto& it : _clss)
		{
			ret[i] = it.second.get();
			++i;
		}
	}
	return _clss.size();
}

void ClrAssembly::clearClass()
{
	_clss.clear();
}

const char* ClrAssembly::getLastError() const
{
	return _lastError.c_str();
}
