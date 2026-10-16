# Tools

Scripts that build, test and document FMT. Each launcher finds the repository root from its own
location: run it by a double-click in the Explorer, or from a console at the root, for example
`tools\tests\RunTests.bat system`.

| Folder | Script | What it does |
| --- | --- | --- |
| `build/` | `CMakeFMTVS2022vcpkg.bat` | Configures, builds and installs the debug and release builds with Visual Studio 2022 and vcpkg. |
| `build/` | `CMakeFMTVS2022vcpkg_GL.bat` | Release build with tests, mirrored by the `release-gl` preset of `CMakePresets.json`. |
| `build/` | `CMakeFMTVS2022vcpkg_release.bat` | Release package, without tests. |
| `build/` | `CMakeFMT_MAM_NEW.bat` | Release package, with tests. |
| `build/` | `CMakeFMTMSYS2rcran45.sh`, `CMakeFMTMSYS2rcran45_release.sh` | MSYS2 builds of the R package, run in the Rtools shell from `/home/FMT`. |
| `tests/` | `RunTests.bat [preset] [ctest options...]` | Runs a test preset (`all-public` by default) and prints the summary by level. |
| `tests/` | `RunPrivateTests.bat [ctest options...]` | Checks that `T:\` is reachable, then runs `RunTests.bat system-private`. |
| `testMigration/` | `Update_PrivateTests.bat [-Worktree <folder>]` | Renames the test executables in the private copy of `BFECtests.csv` of a worktree, after their renaming under #350, and moves a copy left in `Examples/C++/tests` to `Tests/System/Cpp`. |
| `performance/` | `Measure_Performance.bat` | Measures the benchmarks in full mode and compares them with reference runs ([PerformanceTesting.md](../Documentation/PerformanceTesting.md)). |
| `commitMessage/` | `CommitMessage_Generator.bat` | Copies a Conventional Commits prompt built from the staged diff to the clipboard. |
| `changelog/` | `Changelog_Generate.bat [since]` | Copies a changelog prompt to the clipboard, then, after confirmation, commits and pushes the changelog, and tags a version. |
| `HeapCorruption/` | `HeapCorruptionDebug.bat` | gflags and ntsd commands to track heap corruption with the page heap. |
| `RToolsSetup/` | `Rtools_dependencies_setup.sh` | The `/etc/profile` settings of Rtools that the MSYS2 build needs. |

Running the tests, the presets and the summary: [AGENTS.md, Testing](../AGENTS.md#testing).
