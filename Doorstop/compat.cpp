#include "compat.h"

#include <Windows.h>
#include <type_traits>

using namespace Rivet;

#define GET_INDEX(name) \
	(static_cast<size_t>(std::distance(std::begin(names), std::find(std::begin(names), std::end(names), name))))
#define GET_FUNC_PTR(name) \
	fnPtrs[GET_INDEX(__func__)]
#define CALL_FUNC(name, ...) \
	reinterpret_cast<decltype(name)*>(GET_FUNC_PTR(__func__))(__VA_ARGS__)
#define EXPORT __declspec(dllexport)
#define FASTCALL __fastcall
#define STDCALL __stdcall
#define GET_REAL_FUNC_PTR(name) \
	P##name = reinterpret_cast<PVOID>(GetProcAddress(hModule, #name))

extern "C" {
PVOID PGetFileVersionInfoW = nullptr;
PVOID PGetFileVersionInfoA = nullptr;
PVOID PGetFileVersionInfoSizeW = nullptr;
PVOID PVerQueryValueA = nullptr;
PVOID PVerQueryValueW = nullptr;
PVOID PGetFileVersionInfoByHandle = nullptr;
PVOID PGetFileVersionInfoExA = nullptr;
PVOID PGetFileVersionInfoExW = nullptr;
PVOID PGetFileVersionInfoSizeA = nullptr;
PVOID PGetFileVersionInfoSizeExA = nullptr;
PVOID PGetFileVersionInfoSizeExW = nullptr;
PVOID PVerFindFileA = nullptr;
PVOID PVerFindFileW = nullptr;
PVOID PVerInstallFileA = nullptr;
PVOID PVerInstallFileW = nullptr;
}

void Compat::InitializeFunctionPointers() {
	wchar_t systemDirectory[MAX_PATH];
	GetSystemDirectory(systemDirectory, MAX_PATH);

	wchar_t versionDllPath[MAX_PATH];
	wcscpy_s(versionDllPath, systemDirectory);

	const size_t len = wcslen(versionDllPath);
	if (len > 0 && versionDllPath[len - 1] != L'\\')
		wcscat_s(versionDllPath, L"\\");

	wcscat_s(versionDllPath, TEXT(TARGET_NAME));

	const HMODULE hModule = LoadLibraryW(versionDllPath); // Load the real DLL
	if (!hModule)
		return;

	GET_REAL_FUNC_PTR(GetFileVersionInfoW);
	GET_REAL_FUNC_PTR(GetFileVersionInfoA);
	GET_REAL_FUNC_PTR(GetFileVersionInfoSizeW);
	GET_REAL_FUNC_PTR(VerQueryValueA);
	GET_REAL_FUNC_PTR(VerQueryValueW);
	GET_REAL_FUNC_PTR(GetFileVersionInfoByHandle);
	GET_REAL_FUNC_PTR(GetFileVersionInfoExA);
	GET_REAL_FUNC_PTR(GetFileVersionInfoExW);
	GET_REAL_FUNC_PTR(GetFileVersionInfoSizeA);
	GET_REAL_FUNC_PTR(GetFileVersionInfoSizeExA);
	GET_REAL_FUNC_PTR(GetFileVersionInfoSizeExW);
	GET_REAL_FUNC_PTR(VerFindFileA);
	GET_REAL_FUNC_PTR(VerFindFileW);
	GET_REAL_FUNC_PTR(VerInstallFileA);
	GET_REAL_FUNC_PTR(VerInstallFileW);
}

using AllocConsoleFunc = std::add_pointer_t<decltype(AllocConsole)>;
AllocConsoleFunc OAllocConsole = nullptr;

BOOL WINAPI HkAllocConsole() {
	OAllocConsole();
	return TRUE;
}

void Compat::PatchAllocConsole() {
	// We don't want to use minhook just to patch AllocConsole, that doesn't make any sense!
	// So we are going to do IAT hooking which means we are going to replace the address of AllocConsole
	// in our process's import table with our own.

	const DWORD64 moduleBase = reinterpret_cast<DWORD64>(GetModuleHandle(nullptr));
	const IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(moduleBase);
	const IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(moduleBase + dosHeader->e_lfanew);

	// We need to get the import descriptor to access all imported dlls.
	const IMAGE_DATA_DIRECTORY importDir = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	if (importDir.VirtualAddress == 0)
		return;

	const DWORD64 importVa = moduleBase + importDir.VirtualAddress;

	const IMAGE_IMPORT_DESCRIPTOR* importDesc = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(importVa);
	const IMAGE_IMPORT_DESCRIPTOR* current = importDesc;

	// AllocConsole is inside Kernel32.dll, we need to find the dll in our descriptor.
	while (current->Name != 0) {
		const auto dllName = reinterpret_cast<LPCSTR>(moduleBase + current->Name);
		if (_stricmp(dllName, "KERNEL32.dll") == 0)
			break; // Found it

		++current;
	}

	if (current->Name == 0)
		return;

	// Now that we have our Kernel32.dll, we can just go through all thunk data's and find our function.
	const IMAGE_THUNK_DATA* thunkIlt = reinterpret_cast<IMAGE_THUNK_DATA*>(moduleBase + current->OriginalFirstThunk);
	auto thunkIat = reinterpret_cast<IMAGE_THUNK_DATA*>(moduleBase + current->FirstThunk);

	while (thunkIlt->u1.AddressOfData != 0) {
		const IMAGE_IMPORT_BY_NAME* importByName = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(moduleBase + thunkIlt->u1.AddressOfData);

		if (strcmp(importByName->Name, "AllocConsole") == 0)
			break; // Found it

		++thunkIlt;
		++thunkIat;
	}

	if (thunkIlt->u1.AddressOfData == 0)
		return;

	// We now got the address where the game dereferences it to run AllocConsole. we can now
	// overwrite this with our own making the game call our function.

	DWORD oldProtect;
	VirtualProtect(&thunkIat->u1.Function, sizeof(LPVOID), PAGE_READWRITE, &oldProtect);

	OAllocConsole = reinterpret_cast<AllocConsoleFunc>(thunkIat->u1.Function);
	thunkIat->u1.Function = reinterpret_cast<ULONGLONG>(HkAllocConsole);

	VirtualProtect(&thunkIat->u1.Function, sizeof(LPVOID), oldProtect, &oldProtect);
	FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
}

void Compat::Initialize() {
	InitializeFunctionPointers();
	PatchAllocConsole();
}
