# Configuration of Rivet

## End user guide

If you're simply looking to configure Rivet for your game, you can do so by editing the `Rivet.ini` file located in your game's directory. This file is created automatically the first time you run the game with Rivet and has all the available configuration options listed.

## File based configuration

Rivet reads its configuration from an INI file called `Rivet.ini` in the game directory. If this file does not exist, it is created automatically with sensible defaults the first time the game is launched with Rivet present.

The INI file has two sections:

- `Doorstop`: options that control Rivet Doorstop itself
- `Loader`: options that control how Rivet loads your mods

Below is an example configuration with all available options and their default values:

```ini
[Doorstop]
; Whether to enable Rivet Doorstop for this game.
; true / false
enable=false

; Path to the log file relative to the game directory.
log=rivet.log

; Name of the target DLL that Rivet Doorstop should load.
; This is usually rivet.dll, but you can change it
; if you want Doorstop to load a different mod loader.
target=rivet.dll

; Hide the Rivet console window when the game starts.
; true / false
hideConsole=false

[Loader]
; Directory that contains your mods, relative to the game directory.
directory=Mods
```

### Saving configuration from CLI

In addition to editing `Rivet.ini` directly, you can ask Rivet to write the current CLI options back into the configuration file by setting `rivetSaveConfig` on the command line. When this flag is present, the effective values for the options above are saved into `Rivet.ini` so that future launches can omit the corresponding CLI flags.

This affects the following keys:

- `Doorstop.enable`
- `Doorstop.log`
- `Doorstop.target`
- `Doorstop.hideConsole`
- `Loader.directory`

For example:

```text
-rivetEnable true -rivetDirectory CustomMods -rivetSaveConfig
```

will result in `enable`, `directory`, and other values being written to `Rivet.ini`.

## Command line configuration

All configuration options can also be provided via command line arguments. Command line values always take precedence over values from `Rivet.ini`.

Rivet parses arguments in the following formats:

- `-key value`
- `-key=value`
- `-key` (for boolean flags, equivalent to `-key true`)

Supported keys are:

- `rivetEnable` (bool): enable or disable Rivet Doorstop for this launch. Overrides `Doorstop.enable`.
- `rivetLog` (string): path to the log file. Overrides `Doorstop.log`.
- `rivetTarget` (string): name or path of the target DLL to load. Overrides `Doorstop.target`.
- `rivetHideConsole` (bool): hide the Rivet console window. Overrides `Doorstop.hideConsole`.
- `rivetSaveConfig` (bool): when true, save the effective configuration back to `Rivet.ini`.
- `rivetDirectory` (string): mods directory. Overrides `Loader.directory`.

### Example command line

```text
-rivetEnable true -rivetTarget rivet.dll -rivetDirectory Mods -rivetLog rivet.log
```

This will enable Rivet, log to `rivet.log`, load `rivet.dll`, and look for mods in the `Mods` directory, regardless of what is in `Rivet.ini`.

## Steam launch configuration example

When launching a game through Steam, you can pass Rivet arguments using Steam launch options. This is particularly useful when you do not want to edit `Rivet.ini` manually or when testing different setups.

Open the game's properties in Steam, go to "Launch Options" and enter:

```text
steam://run/387990//-rivetEnable -rivetTarget C:\Users\Ben\Desktop\rivetDemo\Rivet.dll -rivetDirectory C:\Users\Ben\Desktop\rivetDemo\Mods
```

In this example:

- `-rivetEnable` turns Rivet on for this game.
- `-rivetTarget` points to the DLL that Rivet Doorstop should load.
- `-rivetDirectory` points to the directory containing your mods.

You can combine these with other options described above, such as `-rivetLog` or `-rivetHideConsole`, as needed.