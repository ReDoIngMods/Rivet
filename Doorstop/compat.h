#pragma once

#include "rivet/utils/includes.h"

#define GET_INDEX(name) \
			(static_cast<size_t>(std::distance(std::begin(names), std::find(std::begin(names), std::end(names), name))))
#define GET_FUNC_PTR(name) \
			fnPtrs[GET_INDEX(__func__)]
#define CALL_FUNC(name, ...) \
			reinterpret_cast<decltype(name)*>(GET_FUNC_PTR(__func__))(__VA_ARGS__)
#define EXPORT __declspec(dllexport)
#define FASTCALL __fastcall
#define STDCALL __stdcall

namespace Rivet {
	namespace Compat {
		extern "C" {
			PVOID pGetFileVersionInfoW = 0;
			PVOID pGetFileVersionInfoA = 0;
			PVOID pGetFileVersionInfoSizeW = 0;
			PVOID pVerQueryValueA = 0;
			PVOID pVerQueryValueW = 0;
			PVOID pGetFileVersionInfoByHandle = 0;
			PVOID pGetFileVersionInfoExA = 0;
			PVOID pGetFileVersionInfoExW = 0;
			PVOID pGetFileVersionInfoSizeA = 0;
			PVOID pGetFileVersionInfoSizeExA = 0;
			PVOID pGetFileVersionInfoSizeExW = 0;
			PVOID pVerFindFileA = 0;
			PVOID pVerFindFileW = 0;
			PVOID pVerInstallFileA = 0;
			PVOID pVerInstallFileW = 0;
		}

		void InitializeFunctionPointers() {
			wchar_t sysPathW[MAX_PATH];
			GetSystemDirectoryW(sysPathW, MAX_PATH);
			std::filesystem::path sysPathP = sysPathW;
			sysPathP /= TARGET_NAME;
			LPCWSTR sysPath = sysPathP.c_str();

			HMODULE hModule = LoadLibraryW(sysPath); // Load the real DLL
			if (!hModule) {
				return;
			}

			pGetFileVersionInfoW = reinterpret_cast<PVOID>(GetProcAddress(hModule, "GetFileVersionInfoW"));
			pGetFileVersionInfoA = reinterpret_cast<PVOID>(GetProcAddress(hModule, "GetFileVersionInfoA"));
			pGetFileVersionInfoSizeW = reinterpret_cast<PVOID>(GetProcAddress(hModule, "GetFileVersionInfoSizeW"));
			pVerQueryValueA = reinterpret_cast<PVOID>(GetProcAddress(hModule, "VerQueryValueA"));
			pVerQueryValueW = reinterpret_cast<PVOID>(GetProcAddress(hModule, "VerQueryValueW"));
			pGetFileVersionInfoByHandle = reinterpret_cast<PVOID>(GetProcAddress(hModule, "GetFileVersionInfoByHandle"));
			pGetFileVersionInfoExA = reinterpret_cast<PVOID>(GetProcAddress(hModule, "GetFileVersionInfoExA"));
			pGetFileVersionInfoExW = reinterpret_cast<PVOID>(GetProcAddress(hModule, "GetFileVersionInfoExW"));
			pGetFileVersionInfoSizeA = reinterpret_cast<PVOID>(GetProcAddress(hModule, "GetFileVersionInfoSizeA"));
			pGetFileVersionInfoSizeExA = reinterpret_cast<PVOID>(GetProcAddress(hModule, "GetFileVersionInfoSizeExA"));
			pGetFileVersionInfoSizeExW = reinterpret_cast<PVOID>(GetProcAddress(hModule, "GetFileVersionInfoSizeExW"));
			pVerFindFileA = reinterpret_cast<PVOID>(GetProcAddress(hModule, "VerFindFileA"));
			pVerFindFileW = reinterpret_cast<PVOID>(GetProcAddress(hModule, "VerFindFileW"));
			pVerInstallFileA = reinterpret_cast<PVOID>(GetProcAddress(hModule, "VerInstallFileA"));
			pVerInstallFileW = reinterpret_cast<PVOID>(GetProcAddress(hModule, "VerInstallFileW"));
		}

		void PatchMyGUI() {
			HMODULE hMyGUI = LoadLibraryW(L"MyGUIEngine.dll");
			if (!hMyGUI) {
				return;
			}

			// Get `struct tm *__fastcall MyGUI::LogManager::log(_QWORD **a1, int a2, int a3, __int64 a4, __int64 a5, int a6)`
			PVOID pLogFunc = reinterpret_cast<PVOID>(GetProcAddress(hMyGUI, "?log@LogManager@MyGUI@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@ULogLevel@2@0PEBDH@Z"));
			if (!pLogFunc) {
				return;
			}

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

		void Initialize() {
			InitializeFunctionPointers();
			PatchMyGUI();
		}
	}
}
