#pragma once

#include "package_manifest.h"

#include <filesystem>
#include <string>
#include <vector>

namespace Rivet {
struct Package {
	std::filesystem::path root;
	std::string id;
	PackageManifest manifest;
	std::vector<std::filesystem::path> dlls;
};

class PackageLoader {
public:
	bool Discover(const std::filesystem::path& modsDirectory, std::string& error);
	bool ResolveLoadOrder(std::vector<std::size_t>& order, std::string& error) const;
	bool LoadPackages(const std::vector<std::size_t>& order, std::string& error);

	const std::vector<Package>& GetPackages() const { return packages_; }

private:
	bool LoadPackage(const Package& package, std::string& error);
	std::vector<Package> packages_;
};
} // namespace Rivet
