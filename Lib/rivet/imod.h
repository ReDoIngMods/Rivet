#pragma once

namespace Rivet {
	class IMod {
	public:
		virtual ~IMod() = default;

		// Rivet is the modding framework
		virtual void onRivetInitialize() {}
		// Rivet is the modding framework
		virtual void onRivetShutdown() {}

		// Contraption is the game engine, this is unrelated to creations
		virtual void onContraptionInitialize() {}
		// Contraption is the game engine, this is unrelated to creations
		virtual void onContraptionShutdown() {}
		// Contraption is the game engine, this is unrelated to creations
		// This runs when the game state changes
		// newState is a GameState enum value
		virtual void onContraptionStateChange(int newState) {}

		// This runs before the game update logic
		virtual void onContraptionUpdate(float deltaTime) {}
		// This runs after the game update logic
		virtual void onContraptionPostUpdate(float deltaTime) {}

		// This runs before the game render logic
		// This is called from the main render function, be wary of threading issues
		virtual void onContraptionRender() {}
		// This runs after the game render logic
		// This is called from the main render function, be wary of threading issues
		virtual void onContraptionPostRender() {}
	};
};
