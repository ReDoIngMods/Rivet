# Rivet

Rivet is a mod loader for Scrap Mechanic.

> [!WARNING]
> Currently under heavy development, not ready for real use.

## Rivet Doorstop

Doorstop is the basic tool to get a DLL loaded into the game as soon as possible, this is achieved by DLL hijacking.
You can imagine this like [Unity Doorstop](https://github.com/NeighTools/UnityDoorstop) but for Contraption.

### TODO:

- [x] Basic hijacking
- [x] Basic loading of DLL based on CLI args/config flags
- [x] Pre contraption init hijacking
- [ ] Debugging utilities

## Rivet common

Rivet common may be shared across multiple modules of Rivet. For example, the loader and the lib.

### TODO:

- [ ] API for mod (`IRivetMod`)

## Rivet loader

### TODO:

- [ ] Loading of DLLs

## Rivet lib

Rivet lib is what every mod uses to create a mod compatible with Rivet.

- [ ] Multihooking
- [ ] Mod info inspection (including other mods)
- [ ] SmSdk integration
