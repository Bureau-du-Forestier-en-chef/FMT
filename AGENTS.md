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

## Repository map

| Path | Contents |
| --- | --- |
| `Include/`, `Source/` | The FMT library: 247 headers, 224 sources. Namespaces `Core` (model concepts), `Models` (planning models), `Spatial`, `Graph`, `Parser` (Woodstock files), `Heuristics`, `Parallel`, `Logging`, `Exception`. |
| `FMTWrapperCore/` | Portable wrapper core shared by the interfaces, with its own `tests/`. |
| `UI/` | `FMTForm` and the user interface. |
| `Excel/` | Excel integration. |
| `Examples/C++/` | Example programs. Each `.cpp` also becomes a test executable. |
| `Examples/Python/tests/`, `Examples/R/tests/` | pytest and testthat suites, run through the install targets. |
| `Examples/Models/` | Woodstock models used by the tests. |
| `Templates/` | Packaging inputs and generated artifacts: R package, `setup.py.in`, `__init__.py.in`, stub normalization. |
| `cmake/`, `Modules/` | Install and configuration scripts; `Find*.cmake` for GEOS, MOSEK, OSI, R, Rcpp, ONNX Runtime. |
| `tools/` | `commitMessage/`, `changelog/`, `HeapCorruption/`, `RToolsSetup/`. |
| `vcpkg.json`, `vcpkg-custom-ports/`, `vcpkg-overlays/` | Dependencies: GDAL, Arrow, COIN-OR, Python. |
| `Documentation/` | Architecture, coding standards, Doxygen configuration, training material. |

## Building

Configure presets are defined in `CMakePresets.json`: `release-mam`, `release-gc` and `release-gl`,
all Visual Studio 17 2022 generators with testing enabled. They all build into `build/release`.

```bash
cmake --preset release-gl
cmake --build --preset release-gl
```

Scripted entry points exist for the usual configurations: `CMakeFMTVS2022vcpkg.bat` and its variants
on Windows, `CMakeFMTMSYS2rcran45.sh` for the MSYS2 build of the R package. Configuring with
`-DWITHOUT_TESTING=ON` skips test registration entirely.

## Testing

```bash
ctest --test-dir build/release -C Release
```

### Adding a C++ test

Registration is data-driven. There is no `add_test` call to copy.

1. Add a `.cpp` file to `Examples/C++/`. Every file in that directory becomes an executable
   ([Examples/C++/CMakeLists.txt](Examples/C++/CMakeLists.txt), line 11).
2. Add a row to a CSV in `Examples/C++/tests/`, in the form
   `TEST;primarylocation;scenario;doublevalue`. The row registers the test with CTest only if a
   target of that name exists (same file, line 42).
3. `basetests.csv` is versioned. `BFECtests.csv` is listed in `.gitignore`, so `git grep` never shows
   it. Read that file directly before concluding what CTest actually runs.

Python and R behaviour is covered by `Examples/Python/tests/` and `Examples/R/tests/`, wired in
`cmake/BaseInstallPython.cmake` and `cmake/BaseInstallR.cmake`.

Do not report that tests passed unless they were executed. When you could not run them, state
explicitly which validation is missing.

## Conventions that are easy to break

Each of these is defined in full where it is linked. They are listed here because they are easy to
break before you have read anything else.

| Trap | What it means for you |
| --- | --- |
| Source files are encoded in cp1252, not UTF-8, and hold accented characters | Preserve the encoding and the CRLF endings of every file you edit. Rewriting one as UTF-8 corrupts every accent in it. [Rule](Documentation/CodingStandards.md#file-encoding-and-line-endings) |
| Every source and CMake file carries the LiLiQ-R license header | Copy it into any new file. [Rule](Documentation/CodingStandards.md#license-header) |
| A Doxygen block needs a `// DocString:` marker to reach Python and R users | Add the marker above the block, naming the symbol. [Rule](Documentation/CodingStandards.md#docstring-markers) |
| Parameters use the `p_` prefix in modernized code | Do not add to the legacy `l` prefix, and do not mix both in one interface. [Rule](Documentation/CodingStandards.md#parameters) |
| `Examples/C++/tests/BFECtests.csv` is listed in `.gitignore` | `git grep` never shows it. Read it directly before concluding what CTest runs. |

### Language

- Code, comments, Doxygen and test output: English.
- User-facing interface strings, `ETAT.md` files and `CHANGELOG.fr.md`: French.
- `CHANGELOG.md` and `CHANGELOG.fr.md` cover the same releases and are updated together.

## Commits and changelog

Commit messages follow Conventional Commits 1.0.0, as produced by `CommitMessage_Generator.bat`
(`tools/commitMessage/`): `type(scope): description`, lowercase type, imperative subject, 72
characters or fewer, `!` after the type or scope for a breaking change.

The changelog follows Keep a Changelog and SemVer, produced by `Changelog_Generate.bat`
(`tools/changelog/`).

Do not create commits unless you are asked to. Describe what changed and leave the commit to the
maintainer.

## Work that spans several sessions

For a campaign that runs over several sessions, such as a migration, a large rename or a test
effort, keep a status file next to the work: `FMTWrapperCore/ETAT.md`, `Examples/C++/tests/ETAT.md`.
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
