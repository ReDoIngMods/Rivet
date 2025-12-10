#include "config.h"

using namespace Rivet;

Config::Config(std::wstring_view section, std::wstring_view configPath) : configPath_(configPath), section_(section) {
	if (!fs::exists(configPath_) || fs::file_size(configPath_) == 0) {
		// Create default doorstop config
		WritePrivateProfileStringW(L"Doorstop", nullptr, nullptr, configPath_.data());
		WritePrivateProfileStringW(L"Doorstop", L"enable", L"false", configPath_.data());
		WritePrivateProfileStringW(L"Doorstop", L"log", L"Rivet/latest.log", configPath_.data());
		WritePrivateProfileStringW(L"Doorstop", L"target", L"Rivet/Loader.dll", configPath_.data());
		WritePrivateProfileStringW(L"Doorstop", L"hideConsole", L"false", configPath_.data());

		// Create default loader config
		WritePrivateProfileStringW(L"Loader", L"directory", L"Rivet/Mods", configPath_.data());
	}
}

void Config::setValue(const std::string& key, const std::string& value) {
	WritePrivateProfileStringW(section_.data(), utf8ToUtf16(key).data(), utf8ToUtf16(value).data(), configPath_.data());
}

std::string Config::getValue(const std::string& key, const std::string& defaultValue) {
	wchar_t buffer[128];
	DWORD charsRead = GetPrivateProfileStringW(section_.data(), utf8ToUtf16(key).data(), utf8ToUtf16(defaultValue).data(), buffer, sizeof(buffer) / sizeof(wchar_t), configPath_.data());
	return utf16ToUtf8(std::wstring(buffer, charsRead));
}

std::wstring Config::utf8ToUtf16(const std::string& str) {
	if (str.empty()) return {};
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), wstr.data(), size_needed);
	return wstr;
}

std::string Config::utf16ToUtf8(const std::wstring& wstr) {
	if (wstr.empty()) return {};
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), str.data(), size_needed, nullptr, nullptr);
	return str;
}
