# Installation

This page is for end users who want to play modded games. If you are writing a mod, see [Writing Your First Mod](writing-your-first-mod.md).

## Steps

1. **Download Rivet.** Grab the latest release from the [releases page](https://github.com/ReDoIngMods/Rivet/releases) (e.g. `Rivet_0.1.0-alpha.zip`).
2. **Extract the archive** to a temporary location.
3. **Copy the files into your game's directory.** This is the folder containing the game's `.exe`. In Steam you can find it via *right-click the game in your library -> Properties -> Installed Files -> Browse*.
4. **Launch the game.** Rivet will automatically create a `Rivet.ini` file in the game directory on first run.
5. **(Optional) Configure Rivet** by editing `Rivet.ini` or by passing CLI flags. See [Configuration](../reference/configuration.md) for the full list of options.

## Verifying it worked

When Rivet starts, it opens a console window logging the loader status and any mods that were found. If the console does not appear, check that:

- The game directory contains both `version.dll` (Doorstop) and `Rivet.dll` (the loader).
- `Doorstop.enable=true` is set in `Rivet.ini`, or `-rivetEnable` is passed on the command line.
- No antivirus is quarantining the DLLs (Rivet's hooking can look suspicious to some scanners).

## Updating

To update, replace the Rivet files in the game directory with the new release. Your `Rivet.ini` and the `Mods` directory are not touched.
