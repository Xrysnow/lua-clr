#pragma once
#include "common.h"
#include "ClrAssembly.h"

class ClrDomain
{
	Microsoft::WRL::ComPtr<ICLRMetaHost>	pMeta_;
	Microsoft::WRL::ComPtr<ICLRRuntimeInfo> pRuntime_;
	Microsoft::WRL::ComPtr<ICorRuntimeHost> pHost_;
	std::vector<std::shared_ptr<SAFEARRAY>>	arr_;
	HRESULT _hr = S_OK;
	std::string _lastError;
	std::wstring _comError;
	bool inited = false;
	std::string _clrVersion;
	std::unordered_map<std::string, std::shared_ptr<ClrAssembly>> _ass;
public:
	ClrDomain();
	~ClrDomain();
private:

	std::wstring find_runtime();

	std::shared_ptr<ClrAssembly> constructAssembly(uint8_t* mod, size_t modSize);

	std::shared_ptr<ClrAssembly> constructAssembly(const std::string& name);
public:
	bool init(const std::string& clrVersion);

	ClrAssembly* loadAssembly(const std::string& assemblyName, uint8_t* mod, size_t modSize);
	ClrAssembly* getAssembly(const std::string& assemblyName);
	void unloadAssembly(const std::string& assemblyName);

	const char* getClrVersion() const;
	const char* getLastError() const;
};
