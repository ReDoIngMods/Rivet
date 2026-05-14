#pragma once

namespace Rivet {
/// <summary>
/// A mod definition, returned by the GET_RIVET_MOD_DEF export of a mod DLL.
/// `entry` is called once after the mod is loaded; the mod uses it to
/// register event subscriptions via Rivet::Events::Subscribe.
/// </summary>
struct ModDef {
	void (*entry)();
	const char* (*getName)();
	const char* (*getAuthor)();
};
} // namespace Rivet
