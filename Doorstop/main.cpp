/// Rivet Doorstop is a native code injection library for Scrap Mechanic.

#include "utils/flags.h"
#include "utils/logging.h"

#include "compat.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	Rivet::Flags genericFlags = Rivet::Flags::GetFlags();
	Rivet::DoorstopFlags& flags = genericFlags.doorstop;

	Rivet::Logger logger(flags.log, !flags.hideConsole);
	Rivet::Logger::Info("Rivet Doorstop starting...");

	Rivet::Compat::Initialize();

	Rivet::Logger::Info("Rivet Doorstop initialized.");

	if (flags.enable) {
		MessageBoxA(nullptr, "Rivet Doorstop is enabled. The target DLL will be loaded.", "Rivet Doorstop", MB_OK);

		// Resolve to full path
		char fullTarget[MAX_PATH];
		GetFullPathNameA(flags.target.c_str(), MAX_PATH, fullTarget, nullptr);
		Rivet::Logger::Info("Loading target DLL: %s", fullTarget);

		HMODULE hTarget = LoadLibraryA(fullTarget);
		if (!hTarget) {
			Rivet::Logger::Error("Failed to load target DLL: %s", fullTarget);

			DWORD error = GetLastError();
			LPSTR errorMsg = nullptr;
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&errorMsg,
				0,
				nullptr
			);
			if (errorMsg) {
				Rivet::Logger::Error("Error %d: %s", error, errorMsg);
				LocalFree(errorMsg);
			}

			return TRUE;
		}
	}

	return TRUE;
}