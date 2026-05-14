#pragma once

struct lua_State;

namespace Rivet {
class IMod {
public:
	virtual ~IMod() = default;

	// Rivet is the modding framework
	virtual void OnRivetInitialize() {}
	// Rivet is the modding framework
	virtual void OnRivetShutdown() {}

	// Contraption is the game engine, this is unrelated to creations
	virtual void OnContraptionInitialize() {}
	// Contraption is the game engine, this is unrelated to creations
	virtual void OnContraptionShutdown() {}
	// Contraption is the game engine, this is unrelated to creations
	// This runs when the game state changes
	// newState is a GameState enum value
	virtual void OnContraptionStateChange(int newState) {}

	// This runs before the game update logic
	virtual void OnContraptionUpdate(float deltaTime) {}
	// This runs after the game update logic
	virtual void OnContraptionPostUpdate(float deltaTime) {}

	// This runs before the game render logic
	// This is called from the main render function, be wary of threading issues
	virtual void OnContraptionRender() {}
	// This runs after the game render logic
	// This is called from the main render function, be wary of threading issues
	virtual void OnContraptionPostRender() {}

	// This runs just before the game's LuaVM_Initialize, allowing you to register your own Lua functions and such.
	// isTerrain is true when the VM being initialized is the terrain VM; false for the main game VM.
	// L may be null at this point (the game has not yet populated *pL).
	virtual void OnLuaInitialize(lua_State* L, bool isTerrain) {}
	// This runs immediately after the game's LuaVM_Initialize returns, allowing you to interact with the
	// Lua environment after the game has set it up. Recommended over OnLuaInitialize as `sm` and other
	// important tables will become available by then.
	// isTerrain is true when the VM being initialized is the terrain VM; false for the main game VM.
	virtual void OnLuaPostInitialize(lua_State* L, bool isTerrain) {}
};
}; // namespace Rivet
