#include <rivet/memory/PeHeaders.h>

#include <ranges>
#include <utility>

RIVET_LIB_API std::vector<std::string> Rivet::PEHeaderManager::GetLoadedModuleNames() {
	std::vector<std::string> output;

	for (const auto& name : this->modules_ | std::views::keys)
		output.emplace_back(name);

	return output;
}

RIVET_LIB_API bool Rivet::PEHeaderManager::LoadHeadersForModule(const std::string& moduleName) {
	const DWORD64 baseAddress = reinterpret_cast<DWORD64>(GetModuleHandleA(moduleName.data()));
	if (baseAddress == 0)
		return false;

	PEHeadersMap headers;

	auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(baseAddress);
	auto ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(baseAddress + dosHeader->e_lfanew);

	auto pSection = IMAGE_FIRST_SECTION(ntHeader);
	for (int i = 0; std::cmp_less(i, ntHeader->FileHeader.NumberOfSections); ++i, ++pSection) {
		PEHeader header;
		header.startAddress = baseAddress + pSection->VirtualAddress;
		header.endAddress = header.startAddress + pSection->Misc.VirtualSize;

		std::string name;
		name.assign(reinterpret_cast<const char*>(pSection->Name), strlen(reinterpret_cast<const char*>(pSection->Name)));

		headers[name] = header;
	}

	this->modules_[moduleName] = headers;
	return true;
}

RIVET_LIB_API bool Rivet::PEHeaderManager::QueryModuleHeaders(const std::string& moduleName, PEHeadersMap& headers) {
	const auto& iterator = this->modules_.find(moduleName);
	if (iterator != this->modules_.end()) {
		headers = iterator->second;
		return true;
	}

	if (!this->LoadHeadersForModule(moduleName))
		return false;

	headers = this->modules_[moduleName];
	return true;
}
