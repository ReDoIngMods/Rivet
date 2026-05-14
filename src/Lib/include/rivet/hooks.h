#pragma once

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Rivet::Hooks {

using Handle = std::uintptr_t;

namespace detail {

using InstallFn = Handle (*)(void* target, void* detour, void** origOut, const char* ownerName);
using RemoveFn  = bool (*)(Handle handle);
using OwnerFn   = const char* (*)(void* target);

inline FARPROC ResolveExport(const char* name) {
	static HMODULE mod = GetModuleHandleA("RivetLoader.dll");
	if (!mod) {
		return nullptr;
	}
	return GetProcAddress(mod, name);
}

} // namespace detail

// Install a hook on `target`. Rivet owns the underlying MinHook trampoline.
// Returns 0 if `target` is already hooked by another mod (call GetOwner() to
// find out who) or if MinHook failed. On success, `*origOut` receives the
// trampoline pointer for calling the original function. `ownerName` should
// identify your mod and is shown in conflict diagnostics.
//
// Stage 1: at most one detour per target. Chaining (multiple subscribers on
// the same address) is not yet supported — coordinate via Rivet events when
// they ship, or via mod-to-mod conventions for now.
inline Handle Install(void* target, void* detour, void** origOut, const char* ownerName) {
	static auto fn = reinterpret_cast<detail::InstallFn>(detail::ResolveExport("Rivet_HookInstall"));
	return fn ? fn(target, detour, origOut, ownerName) : 0;
}

inline bool Remove(Handle handle) {
	static auto fn = reinterpret_cast<detail::RemoveFn>(detail::ResolveExport("Rivet_HookRemove"));
	return fn ? fn(handle) : false;
}

// Returns the owner name of an existing hook on `target`, or nullptr if the
// address is not currently hooked through Rivet.
inline const char* GetOwner(void* target) {
	static auto fn = reinterpret_cast<detail::OwnerFn>(detail::ResolveExport("Rivet_HookGetOwner"));
	return fn ? fn(target) : nullptr;
}

} // namespace Rivet::Hooks
