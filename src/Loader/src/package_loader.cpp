#include "package_loader.h"

#include "console.h"
#include "state.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>

namespace {
bool IsPackageIdentity(const std::string& id, const Rivet::PackageManifest& manifest) {
	const std::string suffix = "-" + manifest.name + "-" + manifest.version;
	return id.size() > suffix.size() && id.ends_with(suffix);
}

bool IsSemanticVersion(const std::string& version) {
	std::size_t start = 0;
	for (int component = 0; component != 3; ++component) {
		const std::size_t end = version.find('.', start);
		const std::size_t length = end == std::string::npos ? version.size() - start : end - start;
		if (length == 0 || !std::all_of(version.begin() + static_cast<std::ptrdiff_t>(start),
		                                version.begin() + static_cast<std::ptrdiff_t>(start + length),
		                                [](const char character) { return std::isdigit(static_cast<unsigned char>(character)) != 0; }))
			return false;
		if (end == std::string::npos)
			return component == 2;
		start = end + 1;
	}
	return false;
}

bool IsManifestDirectory(const std::filesystem::directory_entry& entry) {
	return entry.is_directory() && std::filesystem::exists(entry.path() / "manifest.json");
}
} // namespace

namespace Rivet {
bool PackageLoader::Discover(const std::filesystem::path& modsDirectory, std::string& error) {
	packages_.clear();
	std::error_code filesystemError;
	for (const auto& entry : std::filesystem::directory_iterator(modsDirectory, filesystemError)) {
		if (filesystemError)
			break;
		if (!IsManifestDirectory(entry))
			continue;

		Package package;
		package.root = entry.path();
		package.id = entry.path().filename().string();
		if (!ReadPackageManifest(package.root / "manifest.json", package.manifest, error)) {
			CONSOLE_ERROR("Skipping package %s: %s", package.id.c_str(), error.c_str());
			continue;
		}
		if (!IsPackageIdentity(package.id, package.manifest)) {
			CONSOLE_ERROR("Skipping package %s: directory name does not match Team-Package-Version.", package.id.c_str());
			continue;
		}
		if (!IsSemanticVersion(package.manifest.version)) {
			CONSOLE_ERROR("Skipping package %s: version_number is not Major.Minor.Patch.", package.id.c_str());
			continue;
		}
		for (const auto& dll : std::filesystem::directory_iterator(package.root)) {
			if (dll.is_regular_file() && dll.path().extension() == ".dll")
				package.dlls.push_back(dll.path());
		}
		std::sort(package.dlls.begin(), package.dlls.end());
		packages_.push_back(std::move(package));
	}
	if (filesystemError) {
		error = filesystemError.message();
		return false;
	}
	std::sort(packages_.begin(), packages_.end(), [](const Package& left, const Package& right) {
		return left.id < right.id;
	});
	return true;
}

bool PackageLoader::ResolveLoadOrder(std::vector<std::size_t>& order, std::string& error) const {
	std::map<std::string, std::size_t> indices;
	for (std::size_t index = 0; index < packages_.size(); ++index) {
		if (!indices.emplace(packages_[index].id, index).second) {
			error = "duplicate package identity: " + packages_[index].id;
			return false;
		}
	}

	std::vector<bool> blocked(packages_.size());
	std::vector<std::vector<std::size_t>> dependencies(packages_.size());
	for (std::size_t index = 0; index < packages_.size(); ++index) {
		for (const auto& dependency : packages_[index].manifest.dependencies) {
			auto found = indices.find(dependency);
			if (found == indices.end()) {
				CONSOLE_ERROR("Skipping %s: missing dependency %s.", packages_[index].id.c_str(), dependency.c_str());
				blocked[index] = true;
				continue;
			}
			dependencies[index].push_back(found->second);
		}
	}

	while (true) {
		for (std::size_t index = 0; index < packages_.size(); ++index) {
			if (blocked[index])
				continue;
			for (const auto dependency : dependencies[index]) {
				if (blocked[dependency]) {
					CONSOLE_ERROR("Skipping %s: dependency %s was not loadable.", packages_[index].id.c_str(), packages_[dependency].id.c_str());
					blocked[index] = true;
					break;
				}
			}
		}

		std::vector<std::size_t> candidateOrder;
		std::vector<std::size_t> indegree(packages_.size());
		std::vector<std::vector<std::size_t>> dependents(packages_.size());
		for (std::size_t index = 0; index < packages_.size(); ++index) {
			if (blocked[index])
				continue;
			for (const auto dependency : dependencies[index]) {
				if (!blocked[dependency]) {
					++indegree[index];
					dependents[dependency].push_back(index);
				}
			}
		}
		std::set<std::size_t> ready;
		for (std::size_t index = 0; index < packages_.size(); ++index)
			if (!blocked[index] && indegree[index] == 0)
				ready.insert(index);
		while (!ready.empty()) {
			const std::size_t current = *ready.begin();
			ready.erase(ready.begin());
			candidateOrder.push_back(current);
			for (const auto dependent : dependents[current])
				if (--indegree[dependent] == 0)
					ready.insert(dependent);
		}

		bool cycleFound = false;
		for (std::size_t index = 0; index < packages_.size(); ++index) {
			if (!blocked[index] && std::find(candidateOrder.begin(), candidateOrder.end(), index) == candidateOrder.end()) {
				CONSOLE_ERROR("Skipping %s: dependency cycle detected.", packages_[index].id.c_str());
				blocked[index] = true;
				cycleFound = true;
			}
		}
		if (!cycleFound) {
			order = std::move(candidateOrder);
			return true;
		}
	}
}

bool PackageLoader::LoadPackages(const std::vector<std::size_t>& order, std::string& error) {
	std::set<std::string> failedPackages;
	for (const auto index : order) {
		const auto& package = packages_[index];
		bool dependencyFailed = false;
		for (const auto& dependency : package.manifest.dependencies) {
			if (failedPackages.contains(dependency)) {
				CONSOLE_ERROR("Skipping %s because dependency %s failed to load.", package.id.c_str(), dependency.c_str());
				dependencyFailed = true;
				break;
			}
		}
		if (dependencyFailed) {
			failedPackages.insert(package.id);
			continue;
		}
		if (!LoadPackage(package, error)) {
			CONSOLE_ERROR("Skipping package %s: %s", package.id.c_str(), error.c_str());
			failedPackages.insert(package.id);
		}
	}
	return true;
}

bool PackageLoader::LoadPackage(const Package& package, std::string& error) {
	for (const auto& dll : package.dlls) {
		const HMODULE module = LoadLibraryW(dll.c_str());
		if (!module) {
			error = package.id + ": failed to load " + dll.filename().string();
			return false;
		}

		auto getDefinition = reinterpret_cast<ModDef (*)()>(GetProcAddress(module, "GET_RIVET_MOD_DEF"));
		if (!getDefinition)
			continue;
		const ModDef definition = getDefinition();
		if (!definition.entrypoint) {
			error = package.id + ": mod DLL has a null entrypoint: " + dll.filename().string();
			return false;
		}
		LoaderState::GetInstance().AddMod(LoadedMod{dll, package.id, definition});
		CONSOLE_INFO("Loaded Rivet mod DLL: %s (%s)", dll.filename().string().c_str(), package.id.c_str());
	}
	return true;
}
} // namespace Rivet
