test_that("Spatial exports construct and preserve coordinates", {
  for (className in c("FMTcoordinate", "FMTforest", "FMTSpatialSchedule")) {
    expect_true(exists(className, inherits = TRUE))
  }

  coordinate <- new(FMTcoordinate, 4L, 7L)
  expect_equal(coordinate$getx(), 4L)
  expect_equal(coordinate$gety(), 7L)
})
