#include "sigscanner.h"
#include <sstream>

inline static void GetTextRange(const DWORD64 baseAddress, DWORD64& startAddress, DWORD64& endAddress) {
	IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(baseAddress);
	IMAGE_NT_HEADERS* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(baseAddress + dosHeader->e_lfanew);

	IMAGE_SECTION_HEADER* pSection = IMAGE_FIRST_SECTION(ntHeader);
	for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i, ++pSection) {
		if (memcmp(pSection->Name, ".text", 5) != 0)
			continue;

		startAddress = baseAddress + pSection->VirtualAddress;
		endAddress = startAddress + pSection->Misc.VirtualSize;

		break;
	}
}

Rivet::SignatureScanner::SignatureScanner(const wchar_t* moduleName) {
	GetTextRange(reinterpret_cast<DWORD64>(GetModuleHandle(moduleName)), addressStart_, addressEnd_);
}

Rivet::SignatureScanner::SignatureScanner(const char* moduleName) {
	GetTextRange(reinterpret_cast<DWORD64>(GetModuleHandleA(moduleName)), addressStart_, addressEnd_);
}

Rivet::SignatureScanner::SignatureScanner(const std::wstring& moduleName) {
	GetTextRange(reinterpret_cast<DWORD64>(GetModuleHandle(moduleName.c_str())), addressStart_, addressEnd_);
}

Rivet::SignatureScanner::SignatureScanner(const std::string& moduleName) {
	GetTextRange(reinterpret_cast<DWORD64>(GetModuleHandleA(moduleName.c_str())), addressStart_, addressEnd_);
}

Rivet::SignatureScanner::SignatureScanner(const DWORD64 startAddress, const DWORD64 endAddress) {
	addressStart_ = startAddress;
	addressEnd_ = endAddress;
}

bool Rivet::SignatureScanner::ParseIDAStyle(const std::string& pattern, std::vector<uint8_t>& bytes, std::vector<uint8_t>& mask) {
	std::istringstream iss(pattern);
	std::string token;

	while (iss >> token) {
		bool isMaskByte = token == "?" || token == "??";

		if (isMaskByte) {
			bytes.push_back(0);
			mask.push_back(0);
		} else {
			if (token.size() > 2 || token.empty())
				return false;

			unsigned long value = stoul(token, nullptr, 16);
			if (value > 0xFF)
				return false;

			bytes.push_back(static_cast<uint8_t>(value));
			mask.push_back(1);
		}
	}

	return true;
}

RIVET_LIB_API DWORD64 Rivet::SignatureScanner::ScanPatternRaw(const std::vector<uint8_t>& bytes, const std::vector<uint8_t>& mask, DWORD64 offset) {
	if (bytes.empty() || mask.empty() || bytes.size() != mask.size())
		return 0;

	const size_t patternSize = bytes.size();
	const size_t tableSize = 256;

	uint8_t badCharacters[tableSize];
	std::fill_n(badCharacters, tableSize, static_cast<uint8_t>(patternSize));
	
	for (size_t index = 0; index < patternSize - 1; ++index) {
		if (mask[index])
			badCharacters[bytes[index]] = static_cast<uint8_t>(patternSize - 1 - index);
	}

    uint8_t* start = reinterpret_cast<uint8_t*>(addressStart_);
    uint8_t* end = reinterpret_cast<uint8_t*>(addressEnd_ - patternSize + 1);

    size_t index = 0;

    while (start + index < end) {
        size_t patternIndex = patternSize;
        while (patternIndex > 0) {
            --patternIndex;
            if (mask[patternIndex] && bytes[patternIndex] != start[index + patternIndex])
                break;
        }

        if (patternIndex == 0)
            return reinterpret_cast<DWORD64>(start + index);

        index += badCharacters[start[index + patternSize - 1]];
    }

    return 0;
}
