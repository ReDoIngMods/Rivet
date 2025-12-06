# Configuration of Rivet Doorstop

## End user guide

If you're simply looking to configure Rivet Doorstop for your game, you can do so by editing the `RivetDoorstop.ini` file located in your game's directory. This file is created automatically the first time you run the game with Rivet Doorstop and has all the available configuration options listed.

## Details

Rivet Doorstop can be configured using `RivetDoorstop.ini` (automatically generated in the game directory on first run) or by using command line arguments.

Here are the available configuration options:
| Option                 | Description                                                                 | Default Value        |
|------------------------|-----------------------------------------------------------------------------|----------------------|
| `rivetEnable`          | Enable or disable Rivet Doorstop functionality.                             | `false`              |
| `rivetLog <FILE>`      | Specify a log file for Rivet Doorstop logs.                                 | `rivet.log`          |
| `rivetTarget <FILE>`   | Specify the target file for Rivet Doorstop to load.                         | `rivet.dll`          |
| `rivetHideConsole`     | Suppress the console window for Rivet Doorstop.                             | `false`              |
| `rivetSaveConfig`      | Save the current configuration to `RivetDoorstop.ini`.                      | `false`              |

To use these options, you can either edit the `RivetDoorstop.ini` file directly or pass them as command line arguments when launching the game. For example by adding the following to your game's launch options:

```
-rivetEnable -rivetLog custom_log.txt -rivetTarget custom_target.dll
```

Or if launching the game from the command line (don't forget to have an `steam_appid.txt` in the game directory if it's a Steam game):

```
game.exe -rivetEnable -rivetLog custom_log.txt -rivetTarget custom_target.dll
```

Or you can change them in the `RivetDoorstop.ini` file like so:

```ini
[RivetDoorstop]
rivetEnable=true
rivetLog=custom_log.txt
rivetTarget=custom_target.dll
rivetHideConsole=true
```

Don't forget you can also save your current configuration back to `RivetDoorstop.ini` by using the `-rivetSaveConfig` option on the command line.