# Copyright (c) 2019 Gouvernement du Québec
#
# SPDX-License-Identifier: LiLiQ-R-1.1
# License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt

from FMT import Version


def testVersionInformationIsConsistent():
    major = Version.FMTversion.getmajor()
    minor = Version.FMTversion.getminor()
    patch = Version.FMTversion.getpatch()
    version = Version.FMTversion.getversion()
    assert major >= 0
    assert minor >= 0
    assert patch >= 0
    assert isinstance(version, str)
    assert version
    assert Version.FMTversion.isatleast(major, minor, patch)


def testVersionResourcesAreReadable():
    licenseText = Version.FMTversion.getlicense(False)
    changelog = Version.FMTversion.getchangelog(False)
    assert licenseText
    assert changelog
