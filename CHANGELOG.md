## [v1.5.0] - 2026-11-04 (84b8301b)

### Added

- Added extensive test coverage for the Core, serialization, parallel execution, and regression scenarios, including:
  - unit and integration tests for expressions, yields, constraints, actions, transitions, schedules, models, solvers, and spatial components;
  - C++ and Python serialization tests;
  - integration tests for parallel task execution;
  - code coverage measurement and hot-path allocation benchmarks;
  - multiple regression scenarios such as `defaultdeath`, `spatialconstraints`, `harvestfromperiod3`, and `decisiondeath`.

- Added additional diagnostic information for "vector too long" failures along with regression tests covering the related exception paths (#401).

### Changed

- `FMTTaskHandler::onDemandRun` now waits up to 10 ms between worker checks instead of continuously polling, reducing unnecessary CPU usage.

- `FMTModel::preSolve` now raises a more explicit `FMTmissingdevelopment` exception that identifies the model whose area is empty.

- FMT exceptions no longer print messages directly to standard output, improving integration with host applications and automated environments.

- Improved Rtools compatibility by fixing the `const` iterator implementation in `FMTYields`.

### Fixed

- Fixed default `_DEATH` handling when lifespans are defined on aggregates. Dead stands now retain the correct attributes instead of inheriting aggregate values (#415).

- Fixed `_ENDPOINT` access violations when a source yield falls outside its valid bounds (#392).

- Fixed `isPeriodBounded`, which incorrectly returned `true` for every graph period (#400).

- Fixed several `_DISCOUNTFACTOR` defects:
  - the factor value is now used correctly instead of recomputing from its parameters;
  - complex-yield cache entries are no longer reused across unrelated models;
  - serialization and rewrite behavior are now consistent (#380, #390, #403).

- Fixed `FMTYields::operator==`, which compared yield-handler addresses instead of their contents, causing identical model copies to be considered different (#383).

- Fixed `FMTTimeYieldHandler` so each yield in a `*YT` block uses its own starting period (#388).

- Fixed `_MAX` evaluation for complex yields when all values are negative (#389).

- Fixed malformed serialization of `_SIZE`, `_ADJACENCY`, and `_GROUP` spatial constraints that produced models that could not be parsed again (#397).

- Fixed `FMTConstantParser::write` so multi-period constants are preserved correctly through write/read round trips (#404).

- Fixed serialization of `_RANDOM(<output>)` constraints so exported models can be reloaded without modification (#382).

- Fixed serialization of unsigned `_PENALTY` objectives (#379).

- Fixed reading of the `MAX_RET` operating-area scheduler parameter (#359).

- Fixed neighborhood calculations involving multipolygons and geometries with Z coordinates, restoring correct spatial adjacency detection (#394, #396).

- Restored the `FMTLpSolver` logger-lifetime fix that had been lost during a previous merge.

- Fixed Excel add-in compatibility with renamed `FMTExcelCache` methods.

- Fixed action-series evaluation behavior to correctly handle action sequences.