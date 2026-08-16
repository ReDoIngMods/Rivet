# The Loader

`RivetLoader.dll` is the central runtime piece. It is loaded by Doorstop and runs inside the game process.

## Startup Responsibilities

During startup, the loader:

1. Reads loader flags from `Rivet.ini` and command-line overrides.
2. Initialises MinHook.
3. Installs Rivet's own internal hooks, currently the hook on `LuaVM_Initialize`.
4. Discovers package directories containing a Thunderstore `manifest.json`.
5. Resolves the declared Thunderstore dependency strings into a deterministic load order.
6. Loads root-level support DLLs and Rivet DLLs from each package.
7. Looks for the `GET_RIVET_MOD_DEF` export.
8. Calls each exported entrypoint in dependency order.
9. Publishes `Rivet::BuiltinEvents::RivetInitialized`.

Packages without a valid manifest are ignored so loose support files do not become mods.

## Mod Entry Contract

A Rivet mod is any root-level package DLL that exports `GET_RIVET_MOD_DEF`. The `RIVET_REGISTER_MOD(entrypoint)` macro emits that export and returns a `Rivet::ModDef` containing only the entrypoint.

If a DLL does not export `GET_RIVET_MOD_DEF`, it is treated as a support DLL and is never invoked as a mod. Package names, versions, and dependencies come from Thunderstore's `manifest.json`, not the DLL.

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

- `LoaderState` stores package IDs, module paths, and loaded entrypoints.
- `EventManager` owns type registration and event dispatch.
- `HookManager` owns MinHook handles and conflict tracking.

Declared dependencies are required. Missing dependencies, invalid versions, and dependency cycles prevent only the affected package tree from loading.

There is no runtime mod unload path today. A loaded mod's hooks and event subscriptions last until process exit.

