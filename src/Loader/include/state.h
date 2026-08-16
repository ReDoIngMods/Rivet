#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <rivet/moddef.h>

namespace Rivet {
struct LoadedMod {
	std::filesystem::path modulePath;
	std::string packageId;
	ModDef definition{};
};

class LoaderState {
public:
	static LoaderState& GetInstance() {
		static LoaderState instance;
		return instance;
	}

	void AddMod(LoadedMod mod) { loadedMods_.push_back(std::move(mod)); }

	const std::vector<LoadedMod>& GetLoadedMods() const { return loadedMods_; }

private:
	std::vector<LoadedMod> loadedMods_;
};
} // namespace Rivet
