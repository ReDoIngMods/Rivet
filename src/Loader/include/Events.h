#pragma once

#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace Rivet {

using EventId = std::uint32_t;
using EventHandle = std::uint64_t;
using EventCallback = void (*)(const void* payload, void* user);

// EventManager owns the dispatch tables for typed events. Mods reach it via
// the C ABI exported from RivetLoader.dll (see Rivet_Event* in Events.cpp);
// the Loader itself uses it directly. Type identity is established by a
// compile-time FNV1a hash of std::source_location::current().function_name()
// for IdOf<T>() in the public header. Identical mangling across DLLs for
// the same T, so any mod or Rivet itself agrees on the id for a given type.
//
// Dispatch is integer-indexed: the EventId is a dense u32 that maps directly
// into byEvent_[id], a vector<Subscriber>. No string compares, no hashmap on
// the hot path. Subscribe and Publish share a mutex in v1, fine for the
// expected volume; revisit if dispatch ever becomes hot.
class EventManager {
public:
	static EventManager& GetInstance();

	EventId RegisterType(std::uint64_t typeHash);
	EventHandle Subscribe(EventId id, EventCallback fn, void* user);
	bool Unsubscribe(EventHandle handle);
	void Publish(EventId id, const void* payload);

private:
	struct Subscriber {
		EventHandle handle;
		EventCallback fn;
		void* user;
	};

	std::mutex mu_;
	std::unordered_map<std::uint64_t, EventId> hashToId_;
	std::vector<std::vector<Subscriber>> byEvent_;
	EventHandle nextHandle_ = 1;
};

} // namespace Rivet
