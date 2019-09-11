#include "stdafx.h"
#include "helper.h"
#include <codecvt>

using namespace std;
using namespace helper;

wstring_convert<codecvt_utf8<wchar_t>> cv;

wstring helper::MultiByteToWideChar_UTF8(const string& Org)
{
	wstring ret;
	try
	{
		ret = cv.from_bytes(Org);
	}
	catch (const range_error&)
	{
		ret = L"";
	}
	return ret;
}

string helper::WideCharToMultiByte_UTF8(const wstring& Org)
{
	string ret;
	try
	{
		ret = cv.to_bytes(Org);
	}
	catch (const range_error&)
	{
		ret = "";
	}
	return ret;
}

_bstr_t helper::MultiByteToBSTR(const std::string& Org)
{
	return MultiByteToWideChar_UTF8(Org).c_str();
}

std::string helper::BSTRToMultiByte(const _bstr_t& Org)
{
	return WideCharToMultiByte_UTF8((const wchar_t*)Org);
}

SafeArrayVector::SafeArrayVector(VARENUM vt_, ULONG size_)
{
	if (size_ > 0)
		arr = SafeArrayCreateVector(vt_, 0, size_);
	if(arr)
	{
		vt = vt_;
		size = size_;
	}
}

SafeArrayVector::~SafeArrayVector()
{
	if (arr)
	{
		SafeArrayDestroy(arr);
	}
}

HRESULT SafeArrayVector::putElement(LONG i, void* v)
{
	if (arr)
		return SafeArrayPutElement(arr, &i, v);
	return -1;
}

HRESULT SafeArrayVector::putVariants(_variant_t** v, size_t count, size_t start)
{
	if (count == 0)
		return S_OK;
	if (!v || vt != VT_VARIANT || !arr)
		return -1;
	for (auto i = start; i < start + count; ++i)
	{
		const auto hr = putElement(i, v[i - start]);
		if (FAILED(hr))
			return hr;
	}
	return S_OK;
}

