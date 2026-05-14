#include <rivet/memory/sigScanner.h>
#include <rivet/memory/peHeaders.h>

#include <sstream>
#include <mutex>

using namespace Rivet;

namespace {
std::unordered_map<size_t, DWORD64> SignatureCache;
std::mutex SignatureCacheMutex;

size_t HashPattern(const std::vector<uint8_t>& bytes, const std::vector<uint8_t>& mask) {
	size_t hash = 0xcbf29ce484222325; // FNV-1a offset basis

	for (size_t index = 0; index < bytes.size(); ++index) {
		uint8_t b = mask[index] ? 0xFF : bytes[index];
		hash ^= b;
		hash *= 0x100000001b3; // FNV prime
	}

	return hash;
}
} // namespace

RIVET_LIB_API SignatureScanner::SignatureScanner(const std::string& moduleName) {
	PEHeaderManager& peHeaderMgr = PEHeaderManager::GetInstance();

	PEHeadersMap headers;
	if (!peHeaderMgr.QueryModuleHeaders("ScrapMechanic.exe", headers))
		return;

	const auto [startAddress, endAddress] = headers[".text"];
	addressStart_ = startAddress;
	addressEnd_ = endAddress;
}

RIVET_LIB_API SignatureScanner::SignatureScanner(const DWORD64 startAddress, const DWORD64 endAddress) {
	addressStart_ = startAddress;
	addressEnd_ = endAddress;
}

RIVET_LIB_API bool SignatureScanner::ParseIDAStyle(const std::string& pattern, std::vector<uint8_t>& bytes, std::vector<uint8_t>& mask) {
	std::istringstream iss(pattern);
	std::string token;

	while (iss >> token) {
		if (token == "?" || token == "??") {
			bytes.push_back(0);
			mask.push_back(0);
		} else {
			if (token.size() > 2 || token.empty())
				return false;

			const unsigned long value = stoul(token, nullptr, 16);
			if (value > 0xFF)
				return false;

			bytes.push_back(static_cast<uint8_t>(value));
			mask.push_back(1);
		}
	}

	return true;
}

RIVET_LIB_API DWORD64 SignatureScanner::ScanPatternRaw(const std::vector<uint8_t>& bytes, const std::vector<uint8_t>& mask, DWORD64 offset, bool useCache, bool cacheResult) const {
	if (bytes.empty() || mask.empty() || bytes.size() != mask.size())
		return 0;

	const size_t patternKey = HashPattern(bytes, mask);
	if (useCache) {
		std::scoped_lock lock(SignatureCacheMutex);

		const auto it = SignatureCache.find(patternKey);
		if (it != SignatureCache.end())
			return it->second;
	}

	const size_t patternSize = bytes.size();
	constexpr size_t tableSize = 256;

	uint8_t badCharacters[tableSize];
	std::fill_n(badCharacters, tableSize, static_cast<uint8_t>(patternSize));

	for (size_t index = 0; index < patternSize - 1; ++index) {
		if (mask[index])
			badCharacters[bytes[index]] = static_cast<uint8_t>(patternSize - 1 - index);
	}

	auto start = reinterpret_cast<uint8_t*>(addressStart_);
	auto end = reinterpret_cast<uint8_t*>(addressEnd_ - patternSize + 1);

	size_t index = 0;

	DWORD64 result = 0;
	while (start + index < end) {
		size_t patternIndex = patternSize;
		while (patternIndex > 0) {
			--patternIndex;

			if (mask[patternIndex] && bytes[patternIndex] != start[index + patternIndex])
				break;
		}

		if (patternIndex == 0) {
			result = reinterpret_cast<DWORD64>(start + index);
			break;
		}

		index += badCharacters[start[index + patternSize - 1]];
	}

	if (cacheResult && result != 0) {
		std::scoped_lock lock(SignatureCacheMutex);
		SignatureCache[patternKey] = result;
	}

	return result;
}
