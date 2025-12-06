# Rivet versioning

There are multiple "stages" to Rivet's versioning:
- Prealpha: Initial development, not feature complete, likely to have many bugs.
- Alpha: Prototype stage, features may be incomplete/unstable, but basic functionality is present.
- Beta: Feature complete, undergoing testing and bug fixing.
- Stable Release: Official release version, considered stable for general use.

Each release of Rivet will have a version number in the format `MAJOR.MINOR.PATCH-STAGE`, where:
- `MAJOR`: Incremented for significant changes or overhauls that may break compatibility.
- `MINOR`: Incremented for new features or improvements that are backward compatible.
- `PATCH`: Incremented for bug fixes and minor changes that do not affect compatibility.
- `STAGE`: Indicates the development stage (e.g., alpha, beta, stable).

For example, a version number of `1.2.3-beta` indicates the first major release, with two minor updates and three patches, currently in the beta stage.
Rivet's versioning follows [Semantic Versioning](https://semver.org/) principles to ensure clarity and consistency in version numbers.