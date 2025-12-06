#pragma once

#include "includes.h"

namespace Rivet {
	class CLI {
	public:
		CLI() {
			auto args = GetCommandLineArgs();

			for (size_t i = 1; i < args.size(); ++i) {
				if (args[i].rfind("-", 0) == 0) { // starts with '-'
					std::string arg = args[i].substr(1); // remove the '-'
					std::string key, value = "true"; // default for flags

					// Check if arg contains '='
					size_t eqPos = arg.find('=');
					if (eqPos != std::string::npos) {
						// Format: -key=value
						key = arg.substr(0, eqPos);
						value = arg.substr(eqPos + 1);
					} else {
						// Format: -key (flag) or -key value (next arg is value)
						key = arg;
						// Check if next argument exists and is not a flag
						if (i + 1 < args.size() && args[i + 1].rfind("-", 0) != 0) {
							value = args[i + 1];
							++i; // skip next argument as it's a value
						}
					}
					argsMap_[key] = value;
				}
			}
		}

		std::optional<std::string> getArgValue(const std::string& key) const {
			auto it = argsMap_.find(key);
			if (it != argsMap_.end()) {
				return it->second;
			}

			return std::nullopt;
		}

		template <StringOrBool T>
		T getValue(const std::string& key, const T& defaultValue) const {
			auto it = argsMap_.find(key);
			if (it != argsMap_.end()) {
				if constexpr (std::is_same_v<T, bool>) {
					return it->second == "true" || it->second == "1";
				}
				else if constexpr (std::is_same_v<T, std::string>) {
					return it->second;
				}
			}
			return defaultValue;
		}

		const std::unordered_map<std::string, std::string>& getAllArgs() const {
			return argsMap_;
		}
	private:
		std::unordered_map<std::string, std::string> argsMap_;

		static std::vector<std::string> GetCommandLineArgs() {
			std::vector<std::string> args;
			LPWSTR* szArglist;
			int nArgs;
			szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
			if (szArglist) {
				for (int i = 0; i < nArgs; i++) {
					int size_needed = WideCharToMultiByte(CP_UTF8, 0, szArglist[i], -1, NULL, 0, NULL, NULL);
					std::string arg(size_needed - 1, 0); // -1 to exclude null terminator
					WideCharToMultiByte(CP_UTF8, 0, szArglist[i], -1, &arg[0], size_needed, NULL, NULL);
					args.push_back(arg);
				}
				LocalFree(szArglist);
			}
			return args;
		}
	};
} // namespace Rivet
