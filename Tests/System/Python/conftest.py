import os
from pathlib import Path
import pytest
from FMT import Version


def fmtSourceDirectory() -> Path:
    value = os.environ.get("FMT_SOURCE_DIR")
    if not value:
        pytest.fail("FMT_SOURCE_DIR is not defined by the install workflow")
    path = Path(value).resolve()
    if not path.is_dir():
        pytest.fail(f"FMT_SOURCE_DIR does not exist: {path}")
    return path


def twdLandPrimary() -> Path:
    path = fmtSourceDirectory() / "Examples" / "Models" / "TWD_land" / "TWD_land.pri"
    if not path.is_file():
        pytest.fail(f"TWD_land primary file does not exist: {path}")
    return path


def hasFeature(featureName: str) -> bool:
    result = bool(Version.FMTversion.hasfeature(featureName))
    return result


def requireFeature(featureName: str) -> None:
    if not hasFeature(featureName):
        pytest.skip(f"FMT was compiled without {featureName} support")


@pytest.fixture(scope="session")
def modelPath() -> Path:
    path = twdLandPrimary()
    return path
