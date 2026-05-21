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

	// Entry is deferred to the game-entry-point hook so it runs outside the
	// Windows loader lock. Calling mod code from here (inside DllMain) was
	// causing ERROR_DLL_INIT_FAILED whenever a mod did anything non-trivial
	// (MessageBox, LoadLibrary, etc.).
	Rivet::LoaderState::GetInstance().AddMod(modDef);
}

// envMode: 1 = terrain VM, 0 = main game VM. 32-bit boolean in the game's ABI.
constexpr std::uintptr_t LuaVM_Initialize_RVA = 0x54A7F0;

using LuaVM_Initialize_t = void* (*)(lua_State** pL, void** modOpenerLists, int envMode);
static LuaVM_Initialize_t oLuaVM_Initialize = nullptr;

static void* hk_LuaVM_Initialize(lua_State** pL, void** modOpenerLists, int envMode) {
	const bool isTerrain = envMode != 0;
	CONSOLE_INFO("hk_LuaVM_Initialize entered (envMode=%d, isTerrain=%d).", envMode, isTerrain ? 1 : 0);

	lua_State* L = pL ? *pL : nullptr;
	Rivet::Events::Publish(Rivet::BuiltinEvents::LuaInitializing{L, isTerrain});

	void* result = oLuaVM_Initialize(pL, modOpenerLists, envMode);

	L = pL ? *pL : nullptr;
	Rivet::Events::Publish(Rivet::BuiltinEvents::LuaInitialized{L, isTerrain});

	CONSOLE_INFO("hk_LuaVM_Initialize returning.");
	return result;
}

// The game EXE's entry point. Hooked so that mod `entry()` callbacks and the
// RivetInitialized event fire outside the Windows loader lock, but still on
// the main thread before the game has run any of its own code. Standard
// MSVC-CRT EXE entry returns void / calls ExitProcess, so the return is
// unreachable.
using OEP_t = void (*)();
static OEP_t oGameEntry = nullptr;

static void hk_GameEntry() {
	const auto& mods = Rivet::LoaderState::GetInstance().GetLoadedMods();
	CONSOLE_INFO("hk_GameEntry: invoking %zu mod entry function(s).", mods.size());
	for (const auto& mod : mods) {
		const char* name = mod.getName ? mod.getName() : "<unknown>";
		if (mod.entry) {
			CONSOLE_INFO("hk_GameEntry: calling entry for '%s'.", name);
			mod.entry();
			CONSOLE_INFO("hk_GameEntry: entry for '%s' returned.", name);
		} else {
			CONSOLE_WARN("hk_GameEntry: mod '%s' has no entry function.", name);
		}
	}
	CONSOLE_INFO("hk_GameEntry: publishing RivetInitialized.");
	Rivet::Events::Publish(Rivet::BuiltinEvents::RivetInitialized{});
	CONSOLE_INFO("hk_GameEntry: chaining to original entry point.");
	oGameEntry();
}

static void* ResolveGameEntryPoint() {
	const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
	if (!base) {
		return nullptr;
	}
	auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
	if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
		return nullptr;
	}
	auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
	if (nt->Signature != IMAGE_NT_SIGNATURE) {
		return nullptr;
	}
	return reinterpret_cast<void*>(base + nt->OptionalHeader.AddressOfEntryPoint);
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

	if (void* oep = ResolveGameEntryPoint()) {
		CONSOLE_INFO("Installing game entry-point hook at %p.", oep);
		if (!Rivet::HookManager::GetInstance().Install(
				oep, &hk_GameEntry,
				reinterpret_cast<void**>(&oGameEntry), "Rivet")) {
			CONSOLE_ERROR("Failed to install game entry-point hook; firing RivetInitialized from DllMain as a fallback.");
			for (const auto& mod : loaderState.GetLoadedMods()) {
				if (mod.entry) mod.entry();
			}
			Rivet::Events::Publish(Rivet::BuiltinEvents::RivetInitialized{});
		}
	} else {
		CONSOLE_ERROR("Could not resolve game entry point; firing RivetInitialized from DllMain as a fallback.");
		for (const auto& mod : loaderState.GetLoadedMods()) {
			if (mod.entry) mod.entry();
		}
		Rivet::Events::Publish(Rivet::BuiltinEvents::RivetInitialized{});
	}

	return TRUE;
}
