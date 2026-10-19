# FMT Agent Guidelines

Operational guide for contributors and coding assistants working in this repository.

This file covers what is specific to FMT: where things are, how to build and test, and the
conventions that are easy to break without noticing. Design rules are not repeated here. Each rule
lives in exactly one document, and changes to it belong in that document only.

| What you need | Where it is defined |
| --- | --- |
| Layers, responsibilities, dependency direction, loose coupling, performance and memory | [Documentation/Architecture.md](Documentation/Architecture.md) |
| Naming, file organization, documentation, type safety, ownership, errors, tests, warnings, compatibility | [Documentation/CodingStandards.md](Documentation/CodingStandards.md) |
| Repository map, build and test mechanics, repository traps, task workflow | this file |
| Performance benchmarks: running them, reading their results, adding one | [Documentation/PerformanceTesting.md](Documentation/PerformanceTesting.md) |

## Repository map

| Path | Contents |
| --- | --- |
| `Include/`, `Source/` | The FMT library: 247 headers, 224 sources. Namespaces `Core` (model concepts), `Models` (planning models), `Spatial`, `Graph`, `Parser` (Woodstock files), `Heuristics`, `Parallel`, `Logging`, `Exception`. |
| `FMTWrapperCore/` | Portable wrapper core shared by the interfaces. |
| `UI/` | `FMTForm` and the user interface. |
| `Excel/` | Excel integration. |
| `Examples/Models/` | Woodstock models used by the tests. |
| `Tests/` | Test registration and the tests by level: see [Testing](#testing). |
| `Tests/Unit/Core/`, `Tests/Integration/` | GoogleTest tests: the unit tests of FMTlib, one source per class, built into `UnitCore`; the integration tests, none yet. |
| `Tests/System/Cpp/` | C++ system tests: each `.cpp` becomes a test executable, which the rows of the CSV files of the folder run; `FMTWrapperCore/` and `Excel/` hold those of the wrapper core and of Excel. |
| `Tests/System/Python/`, `Tests/System/R/` | pytest and testthat suites, one system test per file, run by ctest. |
| `Tests/Performance/` | Performance and allocation benchmarks: `FMTPerformanceTests`, its harness and `performance.csv`. |
| `Templates/` | Packaging inputs and generated artifacts: R package, `setup.py.in`, `__init__.py.in`, stub normalization. |
| `cmake/`, `Modules/` | Install and configuration scripts; `Find*.cmake` for GEOS, MOSEK, OSI, R, Rcpp, ONNX Runtime. |
| `tools/` | `commitMessage/`, `changelog/`, `HeapCorruption/`, `RToolsSetup/`. |
| `vcpkg.json`, `vcpkg-custom-ports/`, `vcpkg-overlays/` | Dependencies: GDAL, Arrow, COIN-OR, Python, and GoogleTest for the unit and integration tests. |
| `Documentation/` | Architecture, coding standards, Doxygen configuration, training material. |

## Building

Configure presets are defined in `CMakePresets.json`: `release-mam`, `release-gc` and `release-gl`,
all Visual Studio 17 2022 generators with testing enabled. They all build into `build/release`.

```bash
cmake --preset release-gl
cmake --build --preset release-gl
```

Scripted entry points exist for the usual configurations, in `tools/build/`:
`CMakeFMTVS2022vcpkg.bat` and its variants on Windows, `CMakeFMTMSYS2rcran45.sh` for the MSYS2 build
of the R package. [tools/README.md](tools/README.md) lists every script of `tools/`. Configuring with
`-DWITHOUT_TESTS=ON` (or `TRUE`, as the release scripts pass it) leaves out every test and every test
executable; `OFF`, the default, keeps them. `WITHOUT_TESTING`, its former name, still turns them off,
with a deprecation warning.

The executables that CMake builds (examples, tests, benchmarks) run on the C runtime heap, like FMT in
Python, Excel and the .NET interface, so that the tests and the benchmarks see what the users get. With
MSVC, configuring with `-DWITH_MIMALLOC=ON` links mimalloc first into every executable, so that mimalloc
replaces the C runtime heap of its process: such a build measures what mimalloc would bring, and is not
for testing (see [The allocator](Documentation/PerformanceTesting.md#the-allocator)). mimalloc can take
over only before the C runtime starts: FMTlib does not load it, and the processes that load FMTlib later
keep the C runtime heap. In such a build, `MIMALLOC_DISABLE_REDIRECT=1` keeps the executables on the C
runtime heap for one run, as a heap debugger such as the page heap of gflags needs, since it only sees
the heap of Windows. Why the interface cannot use mimalloc is one of the points to consider for a
reworked interface, gathered in
[FMTWrapperCore/INTERFACE_MIGRATION.md](FMTWrapperCore/INTERFACE_MIGRATION.md) (in French).

## Testing

What to test, the three test levels, the Arrange / Act / Assert structure and test names are
defined in [CodingStandards.md, Testing](Documentation/CodingStandards.md#testing); a test that has
to work around the code is covered in
[Tests that reveal architectural debt](Documentation/CodingStandards.md#tests-that-reveal-architectural-debt).
This section covers where tests live and how they run.

The root `CMakeLists.txt` includes `Tests/CMakeLists.txt`, unless `WITHOUT_TESTS` is on, after `Excel/`
and `FMTWrapperCore/`, whose libraries the C++ tests link to. That file declares the `ExamplesModels`
fixture, which every system test and benchmark requires: `Tests/Support/TestsDataSnapshot.cmake`
hashes `Examples/Models` before the tests and fails after them if a file changed. It then includes one
folder per level: `Unit/` and `Integration/`, the GoogleTest levels, when `find_package(GTest CONFIG)`
finds GoogleTest (without it, as in the MSYS2 build of the R package, a warning says that these two
levels are not registered), then `System/`, which holds the C++ tests and the Python and R suites, then
`Performance/`. The functions that register a test are in `Tests/Support/TestRegistration.cmake`,
included first: `registersystemtest` registers one system test, under a name that no other test may
have, `registercsvtests` registers the rows of CSV files with it, and `registergoogletests` creates a
GoogleTest executable and registers each of its cases. `Tests/System/Cpp/CMakeLists.txt` calls
`registercsvtests` for its CSV files and `registersystemtest` for the Excel test,
`Tests/System/Python/CMakeLists.txt` and `Tests/System/R/CMakeLists.txt` call `registersystemtest` for
their tests, and `Tests/Unit/Core/CMakeLists.txt` calls `registergoogletests` for `UnitCore`.

Each test carries one level label (`unit` for the GoogleTest tests of `Tests/Unit`, `system` for every
C++, Python and R system test, `performance` for the benchmarks), a system test also the label of its
interface (`cpp`, `python` or `r`), a test of a component the label of that component (`core` for the
unit tests of FMTlib, `wrappercore` or `excel` for the C++ system tests of the wrapper core or of
Excel), and its CTest name follows
[Test names](Documentation/CodingStandards.md#test-names). The private tests,
rows of the local `BFECtests.csv` on models of `T:\`, keep `T:/` in their name. Test presets in
`CMakePresets.json` select a level on `build/release` (configure preset `release-gl`), eight tests at a
time:

```bash
ctest --preset all-public
ctest --preset unit
ctest --preset integration
ctest --preset system
ctest --preset system-private
ctest --preset performance
```

`all-public` runs everything but the private tests; `integration` selects nothing yet. Without a
preset, filter by label, always anchored since `-L` takes a regular expression:

```bash
ctest --test-dir build/release -C Release -L "^system$" -E "T:/" -j 8 --output-on-failure
```

`tools\tests\RunTests.bat [preset] [ctest options...]` runs a test preset (`all-public` by
default) and ends with a summary: a table by level (passed, failed, skipped, disabled, total), then
the failed and the skipped tests with their reason. It finds `ctest` in the `PATH`, otherwise in the
CMake of Visual Studio 2022, waits for a key, and returns the exit code of ctest. The summary is
`Tests/Support/TestSummary.cmake`, which reads the results of `ctest --output-junit`.

`cmake --install` runs the ctest pass that the cache variable `INSTALL_TEST_SUITE` selects, and
prints the same summary: `public` by default (everything but the private tests), `all`, or `none`,
which runs no test and leaves the README badges unchanged. The install first builds and installs the
Python wheel or the R package, without running any test, so that this pass runs their suites with the
rest. The private campaign runs apart, before a release: `ctest --preset system-private`, or
`tools\tests\RunPrivateTests.bat [ctest options...]`, which checks that `T:\` is reachable, then
runs `RunTests.bat system-private` and its summary.

### Adding a unit or integration test

A unit or integration test is a GoogleTest case, `TEST(<Class>, Given<...>When<...>Then<...>)`, written
as in [Arrange, Act, Assert](Documentation/CodingStandards.md#arrange-act-assert) and named as in
[Test names](Documentation/CodingStandards.md#test-names). There is no `add_test` call to write.

1. Add the case to the source of its class in `Tests/Unit/Core/`, named after the class: the suite
   `FMTSpec` is in `FMTSpec.cpp`. A new source is built into `UnitCore` at the next configuration,
   since every `.cpp` file of the folder is ([Tests/Unit/Core/CMakeLists.txt](Tests/Unit/Core/CMakeLists.txt)).
   A new case in an existing source needs only the build.
2. A case that reveals an open defect is prefixed by `DISABLED_` and gets an issue: ctest registers it
   disabled, under its name without the prefix.
3. A test that has to work around the code adds a row to
   [Known Architectural Debt](Documentation/Architecture.md#known-architectural-debt).
4. The tests of another component, or the first integration tests, get their own folder in
   `Tests/Unit/` or `Tests/Integration/`, included by the `CMakeLists.txt` of the level. Its
   `CMakeLists.txt` calls `registergoogletests(<Level><Component> <Level> <component> <sources>)`, as
   `Tests/Unit/Core/CMakeLists.txt` does, then links the real dependency of an integration test to that
   executable with `target_link_libraries`.

ctest lists the cases when it runs, from the executable (`--gtest_list_tests`, `registergoogletests` in
[Tests/Support/TestRegistration.cmake](Tests/Support/TestRegistration.cmake)): the build runs nothing.
Each case is the CTest test `<Level>.<Class>.<Case>`, labelled with its level and its component
(`unit;core`), and a name already given to a case of another GoogleTest executable stops ctest, which
names both. They run with their level, or alone:

```bash
tools\tests\RunTests.bat unit
tools\tests\RunTests.bat integration
ctest --preset unit -R "^Unit\.FMTSpec\."
```

### Adding a C++ system test

Registration is data-driven. There is no `add_test` call to copy.

1. Add a `.cpp` file to `Tests/System/Cpp/`, or to its `FMTWrapperCore/` folder for a test of the
   wrapper core, named as in [Test names](Documentation/CodingStandards.md#test-names). Every file in
   these folders becomes an executable ([Tests/System/Cpp/CMakeLists.txt](Tests/System/Cpp/CMakeLists.txt),
   line 22).
2. Add a row to a CSV in `Tests/System/Cpp/`, in the form
   `TEST;primarylocation;scenario;doublevalue`. The row registers the test with CTest only if a
   target of that name exists (`registercsvtests` in
   [Tests/Support/TestRegistration.cmake](Tests/Support/TestRegistration.cmake)), and a name already
   given to another test stops the configuration.
3. `basetests.csv` is versioned. `BFECtests.csv` is listed in `.gitignore`, so `git grep` never shows
   it. Read that file directly before concluding what CTest actually runs. A row that names an
   executable by its name before issue #350 (`Tests/Support/RenamedTests.csv`) still registers, with a
   warning, until `tools\testMigration\Update_PrivateTests.bat` renames the rows of that copy.

### Python and R tests

Python and R behaviour is covered by the pytest suite of `Tests/System/Python/` and the testthat suite
of `Tests/System/R/`. Each `test<Domain>.py` or `test<Domain>.R` file is one system test,
`System.Python.<Domain>` or `System.R.<Domain>`, labelled `system` and `python` or `r`. A new file is
registered at the next configuration.

- The Python tests are registered where `cmake/BaseInstallPython.cmake` builds the wheel: a build with
  the `FMT` target and a Python interpreter, without `R_DIR` or `WITHOUT_PYTHONLIB`: the MSVC builds.
  `runTestFile.py` runs one file with pytest against `FMT.pyd` in `build/release/bin/Release`, put first
  on the path by `PYTHONPATH`, so they check the current build rather than the wheel last installed.
  `FMT.pyd` needs the DLLs that `cmake --install` copies next to it: until then, the tests are skipped.
- The R tests are registered with `R_DIR`, when its `Rscript` is found at configuration: the MSYS2
  builds. `runTestFile.R` runs one file with testthat against the FMT package installed in that R by
  `cmake/BaseInstallR.cmake`: the tests are skipped until the package of this build's version is
  installed. `runTests.R` still runs the whole folder by hand.

A test that cannot run, because pytest, testthat or the package is missing, returns 77 and prints
`SKIPPED: <reason>`, which the summary shows. Both suites run in the ctest pass of `cmake --install`,
and like any system test:

```bash
ctest --test-dir build/release -C Release -L "^python$" --output-on-failure
tools\tests\RunTests.bat system -R "System\.Python\."
```

Reporting a test result: [CodingStandards.md, Testing](Documentation/CodingStandards.md#testing).

### Benchmarks

`Tests/Performance/performance.csv` registers the benchmarks of `FMTPerformanceTests` with CTest,
labelled `performance`, `allocation` when a row bounds the allocations of a call, and `memory` when it
bounds the memory a call keeps or the peak memory of the process. With the rest of the suite they run in
a short mode, which only checks their results and bounds. The private benchmarks, rows of the local
`performance-private.csv` on models of `T:\`, are labelled `bfec-perf` and stay out of the base suite. A
measurement runs them apart, one at a time:

```bash
FMT_BENCHMARK_MODE=full ctest --test-dir build/release -C Release -L performance
```

Running them, reading their results and adding one: [Documentation/PerformanceTesting.md](Documentation/PerformanceTesting.md).

## Conventions that are easy to break

Each of these is defined in full where it is linked. They are listed here because they are easy to
break before you have read anything else.

| Trap | What it means for you |
| --- | --- |
| Source files are encoded in cp1252, not UTF-8, and hold accented characters | Preserve the encoding and the CRLF endings of every file you edit. Rewriting one as UTF-8 corrupts every accent in it. [Rule](Documentation/CodingStandards.md#file-encoding-and-line-endings) |
| Every source and CMake file carries the LiLiQ-R license header | Copy it into any new file. [Rule](Documentation/CodingStandards.md#license-header) |
| A Doxygen block needs a `// DocString:` marker to reach Python and R users | Add the marker above the block, naming the symbol. [Rule](Documentation/CodingStandards.md#docstring-markers) |
| Parameters use the `p_` prefix in modernized code | Do not add to the legacy `l` prefix, and do not mix both in one interface. [Rule](Documentation/CodingStandards.md#parameters) |
| `Tests/System/Cpp/BFECtests.csv` is listed in `.gitignore` | `git grep` never shows it. Read it directly before concluding what CTest runs. |

### Language

- Code, comments, Doxygen and test output: English.
- User-facing interface strings, `ETAT.md` files and `CHANGELOG.fr.md`: French.
- Issues, pull requests and their comments: French. They are written for this team, which reads
  French faster; a thread opened in English does not change that.
- `CHANGELOG.md` and `CHANGELOG.fr.md` cover the same releases and are updated together.

## Commits and changelog

Commit messages follow Conventional Commits 1.0.0, as produced by
`tools/commitMessage/CommitMessage_Generator.bat`: `type(scope): description`, lowercase type,
imperative subject, 72 characters or fewer, `!` after the type or scope for a breaking change.

The changelog follows Keep a Changelog and SemVer, produced by
`tools/changelog/Changelog_Generate.bat`.

Do not create commits unless you are asked to. Describe what changed and leave the commit to the
maintainer.

## Work that spans several sessions

For a campaign that runs over several sessions, such as a migration, a large rename or a test
effort, keep a status file next to the work: `FMTWrapperCore/ETAT.md`, `Tests/ETAT.md`.
Write it in French. Read it before each batch and update it afterwards.

It should record:

- the objective, and what is explicitly out of scope;
- what is done and what remains, batch by batch;
- decisions already taken, so they are not reopened;
- traps encountered along the way;
- how to verify the work.

A status file tracks a campaign in progress. This file, `Architecture.md` and `CodingStandards.md`
are permanent.

## Working rules

- Stay within the requested scope. Do not reformat a legacy file you happened to open, and do not
  start a repository-wide rename inside an unrelated change.
- Do not modify generated artifacts, such as the contents of `Templates/` or generated `.pyi` stubs,
  without updating the step that generates them.
- Do not change `vcpkg.json`, `vcpkg-custom-ports/` or `vcpkg-overlays/` unless dependencies are the
  task.
- Existing code does not define the standard for new code. Equally, do not rewrite a working
  component to match a rule as a side effect of an unrelated change.
- Report accurately what was verified and what was not.

## Before you finish

Work through the checklist in
[CodingStandards.md, Code Review Checklist](Documentation/CodingStandards.md#code-review-checklist).
It is the only checklist in the project; this file does not keep a second copy.
