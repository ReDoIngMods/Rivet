#pragma once
#include <rivet/logging.h>

namespace Rivet {
	extern Logger* GLogger;
}

#define CONSOLE_INFO Rivet::GLogger->Info
#define CONSOLE_WARN Rivet::GLogger->Warn
#define CONSOLE_ERROR Rivet::GLogger->Error
#define CONSOLE_DEBUG Rivet::GLogger->Debug
