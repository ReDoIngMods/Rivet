#pragma once

#include <string>

namespace Rivet {
	struct LoaderFlags {
		std::string directory;

		static LoaderFlags Load();
	};
}
