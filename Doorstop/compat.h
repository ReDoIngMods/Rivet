#pragma once

#include <includes.h>

namespace Rivet {
	namespace Compat {
		void InitializeFunctionPointers();
		void PatchMyGUI();
		void PatchAllocConsole();
		void Initialize();
	}
}
