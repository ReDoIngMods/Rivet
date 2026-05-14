# Cross-DLL Type Identity

Rivet events are typed. If Mod A publishes `MyEvent` and Mod B subscribes to `MyEvent`, both DLLs need to agree on the same event id without RTTI, exported symbols, or runtime strings.

Rivet does this in two stages:

1. The public header computes a compile-time `u64` type hash for `T`.
2. The loader maps that hash to a dense `u32` event id for fast dispatch.

## The Compile-time Hash

In `rivet/events.h`, `TypeHash<T>()` hashes the function signature string for the current template instantiation:

```cpp
template <class T>
consteval std::uint64_t TypeHash() {
    return Fnv1a(std::source_location::current().function_name());
}
```

On MSVC, `function_name()` for `TypeHash<MyEvent>()` includes `MyEvent` in the generated signature string. For the same named type and the same compiler family, each DLL sees the same signature string and therefore the same FNV1a result.

The string is only used during constant evaluation. Runtime code only sees the resulting `uint64_t`. Source paths are not part of this string; `file_name()` would include the source path, but Rivet does not hash `file_name()`.

## Loader Registration

The first time a DLL asks for `IdOf<T>()`, the header calls:

```text
Rivet_EventRegisterType(typeHash)
```

The loader then:

1. Checks whether `hashToId_` already contains that hash.
2. Returns the existing id if it does.
3. Otherwise assigns the next dense id and grows `byEvent_`.

Each DLL caches the returned id in a function-local `static`, so later `Publish<T>` and `Subscribe<T>` calls do not cross the registration path again.

## Why Dense IDs

The hot dispatch path should not perform string comparisons or hashmap lookups. Once a type has an id, publish is:

```text
id -> byEvent_[id] -> linear walk over subscribers
```

The hashmap is only used when a type is first registered.

## Limits

This depends on compiler formatting being stable across translation units and DLLs. MSVC's formatting is stable for normal named types, including when source files are built from different directories. MSVC and clang-cl do not emit the same `function_name()` format, so mods built with clang-cl will not share event ids with a loader or other mods built with MSVC unless Rivet switches to an explicit type identity scheme.

Anonymous-namespace or otherwise TU-local event types are a bad contract between mods. Event types intended for cross-mod communication should be named types in shared headers.
