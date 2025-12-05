#pragma once

#include "doorstop.h"

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
		const char* names[] = {
			"DllGetActivationFactory",
			"DllGetClassObject",
			"DllRegisterServer"
		};
		PVOID fnPtrs[3]{};

		extern "C" {
			EXPORT __int64 DllGetActivationFactory() {
				return CALL_FUNC(DllGetActivationFactory);
			}

			EXPORT HRESULT STDCALL DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
				return CALL_FUNC(DllGetClassObject, rclsid, riid, ppv);
			}

			EXPORT HRESULT STDCALL DllRegisterServer() {
				return CALL_FUNC(DllRegisterServer);
			}
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

			for (int i = 0; i < 3; ++i) {
				fnPtrs[i] = GetProcAddress(hModule, names[i]);
			}
		}
	}
}
