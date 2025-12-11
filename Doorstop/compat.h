#pragma once

#include <includes.h>

namespace Rivet {
	namespace Compat {
		void InitializeFunctionPointers();
		void PatchAllocConsole();
		void Initialize();
	}
}
