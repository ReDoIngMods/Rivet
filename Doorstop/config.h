#pragma once

#include "doorstop.h"

namespace Rivet {
    class Config {
    public:
		Config(std::wstring_view configPath = L".\\RivetDoorstop.ini") : configPath_(configPath) {
            if (!fs::exists(configPath_)) {
                WritePrivateProfileStringW(L"RivetDoorstop", nullptr, nullptr, configPath_.data()); // Create empty file
				WritePrivateProfileStringW(L"RivetDoorstop", L"rivetEnable", L"false", configPath_.data()); // don't enable by default
                WritePrivateProfileStringW(L"RivetDoorstop", L"rivetLog", L"rivet.log", configPath_.data());
                WritePrivateProfileStringW(L"RivetDoorstop", L"rivetTarget", L"rivet.dll", configPath_.data());
			}
        }

        void setValue(const std::string& key, const std::string& value) {
            WritePrivateProfileStringW(L"RivetDoorstop", utf8ToUtf16(key).data(), utf8ToUtf16(value).data(), configPath_.data());
		}

        std::string getValue(const std::string& key, const std::string& defaultValue) {
            wchar_t buffer[128];
			DWORD charsRead = GetPrivateProfileStringW(L"RivetDoorstop", utf8ToUtf16(key).data(), utf8ToUtf16(defaultValue).data(), buffer, sizeof(buffer) / sizeof(wchar_t), configPath_.data());
			return utf16ToUtf8(std::wstring(buffer, charsRead));
		}

        template <StringOrBool T>
        T getValue(const std::string& key, const T& defaultValue) {
            wchar_t buffer[256];
            DWORD charsRead = GetPrivateProfileStringW(L"RivetDoorstop", utf8ToUtf16(key).data(), nullptr, buffer, sizeof(buffer) / sizeof(wchar_t), configPath_.data());
            if (charsRead == 0) {
                return defaultValue;
            }
			std::wstring valueW(buffer, charsRead);
            if constexpr (std::is_same_v<T, bool>) {
                std::wstring lowerValue;
                std::transform(valueW.begin(), valueW.end(), std::back_inserter(lowerValue), ::towlower);
                if (lowerValue == L"true" || lowerValue == L"1") {
                    return true;
                } else if (lowerValue == L"false" || lowerValue == L"0") {
                    return false;
                } else {
                    return defaultValue;
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                return utf16ToUtf8(valueW);
            }
			return defaultValue;
        }

    private:
        std::wstring_view configPath_;

        std::wstring utf8ToUtf16(const std::string& str) {
            if (str.empty()) return {};
            int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
            std::wstring wstr(size_needed, 0);
            MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), wstr.data(), size_needed);
            return wstr;
        }

        std::string utf16ToUtf8(const std::wstring& wstr) {
            if (wstr.empty()) return {};
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
            std::string str(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), str.data(), size_needed, nullptr, nullptr);
            return str;
        }
    };
} // namespace Rivet
