#pragma once

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
		
		// This runs during Lua initialization, allowing you to register your own Lua functions and such
		virtual void OnLuaInitialize() {}
		// This runs after Lua initialization, allowing you to interact with the Lua environment after the game has set it up
		// This one is recommended as `sm` and other important tables will become available by then
		virtual void OnLuaPostInitialize() {}
	};
};
