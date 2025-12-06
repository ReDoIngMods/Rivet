/// main.cpp
///
/// Entrypoint for Rivet Doorstop.
/// This DLL must be compiled as `AudioSes.dll` and placed alongside ScrapMechanic.exe
/// This will cause Doorstop to load and initialize itself before the game starts.
/// This way we can proxy all relevant calls and inject our own code.
///
/// Doorstop is a native code injection library for Scrap Mechanic.

#include "config.h"
#include "cli.h"
#include "logging.h"
#include "compat.h"

#include <print>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	Rivet::Config config;
	Rivet::CLI cli;

	auto cfgEnable		= config.getValue<bool>("rivetEnable", false);
	auto cfgLog			= config.getValue<std::string>("rivetLog", "rivet.log");
	auto cfgTarget		= config.getValue<std::string>("rivetTarget", "rivet.dll");
	auto cfgHideConsole = config.getValue<bool>("rivetHideConsole", false);

	auto enable			= cli.getValue<bool>("rivetEnable", cfgEnable);
	auto log			= cli.getValue<std::string>("rivetLog", cfgLog);
	auto target			= cli.getValue<std::string>("rivetTarget", cfgTarget);
	auto hideConsole	= cli.getValue<bool>("rivetHideConsole", cfgHideConsole);

	auto save = cli.getValue<bool>("rivetSaveConfig", false);
	if (save) {
		config.setValue("rivetEnable", enable ? "true" : "false");
		config.setValue("rivetLog", log);
		config.setValue("rivetTarget", target);
	}

	Rivet::Logger logger(log, !hideConsole);
	Rivet::Logger::Info("Rivet Doorstop starting...");

	Rivet::Compat::Initialize();

	Rivet::Logger::Info("Rivet Doorstop initialized.");

	for (auto& [key, value] : cli.getAllArgs()) {
		Rivet::Logger::Debug("CLI Arg: -%s=%s", key.c_str(), value.c_str());
	}

	if (enable) {
		// Resolve to full path
		char fullTarget[MAX_PATH];
		GetFullPathNameA(target.c_str(), MAX_PATH, fullTarget, nullptr);
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