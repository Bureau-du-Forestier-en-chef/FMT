# Copyright (c) 2019 Gouvernement du Québec
#
# SPDX-License-Identifier: LiLiQ-R-1.1
# License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt

"""Run one pytest file of Tests/System/Python as a ctest test.

Usage: python runTestFile.py <binary directory> <test file>

The FMT module under test is FMT.pyd in <binary directory>, the folder of the executables, which ctest
puts ahead of the installed wheel through PYTHONPATH: the tests check this build, not the last wheel.

Exit codes: 0 when the file passes, 77 (skipped by ctest, with a "SKIPPED: <reason>" line) when pytest
is missing or when FMT cannot be imported from a tree that cmake --install never prepared, any other
value when a test fails, when no test is collected, when FMT.pyd is not built, or when FMT cannot be
imported from a prepared tree, or is imported from anywhere else.
"""

import sys
from importlib.machinery import EXTENSION_SUFFIXES
from pathlib import Path

SKIP_RETURN_CODE = 77
NO_TESTS_COLLECTED = 5


def skip(reason: str) -> int:
    print(f"SKIPPED: {reason}", flush=True)
    return SKIP_RETURN_CODE


def isModuleBuilt(binaryDirectory: Path) -> bool:
    return any((binaryDirectory / f"FMT{suffix}").is_file() for suffix in EXTENSION_SUFFIXES)


def isInstalledTree(binaryDirectory: Path) -> bool:
    """cmake --install copies the DLLs of FMT.pyd next to it, then leaves the wheel it builds there."""
    return any(binaryDirectory.glob("FMT*.whl"))


def isSameDirectory(first: Path, second: Path) -> bool:
    return first.resolve() == second.resolve()


def main(arguments: list) -> int:
    if len(arguments) != 2:
        print("Usage: runTestFile.py <binary directory> <test file>", file=sys.stderr)
        return 2
    binaryDirectory = Path(arguments[0])
    testFile = Path(arguments[1])
    if not testFile.is_file():
        print(f"Test file not found: {testFile}", file=sys.stderr)
        return 2

    if not isModuleBuilt(binaryDirectory):
        print(f"The FMT module is not built in {binaryDirectory}: build the FMT target", file=sys.stderr)
        return 1

    # Nothing is written next to the sources: no __pycache__, no rewritten test module.
    sys.dont_write_bytecode = True

    try:
        import pytest
    except ImportError:
        return skip(f"pytest is not installed for {sys.executable}")

    try:
        import FMT
    except ImportError as error:
        if not isInstalledTree(binaryDirectory):
            # Apart from the reason: Windows words the error in its language, which the summary garbles.
            result = skip("FMT.pyd needs the DLLs that cmake --install copies next to it")
            print(f"Import error: {error}", flush=True)
            return result
        print(f"FMT cannot be imported from {binaryDirectory}: {error}", file=sys.stderr, flush=True)
        return 1

    moduleFile = Path(FMT.__file__)
    print(f"FMT.__file__: {moduleFile}", flush=True)
    if not isSameDirectory(moduleFile.parent, binaryDirectory):
        print(f"FMT was imported from {moduleFile.parent}, not from {binaryDirectory}",
              file=sys.stderr, flush=True)
        return 1

    result = int(pytest.main([str(testFile), "-p", "no:cacheprovider"]))
    if result == NO_TESTS_COLLECTED:
        print(f"No test was collected in {testFile}", file=sys.stderr, flush=True)
        return 1
    return result


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
