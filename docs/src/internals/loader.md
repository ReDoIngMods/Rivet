# The Loader

`RivetLoader.dll` is the central runtime piece. It is loaded by Doorstop and runs inside the game process.

## Startup Responsibilities

During `DLL_PROCESS_ATTACH`, the loader:

1. Reads loader flags from `Rivet.ini` and command-line overrides.
2. Initialises MinHook.
3. Installs Rivet's own internal hooks, currently the hook on `LuaVM_Initialize`.
4. Scans the configured mods directory.
5. Loads each `.dll` mod with `LoadLibraryA`.
6. Looks for the `GET_RIVET_MOD_DEF` export.
7. Calls each mod's entry function.
8. Publishes `Rivet::BuiltinEvents::RivetInitialized`.

DLLs ending in `.old` are skipped so users can disable a mod without deleting it.

## Mod Entry Contract

A Rivet mod is any DLL that exports `GET_RIVET_MOD_DEF`. The `RIVET_REGISTER_MOD` macro emits that export and returns a `Rivet::ModDef` containing the mod name, author, and entry function.

If a DLL does not export `GET_RIVET_MOD_DEF`, the loader leaves it alone after `LoadLibraryA`. That allows ordinary dependency DLLs to sit next to mods without being treated as Rivet mods.

## Built-in Hook

The loader currently installs one game hook:

```text
LuaVM_Initialize_RVA = 0x54A7F0
```

The detour publishes:

- `LuaInitializing` before the original function runs.
- `LuaInitialized` after the original function returns.

Both events include the `lua_State*` and an `isTerrain` flag derived from the game's `envMode` argument.

## Shared State

Loader state is intentionally small:

- `LoaderState` stores metadata for loaded mods.
- `EventManager` owns type registration and event dispatch.
- `HookManager` owns MinHook handles and conflict tracking.

There is no runtime mod unload path today. A loaded mod's hooks and event subscriptions last until process exit.

