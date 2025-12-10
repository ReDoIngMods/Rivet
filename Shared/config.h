#pragma once

#include "includes.h"

namespace Rivet {
    class Config {
    public:
        Config(std::wstring_view section, std::wstring_view configPath = L".\\Rivet.ini");

        void setValue(const std::string& key, const std::string& value);

        std::string getValue(const std::string& key, const std::string& defaultValue);

        template <StringOrBool T>
        T getValue(const std::string& key, const T& defaultValue) {
            wchar_t buffer[256];
            DWORD charsRead = GetPrivateProfileStringW(section_.data(), utf8ToUtf16(key).data(), nullptr, buffer, sizeof(buffer) / sizeof(wchar_t), configPath_.data());
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
		std::wstring section_;

        std::wstring utf8ToUtf16(const std::string& str);
        std::string utf16ToUtf8(const std::wstring& wstr);
    };
} // namespace Rivet
