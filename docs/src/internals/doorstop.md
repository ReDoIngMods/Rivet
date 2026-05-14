# Doorstop

`RivetDoorstop` is a `version.dll` proxy. It works because Windows searches the game directory before the system directory when loading DLLs. When the game asks for `version.dll`, it loads Rivet's proxy first.

## What It Does

Doorstop has two jobs:

1. Proxy the real Windows Version API exports so the game still behaves normally.
2. Load the configured target DLL, usually `RivetLoader.dll`, when `Doorstop.enable` is true.

Configuration comes from `Rivet.ini` and command-line overrides. See [Configuration](../reference/configuration.md).

## Startup Flow

```text
Game loads version.dll
  -> RivetDoorstop starts
  -> proxy exports are initialised
  -> DoorstopFlags are loaded
  -> if enabled, LoadLibraryA(target)
  -> target is usually RivetLoader.dll
```

If the target DLL fails to load, Doorstop logs the Windows error code and formatted system message.

## Why This Exists

The game executable does not need to be patched. Dropping `version.dll` next to the game executable is enough to enter the process early and then delegate the real mod loading work to `RivetLoader.dll`.

