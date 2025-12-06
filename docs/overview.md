# Rivet Overview

Rivet is a modding framework designed to make it easy to load and manage mods for your game. It leverages Rivet Doorstop to load custom DLLs into the game process, allowing for extensive modding capabilities without modifying the game's files, only adding a single DLL to the game directory.

Please refer to the [Installation Guide](./installation.md) for detailed steps on how to set up Rivet Doorstop and get started with modding your game.

## Details

Rivet Doorstop works via "DLL hijacking", which allows it to load custom DLLs into the game process without modifying the game's executable. This makes it a safe and effective way to add mods to your game. `version.dll` is what actually contains Rivet Doorstop's code, and it is loaded by the game in place of the original `version.dll` that comes with Windows (you can read more about DLL hijacking [here](https://en.wikipedia.org/wiki/DLL_hijacking)). 

When the game loads `version.dll`, Rivet Doorstop takes control and looks for a target DLL to load (by default `rivet.dll`, but this can be changed in the configuration). This target DLL is your actual mod loader (e.g. `Rivet` itself, or another mod loader). Rivet Doorstop then loads this target DLL into the game process, allowing it to run your mods.

Rivet Doorstop is highly configurable, allowing you to customize its behavior via the `RivetDoorstop.ini` file or command line arguments. You can enable or disable Rivet Doorstop, specify custom log files, change the target DLL, and more. For detailed configuration options, refer to the [Configuration of Rivet Doorstop](./doorstop/config.md) guide.