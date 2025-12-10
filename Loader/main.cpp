#include "includes.h"

#include "flags.h"
#include "state.h"
#include "console.h"

#include <rivet/moddef.h>

#include <MinHook.h>

static Rivet::LoaderState& loaderState = Rivet::LoaderState::GetInstance();

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
		CONSOLE_ERROR("Failed to load mod DLL: %s", modPath.string().c_str());
		return;
	}

	Rivet::ModDef modDef;
	if (!getRivetEnabled(hMod, modDef)) {
		CONSOLE_WARN("DLL is not a valid Rivet mod: %s. No extra steps will be taken, it will run its course as a normal DLL.", modPath.string().c_str());
		return;
	}

	Rivet::IMod* modInstance = modDef.create();
	CONSOLE_INFO("Loaded mod: %s by %s", modDef.getName(), modDef.getAuthor());

	Rivet::LoaderState::GetInstance().addMod(modDef);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	Rivet::Flags genericFlags = Rivet::Flags::GetFlags();
	Rivet::LoaderFlags& flags = genericFlags.loader;

	CONSOLE_INFO("Rivet Loader starting...");

	if (MH_Initialize() != MH_OK) {
		CONSOLE_ERROR("Failed to initialize MinHook.");
		return FALSE;
	}

	fs::path modsDir = flags.directory;
	if (!fs::exists(modsDir)) {
		CONSOLE_WARN("Mods directory does not exist: %s. No mods will be loaded.", modsDir.string().c_str());
	} else {
		for (const auto& entry : fs::recursive_directory_iterator(modsDir)) {
			if (entry.is_regular_file() && entry.path().extension() == ".dll") {
				handleMod(entry.path());
			}
		}
	}

	auto& loadedMods = loaderState.getLoadedMods();
	CONSOLE_INFO("Rivet Loader initialized. %zu mods loaded.", loadedMods.size());

	for (const auto& modDef : loadedMods) {
		Rivet::IMod* modInstance = modDef.create();
		modInstance->onRivetInitialize();
		loaderState.addMod(modDef);
	}

	return TRUE;
}