#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#include "../defines.h"

namespace Rivet {
	enum class SignatureType {
		IDA,
		x64Dbg
	};

	class SignatureScanner {
	public:
		SignatureScanner(const wchar_t* moduleName);
		SignatureScanner(const char* moduleName);
		SignatureScanner(const std::wstring& moduleName);
		SignatureScanner(const std::string& moduleName);

		SignatureScanner(const DWORD64 startAddress, const DWORD64 endAddress);

		template<SignatureType type>
		DWORD64 ScanPattern(const std::string& pattern, DWORD64 offset = 0) {
			std::vector<uint8_t> bytes;
			std::vector<uint8_t> mask;

			if constexpr (type == SignatureType::IDA || type == SignatureType::x64Dbg) {
				if (!ParseIDAStyle(pattern, bytes, mask))
					return 0;
			} else {
				static_assert(false, "Unsupported SignatureType");
			}

			return ScanPatternRaw(bytes, mask, offset);
		}
	private:
		static bool ParseIDAStyle(const std::string& pattern, std::vector<uint8_t>& bytes, std::vector<uint8_t>& mask);

		RIVET_LIB_API DWORD64 ScanPatternRaw(const std::vector<uint8_t>& bytes, const std::vector<uint8_t>& mask, DWORD64 offset);

		DWORD64 addressStart_ = 0;
		DWORD64 addressEnd_ = 0;
	public:
	};
}