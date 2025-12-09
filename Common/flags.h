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
	
		static Flags GetFlags() {
			Config doorstopConfig(L"Doorstop");
			Config loaderConfig(L"Loader");

			CLI cli;

			Flags flags;

			flags.doorstop.enable = cli.getValue<bool>("rivetEnable", doorstopConfig.getValue<bool>("enable", false));
			flags.doorstop.log = cli.getValue<std::string>("rivetLog", doorstopConfig.getValue<std::string>("log", "rivet.log"));
			flags.doorstop.target = cli.getValue<std::string>("rivetTarget", doorstopConfig.getValue<std::string>("target", "rivet.dll"));
			flags.doorstop.hideConsole = cli.getValue<bool>("rivetHideConsole", doorstopConfig.getValue<bool>("hideConsole", false));
			flags.doorstop.saveConfig = cli.getValue<bool>("rivetSaveConfig", false);

			flags.loader.directory = cli.getValue<std::string>("rivetDirectory", loaderConfig.getValue<std::string>("directory", "Mods"));

			if (flags.doorstop.saveConfig) {
				doorstopConfig.setValue("enable", flags.doorstop.enable ? "true" : "false");
				doorstopConfig.setValue("log", flags.doorstop.log);
				doorstopConfig.setValue("target", flags.doorstop.target);
				doorstopConfig.setValue("hideConsole", flags.doorstop.hideConsole ? "true" : "false");

				loaderConfig.setValue("directory", flags.loader.directory);
			}

			return flags;
		}
	};
}
