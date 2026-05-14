# Introduction

Rivet is a modding framework. It loads your DLL into a target game and gives mods a coordinated way to hook engine functions and exchange events with each other.

It is made up of three parts that work together:

- **RivetDoorstop** is a `version.dll` proxy. The game loads it instead of the real Windows `version.dll`, and Doorstop in turn loads Rivet (or any other mod loader you point it at). This is the bootstrap step that gets Rivet's code running inside the game process without modifying the game executable.
- **RivetLoader** is the mod loader. It scans your mods directory, loads each mod DLL, calls its entry function, and runs the event dispatch and hook coordination services that mods rely on.
- **RivetLib** is the header-only library mods include. It exposes `Rivet::Events`, `Rivet::Hooks`, and the `RIVET_REGISTER_MOD` macro. Mods link nothing extra; the headers resolve Rivet's services from the loader at runtime.

When this documentation says "Rivet" without qualification, it means all three together.

## Where to start

If you want to **install Rivet to play modded games**, head to [Installation](getting-started/installation.md).

If you want to **write a mod**, start with [Writing Your First Mod](getting-started/writing-your-first-mod.md), then read [The Event System](guides/events.md) and [The Hook System](guides/hooks.md).

If you want to **work on Rivet itself**, the [Architecture](internals/architecture.md) page is the entry point.

## How Rivet relates to other modding frameworks

Rivet is similar in spirit to BepInEx + Harmony for Unity games, or to MelonLoader for Mono titles. The differences are practical:

- Rivet is for native (C++) games, not managed runtimes. There is no IL rewriting or runtime patching of managed code; mods hook native functions through MinHook.
- Rivet provides a typed, in-process event bus so mods can collaborate without each having to publish their own DLL exports or hook the same functions.
- Hook conflicts are detected at install time. Two mods cannot silently overwrite each other's hooks on the same address.

## Caveats

Rivet is in early development. The API surface is small and changes between minor versions can be breaking until 1.0. See [Versioning](reference/versioning.md) for details.
