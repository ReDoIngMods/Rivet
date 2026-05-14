#include "doorstop_flags.h"

#include "cli.h"
#include "config.h"

using namespace Rivet;

DoorstopFlags DoorstopFlags::Load() {
	Config doorstopConfig(L"Doorstop");
	CLI cli;
	DoorstopFlags flags;

	flags.enable = cli.getValue<bool>("rivetEnable", doorstopConfig.getValue<bool>("enable", false));
	flags.log = cli.getValue<std::string>("rivetLog", doorstopConfig.getValue<std::string>("log", "rivet.log"));
	flags.target = cli.getValue<std::string>("rivetTarget", doorstopConfig.getValue<std::string>("target", "rivet.dll"));
	flags.hideConsole = cli.getValue<bool>("rivetHideConsole", doorstopConfig.getValue<bool>("hideConsole", false));
	flags.saveConfig = cli.getValue<bool>("rivetSaveConfig", false);

	if (flags.saveConfig) {
		doorstopConfig.setValue("enable", flags.enable ? "true" : "false");
		doorstopConfig.setValue("log", flags.log);
		doorstopConfig.setValue("target", flags.target);
		doorstopConfig.setValue("hideConsole", flags.hideConsole ? "true" : "false");
	}

	if (!flags.enable) {
		flags.hideConsole = true;
	}

	return flags;
}
