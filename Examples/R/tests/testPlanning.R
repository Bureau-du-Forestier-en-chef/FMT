test_that("CLP planning solves the TWD_land LP scenario", {
  skipIfFMTFeatureMissing("OSI")
  primary <- fmtModelPath()
  parser <- new(FMTmodelparser)
  parser$setquietlogger()
  models <- parser$readproject(primary, c("LP"), TRUE, TRUE, TRUE)
  expect_length(models, 1L)

  model <- new(FMTlpmodel, models[[1]], FMTsolverinterface$CLP)
  model$setintparameter(FMTintmodelparameters$LENGTH, 2L)

  solved <- model$doplanning(TRUE, list())
  expect_true(solved)

  objective <- model$getobjectivevalue()
  expect_true(is.finite(objective))
  outputs <- model$getoutputs()
  expect_gt(length(outputs), 0L)
})
