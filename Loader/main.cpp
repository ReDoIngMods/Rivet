#include "rivet/utils/includes.h"

#include "rivet/utils/flags.h"
#include "rivet/moddef.h"

#include "state.h"

#include <MinHook.h>

static Rivet::LoaderState& loaderState = Rivet::LoaderState::getInstance();

bool getRivetEnabled(HMODULE hMod, Rivet::ModDef& outModDef) {
	// Look for `GET_RIVET_MOD_DEF` export
	auto getModDefFunc = reinterpret_cast<Rivet::ModDef(*)()>(GetProcAddress(hMod, "GET_RIVET_MOD_DEF"));
	if (!getModDefFunc) {
		return false;
	}

	outModDef = getModDefFunc();
	return true;
}

static void handleMod(fs::path modPath) {
	HMODULE hMod = LoadLibraryA(modPath.string().c_str());
	if (!hMod) {
		Rivet::Logger::Error("Failed to load mod DLL: %s", modPath.string().c_str());
		return;
	}

	Rivet::ModDef modDef;
	if (!getRivetEnabled(hMod, modDef)) {
		Rivet::Logger::Warning("DLL is not a valid Rivet mod: %s. No extra steps will be taken, it will run its course as a normal DLL.", modPath.string().c_str());
		return;
	}

	Rivet::IMod* modInstance = modDef.create();
	Rivet::Logger::Info("Loaded mod: %s by %s", modDef.getName(), modDef.getAuthor());

	Rivet::LoaderState::getInstance().addMod(modDef);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	Rivet::Flags genericFlags = Rivet::Flags::GetFlags();
	Rivet::LoaderFlags& flags = genericFlags.loader;

	Rivet::Logger logger("rivet_loader.log", genericFlags.doorstop.hideConsole);
	Rivet::Logger::Info("Rivet Loader starting...");

	if (MH_Initialize() != MH_OK) {
		Rivet::Logger::Error("Failed to initialize MinHook.");
		return FALSE;
	}

	fs::path modsDir = flags.directory;
	if (!fs::exists(modsDir)) {
		Rivet::Logger::Warning("Mods directory does not exist: %s. No mods will be loaded.", modsDir.string().c_str());
	} else {
		for (const auto& entry : fs::recursive_directory_iterator(modsDir)) {
			if (entry.is_regular_file() && entry.path().extension() == ".dll") {
				handleMod(entry.path());
			}
		}
	}

	auto& loadedMods = loaderState.getLoadedMods();
	Rivet::Logger::Info("Rivet Loader initialized. %zu mods loaded.", loadedMods.size());

	for (const auto& modDef : loadedMods) {
		Rivet::IMod* modInstance = modDef.create();
		modInstance->onRivetInitialize();
		loaderState.addMod(modDef);
	}

	return TRUE;
}