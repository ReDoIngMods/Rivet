# Rivet

A mod loader for **Scrap Mechanic**. Install it, drop mods into a folder, and the game loads them at startup.

> Rivet is early in development. Expect rough edges and back up your saves.

## Installing

The easiest way is through a mod manager like [r2modman](https://thunderstore.io/c/scrap-mechanic/p/ebkr/r2modman/) or the Thunderstore Mod Manager — click **Install** at the top of this page and you're done.

If you'd rather install by hand, see the [manual install guide](https://github.com/ReDoIngMods/Rivet/blob/main/docs/installation.md).

## Turning Rivet on

Rivet ships **disabled** so it can't surprise you. To turn it on:

- **Through a mod manager:** launch the game from the manager — Rivet is enabled automatically.
- **Manually:** open `Rivet.ini` in your game folder and set `enable=true`, or add `-rivetEnable` to your Steam launch options.

## Adding mods

Drop mod `.dll` files into the `Rivet/Mods/` folder next to `ScrapMechanic.exe`. Most mods from Thunderstore will place themselves there for you.

## Tweaking it

Rivet has a handful of options in `Rivet.ini` — things like hiding the console window, changing the mods folder, or pointing it at a different log file. The full list lives in the [configuration guide](https://github.com/ReDoIngMods/Rivet/blob/main/docs/config.md).

## Something broken?

- Check your `rivet.log` in the game folder first — it usually says what went wrong.
- Report issues on the [GitHub issue tracker](https://github.com/ReDoIngMods/Rivet/issues).

## Credits

Rivet is built and maintained with help from a lot of people:

- [@VeraDev0](https://github.com/VeraDev0) — contributions and ongoing development
- [@QuestionableM](https://github.com/QuestionableM) — DLL proxying knowledge
- [@crackx02](https://github.com/crackx02) — debugging the DLL loading pipeline
- Everyone in [ReDoIng Mods](https://github.com/ReDoIngMods) for ideas, testing, and support

Source code, docs, and mod-development info: [github.com/ReDoIngMods/Rivet](https://github.com/ReDoIngMods/Rivet)
