## [Unreleased] - 2026-09-17 (c8275b97)

### Added
- Major migration of functionality into `FMTWrapperCore` with the addition of dedicated service modules:
  - `Planning` for planning and replanning;
  - `OperatingArea` for operating area scheduling;
  - `AreaVariability` for initial area variability processing;
  - `Rasterization` for vector-to-raster workflows;
  - `Environment` and `ModelQuery` for core service operations;
  - `Controller` as the central wrapper entry point.
- Added extensive automated test coverage for the new core services (`Planning`, `OperatingArea`, `AreaVariability`, `Rasterization`, `Environment`) and `UnitTestCallbackLogger`.
- Added automated COIN-OR stack build support for RTools environments (#335, #276).
- Added stronger testing infrastructure:
  - automatic verification that `Examples/Models` data remains unchanged during tests;
  - support for disabled known-bug test suites;
  - resource locking for tests sharing output files;
  - support for gracefully skipping unavailable compiled features.

### Changed
- Significant architectural refactoring of `FMTWrapperCore` to split responsibilities into dedicated modules and reduce business logic inside the UI wrapper.
- Refactored bounds and specification classes by splitting `FMTBounds.hpp` into specialized components (`FMTAgeBounds`, `FMTLockBounds`, `FMTPerBounds`, `FMTYldBounds`, `FMTSpec`) (#202).
- Centralized validation, error handling and logging across internal development tooling.
- Unified .NET-to-native conversion mechanisms through a shared conversion layer.
- Updated R examples to reflect the `FMTSolverInterface` naming.
- Updated Doxygen documentation and version references.
- Renamed and modernized core infrastructure components:
  - `FMTFormCache` → `ModelCache`;
  - `FMTFormLogger` → `CallbackLogger`;
  - `FMTExceptionHandlerWarning` → `WarningExceptionHandler`.
- Removed the migration tracking document `FMTWrapperCore/ETAT.md` and migrated test-related tracking to dedicated test resources.
- Tests that modify project data now operate on copied datasets to prevent accidental modification of reference models.

### Fixed
- Fixed a parser issue involving theme comparison defaults in `_isMapWithSameThemes` (#341).
- Fixed registration and execution of several tests that were previously missing or incorrectly configured in CTest.
- Fixed `testWrapperCoreGetYield`, which was not properly executed and used incorrect numeric validation.
- Improved error reporting and user-facing error descriptions in the interface (#336).
- Fixed multiple issues discovered during the `FMTWrapperCore` migration, including scenario handling, output retrieval, string conversions and error-path behaviors.
- Updated examples and tests to reflect renamed classes and interfaces.

### Removed
- Removed the legacy monolithic `Tools` component, replaced by the dedicated `Environment` and `ModelQuery` services.
- Removed duplicated code and obsolete wrapper helper functions following the core migration.

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

## [v1.2.0] - 2026-08-20 (26e42c7a)

### Added
- Added support for the `_SHIFT` keyword for model and scenario definitions, including dedicated examples.
- Added **Parquet** file support through an updated GDAL integration.
- Added **GLPK** solver support and related functionality, together with dedicated tests.
- Exposed available solvers through public interfaces and added new solver-management capabilities.
- Exposed `FMTmask::decompose` through the Python interface.
- Exposed static themes through the R interface.
- Added new exception-management capabilities, including `FMTExceptionHandler::getErrorsToIgnore`.
- Added logger and exception-handler recovery mechanisms in the UI layer, allowing crash recovery while preserving existing log files and configuration.
- Added explicit UTF-8 to system string conversion in the R interface (`_convertToSystemString`), improving support for accented and international characters.
- Added new C++, Python, and R examples, including *Map to Area*, output exploration, yield categorization workflows, and scenarios demonstrating SQL query integration, SQLite-backed yield definitions, and constant substitution.
- Restored **FMTExcel** support.


### Changed
- **Breaking:** major standardization of class, source, and header names (`FMTAction`, `FMTModel`, `FMTAreaParser`, etc.), accompanied by a significant public API reorganization.
- Refactored the R API and wrapper layers with `camelCase` standardization, naming cleanup, and improved consistency.
- Significantly improved exception handling with better encapsulation, richer abstractions, and enhanced error propagation.
- Refactored the wrapper logging and exception-handling infrastructure to improve reliability, recovery after failures, and log preservation.
- Introduced and matured a batch and mini-batch optimization workflow.
- Improved Simulated Annealing optimization behavior, including annealing-rate handling and optimization performance.
- Improved compatibility with older GDAL/PROJ versions for spatial operations, including OGR layer handling, projection management, and rasterization workflows.
- Optimized `FMTSemodel::postSolve`, graph traversal routines, and internal caching mechanisms to improve performance on large spatial models.

- Enhanced the parsing engine:
  - improved handling of constants used in SQL queries;
  - more robust support for `FOREACH` and `*INCLUDE`;
  - improved case-insensitive processing of constants, directives, and variables;
  - improved schedule file parsing with constant resolution support;
  - improved recursive dependency detection in complex yield evaluation.
- Updated Python/R auto-generated documentation, UI/Excel documentation, examples, README files, test coverage, and changelog content.
- Modernized build and distribution infrastructure:
  - improved compatibility with R 4.5, MSVC, and vcpkg;
  - integrated **mimalloc** into MSVC builds;
  - simplified and cleaned up build and release tooling.
- Improved changelog generation, exposure, and packaging in released artifacts.

### Fixed
- Fixed multiple build and integration issues affecting R, Python, Excel, and Windows environments.
- Resolved several wrapper hangs, lost log messages, and silent crash scenarios related to logging, logger destruction, and exception propagation (#313 Adaptation Log and Exception Handler in FMTWrapper).
- Fixed a user interface logger pointer crash.
- Fixed spatially explicit optimization when no cache is available (#317 Spatially Explicit Optimization Not Functional Without Cache).
- Fixed and optimized `FMTSemodel::postSolve`, improving both stability and performance of spatial scheduling workflows.
- Fixed neighborhood and adjacency handling in spatial scheduling and optimization workflows.
- Fixed replanning optimization behavior.
- Fixed decision-tree yield model issues.
- Fixed loading of empty GCBM transitions (#311 Reading an Empty GCBM Transition).
- Fixed rasterization failures caused by invalid themes (#326 Invalid Theme Missing Mask During Rasterization).
- Fixed **#201 Unable to Rasterize**, improving GDAL/OGR projection handling during rasterization and reprojection workflows.
- Fixed replacement mechanisms and new modeling syntax support (#323 New Syntax, #322 rxreplace, #321 _replace, #316 SQL Constant Regex Adaptation).
- Fixed constant parsing used inside SQL queries and resolved **#338 SQL Query**, restoring correct SQL variable and constant resolution behavior.
- Fixed update-period handling (#331 Update Period Handling).
- Fixed issues related to patch rules, escaping, multithreading, and user interfaces.
- Fixed compilation when Mosek support is disabled.

- Fixed several warning-handling and exception-management compatibility issues.
- Reduced memory consumption during presolve operations (#204 Too Much Memory Used for Presolve).
- Fixed a minor documentation coverage rendering issue.
- Fixed numerous dependency, installation, documentation, and build-system issues.

### Removed
- Removed the `magic_enum` dependency.
- Removed an obsolete UI cache component, simplifying the internal implementation.
- Removed obsolete headers, legacy file variants, and deprecated API artifacts following API normalization.
- Removed outdated build scripts and redundant compilation configurations.