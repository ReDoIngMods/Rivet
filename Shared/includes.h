#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shellapi.h>

#include <string>
#include <optional>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

namespace Rivet {
	template <typename T>
	concept StringOrBool = std::is_same_v<T, std::string> || std::is_same_v<T, bool>;
}