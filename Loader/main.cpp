#include "utils/includes.h"

#include "utils/flags.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	Rivet::Flags genericFlags = Rivet::Flags::GetFlags();
	Rivet::LoaderFlags& flags = genericFlags.loader;

	Rivet::Logger logger("rivet_loader.log", genericFlags.doorstop.hideConsole);
	Rivet::Logger::Info("Rivet Loader starting...");

	fs::path modsDir = flags.directory;
	if (!fs::exists(modsDir)) {
		Rivet::Logger::Warning("Mods directory does not exist: %s. No mods will be loaded.", modsDir.string().c_str());
	} else {
		for (const auto& entry : fs::directory_iterator(modsDir)) {
			if (entry.is_regular_file() && entry.path().extension() == ".dll") {
				Rivet::Logger::Info("Loading mod DLL: %s", entry.path().string().c_str());
				HMODULE hMod = LoadLibraryA(entry.path().string().c_str());
				if (!hMod) {
					Rivet::Logger::Error("Failed to load mod DLL: %s", entry.path().string().c_str());
				}
			}
		}
	}

	return TRUE;
}