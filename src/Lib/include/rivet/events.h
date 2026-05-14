#pragma once

#include <cstdint>
#include <source_location>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Rivet::Events {

using EventId = std::uint32_t;
using Handle = std::uint64_t;

namespace detail {

using TypeErasedCallback = void (*)(const void* payload, void* user);

using RegisterTypeFn = EventId (*)(std::uint64_t typeHash);
using SubscribeFn = Handle (*)(EventId id, TypeErasedCallback fn, void* user);
using UnsubscribeFn = bool (*)(Handle handle);
using PublishFn = void (*)(EventId id, const void* payload);

inline FARPROC ResolveExport(const char* name) {
	static HMODULE mod = GetModuleHandleA("RivetLoader.dll");
	if (!mod) {
		return nullptr;
	}
	return GetProcAddress(mod, name);
}

consteval std::uint64_t Fnv1a(const char* s) {
	std::uint64_t h = 14695981039346656037ULL;
	while (*s) {
		h ^= static_cast<unsigned char>(*s);
		h *= 1099511628211ULL;
		++s;
	}
	return h;
}

// Compile-time hash of source_location::function_name() inside this template,
// which embeds the compiler's spelling of T. Stable across DLL boundaries
// when all participants use the same compiler family and formatting rules.
template <class T>
consteval std::uint64_t TypeHash() {
	return Fnv1a(std::source_location::current().function_name());
}

} // namespace detail

// Returns the dense EventId Rivet has assigned to event type T. Cached per
// translation unit; first call in this DLL crosses to the Loader to look up
// (or assign) the id; subsequent calls are a single load.
template <class T>
inline EventId IdOf() {
	static EventId id = []() -> EventId {
		static auto fn = reinterpret_cast<detail::RegisterTypeFn>(
		    detail::ResolveExport("Rivet_EventRegisterType"));
		return fn ? fn(detail::TypeHash<T>()) : 0;
	}();
	return id;
}

// Subscribe a non-capturing function pointer. The callback receives the event
// payload by const reference.
template <class T>
inline Handle Subscribe(void (*fn)(const T&)) {
	if (!fn) {
		return 0;
	}
	static auto subscribe = reinterpret_cast<detail::SubscribeFn>(
	    detail::ResolveExport("Rivet_EventSubscribe"));
	if (!subscribe) {
		return 0;
	}
	// Adapter is templated on T but not on fn. fn is carried in the user
	// slot and reinterpreted on dispatch. Zero allocation.
	auto adapter = +[](const void* payload, void* user) {
		auto typedFn = reinterpret_cast<void (*)(const T&)>(user);
		typedFn(*static_cast<const T*>(payload));
	};
	return subscribe(IdOf<T>(), adapter, reinterpret_cast<void*>(fn));
}

// Subscribe a function pointer that also receives a user-data pointer. The
// user slot is forwarded verbatim on each dispatch. No heap allocation.
template <class T>
inline Handle Subscribe(void (*fn)(const T&, void*), void* user) {
	if (!fn) {
		return 0;
	}
	static auto subscribe = reinterpret_cast<detail::SubscribeFn>(
	    detail::ResolveExport("Rivet_EventSubscribe"));
	if (!subscribe) {
		return 0;
	}
	// `void(*)(const T&, void*)` and `void(*)(const void*, void*)` share the
	// x64 calling convention (a const T& is passed exactly like a const void*),
	// so the cast preserves arg passing. The callee decodes its first arg as
	// const T*, which is its declared type.
	return subscribe(
	    IdOf<T>(),
	    reinterpret_cast<detail::TypeErasedCallback>(fn),
	    user);
}

inline bool Unsubscribe(Handle handle) {
	static auto fn = reinterpret_cast<detail::UnsubscribeFn>(
	    detail::ResolveExport("Rivet_EventUnsubscribe"));
	return fn ? fn(handle) : false;
}

template <class T>
inline void Publish(const T& payload) {
	static auto fn = reinterpret_cast<detail::PublishFn>(
	    detail::ResolveExport("Rivet_EventPublish"));
	if (fn) {
		fn(IdOf<T>(), &payload);
	}
}

} // namespace Rivet::Events
