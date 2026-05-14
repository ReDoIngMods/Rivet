#include "LoaderFlags.h"
#include "state.h"
#include "console.h"
#include "Hooks.h"

#include <rivet/moddef.h>
#include <rivet/events.h>
#include <rivet/events_builtin.h>

#include <MinHook.h>

static Rivet::LoaderState& loaderState = Rivet::LoaderState::GetInstance();

bool GetRivetEnabled(HMODULE hMod, Rivet::ModDef& outModDef) {
	// Look for `GET_RIVET_MOD_DEF` export
	auto getModDefFunc = reinterpret_cast<Rivet::ModDef (*)()>(GetProcAddress(hMod, "GET_RIVET_MOD_DEF"));
	if (!getModDefFunc) {
		return false;
	}

	outModDef = getModDefFunc();
	return true;
}

static void HandleMod(fs::path modPath) {
	// Check if file ends in .old, if it does, it's been disabled and we skip it
	if (modPath.extension() == ".old") {
		CONSOLE_INFO("Mod %s is disabled (ends with .old), skipping.", modPath.filename().string());
		return;
	}

	HMODULE hMod = LoadLibraryA(modPath.string().c_str());
	if (!hMod) {
		CONSOLE_ERROR("Failed to load mod DLL: %s", modPath.string().c_str());
		return;
	}

	Rivet::ModDef modDef;
	if (!GetRivetEnabled(hMod, modDef)) {
		CONSOLE_WARN("DLL is not a valid Rivet mod: %s. No extra steps will be taken, it will run its course as a normal DLL.", modPath.string().c_str());
		return;
	}

	CONSOLE_INFO("Loaded mod: %s by %s", modDef.getName(), modDef.getAuthor());

	if (modDef.entry) {
		modDef.entry();
	}
	Rivet::LoaderState::GetInstance().AddMod(modDef);
}

// envMode: 1 = terrain VM, 0 = main game VM. 32-bit boolean in the game's ABI.
constexpr std::uintptr_t LuaVM_Initialize_RVA = 0x54A7F0;

using LuaVM_Initialize_t = void* (*)(lua_State** pL, void** modOpenerLists, int envMode);
static LuaVM_Initialize_t oLuaVM_Initialize = nullptr;

static void* hk_LuaVM_Initialize(lua_State** pL, void** modOpenerLists, int envMode) {
	const bool isTerrain = envMode != 0;

	lua_State* L = pL ? *pL : nullptr;
	Rivet::Events::Publish(Rivet::BuiltinEvents::LuaInitializing{L, isTerrain});

	void* result = oLuaVM_Initialize(pL, modOpenerLists, envMode);

	L = pL ? *pL : nullptr;
	Rivet::Events::Publish(Rivet::BuiltinEvents::LuaInitialized{L, isTerrain});

	return result;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	Rivet::LoaderFlags flags = Rivet::LoaderFlags::Load();

	CONSOLE_INFO("Rivet Loader starting...");

	if (MH_Initialize() != MH_OK) {
		CONSOLE_ERROR("Failed to initialize MinHook.");
		return FALSE;
	}

	{
		const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
		auto* target = reinterpret_cast<void*>(base + LuaVM_Initialize_RVA);
		if (!Rivet::HookManager::GetInstance().Install(
				target, &hk_LuaVM_Initialize,
				reinterpret_cast<void**>(&oLuaVM_Initialize), "Rivet")) {
			CONSOLE_ERROR("Failed to install LuaVM_Initialize hook.");
		}
	}

	fs::path modsDir = flags.directory;
	if (!fs::exists(modsDir)) {
		CONSOLE_WARN("Mods directory does not exist: %s. No mods will be loaded.", modsDir.string().c_str());
	} else {
		for (const auto& entry : fs::recursive_directory_iterator(modsDir)) {
			if (entry.is_regular_file() && entry.path().extension() == ".dll") {
				HandleMod(entry.path());
			}
		}
	}

	const auto& loadedMods = loaderState.GetLoadedMods();
	CONSOLE_INFO("Rivet Loader initialized. %zu mods loaded.", loadedMods.size());

	Rivet::Events::Publish(Rivet::BuiltinEvents::RivetInitialized{});

	return TRUE;
}
