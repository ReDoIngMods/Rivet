#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "../defines.h"

namespace Rivet {
enum class SignatureType {
	IDA,
	X64Dbg
};

class SignatureScanner {
public:
	~SignatureScanner() = default;

	explicit RIVET_LIB_API SignatureScanner(const std::string& moduleName);
	explicit RIVET_LIB_API SignatureScanner(DWORD64 startAddress, DWORD64 endAddress);

	template <SignatureType Type>
	DWORD64 ScanPattern(const std::string& pattern, DWORD64 offset = 0, bool useCache = true, bool cacheResult = true) const {
		std::vector<uint8_t> bytes;
		std::vector<uint8_t> mask;

		if constexpr (Type == SignatureType::IDA || Type == SignatureType::X64Dbg) {
			if (!ParseIDAStyle(pattern, bytes, mask))
				return 0;
		} else {
			static_assert(false, "Unsupported SignatureType");
		}

		return ScanPatternRaw(bytes, mask, offset, useCache, cacheResult);
	}

private:
	RIVET_LIB_API static bool ParseIDAStyle(const std::string& pattern, std::vector<uint8_t>& bytes, std::vector<uint8_t>& mask);

	RIVET_LIB_API DWORD64 ScanPatternRaw(const std::vector<uint8_t>& bytes, const std::vector<uint8_t>& mask, DWORD64 offset, bool useCache, bool cacheResult) const;

	DWORD64 addressStart_ = 0;
	DWORD64 addressEnd_ = 0;
};
} // namespace Rivet