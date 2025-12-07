#pragma once

#include <includes.h>

#include <rivet/moddef.h>

namespace Rivet {
	class LoaderState {
	public:
		static LoaderState& getInstance() {
			static LoaderState instance;
			return instance;
		}

		void addMod(const ModDef& modDef) {
			loadedMods_.push_back(modDef);
		}

		void addModInstance(IMod* modInstance) {
			modInstances_.push_back(modInstance);
		}

		const std::vector<ModDef>& getLoadedMods() const {
			return loadedMods_;
		}

	private:
		std::vector<ModDef> loadedMods_;
		std::vector<IMod*> modInstances_;
	};
}
