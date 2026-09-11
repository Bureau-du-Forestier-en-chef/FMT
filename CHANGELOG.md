## [v1.3.0] - 2026-09-11 (e5a606bc)

### Added
- Added explicit UTF-8 to system string conversion in the R interface (`_convertToSystemString`), improving support for accented and international characters.
- Added automated Python stub generation and packaging support to simplify FMT distribution and Python integration.
- Added SQLite-backed scenarios, datasets, and tests demonstrating SQL query integration within yield definitions, including constant substitution support.
- Added a `CMakePresets.json` configuration to simplify build setup and improve IDE integration.

### Changed
- Improved compatibility with older GDAL/PROJ versions for spatial operations, including OGR layer handling, projection management, and rasterization workflows.
- Improved `FMTObject` portability and GDAL initialization for better cross-platform support.
- Optimized `FMTSemodel::postSolve`, graph traversal routines, and caching mechanisms to improve performance on large spatial models.
- Enhanced the parsing engine:
  - improved handling of constants used in SQL queries;
  - more robust support for `FOREACH` and `*INCLUDE`;
  - improved case-insensitive processing;
  - improved schedule file parsing with constant resolution support;
  - improved recursive dependency detection in complex yield evaluation.
- Extended internal APIs for GDAL projection retrieval, mask filtering operations, and optimized yield cache access.
- Modernized build and development infrastructure:
  - added CMake preset support;
  - improved IDE integration through repository configuration updates;
  - removed legacy build scripts.
- Updated documentation, README files, test coverage, examples, and changelog and commit-message generation workflows.

### Fixed
- Fixed a user interface logger pointer crash.
- Fixed constant parsing used inside SQL queries.
- Fixed **#338 SQL Query**, restoring correct SQL variable and constant resolution behavior.
- Fixed **#201 Unable to Rasterize**, improving GDAL/OGR projection handling during rasterization and reprojection workflows.
- Fixed and optimized `FMTSemodel::postSolve`, improving both stability and performance of spatial scheduling workflows.
- Fixed compilation when Mosek support is disabled.
- Fixed Build & Coverage status reporting and coverage badge rendering issues.
- Fixed several warning-handling and exception-management compatibility issues.
- Fixed formatting issues in automatically generated messages containing quoted strings.
- Fixed compatibility issues across multiple GDAL/OGR versions.

### Removed
- Removed an obsolete UI cache component, simplifying the internal implementation.
- Removed legacy Windows build scripts made redundant by CMake presets.