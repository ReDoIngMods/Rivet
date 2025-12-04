/// main.cpp
///
/// Entrypoint for Rivet Doorstop.
/// This DLL must be compiled as `version.dll` and placed alongside ScrapMechanic.exe
/// This will cause Doorstop to load and initialize itself before the game starts.
/// This way we can proxy all relevant calls and inject our own code.
///
/// Doorstop is a native code injection library for Scrap Mechanic.

#include "config.h"
#include "cli.h"

#include <print>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	// -rivet-enable | Enable Rivet mod loading
	// -rivet-log <file> | Enable logging to <file>
	// -rivet-target <path> | Set the target directory for dll (default: ./rivet.dll)
	// -rivet-save-config | Save the current config (with modified values) to RivetDoorstop.ini

	Rivet::Config config;
	Rivet::CLI cli;

	auto cfgEnable = config.getValue<bool>("rivet-enable", false);
	auto cfgLog = config.getValue<std::string>("rivet-log", ".\\rivet.log");
	auto cfgTarget = config.getValue<std::string>("rivet-target", ".\\rivet.dll");

	auto enable = cli.getValue<bool>("rivet-enable", cfgEnable);
	auto log = cli.getValue<std::string>("rivet-log", cfgLog);
	auto target = cli.getValue<std::string>("rivet-target", cfgTarget);

	auto save = cli.getValue<bool>("rivet-save-config", false);
	if (save) {
		config.setValue("rivet-enable", enable ? "true" : "false");
		config.setValue("rivet-log", log);
		config.setValue("rivet-target", target);
	}

	std::println("Rivet Doorstop initialized:\n  Enable: {}\n  Log: {}\n  Target: {}", enable ? "true" : "false", log, target);

	return TRUE;
}