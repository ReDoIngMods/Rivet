/// Rivet Doorstop is a native code injection library for Scrap Mechanic.

#include "flags.h"
#include "console.h"

#include "loggerManager.h"

#include "compat.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	Rivet::Flags genericFlags = Rivet::Flags::GetFlags();
	Rivet::DoorstopFlags& flags = genericFlags.doorstop;

	CONSOLE_INFO("Rivet Doorstop starting...");
	Rivet::Compat::Initialize();

	CONSOLE_INFO("Rivet Doorstop initialized.");

	if (!flags.enable)
		return TRUE;

	MessageBoxA(nullptr, "Rivet Doorstop is enabled. The target DLL will be loaded.", "Rivet Doorstop", MB_OK);

	// Resolve to full path
	char fullTarget[MAX_PATH];
	GetFullPathNameA(flags.target.c_str(), MAX_PATH, fullTarget, nullptr);
	CONSOLE_INFO("Loading target DLL: %s", fullTarget);

	if (!LoadLibraryA(fullTarget)) {
		CONSOLE_ERROR("Failed to load target DLL: %s", fullTarget);

		DWORD error = GetLastError();
		LPSTR errorMsg = nullptr;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&errorMsg), 0, nullptr);

		if (errorMsg) {
			CONSOLE_ERROR("Error %d: %s", error, errorMsg);
			LocalFree(errorMsg);
		}
	}

	return TRUE;
}