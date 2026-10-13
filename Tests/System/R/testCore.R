test_that("Core exports construct and preserve values", {
  classNames <- c(
    "FMTobject", "FMTmask", "FMTmaskfilter", "FMTdevelopment",
    "FMTagebounds", "FMTperbounds", "FMTyldbounds", "FMTlockbounds",
    "FMTspec", "FMTactualdevelopment", "FMTfuturdevelopment",
    "FMTaction", "FMTlifespans", "FMTtransitionmask", "FMTfork",
    "FMTyieldhandler", "FMTyields", "FMTtransition", "FMToutputsource",
    "FMToperator", "FMToutputnode", "FMToutput", "FMTtheme",
    "FMTconstants", "FMTschedule", "FMTGCBMtransition", "FMTconstraint"
  )
  for (className in classNames) {
    expect_true(exists(className, inherits = TRUE))
  }

  schedule <- new(FMTschedule)
  schedule$setperiod(3L)
  expect_equal(schedule$getperiod(), 3L)
})
