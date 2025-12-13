#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "../defines.h"

namespace Rivet {
	enum class SignatureType {
		IDA,
		x64Dbg
	};

	class SignatureScanner {
	public:
		~SignatureScanner() = default;

		explicit RIVET_LIB_API SignatureScanner(const std::wstring& moduleName);
		explicit RIVET_LIB_API SignatureScanner(const std::string& moduleName);

		explicit RIVET_LIB_API SignatureScanner(const DWORD64 startAddress, const DWORD64 endAddress);

		template<SignatureType type>
		DWORD64 scanPattern(const std::string& pattern, DWORD64 offset = 0, bool useCache = true, bool cacheResult = true) {
			std::vector<uint8_t> bytes;
			std::vector<uint8_t> mask;

			if constexpr (type == SignatureType::IDA || type == SignatureType::x64Dbg) {
				if (!ParseIDAStyle(pattern, bytes, mask))
					return 0;
			} else {
				static_assert(false, "Unsupported SignatureType");
			}

			return scanPatternRaw(bytes, mask, offset, useCache, cacheResult);
		}
	private:
		RIVET_LIB_API static bool ParseIDAStyle(const std::string& pattern, std::vector<uint8_t>& bytes, std::vector<uint8_t>& mask);

		RIVET_LIB_API DWORD64 scanPatternRaw(const std::vector<uint8_t>& bytes, const std::vector<uint8_t>& mask, DWORD64 offset, bool useCache, bool cacheResult);

		DWORD64 addressStart_ = 0;
		DWORD64 addressEnd_ = 0;
	public:
	};
}