#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace Rivet {
struct PackageManifest {
	std::string name;
	std::string version;
	std::string websiteUrl;
	std::string description;
	std::vector<std::string> dependencies;
};

bool ReadPackageManifest(const std::filesystem::path& path, PackageManifest& manifest, std::string& error);
} // namespace Rivet
