#include "stdafx.h"
#include "ClrDomain.h"
#include <csignal>
#include <ctime>

#ifdef ERROR
#undef ERROR
#endif // ERROR
#define ERROR(msg) _lastError = msg;

using namespace mscorlib;

ClrDomain::ClrDomain()
{
}

ClrDomain::~ClrDomain()
{
	if (pHost_)
		pHost_->Stop();
}

std::wstring ClrDomain::find_runtime()
{
	HRESULT hr = S_OK;
	int g_maj = 0;
	int g_min = 0;
	int g_build = 0;
	std::wstring ver = clr_default_version;

	if (!pMeta_)
		return ver;

	IEnumUnknown* pRuntimes = nullptr;
	ICLRRuntimeInfo* pInfo = nullptr;
	ULONG fetched = 0;

	if (FAILED((hr = pMeta_->EnumerateInstalledRuntimes(&pRuntimes))))
	{
		_lastError = "Failed to enumerate installed runtimes!";
		_hr = hr;
		return ver;
	}

	while (SUCCEEDED((hr = pRuntimes->Next(1, (IUnknown**)&pInfo, &fetched))) && 0 != fetched)
	{
		wchar_t ver_string[clr_ver_reservation] = {0};
		DWORD ver_size = clr_ver_reservation;
		int c_min = 0, c_maj = 0, c_build = 0;
		if (FAILED((hr = pInfo->GetVersionString(ver_string, &ver_size))))
		{
			HR_ERROR("Failed to get version string!", hr);
			continue;
		}
		swscanf_s(ver_string, L"v%d.%d.%d", &c_maj, &c_min, &c_build);
		if (c_maj > g_maj)
		{
			g_maj = c_maj;
			g_min = c_min;
			g_build = c_build;
			ver = ver_string;
		}
		else if (c_maj == g_maj)
		{
			if (c_min > g_min || (c_min == g_min && c_build > g_build))
			{
				g_min = c_min;
				g_build = c_build;
				ver = ver_string;
			}
		}
	}

	return ver;
}

std::shared_ptr<ClrAssembly> ClrDomain::constructAssembly(uint8_t* mod, size_t modSize)
{
	if (!inited || !mod)
		return nullptr;
	IUnknownPtr pDomainThunk = nullptr;
	_AppDomainPtr pAppDomain = nullptr;
	_AssemblyPtr pAsm = nullptr;
	HRESULT hr = S_OK;
	SAFEARRAY* pModContainer = nullptr;

	if (modSize > ULONG_MAX)
	{
		ERROR("Failed to load module, file size is too large!");
		return nullptr;
	}
	hr = pHost_->GetDefaultDomain(&pDomainThunk);
	if (FAILED(hr))
	{
		HR_ERROR("Failed to get default appdomain!", hr);
		return nullptr;
	}
	hr = pDomainThunk->QueryInterface(IID_PPV_ARGS(&pAppDomain));
	if (FAILED(hr))
	{
		HR_ERROR("Failed to get app domain interface from thunk!", hr);
		return nullptr;
	}

	if (nullptr == (pModContainer = SafeArrayCreateVector(VT_UI1, 0, static_cast<ULONG>(modSize))))
	{
		ERROR("Failed to allocate safe array vector!");
		return nullptr;
	}

	unsigned char* buf = nullptr;
	if (FAILED((hr = SafeArrayAccessData(pModContainer, reinterpret_cast<void**>(&buf)))))
	{
		HR_ERROR("Failed to access safe array!", hr);
		return nullptr;
	}

	memcpy(buf, mod, modSize);
	SafeArrayUnaccessData(pModContainer);
	try
	{
		pAsm = pAppDomain->Load_3(pModContainer);
	}
	catch (const _com_error& e)
	{
		ERROR("Failed to load assembly!");
		return nullptr;
	}
	if (nullptr == pAsm)
	{
		ERROR("Failed to load assembly!");
		return nullptr;
	}
	arr_.push_back(std::shared_ptr<SAFEARRAY>(pModContainer, [](auto _p) { if (_p) SafeArrayDestroy(_p); }));
	return std::make_shared<ClrAssembly>(pAsm);
}

std::shared_ptr<ClrAssembly> ClrDomain::constructAssembly(const std::string& name)
{
	if (!inited || name.empty())
		return nullptr;
	IUnknownPtr pDomainThunk = nullptr;
	_AppDomainPtr pAppDomain = nullptr;
	_AssemblyPtr pAsm = nullptr;
	HRESULT hr = S_OK;

	hr = pHost_->GetDefaultDomain(&pDomainThunk);
	if (FAILED(hr))
	{
		HR_ERROR("Failed to get default appdomain!", hr);
		return nullptr;
	}
	hr = pDomainThunk->QueryInterface(IID_PPV_ARGS(&pAppDomain));
	if (FAILED(hr))
	{
		HR_ERROR("Failed to get app domain interface from thunk!", hr);
		return nullptr;
	}
	bstr_t bstrAssemblyName(name.c_str());
	try
	{
		pAsm = pAppDomain->Load_2(bstrAssemblyName);
	}
	catch (const _com_error& e)
	{
		_lastError = "Failed to load assembly! | " +
			e.Description();
		return nullptr;
	}
	if (nullptr == pAsm)
	{
		ERROR("Failed to load assembly!");
		return nullptr;
	}
	return std::make_shared<ClrAssembly>(pAsm);
}

bool ClrDomain::init(const std::string& clrVersion)
{
	HRESULT hr = S_OK;
	BOOL loadable = FALSE;
	std::wstring clr_version;

	hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(pMeta_.GetAddressOf()));
	if (FAILED(hr))
	{
		HR_ERROR("Failed to initialize metahost!", hr);
		return false;
	}

	if (!clrVersion.empty())
		clr_version = helper::MultiByteToWideChar_UTF8(clrVersion);
	else
		clr_version = find_runtime();

	hr = pMeta_->GetRuntime(clr_version.c_str(), IID_PPV_ARGS(pRuntime_.GetAddressOf()));
	if (FAILED(hr))
	{
		_lastError = "Runtime initialization failed! clr version: "
			+ clrVersion + ", hr: " + std::to_string(hr);
		_hr = hr;
		return false;
	}
	hr = pRuntime_->IsLoadable(&loadable);
	if (FAILED(hr) || !loadable)
	{
		HR_ERROR("Runtime not loadable!", hr);
		return false;
	}
	hr = pRuntime_->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(pHost_.GetAddressOf()));
	if (FAILED(hr))
	{
		HR_ERROR("Failed to get runtime host!", hr);
		return false;
	}
	hr = pHost_->Start();
	if (FAILED(hr))
	{
		HR_ERROR("Host failed to start!", hr);
		return false;
	}
	inited = true;
	_clrVersion = helper::WideCharToMultiByte_UTF8(clr_version);
	return true;
}

ClrAssembly* ClrDomain::loadAssembly(const std::string& assemblyName, uint8_t* mod, size_t modSize)
{
	std::shared_ptr<ClrAssembly> a;
	if (!mod || modSize == 0)
		a = constructAssembly(assemblyName);
	else
		a = constructAssembly(mod, modSize);
	if (!a)
		return nullptr;
	_ass[assemblyName] = a;
	return a.get();
}

ClrAssembly* ClrDomain::getAssembly(const std::string& assemblyName)
{
	const auto it = _ass.find(assemblyName);
	if (it == _ass.end())
		return nullptr;
	return it->second.get();
}

void ClrDomain::unloadAssembly(const std::string& assemblyName)
{
	_ass.erase(assemblyName);
}

const char* ClrDomain::getClrVersion() const
{
	return _clrVersion.c_str();
}

const char* ClrDomain::getLastError() const
{
	return _lastError.c_str();
}
