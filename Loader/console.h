#pragma once
#include "logger.h"

namespace Rivet {
	extern Logger* GLogger;
}

#define CONSOLE_INFO Rivet::GLogger->info
#define CONSOLE_WARN Rivet::GLogger->warn
#define CONSOLE_ERROR Rivet::GLogger->error
#define CONSOLE_DEBUG Rivet::GLogger->debug
