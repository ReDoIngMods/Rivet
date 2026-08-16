# Rivet

Rivet is a native mod loader for Scrap Mechanic. It injects a loader into the game process, discovers Thunderstore-style packages, resolves their dependencies, and invokes the entrypoints exported by Rivet mod DLLs.

Rivet consists of:

- `RivetDoorstop`: a `version.dll` proxy that loads Rivet into the game process.
- `RivetLoader`: the runtime loader that discovers mods, owns hooks, and dispatches events.
- `RivetLib`: the public headers mods include to use optional Rivet APIs.

Each mod DLL exports only `GET_RIVET_MOD_DEF`, whose descriptor contains an entrypoint. Package metadata and dependencies live in Thunderstore's standard `manifest.json`.

Documentation lives in [`docs/`](./docs/src/introduction.md), is built with mdBook, and publishes to GitHub Pages.

> [!WARNING]
> Currently under heavy development, not ready for real use.

## Thanks

This project would not exist without the help and feedback of many people.

- [@VeraDev0](https://github.com/VeraDev0): For contributing to the repository and helping development
- [@QuestionableM](https://github.com/QuestionableM): For helping with proxying knowledge (e.g. for prompting me to look into asm thunks for proxying DLL exports).
- [@crackx02](https://github.com/crackx02): For helping debug various issues with the DLL loading process.
- Everyone in the [@ReDoIngMods](https://github.com/ReDoIngMods) organization for ideas, testing, and general support.
