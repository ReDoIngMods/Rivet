#include "loaderFlags.h"

#include "cli.h"
#include "config.h"

using namespace Rivet;

LoaderFlags LoaderFlags::Load() {
	Config loaderConfig(L"Loader");
	CLI cli;
	LoaderFlags flags;

	flags.directory = cli.GetValue<std::string>("rivetDirectory", loaderConfig.GetValue<std::string>("directory", "Mods"));

	return flags;
}
