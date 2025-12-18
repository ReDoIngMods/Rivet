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
		RIVET_LIB_API static Rivet::PEHeaderManager& getInstance() {
			static PEHeaderManager instance;
			return instance;
		}

		RIVET_LIB_API std::vector<std::string> getLoadedModuleNames();

		RIVET_LIB_API bool loadHeadersForModule(const std::string& moduleName);
		RIVET_LIB_API bool queryModuleHeaders(const std::string& moduleName, PEHeadersMap& headers);
	private:
		std::unordered_map<std::string, PEHeadersMap> modules_;
	public:
	};
}