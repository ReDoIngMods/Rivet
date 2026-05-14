#include "Hooks.h"
#include "console.h"

#include <MinHook.h>

namespace Rivet {

HookManager& HookManager::GetInstance() {
	static HookManager instance;
	return instance;
}

HookHandle HookManager::Install(void* target, void* detour, void** origOut, const char* ownerName) {
	if (!target || !detour) {
		return 0;
	}

	std::lock_guard lock(mu_);

	if (auto it = byTarget_.find(target); it != byTarget_.end()) {
		const auto& existing = byHandle_.at(it->second);
		CONSOLE_WARN("Hook conflict at %p: '%s' tried to install but '%s' already owns it.",
			target, ownerName ? ownerName : "<anonymous>", existing.owner.c_str());
		return 0;
	}

	if (MH_CreateHook(target, detour, origOut) != MH_OK) {
		CONSOLE_ERROR("MH_CreateHook failed at %p (owner '%s').", target, ownerName ? ownerName : "<anonymous>");
		return 0;
	}
	if (MH_EnableHook(target) != MH_OK) {
		CONSOLE_ERROR("MH_EnableHook failed at %p (owner '%s').", target, ownerName ? ownerName : "<anonymous>");
		MH_RemoveHook(target);
		return 0;
	}

	const HookHandle handle = nextHandle_++;
	byHandle_[handle] = Entry{target, detour, ownerName ? ownerName : ""};
	byTarget_[target] = handle;
	return handle;
}

bool HookManager::Remove(HookHandle handle) {
	std::lock_guard lock(mu_);

	auto it = byHandle_.find(handle);
	if (it == byHandle_.end()) {
		return false;
	}

	void* target = it->second.target;
	if (MH_DisableHook(target) != MH_OK) {
		CONSOLE_ERROR("MH_DisableHook failed at %p.", target);
		return false;
	}
	if (MH_RemoveHook(target) != MH_OK) {
		CONSOLE_ERROR("MH_RemoveHook failed at %p.", target);
		return false;
	}

	byTarget_.erase(target);
	byHandle_.erase(it);
	return true;
}

const char* HookManager::GetOwner(void* target) {
	std::lock_guard lock(mu_);
	auto it = byTarget_.find(target);
	if (it == byTarget_.end()) {
		return nullptr;
	}
	return byHandle_.at(it->second).owner.c_str();
}

} // namespace Rivet

extern "C" {

__declspec(dllexport) Rivet::HookHandle Rivet_HookInstall(
	void* target, void* detour, void** origOut, const char* ownerName) {
	return Rivet::HookManager::GetInstance().Install(target, detour, origOut, ownerName);
}

__declspec(dllexport) bool Rivet_HookRemove(Rivet::HookHandle handle) {
	return Rivet::HookManager::GetInstance().Remove(handle);
}

__declspec(dllexport) const char* Rivet_HookGetOwner(void* target) {
	return Rivet::HookManager::GetInstance().GetOwner(target);
}

} // extern "C"
