test_that("CLP planning solves the TWD_land LP scenario", {
  skipIfFMTFeatureMissing("OSI")
  skipIfSolverInterfaceMissing()

  solverInterface <- fmtSolverInterface()

  if (is.null(solverInterface$CLP)) {
    skip("CLP is not available in the exported solver interface")
  }

  primary <- fmtModelPath()
  parser <- new(FMTmodelparser)

  models <- parser$readproject(
    primary,
    c("LP"),
    TRUE,
    TRUE,
    TRUE
  )

  expect_length(models, 1L)

  model <- new(
    FMTlpmodel,
    models[[1]],
    solverInterface$CLP
  )

  model$setintparameter(
    FMTintmodelparameters$LENGTH,
    2L
  )

  solved <- model$doplanning(TRUE, list())
  expect_true(solved)

  objective <- model$getobjectivevalue()
  expect_true(is.finite(objective))

  outputs <- model$getoutputs()
  expect_gt(length(outputs), 0L)
})
