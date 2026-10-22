## [Unreleased] - 2026-10-20 (013913b2)

### Added

- Added a service-oriented architecture to `FMTWrapperCore`, with a central controller and dedicated use cases for planning, model queries, scenarios, sessions, transformations, and spatial operations (#340 Créer un controleur de Larman et restructurer Wrapper et WrapperCore).
- Migrated operating-area management, area variability, rasterization, selection, and planning capabilities to portable `FMTWrapperCore` components.
- Added a typed event system for forwarding logs, warnings, errors, and results between the Core and user interfaces.
- Added interface-validation scenarios covering planning, rasterization, transformations, spatially explicit optimization, and replanning workflows.
- Added automated Python and R test suites, run as CTest system tests.
- Added automated COIN-OR stack builds for RTools, including CLP, GLPK, and MOSEK integration (#335 Compiler Osi avec Mosek+Clp+Glpk pour Rtools, #276 Support for GLPK).
- Added detailed project documentation covering architecture, coding standards, build procedures, testing, and contribution rules.
- Added structured GitHub templates for bug reports and feature requests.
- Added three test levels in `Tests/`, `Unit`, `Integration` and `System`, next to the `Performance` benchmarks. Each test carries a level label, and test presets run one level at a time: `ctest --preset unit`, `integration`, `system`, `system-private`, `performance` or `all-public` (#350 [FEATURE] Add comprehensive unit tests for the Core namespace).
- Added a summary at the end of every test pass, in the installation as in `tools/tests/RunTests.bat <preset>`: passed, failed, skipped and disabled tests by level, then the failed and skipped tests with their reason.
- Added GoogleTest to the dependencies (`vcpkg.json`) and the first unit tests of the Core, on `FMTOperator`, `FMTBounds` and `FMTSpec`. Without GoogleTest, the unit and integration levels are not registered, and everything else builds and runs as before.
- Added the first integration tests, in `Tests/Integration/Parser`: round trips of `FMTModelParser::write`, `writeToProject` and `FMTScheduleParser`, which compare the model read back with the model written, then both writes file by file. They revealed five write defects, each kept as a disabled case until it is fixed:
  - #379 [BUG] Un objectif `_PENALTY` sans signe est réécrit `y_PENALTY`
  - #380 [BUG] `_DISCOUNTFACTOR` est réécrit en série temporelle de ses paramètres, et son écriture plante quand son bloc `*YT` n'a pas d'autre yield
  - #381 [BUG] Un output qui mêle sources d'inventaire et d'action écrit ses sorties cachées, que chaque réécriture répète
  - #382 [BUG] Une contrainte `_RANDOM(<output>)` est réécrite sans `_RANDOM`
  - #383 [BUG] `FMTYields::operator==` compare les adresses des gestionnaires : deux copies d'un modèle sont inégales, et `writeToProject` écrit toujours le `._yld` d'un scénario
- Exported from `FMTlib` 18 classes of the Core that a program linked to the DLL, such as a unit test, could not use: the comparators `FMTActionComparator`, `FMTActualDevelopmentComparator`, `FMTMaskComparator`, `FMTOutputNodeValueComparator`, `FMTOutputNodeOriginComparator`, `FMTOutputSourceComparator`, `FMTThemeComparator` and `FMTTransitionComparator`, `FMTOutputNodeHasher`, `FMTYieldRequest`, `FMTExpression`, `FMTFunctionCall`, `FMTYieldsCache`, `FMTYieldDevelopment`, and the yield models `FMTYieldModel`, `FMTYieldModelRandom`, `FMTYieldModelTsla` and `FMTYieldModelUnitCoverage` (#350 [FEATURE] Add comprehensive unit tests for the Core namespace).
- Added unit tests of the expressions and of the yield handlers of the Core, built in code without a model: `FMTOperator`, `FMTExpression`, `FMTFunctionCall`, `FMTData`, `FMTAgeYieldHandler`, `FMTTimeYieldHandler` and `FMTComplexYieldHandler` (#350 [FEATURE] Add comprehensive unit tests for the Core namespace). They characterize the known behaviours of #346 and #347, and revealed three defects, the first two kept as disabled cases until they are fixed:
  - #386 [BUG] Un moins qui suit un opérateur est lu comme une soustraction dans `_EQUATION`
  - #388 [BUG] Les yields d'un bloc `*YT` lisent tous la période de départ du premier
  - #387 [BUG] En colonnes (`_CP`), chaque ligne d'un bloc `*YT` retarde d'une période les valeurs des lignes suivantes
- Added unit tests of the bounds, specifications, constraints, outputs and yields of the Core, built in code without a model: `FMTBounds` through `FMTAgeBounds`, `FMTYldBounds`, `FMTLockBounds` and `FMTPerBounds`, `FMTSpec`, `FMTConstraint`, `FMTOutput` and `FMTYields`, with the overrides, `getAge` and `getEndPoint` of the yield handlers, and an integration test of `FMTTransitionParser` that characterizes the known behaviour of #358 (#350 [FEATURE] Add comprehensive unit tests for the Core namespace). They show #379, #382 and #383 closer to their cause, and revealed four defects, each kept as a disabled case until it is fixed:
  - #389 [BUG] `_MAX` part de 0 : le maximum de yields tous négatifs vaut 0
  - #390 [BUG] `FMTYields::getFromFactor` multiplie le taux et les années d'un `_DISCOUNTFACTOR` au lieu de sa valeur
  - #391 [BUG] `FMTYields::getFromFactor` ne multiplie aucun yield quand on n'en nomme aucun, alors que sa documentation dit tous
  - #392 [BUG] `_ENDPOINT` provoque une violation d'accès dès que sa source sort de ses bornes
- Added unit tests of the masks, themes and developments of the Core, built in code without a model: `FMTMask`, `FMTTheme` and `FMTDevelopment`, with operability and `operate` through actions and transitions built in code, and integration tests of `FMTActionParser` and `FMTYieldParser` on two new scenarios of `TWD_land`, `strictbounds` and `timeyieldcolumns` (#350 [FEATURE] Add comprehensive unit tests for the Core namespace). They keep #387 as a disabled case, and revealed one defect, kept as disabled cases until it is fixed:
  - [BUG] Une condition d'opérabilité écrite avec `>` ou `<` est lue sans sa borne : l'action est opérable à tout âge (issue to open)
- Added system tests on the public `TWD_land` model: scenario reading (yields, outputs, operability, transitions, growth), reading errors and their exception codes, scenarios read together, planning statistics and replay of the optimal schedule. A fixture checks that no test modifies `Examples/Models`, and known defects are kept as disabled tests.
- Added checks to `WriteAndReadProject`, `SolveAndWriteSchedules`, `CreateModel` and `UpdateNonSpatialModel`, which now verify what they write, and repaired the dead check of `AddYieldHandlers`.
- Added performance and allocation benchmarks with expected bounds, and `tools/performance/Measure_Performance.bat`, which measures a change by comparing runs before and after it (#349 [FEATURE] Add performance and allocation benchmarks).
- Added `tools/tests/RunPrivateTests.bat`, which checks that the private models are reachable, then runs the private test campaign and its summary.
- Added `tools/testMigration/Update_PrivateTests.bat`, which moves and renames a local private test list (`BFECtests.csv`), and the tables of former and new test names (`Tests/Support/RenamedTests.csv`, `Tests/Support/RenamedTestCases.csv`).

### Changed

- **Breaking:** renamed the public C++ Core namespace from `FMTWrapperCore` to `FMTWrapper::Backend`. Client code using `FMTWrapperCore::*` must be updated.
- **Breaking:** `WITHOUT_TESTS` is now an option read by its value everywhere: `-DWITHOUT_TESTS=OFF` or `false` now configures the tests, where it used to remove them from the root build. `ON` or `true` still removes them.
- Restructured the user interface and wrapper to delegate business operations to the `FMTWrapperCore` controller and services, reducing interface-specific logic (#340 Créer un controleur de Larman et restructurer Wrapper et WrapperCore).
- Split `FMTBounds.hpp` into specialized age, lock, period, and yield bounds classes, together with a dedicated specification class (#202 Diviser le fichier FMTbounds.hpp en 6 fichier 1 par classe).
- Reorganized the CMake installation system into separate runtime, Python, R, and test modules.
- Improved OSI, MOSEK, CLP, and GLPK solver detection and configuration across supported environments.
- Updated R examples to use the current interface naming and consolidated their automated validation.
- Strengthened the commit-message generator with centralized validation, improved error handling, and more robust prompt instructions.
- Updated the Doxygen configuration and API documentation to reflect the new architecture.
- Clarified performance and memory requirements, including resource preallocation and reuse in calculation-intensive and multithreaded code.
- `cmake --install` now runs only the public tests by default; the `INSTALL_TEST_SUITE` cache variable selects `public`, `all` or `none`. The private campaign runs apart, before a release. The installation no longer runs the Python and R suites itself: CTest runs them with the other tests.
- Moved the C++ tests to `Tests/System/Cpp`, with `FMTWrapperCore` and `Excel` subfolders, and the Python and R suites to `Tests/System/Python` and `Tests/System/R`; `Examples/C++` no longer exists.
- Renamed the C++ test executables after the action they run (`testScenarioReading` becomes `ReadScenario`, `doplanning` becomes `DoPlanning`), and gave each public test a Given/When/Then name. CTest names follow `<Level>.<Suite>.<Case>`.
- Moved the scripts of the repository root into `tools/` (`build`, `tests`, `changelog`, `commitMessage`), listed in `tools/README.md`.
- Rewrote the testing rules of `Documentation/CodingStandards.md` (three test levels, Arrange / Act / Assert, Given/When/Then names) and added Clean Code guidance; `Documentation/Architecture.md` now records the architectural debt that tests reveal, and the portability of tests and pipelines.
- `FMTlib` no longer links mimalloc, which it never loaded: the executables use the C runtime heap, and with MSVC, `-DWITH_MIMALLOC=ON` links mimalloc first in each executable.
- Benchmarks built with MinGW use their own bounds (`Tests/Performance/performance-mingw.csv`) and run with `OMP_NUM_THREADS=32` (#373 [BUG] Paquet R (Rtools45) : OpenBLAS compilé avec OpenMP engage environ 65 Mo par processeur logique au chargement de FMT).
- The changelog generator now merges an existing Unreleased section instead of adding a second one.

### Deprecated

- `WITHOUT_TESTING` is deprecated in favour of `WITHOUT_TESTS`: it still removes the tests, with a warning.

### Fixed

- **Breaking:** `_DISCOUNTFACTOR` now uses the rate it reads, instead of the number of years per period, and its `HALF` exponent is the period minus 0.5. Models that use `_DISCOUNTFACTOR` get new discount factors (#357 `_DISCOUNTFACTOR` n'utilise pas le taux lu).
- `_DISCOUNTFACTOR` now reads a decimal rate, such as `0.04` or `4.5%`, and a constant, instead of truncating the first and stopping on the second (#374 [BUG] _DISCOUNTFACTOR tronque un taux décimal et refuse une constante).
- Included `actionsmapping.json` in the Python package to restore functionality in published distributions (#365 Build FMT release 1.3.0 non fonctionnel).
- Fixed ONNX Runtime feature detection through `hasFeature("ONNXRUNTIME")` in builds using `FMTWITHONNXR` (#362 hasFeature("ONNXRUNTIME") rend toujours faux).
- Fixed theme comparison and associated default values in the action-table builder (#341 ; de trop dans un if).
- Fixed model calculations and R interface test execution.
- Improved retrieval and display of error descriptions in the user interface (#336 Description des erreurs dans l'interface).
- Fixed changelog generation and its integration into the application (#328 Change Log).
- Fixed an issue preventing reinterpretation of the `ROOT` scenario (#244 On ne peut pas réinterpréter le scénario ROOT!).
- Fixed `FMTWrapperCore` symbol exports for shared-library builds.
- Stabilized logging, warning handling, and model caching during the migration to the new architecture.
- Fixed the lifetime of the logger of `FMTLpSolver`, which could crash a program that copies a solved model: each solver now holds its own copy of the logger.
- Fixed the MinGW build with RTools 4.5: `FindOSI` no longer reuses COIN-OR paths cached by an older configuration, `build-coin-stack.sh` gets the git that `FindOSI` finds, and the benchmark harness looks up the Windows 10 processor functions at run time.
- Fixed `CMakePresets.json`, which CMake rejected because the `release-mam` build preset referred to a missing configure preset.
- Fixed the changelog generator, which corrupted its prompt when the inserted data contained one of its placeholders.

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