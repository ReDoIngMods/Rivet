#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#include "../defines.h"

namespace Rivet {
	struct FunctionData {
		DWORD64 startAddress;
		DWORD64 endAddress;
	};

	class FunctionScanner {
	public:
		RIVET_LIB_API ~FunctionScanner() = default;

		RIVET_LIB_API explicit FunctionScanner(std::wstring_view moduleName);
		RIVET_LIB_API explicit FunctionScanner(std::string_view moduleName);

		RIVET_LIB_API const std::vector<Rivet::FunctionData> getFunctions() const;
	private:
		void Scan();
		
		DWORD64 baseAddress_ = 0;
	};
}