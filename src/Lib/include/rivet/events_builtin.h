#pragma once

struct lua_State;

namespace Rivet::BuiltinEvents {

// Fired once after all mods have loaded and registered their subscriptions.
struct RivetInitialized {};

// Fired during shutdown, before mods are torn down.
// Not currently published; the loader does not run shutdown logic yet.
struct RivetShutdown {};

// Fired when the engine ("Contraption") completes its initialization.
// Not currently published; no hook installed yet.
struct ContraptionInitialized {};

// Fired when the engine begins shutdown.
// Not currently published; no hook installed yet.
struct ContraptionShutdown {};

// Fired when the engine state changes. newState corresponds to a GameState
// enum value in the host engine.
// Not currently published; no hook installed yet.
struct ContraptionStateChanged {
	int newState;
};

// Fired before the engine update logic runs each frame.
// Not currently published; no hook installed yet.
struct ContraptionUpdate {
	float deltaTime;
};

// Fired after the engine update logic runs each frame.
// Not currently published; no hook installed yet.
struct ContraptionPostUpdate {
	float deltaTime;
};

// Fired before the engine render logic. Runs on the render thread, be wary
// of threading.
// Not currently published; no hook installed yet.
struct ContraptionRender {};

// Fired after the engine render logic. Runs on the render thread, be wary
// of threading.
// Not currently published; no hook installed yet.
struct ContraptionPostRender {};

// Fired immediately before the game's LuaVM_Initialize runs. L may be null
// at this point because the game has not yet populated *pL. isTerrain is
// true when the VM being initialized is the terrain VM, false for the main
// game VM.
struct LuaInitializing {
	lua_State* L;
	bool isTerrain;
};

// Fired immediately after the game's LuaVM_Initialize returns. L is now
// populated. Recommended over LuaInitializing for most use cases. `sm` and
// other important tables become available by this point. isTerrain matches
// the corresponding LuaInitializing event.
struct LuaInitialized {
	lua_State* L;
	bool isTerrain;
};

} // namespace Rivet::BuiltinEvents
