# Mod Lifecycle

The order in which Rivet brings a mod up:

1. **Doorstop loads Rivet.** The game's process is up; Doorstop has called `LoadLibrary` on `RivetLoader.dll`.
2. **Rivet initialises MinHook** and installs its own hooks (e.g. on `LuaVM_Initialize`).
3. **Rivet discovers package directories** containing a Thunderstore `manifest.json`.
4. **Rivet resolves dependencies** and computes a deterministic package load order.
5. **For each package DLL:**
   - `LoadLibraryW` brings the root-level DLL into the process. Your DLL's `DllMain` runs at this point under the loader lock, so do nothing meaningful here.
   - Rivet looks for the `GET_RIVET_MOD_DEF` export. If absent, the DLL is support code and is not treated as a mod.
   - Rivet calls the exported entrypoint after the package dependencies have loaded. **This is where you register subscriptions and install hooks.**
5. **Rivet publishes `RivetInitialized`.** Every mod that subscribed to it in its entry function fires now.
6. **Game continues normally.** When game code triggers events Rivet has hooked (currently `LuaVM_Initialize`), Rivet publishes the corresponding events and your handlers run.

## What runs when

| Phase | What you can do | What you can't do reliably |
|-------|------------------|----------------------------|
| `DllMain` (DLL_PROCESS_ATTACH) | Set static initialisers. | Anything. The Windows loader lock is held; calling Rivet APIs or `LoadLibrary` here can deadlock. |
| Your `entry()` function | Register subscriptions, install hooks, read config. | Touch the game's Lua state (it doesn't exist yet). |
| `RivetInitialized` handler | Anything that doesn't depend on game world state. | Touch the game's Lua state (still not created). |
| `LuaInitializing` handler | Pre-VM setup. `L` may be null. | Use `sm`, the game's Lua tables (not populated yet). |
| `LuaInitialized` handler | Use `L`, register Lua functions, read `sm.*`. | Long-running blocking work (this is on the engine thread). |

## Mod unload

Mods are not unloaded once registered. Their subscriptions and hooks persist for the process lifetime. There is no `OnUnload` event today.

## Disabling a mod without removing it

Move the package directory out of Rivet's configured `Mods` directory. Invalid or incomplete packages are skipped without affecting unrelated dependency trees.
