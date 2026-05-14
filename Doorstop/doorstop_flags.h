#pragma once

#include <string>

namespace Rivet {
	struct DoorstopFlags {
		bool enable;
		std::string log;
		std::string target;
		bool hideConsole;
		bool saveConfig;

		static DoorstopFlags Load();
	};
}
