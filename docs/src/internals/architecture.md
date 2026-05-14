# Architecture

Rivet is split into three projects:

- `src/Doorstop`: the `version.dll` proxy that gets code into the game process.
- `src/Loader`: the runtime service DLL that loads mods, owns hooks, and dispatches events.
- `src/Lib`: public headers used by mods.

The design goal is to keep mod binaries simple. Mods include headers, export a `ModDef`, and call a small C ABI exported by `RivetLoader.dll`. They do not link a Rivet import library, initialise MinHook, or coordinate directly with other mods.

## Startup Flow

```text
Game starts
  -> Windows loads version.dll from the game directory
  -> RivetDoorstop proxies real Version APIs
  -> RivetDoorstop loads RivetLoader.dll if enabled
  -> RivetLoader initialises MinHook and internal hooks
  -> RivetLoader scans Mods for DLLs
  -> each Rivet mod entry function registers hooks and event subscribers
  -> RivetLoader publishes RivetInitialized
```

## Runtime Services

`RivetLoader.dll` owns the two shared services mods need:

- `EventManager`: maps compile-time event type hashes to dense event ids, stores subscribers, and dispatches typed event payloads through a type-erased callback ABI.
- `HookManager`: wraps MinHook, tracks hook ownership by target address, and rejects conflicting detours.

The public headers in `src/Lib/include/rivet` resolve exported loader functions with `GetProcAddress`. That keeps the mod ABI narrow and avoids an extra `.lib` dependency for mod authors.

## Public ABI

The stable boundary is a small `extern "C"` export set from `RivetLoader.dll`:

```text
Rivet_EventRegisterType
Rivet_EventSubscribe
Rivet_EventUnsubscribe
Rivet_EventPublish
Rivet_HookInstall
Rivet_HookRemove
Rivet_HookGetOwner
```

Everything else is implementation detail. Mod-facing types and helpers live in the headers.

