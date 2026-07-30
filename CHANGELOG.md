# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

> Version française : [CHANGELOG.fr.md](CHANGELOG.fr.md)

## [v1.1.0] - 2026-07-30 (ac3bddd1)

### Added
- Exposed static themes to the R interface and introduced additional patch-rule capabilities.
- Restored FMTExcel support and added new usage examples, including a Python “Map to Area” example.
- Added a utility allowing the wrapper to properly close log files.
- Added wrapper functionality supporting the new syntax and replacement mechanisms (`_replace`, `rxreplace`).

### Changed
- Major cleanup of the R wrapper API, including method renaming, `camelCase` standardization, and case-consistency improvements.
- Introduced and refined a new batch/mini-batch optimization workflow.
- Refactored block-related components and internal structures to improve maintainability.
- Improved the Simulated Annealing optimization workflow, including temperature and convergence-related behavior.
- Enhanced Doxygen documentation, examples, and changelog generation tooling.
- Updated build infrastructure (CMake, MAM build) and improved R interface compatibility.

### Fixed
- Fixed spatially explicit optimization when no cache is available (#317 Spatially Explicit Optimization Not Functional Without Cache).
- Resolved multiple logging and exception-handling issues in the wrapper, including hangs, inconsistencies, and silent crashes (#313 Adaptation Log and Exception Handler in FMTWrapper).
- Fixed neighborhood and adjacency handling in spatial optimization and scheduling components.
- Fixed rasterization failures caused by invalid themes (#326 Invalid Theme Missing Mask During Rasterization).
- Fixed loading of empty GCBM transitions (#311 Reading an Empty GCBM Transition).
- Fixed several R integration and compilation issues, including warnings, case-sensitivity problems, overloaded calls, GDAL output handling, and build compatibility.
- Fixed project-writing operations and stabilized related tests.
- Added support for writing cache data directly from the interface (#320 Create a Button to Write Cache in the Interface).
- Fixed issues related to the new syntax and SQL regular-expression handling (#323 New Syntax, #322 rxreplace, #321 _replace, #316 Regex Adaptation for SQL Constants).
- Improved overall stability by addressing multiple crash scenarios in the wrapper and optimization components.

### Removed
- Removed obsolete header files and unused includes as part of the codebase cleanup and restructuring effort.