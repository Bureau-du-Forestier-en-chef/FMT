# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

> Version française : [CHANGELOG.fr.md](CHANGELOG.fr.md)

## [v1.1.0] - 2026-07-30 (0231c55e)

> **Version sources are out of sync:** last tag `v1.0.2`, `CMakeLists.txt` declares `1.0.3`,
> `vcpkg.json` declares `1.0.0`. The breaking camelCase rename would justify `v2.0.0` under
> strict SemVer. Reconcile all three when you cut the release.

### Added
- Expose static themes and the maximum-warning count to the R interface, with additional patch rules.
- Log-closing utility callable from the wrapper (`FMTWrapperCore`).
- Restore the FMTExcel module.
- New Python example mapping actions to area, plus additional example and Excel files.
- Additional `writeProject` and `testWrapperCore` coverage (including simulated annealing).

### Changed
- **Breaking:** Migrate public method names to camelCase across the API (previously PascalCase/lowercase), including the UI and Excel layers.
- Improve the simulated-annealing optimizer: configurable annealing rate, mini-batch handling and temperature-based stop criteria.
- Redo Doxygen docstrings and fix spelling in method documentation.

### Fixed
- Resolve logger instability: silent crashes, stuck logger and log-timing issues.
- Fix adjacency and neighboring constraints for the operating-area scheduler (`OAscheduler`).
- Fix Replanner optimization.
- Fix `writeProject` and GDAL raster output.
- Fix case-sensitivity regressions introduced by the camelCase rename.
- Fix R build warnings and compilation (including the R `git fetch` issue).
- Resolve issues #317 and #320.

### Removed
- Remove the obsolete `Include/FMTareaparser.h` header.

[v1.1.0]: https://github.com/Bureau-du-Forestier-en-chef/FMT/compare/v1.0.2...HEAD
