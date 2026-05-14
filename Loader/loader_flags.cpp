#include "loader_flags.h"

#include "cli.h"
#include "config.h"

using namespace Rivet;

LoaderFlags LoaderFlags::Load() {
	Config loaderConfig(L"Loader");
	CLI cli;
	LoaderFlags flags;

	flags.directory = cli.getValue<std::string>("rivetDirectory", loaderConfig.getValue<std::string>("directory", "Mods"));

	return flags;
}
