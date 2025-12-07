#pragma once

#include "imod.h"

namespace Rivet {
	/// <summary>
	/// A mod definition, returned by the exported functions in a mod DLL.
	/// </summary>
	struct ModDef {
		IMod* (*create)();
		const char* (*getName)();
		const char* (*getAuthor)();
	};
}
