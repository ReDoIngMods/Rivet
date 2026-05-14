#include <rivet/modding.h>

#include <Windows.h>

static void OnRivetInitialized(const Rivet::BuiltinEvents::RivetInitialized&) {
	MessageBoxA(nullptr, "SampleMod: RivetInitialized!", "SampleMod", MB_OK);
}

static void OnLuaInitialized(const Rivet::BuiltinEvents::LuaInitialized& e) {
	MessageBoxA(nullptr,
		e.isTerrain ? "SampleMod: terrain Lua VM ready" : "SampleMod: game Lua VM ready",
		"SampleMod", MB_OK);
}

static void SampleModEntry() {
	Rivet::Events::Subscribe<Rivet::BuiltinEvents::RivetInitialized>(&OnRivetInitialized);
	Rivet::Events::Subscribe<Rivet::BuiltinEvents::LuaInitialized>(&OnLuaInitialized);
}

RIVET_REGISTER_MOD(SampleModEntry, "Sample Author", "Sample Mod")
