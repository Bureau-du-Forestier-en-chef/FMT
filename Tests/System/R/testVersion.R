# Copyright (c) 2019 Gouvernement du Québec
#
# SPDX-License-Identifier: LiLiQ-R-1.1
# License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt

test_that("Version information is consistent", {
  version <- new(FMTversion)
  major <- version$getmajor()
  minor <- version$getminor()
  patch <- version$getpatch()
  versionString <- version$getversion()
  expect_gte(major, 0L)
  expect_gte(minor, 0L)
  expect_gte(patch, 0L)
  expect_true(nzchar(versionString))
  expect_true(version$isatleast(major, minor, patch))
  expect_true(nzchar(version$getlicense(FALSE)))
  expect_true(nzchar(version$getchangelog(FALSE)))
})
