#include "compat.h"

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
    p##name = reinterpret_cast<PVOID>(GetProcAddress(hModule, #name))

extern "C" {
	PVOID pGetFileVersionInfoW = nullptr;
	PVOID pGetFileVersionInfoA = nullptr;
	PVOID pGetFileVersionInfoSizeW = nullptr;
	PVOID pVerQueryValueA = nullptr;
	PVOID pVerQueryValueW = nullptr;
	PVOID pGetFileVersionInfoByHandle = nullptr;
	PVOID pGetFileVersionInfoExA = nullptr;
	PVOID pGetFileVersionInfoExW = nullptr;
	PVOID pGetFileVersionInfoSizeA = nullptr;
	PVOID pGetFileVersionInfoSizeExA = nullptr;
	PVOID pGetFileVersionInfoSizeExW = nullptr;
	PVOID pVerFindFileA = nullptr;
	PVOID pVerFindFileW = nullptr;
	PVOID pVerInstallFileA = nullptr;
	PVOID pVerInstallFileW = nullptr;
}

void Compat::InitializeFunctionPointers() {
	wchar_t sysPathW[MAX_PATH];
	GetSystemDirectoryW(sysPathW, MAX_PATH);
	std::filesystem::path sysPathP = sysPathW;
	sysPathP /= TARGET_NAME;
	LPCWSTR sysPath = sysPathP.c_str();

	HMODULE hModule = LoadLibraryW(sysPath); // Load the real DLL
	if (!hModule) {
		return;
	}

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

void Compat::PatchMyGUI() {
	HMODULE hMyGUI = LoadLibraryW(L"MyGUIEngine.dll");
	if (!hMyGUI) {
		return;
	}

	// Get `struct tm *__fastcall MyGUI::LogManager::log(_QWORD **a1, int a2, int a3, __int64 a4, __int64 a5, int a6)`
	PVOID pLogFunc = reinterpret_cast<PVOID>(GetProcAddress(hMyGUI, "?log@LogManager@MyGUI@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@ULogLevel@2@0PEBDH@Z"));
	if (!pLogFunc)
		return;

	// Stick a `ret` at the start of the function to noop it out.. it crashes for some reason
	// TODO: Redirect to the games logging system instead of doing this....
	// See issue #1 on GitHub for more info.

	DWORD oldProtect;
	VirtualProtect(pLogFunc, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
	unsigned char retInstr = 0xC3; // x64 ret instruction
	memcpy(pLogFunc, &retInstr, 1);
	VirtualProtect(pLogFunc, 1, oldProtect, &oldProtect);
	FlushInstructionCache(GetCurrentProcess(), pLogFunc, 1);
}

void Compat::Initialize() {
	InitializeFunctionPointers();
	PatchMyGUI();
}
