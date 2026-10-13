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

library(testthat)
library(FMT)

arguments <- commandArgs(
  trailingOnly = FALSE
)

fileArgument <- arguments[
  grepl("^--file=", arguments)
]

if (length(fileArgument) != 1L) {
  stop("Unable to determine the R test runner location")
}

runnerPath <- sub(
  "^--file=",
  "",
  fileArgument
)

testDirectory <- dirname(
  normalizePath(
    runnerPath,
    winslash = "/",
    mustWork = TRUE
  )
)

test_dir(
  path = testDirectory,
  reporter = "summary",
  stop_on_failure = TRUE,
  stop_on_warning = FALSE
)