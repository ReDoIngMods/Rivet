#pragma once

#include <includes.h>

#include <rivet/moddef.h>

namespace Rivet {
class LoaderState {
public:
	static LoaderState& GetInstance() {
		static LoaderState instance;
		return instance;
	}

	void AddMod(const ModDef& modDef) {
		loadedMods_.push_back(modDef);
	}

	const std::vector<ModDef>& GetLoadedMods() const {
		return loadedMods_;
	}

private:
	std::vector<ModDef> loadedMods_;
};
} // namespace Rivet
