# Built-in Events

All built-in event types are declared in `<rivet/events_builtin.h>` under the `Rivet::BuiltinEvents` namespace.

Events marked **Published** are fired by Rivet today. Events marked **Declared only** are defined so mods can reference them, but no Rivet hook publishes them yet. They will start firing in a later release once the corresponding game hook is added.

## Published

### `RivetInitialized`

Fired once after all mods have loaded and registered their subscriptions.

```cpp
struct RivetInitialized {};
```

Use this for one-time setup that needs other mods to be present (e.g. checking whether a dependency mod is loaded).

### `LuaInitializing`

Fired immediately before the game's `LuaVM_Initialize` runs.

```cpp
struct LuaInitializing {
    lua_State* L;
    bool isTerrain;
};
```

`L` may be null at this point because the game has not yet populated `*pL`. `isTerrain` is `true` when the VM being initialised is the terrain VM, `false` for the main game VM.

Most mods don't need this; prefer `LuaInitialized` unless you specifically need to act before the VM is built.

### `LuaInitialized`

Fired immediately after `LuaVM_Initialize` returns.

```cpp
struct LuaInitialized {
    lua_State* L;
    bool isTerrain;
};
```

Recommended over `LuaInitializing` for most use cases. `sm` and other important game-side tables become available by this point. `isTerrain` matches the corresponding `LuaInitializing` event.

## Declared only

The following events are defined in the header but not currently published. Subscribing to them is harmless; they will start firing in a future release.

| Type | Payload | When it will fire |
|------|---------|-------------------|
| `RivetShutdown` | (empty) | During shutdown, before mods are torn down. |
| `ContraptionInitialized` | (empty) | When the engine completes initialisation. |
| `ContraptionShutdown` | (empty) | When the engine begins shutdown. |
| `ContraptionStateChanged` | `int newState` | When the engine state changes. |
| `ContraptionUpdate` | `float deltaTime` | Before the engine's per-frame update logic. |
| `ContraptionPostUpdate` | `float deltaTime` | After the engine's per-frame update logic. |
| `ContraptionRender` | (empty) | Before the engine renders. Will run on the render thread. |
| `ContraptionPostRender` | (empty) | After the engine renders. Will run on the render thread. |
