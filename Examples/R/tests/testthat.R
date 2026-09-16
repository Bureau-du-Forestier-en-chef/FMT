library(testthat)
library(FMT)

test_dir(
  path = dirname(sys.frame(1)$ofile),
  reporter = "summary",
  stop_on_failure = TRUE,
  stop_on_warning = FALSE
)
