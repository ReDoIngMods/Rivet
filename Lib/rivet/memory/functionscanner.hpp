#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#include "../defines.h"

namespace Rivet {
	struct FunctionData {
		ULONG_PTR startAddress;
		ULONG_PTR endAddress;
	};

	class FunctionScanner {
	public:
		RIVET_LIB_API ~FunctionScanner() = default;

		RIVET_LIB_API explicit FunctionScanner(const std::wstring& moduleName);
		RIVET_LIB_API explicit FunctionScanner(const std::string& moduleName);

		RIVET_LIB_API const std::vector<Rivet::FunctionData> getFunctions() const;
	private:
		void Scan();
		
		DWORD64 baseAddress_ = 0;
		struct PDataInfo {
			DWORD64 address = 0;
			DWORD entryCount = 0;
		} pdataInfo;

		void InitFunctionScanner(DWORD64 baseAddress);
	};
}