test_that("Unconditional model exports are available", {
  classNames <- c(
    "FMTmodel",
    "FMTsemodel",
    "FMTsesmodel",
    "FMTsamodel"
  )

  for (className in classNames) {
    expect_true(
      exists(className, envir = asNamespace("FMT"), inherits = FALSE),
      info = paste("Missing R model export:", className)
    )
  }

  expect_true(
    exists(
      "FMTintmodelparameters",
      envir = asNamespace("FMT"),
      inherits = FALSE
    )
  )

  expect_true(
    exists(
      "FMTdblmodelparameters",
      envir = asNamespace("FMT"),
      inherits = FALSE
    )
  )

  expect_true(
    exists(
      "FMTboolmodelparameters",
      envir = asNamespace("FMT"),
      inherits = FALSE
    )
  )
})

test_that("OSI model exports follow compiled features", {
  skipIfFMTFeatureMissing("OSI")

  classNames <- c(
    "FMTlpsolver",
    "FMTsrmodel",
    "FMTnssmodel",
    "FMTlpmodel"
  )

  for (className in classNames) {
    expect_true(
      exists(className, envir = asNamespace("FMT"), inherits = FALSE),
      info = paste("Missing OSI-dependent R export:", className)
    )
  }

  skipIfSolverInterfaceMissing()
  solverInterface <- fmtSolverInterface()
  expect_false(is.null(solverInterface$CLP))
})

test_that("Mosek enum follows compiled features", {
  skipIfFMTFeatureMissing("MOSEK")
  skipIfSolverInterfaceMissing()

  solverInterface <- fmtSolverInterface()

  if (is.null(solverInterface$MOSEK)) {
    skip(
      paste(
        "FMT reports Mosek support, but MOSEK is not",
        "present in the exported solver interface"
      )
    )
  }

  expect_false(is.null(solverInterface$MOSEK))
})
