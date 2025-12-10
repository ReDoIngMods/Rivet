# Rivet

Rivet is a modding toolkit for Scrap Mechanic. It gives you a clean way to inject DLLs into the game process and to load mods in a structured, discoverable way.

At a high level Rivet consists of:

- `RivetDoorstop`: gets a DLL into the game as early as possible using DLL hijacking. You can imagine this alike to [Unity Doorstop](https://github.com/NeighTools/UnityDoorstop).
- `RivetLoader`: discovers and loads mods, using metadata and interfaces where available. You can imagine this like [BepInEx](https://github.com/BepInEx/BepInEx).
- `RivetLib`: the library that mods use to integrate with Rivet, similar in spirit to how BepInEx and Harmony are used for Unity. You can imagine this like [BepInEx's API](https://docs.bepinex.dev/api/index.html) and [Harmony](https://github.com/pardeike/Harmony).

If you just want to get started, the [Installation Guide](./docs/installation.md) walks through setting up Rivet for a game, and the [Overview](./docs/overview.md) explains how these pieces fit together.

> Currently under heavy development, not ready for real use.

> Please refer to the [Installation Guide](./docs/installation.md) to set up Rivet for Scrap Mechanic. There is also an [Overview](./docs/overview.md) document that explains how Rivet works and what each component does.

## Thanks

This project would not exist without the help and feedback of many people.

- [@QuestionableM](https://github.com/QuestionableM): For helping with proxying knowledge (e.g. for prompting me to look into asm thunks for proxying DLL exports).
- [@crackx02](https://github.com/crackx02): For helping debug various issues with the DLL loading process.
- Everyone in the [ReDoIng Mods](https://github.com/ReDoIngMods) organization for ideas, testing, and general support.