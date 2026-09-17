test_that("Parser exports construct and load TWD_land", {
  parserClasses <- c(
    "FMTparser", "FMTareaparser", "FMTlandscapeparser", "FMTactionparser",
    "FMTtransitionparser", "FMTconstantparser", "FMTlifespanparser",
    "FMTyieldparser", "FMToutputparser", "FMToptimizationparser",
    "FMTmodelparser", "FMTscheduleparser"
  )
  for (className in parserClasses) {
    expect_true(exists(className, inherits = TRUE))
  }

  primary <- fmtModelPath()
  parser <- new(FMTmodelparser)
  parser$setquietlogger()
  models <- parser$readproject(primary, c("LP"), TRUE, TRUE, TRUE)
  expect_length(models, 1L)
  expect_true(nzchar(models[[1]]$getname()))
})
