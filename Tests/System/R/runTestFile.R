# Copyright (c) 2019 Gouvernement du Québec
#
# SPDX-License-Identifier: LiLiQ-R-1.1
# License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt

# Runs one testthat file of Tests/System/R as a ctest test, against the FMT package installed in this R.
#
# Usage: Rscript --vanilla runTestFile.R <test file> <build version>
#
# Like runTests.R, it attaches testthat and FMT, and testthat sources helperFMT.R, from the folder of the
# test file, before the tests. Exit codes: 0 when the file passes; 77 (skipped by ctest, with a
# "SKIPPED: <reason>" line) when testthat or FMT is not installed, or when the installed FMT is not the
# version of this build; 1 when a test fails or errors, or when no test runs.

skipTestFile <- function(reason) {
  cat("SKIPPED: ", reason, "\n", sep = "")
  quit(save = "no", status = 77L)
}

failTestFile <- function(reason) {
  cat(reason, "\n", sep = "")
  quit(save = "no", status = 1L)
}

arguments <- commandArgs(
  trailingOnly = TRUE
)

if (length(arguments) != 2L) {
  failTestFile("Usage: Rscript --vanilla runTestFile.R <test file> <build version>")
}

testFile <- normalizePath(
  arguments[[1L]],
  winslash = "/",
  mustWork = TRUE
)

buildVersion <- package_version(arguments[[2L]])

options(
  cli.unicode = FALSE,
  cli.dynamic = FALSE,
  cli.num_colors = 1L,
  crayon.enabled = FALSE,
  testthat.use_colours = FALSE,
  testthat.summary.omit_dots = FALSE
)

Sys.setenv(
  NO_COLOR = "1",
  R_CLI_NUM_COLORS = "1"
)

# Installed, not loaded: a package that is installed but fails to load is a failure, not a skip.
for (packageName in c("testthat", "FMT")) {
  if (length(find.package(packageName, quiet = TRUE)) == 0L) {
    skipTestFile(
      paste0("the R package ", packageName, " is not installed in ", R.home())
    )
  }
}

installedVersion <- utils::packageVersion("FMT")

if (installedVersion != buildVersion) {
  skipTestFile(
    paste0(
      "the installed FMT package is version ", format(installedVersion),
      ", this build is version ", format(buildVersion), ": run cmake --install"
    )
  )
}

library(testthat)
library(FMT)

cat("FMT package: ", find.package("FMT"), "\n", sep = "")

results <- as.data.frame(
  test_file(
    testFile,
    reporter = "summary"
  )
)

if (nrow(results) == 0L) {
  failTestFile(paste("No test ran in", testFile))
}

if (sum(results$failed) > 0L || any(results$error)) {
  quit(save = "no", status = 1L)
}

quit(save = "no", status = 0L)
