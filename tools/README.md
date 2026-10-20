# Tools

Scripts that build, test and document FMT. Each launcher finds the repository root from its own
location: run it by a double-click in the Explorer, or from a console at the root, for example
`tools\tests\RunTests.bat system`. A double-click passes no option: the options of section 2 need a
console.

## 1. Scripts

| Folder | Script | What it does |
| --- | --- | --- |
| `build/` | `CMakeFMTVS2022vcpkg.bat` | Configures, builds and installs the debug and release builds with Visual Studio 2022 and vcpkg. |
| `build/` | `CMakeFMTVS2022vcpkg_GL.bat` | Release build with tests, mirrored by the `release-gl` preset of `CMakePresets.json`. |
| `build/` | `CMakeFMTVS2022vcpkg_release.bat` | Release package, without tests. |
| `build/` | `CMakeFMT_MAM_NEW.bat` | Release package, with tests. |
| `build/` | `CMakeFMTMSYS2rcran45.sh`, `CMakeFMTMSYS2rcran45_release.sh` | MSYS2 builds of the R package, run in the Rtools shell from `/home/FMT`. |
| `tests/` | `RunTests.bat` | Runs a test preset (`all-public` by default) and prints the summary by level. |
| `tests/` | `RunPrivateTests.bat` | Checks that `T:\` is reachable, then runs `RunTests.bat system-private`. |
| `testMigration/` | `Update_PrivateTests.bat` | Renames the test executables in the private copy of `BFECtests.csv` of a worktree, after their renaming under #350, and moves a copy left in `Examples/C++/tests` to `Tests/System/Cpp`. |
| `performance/` | `Measure_Performance.bat` | Measures the benchmarks in full mode and compares them with reference runs ([PerformanceTesting.md](../Documentation/PerformanceTesting.md)). |
| `commitMessage/` | `CommitMessage_Generator.bat` | Copies a Conventional Commits prompt built from the staged diff to the clipboard. |
| `changelog/` | `Changelog_Generate.bat` | Copies a changelog prompt to the clipboard, then, after confirmation, commits and pushes the changelog, and tags a version. |
| `HeapCorruption/` | `HeapCorruptionDebug.bat` | gflags and ntsd commands to track heap corruption with the page heap. |
| `RToolsSetup/` | `Rtools_dependencies_setup.sh` | The `/etc/profile` settings of Rtools that the MSYS2 build needs. |

The scripts that section 2 does not list take no option.

## 2. Options

### 2.1 Builds

`CMakeFMTVS2022vcpkg.bat`, `CMakeFMTVS2022vcpkg_GL.bat` and `CMakeFMT_MAM_NEW.bat` take one option:

| Option | Effect |
| --- | --- |
| `-PrivateTests` | Once the release build succeeded, runs `tools\tests\RunPrivateTests.bat` on `build/release` (about 168 minutes). When the build fails, the tests do not run. |

```bat
tools\build\CMakeFMTVS2022vcpkg_GL.bat -PrivateTests
```

The private tests are the rows of `Tests/System/Cpp/BFECtests.csv`, read at the configuration: without
that file, the `system-private` preset has no test. `CMakeFMTVS2022vcpkg_release.bat` builds without
tests and takes no option.

### 2.2 Tests

`RunTests.bat [preset] [ctest options...]`

| Argument | Effect |
| --- | --- |
| `preset` | Test preset of `CMakePresets.json`: `unit`, `integration`, `system`, `system-private`, `performance` or `all-public` (the default). An argument that starts with `-` is not a preset: it goes to ctest. |
| `ctest options` | Passed to ctest as they are, such as `-R <regex>`, `-E <regex>`, `-L <label>`, `-j <n>`, `--output-on-failure`. |

`RunPrivateTests.bat [ctest options...]` passes its options to `RunTests.bat system-private`.

```bat
tools\tests\RunTests.bat system -R Scenario
tools\tests\RunPrivateTests.bat -R DoPlanning
```

Both wait for a key at the end, then return the exit code of ctest.

### 2.3 Migration of the private tests

`Update_PrivateTests.bat [-Worktree <folder>]`

| Option | Effect |
| --- | --- |
| `-Worktree <folder>` | Worktree whose copy of `BFECtests.csv` is migrated. Default: the one that holds the script. |

Without option, the window waits for a key at the end.

### 2.4 Performance

`Measure_Performance.bat` passes its options to `Measure-Performance.ps1`:

| Option | Effect |
| --- | --- |
| `-Runs <n>` | Number of runs, 1 to 100. Default: 3. |
| `-Label <label>` | ctest label of the benchmarks: `performance` (the default) for the public ones, `bfec-perf` for the private ones. |
| `-Regex <regex>` | ctest regular expression that narrows the benchmarks, such as `ComplexYield`. |
| `-Baseline <runs>` | Reference runs to compare with: folders or `.json` files, wildcards allowed. Without it, no comparison. |
| `-Output <folder>` | Folder that keeps the runs. Default: `perf-references` beside the repository. |
| `-BuildDir <folder>` | Build to measure. Default: `build\release` in the repository. |
| `-Root <folder>` | Repository. Default: the one that holds the script. |

```bat
tools\performance\Measure_Performance.bat -Baseline ..\perf-references\<date>_<commit>_performance_*
tools\performance\Measure_Performance.bat -Label bfec-perf -Runs 2
```

Without option, the window waits for a key at the end.

### 2.5 Changelog

`Changelog_Generate.bat` passes its options to `Generate-Changelog.ps1`:

| Option | Effect |
| --- | --- |
| `since` | Git reference to start from: tag, hash or date. Default: the last tag, or the whole history without tag. |
| `-Diff` | Adds the full diff of the code to the prompt, for small ranges. Choice 2 of the menu of the script adds it too. |
| `-MaxDiffChars <n>` | Maximum size of the diff, in characters, beyond which it is truncated. Default: 60000. |

```bat
tools\changelog\Changelog_Generate.bat v1.0.2
tools\changelog\Changelog_Generate.bat HEAD~3 -Diff
```

The publication step, `Release-Changelog.ps1`, runs without option from the launcher. Run it directly
for its own options: `-DryRun` shows the plan and stops, `-Push:$false` keeps the commit, the tag and
the branch local.

Running the tests, the presets and the summary: [AGENTS.md, Testing](../AGENTS.md#testing).
