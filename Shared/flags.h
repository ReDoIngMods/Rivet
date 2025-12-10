#pragma once

#include "includes.h"

#include "cli.h"
#include "config.h"

namespace Rivet {
	struct DoorstopFlags {
		bool enable;
		std::string log;
		std::string target;
		bool hideConsole;
		bool saveConfig;
	};

	struct LoaderFlags {
		std::string directory;
	};

	struct Flags {
		DoorstopFlags doorstop;
		LoaderFlags loader;
	
		static Flags GetFlags();
	};
}
