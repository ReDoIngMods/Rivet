#pragma once
#include <Windows.h>
#include <unordered_map>
#include <string>

#include "../defines.h"

namespace Rivet {
struct PEHeader {
	DWORD64 startAddress = 0;
	DWORD64 endAddress = 0;
};

typedef std::unordered_map<std::string, PEHeader> PEHeadersMap;

class PEHeaderManager {
public:
	RIVET_LIB_API static PEHeaderManager& GetInstance() {
		static PEHeaderManager instance;
		return instance;
	}

	RIVET_LIB_API std::vector<std::string> GetLoadedModuleNames();

	RIVET_LIB_API bool LoadHeadersForModule(const std::string& moduleName);
	RIVET_LIB_API bool QueryModuleHeaders(const std::string& moduleName, PEHeadersMap& headers);

private:
	std::unordered_map<std::string, PEHeadersMap> modules_;
};
} // namespace Rivet