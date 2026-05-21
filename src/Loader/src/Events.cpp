#include "Events.h"
#include "console.h"

namespace Rivet {

EventManager& EventManager::GetInstance() {
	static EventManager instance;
	return instance;
}

EventId EventManager::RegisterType(std::uint64_t typeHash) {
	std::lock_guard lock(mu_);

	if (auto it = hashToId_.find(typeHash); it != hashToId_.end()) {
		return it->second;
	}

	const EventId id = static_cast<EventId>(byEvent_.size());
	byEvent_.emplace_back();
	hashToId_.emplace(typeHash, id);
	CONSOLE_INFO("RegisterType: typeHash=0x%016llX -> id=%u (total types=%zu).",
		static_cast<unsigned long long>(typeHash), id, byEvent_.size());
	return id;
}

EventHandle EventManager::Subscribe(EventId id, EventCallback fn, void* user) {
	if (!fn) {
		return 0;
	}

	std::lock_guard lock(mu_);
	if (id >= byEvent_.size()) {
		CONSOLE_ERROR("Subscribe: invalid event id %u (registered %zu).", id, byEvent_.size());
		return 0;
	}

	const EventHandle handle = nextHandle_++;
	byEvent_[id].push_back(Subscriber{handle, fn, user});
	CONSOLE_INFO("Subscribe: id=%u handle=%llu fn=%p (total subs for id=%zu).",
		id, static_cast<unsigned long long>(handle), reinterpret_cast<void*>(fn), byEvent_[id].size());
	return handle;
}

bool EventManager::Unsubscribe(EventHandle handle) {
	if (!handle) {
		return false;
	}

	std::lock_guard lock(mu_);
	for (auto& subs : byEvent_) {
		for (auto it = subs.begin(); it != subs.end(); ++it) {
			if (it->handle == handle) {
				subs.erase(it);
				return true;
			}
		}
	}
	return false;
}

void EventManager::Publish(EventId id, const void* payload) {
	std::lock_guard lock(mu_);
	if (id >= byEvent_.size()) {
		CONSOLE_WARN("Publish: id=%u out of range (registered=%zu).", id, byEvent_.size());
		return;
	}
	CONSOLE_INFO("Publish: id=%u subs=%zu.", id, byEvent_[id].size());
	for (const auto& s : byEvent_[id]) {
		s.fn(payload, s.user);
	}
}


} // namespace Rivet

extern "C" {

__declspec(dllexport) Rivet::EventId Rivet_EventRegisterType(std::uint64_t typeHash) {
	return Rivet::EventManager::GetInstance().RegisterType(typeHash);
}

__declspec(dllexport) Rivet::EventHandle Rivet_EventSubscribe(
	Rivet::EventId id, Rivet::EventCallback fn, void* user) {
	return Rivet::EventManager::GetInstance().Subscribe(id, fn, user);
}

__declspec(dllexport) bool Rivet_EventUnsubscribe(Rivet::EventHandle handle) {
	return Rivet::EventManager::GetInstance().Unsubscribe(handle);
}

__declspec(dllexport) void Rivet_EventPublish(Rivet::EventId id, const void* payload) {
	Rivet::EventManager::GetInstance().Publish(id, payload);
}

} // extern "C"
