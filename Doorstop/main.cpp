/// Rivet Doorstop is a native code injection library for Scrap Mechanic.

#include "doorstopFlags.h"
#include "console.h"

#include "compat.h"

BOOL WINAPI DllMain(HINSTANCE, const DWORD fdwReason, LPVOID) {
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	Rivet::DoorstopFlags flags = Rivet::DoorstopFlags::Load();

	CONSOLE_INFO("Rivet Doorstop starting...");
	Rivet::Compat::Initialize();
	CONSOLE_INFO("Rivet Doorstop initialized.");

	if (!flags.enable)
		return TRUE;

	// Resolve to full path
	char fullTarget[MAX_PATH];
	GetFullPathNameA(flags.target.c_str(), MAX_PATH, fullTarget, nullptr);
	CONSOLE_INFO("Loading target DLL: %s", fullTarget);

	if (!LoadLibraryA(fullTarget)) {
		CONSOLE_ERROR("Failed to load target DLL: %s", fullTarget);

		const DWORD error = GetLastError();
		LPSTR errorMsg = nullptr;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&errorMsg), 0, nullptr);

		if (errorMsg) {
			CONSOLE_ERROR("Error %d: %s", error, errorMsg);
			LocalFree(errorMsg);
		}
	}

	return TRUE;
}
