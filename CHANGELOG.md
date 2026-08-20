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
- Added new C++, Python, and R examples, including *Map to Area*, output exploration, and yield categorization workflows.
- Restored **FMTExcel** support.

### Changed
- **Breaking:** major standardization of class, source, and header names (`FMTAction`, `FMTModel`, `FMTAreaParser`, etc.), accompanied by a significant public API reorganization.
- Refactored the R API and wrapper layers with `camelCase` standardization, naming cleanup, and improved consistency.
- Significantly improved exception handling with better encapsulation, richer abstractions, and enhanced error propagation.
- Refactored the wrapper logging and exception-handling infrastructure to improve reliability, recovery after failures, and log preservation.
- Introduced and matured a batch and mini-batch optimization workflow.
- Improved Simulated Annealing optimization behavior, including annealing-rate handling and optimization performance.
- Updated Python/R auto-generated documentation, UI/Excel documentation, examples, and README files.
- Modernized build and distribution infrastructure:
  - improved compatibility with R 4.5, MSVC, and vcpkg;
  - integrated **mimalloc** into MSVC builds;
  - simplified and cleaned up build and release tooling.
- Improved changelog generation, exposure, and packaging in released artifacts.

### Fixed
- Fixed multiple build and integration issues affecting R, Python, Excel, and Windows environments.
- Resolved several wrapper hangs, lost log messages, and silent crash scenarios related to logging, logger destruction, and exception propagation (#313 Adaptation Log and Exception Handler in FMTWrapper).
- Fixed spatially explicit optimization when no cache is available (#317 Spatially Explicit Optimization Not Functional Without Cache).
- Fixed neighborhood and adjacency handling in spatial scheduling and optimization workflows.
- Fixed replanning optimization behavior.
- Fixed decision-tree yield model issues.
- Fixed loading of empty GCBM transitions (#311 Reading an Empty GCBM Transition).
- Fixed rasterization failures caused by invalid themes (#326 Invalid Theme Missing Mask During Rasterization).
- Fixed replacement mechanisms and new modeling syntax support (#323 New Syntax, #322 rxreplace, #321 _replace, #316 SQL Constant Regex Adaptation).
- Fixed update-period handling (#331 Update Period Handling).
- Fixed issues related to patch rules, escaping, multithreading, and user interfaces.
- Reduced memory consumption during presolve operations (#204 Too Much Memory Used for Presolve).
- Fixed numerous dependency, installation, documentation, and build-system issues.

### Removed
- Removed the `magic_enum` dependency.
- Removed obsolete headers, legacy file variants, and deprecated API artifacts following API normalization.
- Removed outdated build scripts and redundant compilation configurations.