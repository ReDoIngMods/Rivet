# Writing Your First Mod

This walkthrough builds a minimal Rivet mod from scratch: a DLL that prints a message when Rivet finishes loading and again when the game's Lua VM is ready. By the end you will have a working `.dll` you can drop into the game's `Mods` directory.

## What you need

- Visual Studio 2022 with the *Desktop development with C++* workload.
- A clone of [the Rivet repository](https://github.com/ReDoIngMods/Rivet) (you only need the `src/Lib/include` headers).
- Rivet itself installed in your game directory (see [Installation](installation.md)).

## 1. Create a DLL project

In Visual Studio:

1. **File -> New -> Project -> Dynamic-Link Library (DLL)**, C++.
2. Name it whatever your mod is called, e.g. `MyFirstMod`.
3. In **Project Properties -> C/C++ -> General -> Additional Include Directories**, add the path to Rivet's `src/Lib/include` folder.
4. In **Configuration Properties -> General -> C++ Language Standard**, set `ISO C++20 Standard (/std:c++20)` or later. C++20 is the minimum because the event system uses `std::source_location`.
5. Build for **x64** (Rivet only supports 64-bit games).

You don't need to link against any Rivet `.lib`. The header forwards calls to `RivetLoader.dll` at runtime via `GetProcAddress`.

## 2. Write the mod

Replace the contents of your generated `dllmain.cpp` (or whichever source file the wizard created) with:

```cpp
#include <rivet/modding.h>

#include <Windows.h>

static void OnRivetReady(const Rivet::BuiltinEvents::RivetInitialized&) {
    MessageBoxA(nullptr, "MyFirstMod loaded!", "MyFirstMod", MB_OK);
}

static void OnLuaReady(const Rivet::BuiltinEvents::LuaInitialized& e) {
    MessageBoxA(
        nullptr,
        e.isTerrain ? "Terrain Lua VM ready" : "Game Lua VM ready",
        "MyFirstMod",
        MB_OK);
}

static void MyFirstModEntry() {
    Rivet::Events::Subscribe<Rivet::BuiltinEvents::RivetInitialized>(&OnRivetReady);
    Rivet::Events::Subscribe<Rivet::BuiltinEvents::LuaInitialized>(&OnLuaReady);
}

RIVET_REGISTER_MOD(MyFirstModEntry, "Your Name", "MyFirstMod")
```

That's the whole mod. Three pieces:

- **`MyFirstModEntry`** is the entry function. Rivet calls it once when your mod loads. Use it to register subscriptions and install hooks. Don't do heavy work here.
- **`Subscribe<T>(handler)`** registers `handler` to fire whenever someone publishes an event of type `T`. The handler runs synchronously on whichever thread published the event.
- **`RIVET_REGISTER_MOD(entry, author, name)`** is what marks your DLL as a Rivet mod. The macro exports the `GET_RIVET_MOD_DEF` symbol that the loader looks for.

## 3. Build and install

1. Build the project. You'll get `MyFirstMod.dll` in your output folder.
2. Copy it into the `Mods` directory in your game folder (next to `version.dll` and `Rivet.dll`). Create the directory if it doesn't exist.
3. Launch the game.

You should see a Rivet console window log:

```text
[INFO] Loaded mod: MyFirstMod by Your Name
[INFO] Rivet Loader initialized. 1 mods loaded.
```

Followed by your `MyFirstMod loaded!` message box. When the game gets to its Lua initialization, you'll see the `Game Lua VM ready` box.

## What just happened

When the game launches, Doorstop loads Rivet. Rivet scans the `Mods` directory, calls `LoadLibraryA` on `MyFirstMod.dll`, finds your `GET_RIVET_MOD_DEF` export, and calls your entry function. Your entry registers two subscriptions in Rivet's internal event tables.

Later, when Rivet finishes loading all mods, it publishes a `RivetInitialized` event, which fires your `OnRivetReady` handler. When the game itself calls its `LuaVM_Initialize` function, Rivet's hook on that function publishes a `LuaInitialized` event, firing your `OnLuaReady` handler.

Your mod never called any of the game's code directly. It never touched MinHook. It just declared what it cared about and Rivet routed events to it.

## Next steps

- See [The Event System](../guides/events.md) for how subscriptions work in detail, including the `void* user` overload for stateful subscribers.
- See [The Hook System](../guides/hooks.md) to install your own hooks on game functions.
- See [Built-in Events](../reference/built-in-events.md) for the full list of events Rivet publishes.

## Troubleshooting

**My mod's DLL fails to load.** Check the Rivet console for a load error. The most common cause is a missing dependency (e.g. a debug-build Visual C++ runtime DLL). Build in Release mode or ensure the runtime is installed on the target machine.

**`GET_RIVET_MOD_DEF` is not exported.** Confirm `RIVET_REGISTER_MOD(...)` is in a `.cpp` file that's actually compiled into your DLL, not a header. The macro expands to an `extern "C" __declspec(dllexport)` definition.

**My handler never runs.** Check the spelling of the event type and that you're subscribing in your entry function (not in `DllMain`, which runs before Rivet has set up its tables).

**Rivet warns "DLL is not a valid Rivet mod".** This means your DLL loaded but the loader couldn't find the `GET_RIVET_MOD_DEF` export. Same fix as above.
