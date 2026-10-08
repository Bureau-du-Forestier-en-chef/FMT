## [Unreleased] - 2026-10-08 (eaed70a8)

### Added
 
- Added a service-oriented architecture to `FMTWrapperCore`, with a central controller and dedicated use cases for planning, model queries, scenarios, sessions, transformations, and spatial operations (#340 Créer un controleur de Larman et restructurer Wrapper et WrapperCore).
- Migrated operating-area management, area variability, rasterization, selection, and planning capabilities to portable `FMTWrapperCore` components.
- Added a typed event system for forwarding logs, warnings, errors, and results between the Core and user interfaces.
- Added interface-validation scenarios covering planning, rasterization, transformations, spatially explicit optimization, and replanning workflows.
- Added automated Python and R test suites integrated into the installation workflow.
- Added automated COIN-OR stack builds for RTools, including CLP, GLPK, and MOSEK integration (#335 Compiler Osi avec Mosek+Clp+Glpk pour Rtools, #276 Support for GLPK).
- Added detailed project documentation covering architecture, coding standards, build procedures, testing, and contribution rules.
- Added structured GitHub templates for bug reports and feature requests.
 
### Changed
 
- **Breaking:** renamed the public C++ Core namespace from `FMTWrapperCore` to `FMTWrapper::Backend`. Client code using `FMTWrapperCore::*` must be updated.
- Restructured the user interface and wrapper to delegate business operations to the `FMTWrapperCore` controller and services, reducing interface-specific logic (#340 Créer un controleur de Larman et restructurer Wrapper et WrapperCore).
- Split `FMTBounds.hpp` into specialized age, lock, period, and yield bounds classes, together with a dedicated specification class (#202 Diviser le fichier FMTbounds.hpp en 6 fichier 1 par classe).
- Reorganized the CMake installation system into separate runtime, Python, R, and test modules.
- Improved OSI, MOSEK, CLP, and GLPK solver detection and configuration across supported environments.
- Updated R examples to use the current interface naming and consolidated their automated validation.
- Strengthened the commit-message generator with centralized validation, improved error handling, and more robust prompt instructions.
- Updated the Doxygen configuration and API documentation to reflect the new architecture.
- Clarified performance and memory requirements, including resource preallocation and reuse in calculation-intensive and multithreaded code.
 
### Fixed
 
- Included `actionsmapping.json` in the Python package to restore functionality in published distributions (#365 Build FMT release 1.3.0 non fonctionnel).
- Fixed ONNX Runtime feature detection through `hasFeature("ONNXRUNTIME")` in builds using `FMTWITHONNXR` (#362 hasFeature("ONNXRUNTIME") rend toujours faux).
- Fixed theme comparison and associated default values in the action-table builder (#341 ; de trop dans un if).
- Fixed model calculations and R interface test execution.
- Improved retrieval and display of error descriptions in the user interface (#336 Description des erreurs dans l'interface).
- Fixed changelog generation and its integration into the application (#328 Change Log).
- Fixed an issue preventing reinterpretation of the `ROOT` scenario (#244 On ne peut pas réinterpréter le scénario ROOT!).
- Fixed `FMTWrapperCore` symbol exports for shared-library builds.
- Stabilized logging, warning handling, and model caching during the migration to the new architecture.

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