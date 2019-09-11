#pragma once
#include <string>
#include "common.h"

namespace helper
{
	std::wstring MultiByteToWideChar_UTF8(const std::string& Org);
	std::string WideCharToMultiByte_UTF8(const std::wstring& Org);

	_bstr_t MultiByteToBSTR(const std::string& Org);
	std::string BSTRToMultiByte(const _bstr_t& Org);

	class SafeArrayVector
	{
		VARENUM vt;
		ULONG size = 0;
		SAFEARRAY* arr = nullptr;
	public:
		SafeArrayVector(VARENUM vt_, ULONG size_);
		~SafeArrayVector();

		VARENUM getType() const { return vt; }
		ULONG getSize() const { return size; }
		SAFEARRAY* getArray() const { return arr; }
		HRESULT putElement(LONG i, void* v);
		HRESULT putVariants(_variant_t** v, size_t count, size_t start = 0);
	};
}
