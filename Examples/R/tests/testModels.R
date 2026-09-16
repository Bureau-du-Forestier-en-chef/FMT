test_that("Unconditional model exports are available", {
  classNames <- c("FMTmodel", "FMTsemodel", "FMTsesmodel", "FMTsamodel")
  for (className in classNames) {
    expect_true(exists(className, inherits = TRUE))
  }
  expect_true(exists("FMTintmodelparameters", inherits = TRUE))
  expect_true(exists("FMTdblmodelparameters", inherits = TRUE))
  expect_true(exists("FMTboolmodelparameters", inherits = TRUE))
})

test_that("OSI model exports follow compiled features", {
  skipIfFMTFeatureMissing("OSI")
  for (className in c("FMTlpsolver", "FMTsrmodel", "FMTnssmodel", "FMTlpmodel")) {
    expect_true(exists(className, inherits = TRUE))
  }
  expect_true(exists("FMTsolverinterface", inherits = TRUE))
  expect_false(is.null(FMTsolverinterface$CLP))
})

test_that("Mosek enum follows compiled features", {
  skipIfFMTFeatureMissing("MOSEK")
  expect_false(is.null(FMTsolverinterface$MOSEK))
})
