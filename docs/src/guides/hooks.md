# The Hook System

Rivet exposes a small wrapper around MinHook so that mods don't install detours directly. The wrapper does two things MinHook alone doesn't:

- Tracks which mod owns each hook, so conflicts produce a clear diagnostic instead of silent corruption.
- Centralises the MinHook lifecycle in the loader, so individual mods don't `MH_Initialize` themselves.

```cpp
#include <rivet/hooks.h>

using SomeFn_t = void* (*)(int, int);
static SomeFn_t oOriginal = nullptr;

static void* hkSomeFn(int a, int b) {
    // pre-call work
    void* r = oOriginal(a, b);
    // post-call work
    return r;
}

static void install() {
    auto* target = /* address of the function to hook */;
    auto handle = Rivet::Hooks::Install(
        target,
        &hkSomeFn,
        reinterpret_cast<void**>(&oOriginal),
        "MyMod"); // owner name shown in conflict diagnostics

    if (!handle) {
        const char* owner = Rivet::Hooks::GetOwner(target);
        // someone else already hooked this address; owner names them
    }
}
```

## Stage 1 limitation: one detour per address

Right now a second `Install` call on an already-hooked address fails and returns `0`. The conflict is logged with both owners' names. Two mods cannot chain detours on the same function.

This is intentional for v1. Chaining arbitrary-signature detours generically requires a runtime thunk emitter. Until that lands, the recommended pattern is: if two mods need to react to the same engine entrypoint, hook it in one place (often Rivet itself) and publish an event that both mods subscribe to.

## Resolving addresses from RVAs

Most game functions are referenced by RVA (offset from the module base). Resolve to an absolute address with:

```cpp
const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
auto* target = reinterpret_cast<void*>(base + 0x12345);
```

Use `nullptr` for the main game executable, or pass a specific module name for hooks targeting a particular DLL.

## Removing hooks

```cpp
Rivet::Hooks::Remove(handle);
```

Returns `true` on success. There is no support for unloading a mod at runtime, so most mods never need to call `Remove`. Their hooks live for the duration of the process.
