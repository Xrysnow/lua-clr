#pragma once

#include <Windows.h>
#include <wrl.h>
#include <metahost.h>
#include <mscoree.h>
#include <comdef.h>
#include "helper.h"

#import "mscorlib.tlb" auto_rename exclude("ITrackingHandler")
#pragma comment(lib, "mscoree.lib")

#include <cstdint>
#include <vector>
#include <memory>
#include <array>
#include <tchar.h>
#include <unordered_map>

constexpr wchar_t clr_default_version[] = L"v4.0.30319";
constexpr unsigned long clr_ver_reservation = 30;
constexpr mscorlib::BindingFlags mem_fn_call = static_cast<mscorlib::BindingFlags>(
	mscorlib::BindingFlags_InvokeMethod | mscorlib::BindingFlags_Instance | mscorlib::BindingFlags_Public);
constexpr mscorlib::BindingFlags static_fn_call = static_cast<mscorlib::BindingFlags>(
	mscorlib::BindingFlags_InvokeMethod | mscorlib::BindingFlags_Static | mscorlib::BindingFlags_Public);


#define HR_ERROR(msg, hr) _lastError = msg; _hr = hr;
#define CERR(msg) _lastError = std::string(__FUNCTION__) + " | " + msg;
#define CERR_E(msg) _lastError = std::string(__FUNCTION__) + " | " + msg + " | " + std::string(e.Description());

#ifdef CLRHELPER_EXPORTS
#define CLRHELPER_API __declspec(dllexport)
#else
#define CLRHELPER_API __declspec(dllimport)
#endif
