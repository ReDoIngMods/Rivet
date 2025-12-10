#pragma once

#include "includes.h"

namespace Rivet {
	class CLI {
	public:
		CLI();

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

		std::optional<std::string> getArgValue(const std::string& key) const;

		const std::unordered_map<std::string, std::string>& getAllArgs() const {
			return argsMap_;
		}
	private:
		std::unordered_map<std::string, std::string> argsMap_;

		static std::vector<std::string> GetCommandLineArgs();
	};
} // namespace Rivet
