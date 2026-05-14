#include "doorstopFlags.h"

#include "cli.h"
#include "config.h"

using namespace Rivet;

DoorstopFlags DoorstopFlags::Load() {
	Config doorstopConfig(L"Doorstop");
	CLI cli;
	DoorstopFlags flags;

	flags.enable = cli.GetValue<bool>("rivetEnable", doorstopConfig.GetValue<bool>("enable", false));
	flags.log = cli.GetValue<std::string>("rivetLog", doorstopConfig.GetValue<std::string>("log", "rivet.log"));
	flags.target = cli.GetValue<std::string>("rivetTarget", doorstopConfig.GetValue<std::string>("target", "rivet.dll"));
	flags.hideConsole = cli.GetValue<bool>("rivetHideConsole", doorstopConfig.GetValue<bool>("hideConsole", false));
	flags.saveConfig = cli.GetValue<bool>("rivetSaveConfig", false);

	if (flags.saveConfig) {
		doorstopConfig.SetValue("enable", flags.enable ? "true" : "false");
		doorstopConfig.SetValue("log", flags.log);
		doorstopConfig.SetValue("target", flags.target);
		doorstopConfig.SetValue("hideConsole", flags.hideConsole ? "true" : "false");
	}

	if (!flags.enable) {
		flags.hideConsole = true;
	}

	return flags;
}
