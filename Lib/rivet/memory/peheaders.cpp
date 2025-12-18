#include "peheaders.hpp"

RIVET_LIB_API std::vector<std::string> Rivet::PEHeaderManager::getLoadedModuleNames() {
	std::vector<std::string> output;
	for (const auto& [name, _] : this->modules_)
		output.emplace_back(name);

	return output;
}

RIVET_LIB_API bool Rivet::PEHeaderManager::loadHeadersForModule(const std::string& moduleName) {
	DWORD64 baseAddress = reinterpret_cast<DWORD64>(GetModuleHandleA(moduleName.data()));
	if (baseAddress == 0)
		return false;

	PEHeadersMap headers;

	IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(baseAddress);
	IMAGE_NT_HEADERS* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(baseAddress + dosHeader->e_lfanew);

	IMAGE_SECTION_HEADER* pSection = IMAGE_FIRST_SECTION(ntHeader);
	for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i, ++pSection) {
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

RIVET_LIB_API bool Rivet::PEHeaderManager::queryModuleHeaders(const std::string& moduleName, PEHeadersMap& headers) {
	const auto& iterator = this->modules_.find(moduleName);
	if (iterator != this->modules_.end()) {
		headers = iterator->second;
		return true;
	}

	if (!this->loadHeadersForModule(moduleName))
		return false;

	headers = this->modules_[moduleName];
	return true;
}
