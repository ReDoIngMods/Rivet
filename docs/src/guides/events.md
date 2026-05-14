# The Event System

Rivet's event system is how mods and Rivet itself communicate without anyone calling each other directly. A mod publishes a typed value; everyone subscribed to that type gets a callback. No strings, no RTTI, no hashmap lookups during dispatch.

If you just want to use it, the [Writing Your First Mod](../getting-started/writing-your-first-mod.md) tutorial covers the common case. This page explains the model in more depth.

## The basic shape

```cpp
#include <rivet/events.h>

// Define an event type. It's just a plain struct.
struct MyEvent {
    int playerId;
    float damage;
};

// Subscribe.
Rivet::Events::Subscribe<MyEvent>([](const MyEvent& e) {
    // ...
});

// Publish from anywhere.
Rivet::Events::Publish(MyEvent{ .playerId = 42, .damage = 1.5f });
```

Any mod that knows the type `MyEvent` can subscribe to it or publish it. Two different mods can publish to and subscribe from the same event type without referencing each other; the type definition is the only contract.

## Subscribing

There are two `Subscribe` overloads:

```cpp
// Plain function pointer. The handler receives only the payload.
Rivet::Events::Subscribe<MyEvent>(&handler);

// Function pointer with userdata. Rivet forwards the user pointer verbatim
// on each dispatch.
Rivet::Events::Subscribe<MyEvent>(&handler_with_user, &myState);
```

Both overloads return a `Rivet::Events::Handle` you can pass to `Unsubscribe(handle)` later if you need to.

Captures aren't supported. Pass a non-capturing lambda or a free function. If you need to thread state through, use the user-pointer overload:

```cpp
struct ModState { int counter = 0; };
static ModState g_state;

static void OnMyEvent(const MyEvent& e, void* user) {
    auto* state = static_cast<ModState*>(user);
    state->counter++;
}

Rivet::Events::Subscribe<MyEvent>(&OnMyEvent, &g_state);
```

## Publishing

```cpp
Rivet::Events::Publish(MyEvent{ ... });
```

`Publish` blocks until every subscriber's handler has run. Handlers run on the thread that called `Publish`. There is no queueing or deferred dispatch; if you need that, build it on top.

The payload is passed to handlers by const reference. It exists for the duration of the `Publish` call only, so do not store the reference past the handler returning.

## Type identity across DLLs

Every event type `T` needs a process-wide stable id so that when Mod A publishes a `MyEvent`, Mod B's subscription to the same `MyEvent` matches it. We can't use `typeid` reliably across DLLs on MSVC, and we don't want to hash strings at runtime.

The trick: `std::source_location::current().function_name()`, called inside a `consteval` template `TypeHash<T>()`, returns a string that embeds the compiler's spelling of `T`. The same `T` produces the same string in every DLL when those DLLs use the same compiler family and formatting rules. Rivet FNV1a-hashes that string at compile time. The result is a `u64` baked into the call site as a constant. Only the integer ever crosses the ABI.

```text
Mod A DLL                                    RivetLoader.dll
  IdOf<MyEvent>()                              EventManager::RegisterType(hash)
    TypeHash<T>()    ->  0xA31F..C702   --->    if (known) return existing id
    static u32 id    <-      id = 7     <---    else assign next dense id

Mod B DLL                                    (RivetLoader sees the same hash
  IdOf<MyEvent>()                             from any DLL for the same T)
    TypeHash<T>()    ->  0xA31F..C702   --->
    static u32 id    <-      id = 7     <---
```

Each DLL caches the resolved `EventId` in a function-local `static` after the first call, so subsequent `Publish` and `Subscribe` calls are a single load. Different source paths do not affect the hash because `function_name()` is used, not `file_name()`.

For deeper detail, see [Cross-DLL Type Identity](../internals/type-identity.md).

## What the loader stores

The dispatch tables are tiny. The cold path uses an `unordered_map`; the hot path is a contiguous indexed vector.

```cpp
std::unordered_map<std::uint64_t, EventId> hashToId_;     // type hash -> dense id
std::vector<std::vector<Subscriber>>       byEvent_;      // id -> subscribers
EventHandle                                nextHandle_;

struct Subscriber {
    EventHandle   handle;   // u64, used by Unsubscribe
    EventCallback fn;       // void(*)(const void* payload, void* user)
    void*         user;
};
```

`hashToId_` is touched once per type per DLL. `byEvent_[id]` is what `Publish` walks: constant-time index lookup, then a linear pass over a contiguous vector calling each subscriber.

## Performance

Per-Publish cost on a warm cache:

- mutex lock + unlock (uncontended `std::mutex`, SRWLOCK underneath): ~25 ns
- vector indexed lookup: ~2 ns
- per subscriber: 1 indirect call + adapter unpack ~= 5-10 ns

So 0 subscribers ~30 ns, 10 subscribers ~100 ns, 100 subscribers ~600 ns to 1 us.

At 60 Hz a tick is ~16.6 ms. **You can publish thousands of events per tick before the dispatcher itself becomes a measurable cost.** What will bite you, in order:

1. Subscribers that block (disk I/O, message boxes, locks). The dispatcher holds its mutex for the whole loop, so a slow subscriber blocks other Publishes too.
2. Cross-thread `Subscribe` during `Publish` (mutex contention).
3. Recursive `Publish` from inside a subscriber currently triggers UB on `std::mutex`. Don't do it. (To be fixed by switching to copy-under-lock or a recursive mutex; see the API surface evolution notes in the issue tracker.)

## Built-in events

Rivet defines a set of built-in event types in `<rivet/events_builtin.h>` and publishes them from the loader. The most useful ones today:

- `Rivet::BuiltinEvents::RivetInitialized`: published once after all mods finish loading.
- `Rivet::BuiltinEvents::LuaInitializing`: published just before the game's `LuaVM_Initialize` runs.
- `Rivet::BuiltinEvents::LuaInitialized`: published just after. This is where `sm` and other game-side Lua tables become available.

Other built-in event types are declared (`ContraptionUpdate`, `ContraptionRender`, etc.) but are not yet published because the corresponding game hooks haven't been added. Subscribing to them today is harmless; they'll start firing when those hooks land.

See [Built-in Events](../reference/built-in-events.md) for the full reference.

## Defining your own events

Any plain struct works. Two mods that both `#include "shared_events.h"` (or otherwise agree on the definition) can publish and subscribe to the same type.

```cpp
// shared_events.h, distributed alongside your mod or as a separate header.
struct PlayerDamaged {
    std::uint32_t playerId;
    float amount;
    std::uint32_t sourceId;
};

// Publishing mod:
Rivet::Events::Publish(PlayerDamaged{ id, dmg, src });

// Subscribing mod:
Rivet::Events::Subscribe<PlayerDamaged>([](const PlayerDamaged& e) {
    // react to damage
});
```

This is how mods are expected to collaborate without each having to expose DLL exports or know about each other.
