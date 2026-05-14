#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

namespace Rivet {

using HookHandle = std::uintptr_t;

// HookManager owns every MinHook trampoline in the process. Mods reach it via
// the C ABI exported from RivetLoader.dll (see Rivet_Hook* in Hooks.cpp); the
// Loader itself uses it directly. Stage 1: ownership tracking only — at most
// one detour per target. A second Install attempt on an already-hooked target
// returns 0 and the caller can read the existing owner via GetOwner().
class HookManager {
public:
	static HookManager& GetInstance();

	HookHandle Install(void* target, void* detour, void** origOut, const char* ownerName);
	bool Remove(HookHandle handle);
	const char* GetOwner(void* target);

private:
	struct Entry {
		void* target;
		void* detour;
		std::string owner;
	};

	std::mutex mu_;
	std::unordered_map<HookHandle, Entry> byHandle_;
	std::unordered_map<void*, HookHandle> byTarget_;
	HookHandle nextHandle_ = 1;
};

} // namespace Rivet
