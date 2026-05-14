# Versioning

Rivet follows [Semantic Versioning](https://semver.org/) and matches Thunderstore's `MAJOR.MINOR.PATCH` convention.

Each release of Rivet has a version number in the format `MAJOR.MINOR.PATCH`, where:

- **`MAJOR`** is incremented for significant changes or overhauls that may break compatibility.
- **`MINOR`** is incremented for new features or improvements that are backward compatible.
- **`PATCH`** is incremented for bug fixes and minor changes that do not affect compatibility.

## Pre-1.0 caveat

Rivet is currently pre-1.0. Until the first stable release, **minor version bumps may include breaking API changes** while we settle on the public surface. Once 1.0 ships, the SemVer guarantees above hold strictly.

If you depend on a specific Rivet version in your mod, pin to a `MAJOR.MINOR` pair until 1.0.
