# Rivet Overview

Rivet is a modding toolkit designed to make it easy to load and manage mods for your game. This repository contains the core components used to inject DLLs into the game and to load mods in a structured way.

It is made up of several parts that work together:

- `RivetDoorstop`: loads a DLL at game start using a raw `LoadLibrary` call.
- `RivetLoader`: loads mods, attempts to use metadata and `IMod` interfaces if they exist, and falls back to a raw `LoadLibrary` when needed.
- `RivetLib`: the library that mods reference to implement these interfaces and integrate with Rivet, similar in spirit to how BepInEx and Harmony are used for Unity.

When this documentation talks about "Rivet" in general, it refers to this toolkit as a whole.

> [!NOTE]
> This repository documents using Rivet Doorstop together with Rivet Loader. Rivet Doorstop itself is generic and can be used to load other mod loaders, but that usage is outside the scope of this documentation.

Please refer to the [Installation Guide](./installation.md) for detailed steps on how to set up Rivet Doorstop and get started with modding your game.

## Details

Rivet Doorstop works via "DLL hijacking", which allows it to load custom DLLs into the game process without modifying the game's executable. This makes it a safe and effective way to add mods to your game. `version.dll` is what actually contains Rivet Doorstop's code, and it is loaded by the game in place of the original `version.dll` that comes with Windows (you can read more about DLL hijacking [here](https://en.wikipedia.org/wiki/DLL_hijacking)). 

When the game loads `version.dll`, Rivet Doorstop takes control and looks for a target DLL to load (by default `rivet.dll`, but this can be changed in the configuration). This target DLL is your actual mod loader (for example the Rivet Loader itself, or another mod loader). Rivet Doorstop then loads this target DLL into the game process, allowing it to run your mods.

Rivet Doorstop is highly configurable, allowing you to customize its behavior via the `RivetDoorstop.ini` file or command line arguments. You can enable or disable Rivet Doorstop, specify custom log files, change the target DLL, and more. For detailed configuration options, refer to the [Configuration of Rivet Doorstop](./doorstop/config.md) guide.