#pragma once

namespace Rivet {
using ModEntrypoint = void (*)();

/// The only data exchanged between Rivet and a loaded mod DLL.
///
/// Package metadata belongs in Thunderstore's manifest.json. This descriptor
/// only supplies the code entrypoint that Rivet invokes after loading.
struct ModDef {
	ModEntrypoint entrypoint;
};
} // namespace Rivet
