# Configuration

Rivet reads its configuration from an INI file called `Rivet.ini` in the game directory. If the file does not exist, it is created automatically with sensible defaults the first time the game is launched with Rivet present.

## File-based configuration

The INI file has two sections:

- `[Doorstop]`: options that control RivetDoorstop itself.
- `[Loader]`: options that control how RivetLoader loads mods.

Example with all available options and their defaults:

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

## Saving configuration from the CLI

You can ask Rivet to write the current CLI options back into `Rivet.ini` by passing `-rivetSaveConfig` on the command line. When this flag is present, the effective values are saved so future launches can omit the corresponding CLI flags.

This affects:

- `Doorstop.enable`
- `Doorstop.log`
- `Doorstop.target`
- `Doorstop.hideConsole`
- `Loader.directory`

For example:

```text
-rivetEnable true -rivetDirectory CustomMods -rivetSaveConfig
```

writes `enable`, `directory`, and the other values into `Rivet.ini`.

## Command-line configuration

All configuration options can also be provided via command-line arguments. Command-line values always take precedence over values from `Rivet.ini`.

Rivet accepts arguments in any of these forms:

- `-key value`
- `-key=value`
- `-key` (for boolean flags, equivalent to `-key true`)

Supported keys:

| Key | Type | Overrides |
|-----|------|-----------|
| `rivetEnable` | bool | `Doorstop.enable` |
| `rivetLog` | string | `Doorstop.log` |
| `rivetTarget` | string | `Doorstop.target` |
| `rivetHideConsole` | bool | `Doorstop.hideConsole` |
| `rivetSaveConfig` | bool | (writes the effective config back to `Rivet.ini`) |
| `rivetDirectory` | string | `Loader.directory` |

### Example command line

```text
-rivetEnable true -rivetTarget rivet.dll -rivetDirectory Mods -rivetLog rivet.log
```

This enables Rivet, logs to `rivet.log`, loads `rivet.dll`, and looks for mods in the `Mods` directory regardless of what is in `Rivet.ini`.

## Steam launch options

When launching through Steam you can pass Rivet arguments via launch options. Open the game's properties in Steam, go to **Launch Options** and enter:

```text
steam://run/<APPID>//-rivetEnable -rivetTarget C:\path\to\Rivet.dll -rivetDirectory C:\path\to\Mods
```

Combine with other options (`-rivetLog`, `-rivetHideConsole`, etc.) as needed.
