# Copyright (c) 2019 Gouvernement du Québec
#
# SPDX-License-Identifier: LiLiQ-R-1.1
# License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt

test_that("Parallel exports are available", {
  classNames <- c("FMTtask", "FMTreplanningtask", "FMTplanningtask", "FMTtaskhandler")
  for (className in classNames) {
    expect_true(exists(className, inherits = TRUE))
  }
})

test_that("Operating area task follows OSI feature", {
  skipIfFMTFeatureMissing("OSI")
  expect_true(exists("FMTopareaschedulertask", inherits = TRUE))
})
