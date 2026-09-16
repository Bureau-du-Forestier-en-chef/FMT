fmtSourceDirectory <- function() {
  sourceDirectory <- Sys.getenv("FMT_SOURCE_DIR")
  if (!nzchar(sourceDirectory)) {
    fail("FMT_SOURCE_DIR is not defined by the install workflow")
  }
  normalizePath(sourceDirectory, mustWork = TRUE)
}

fmtModelPath <- function() {
  modelPath <- file.path(
    fmtSourceDirectory(), "Examples", "Models", "TWD_land", "TWD_land.pri"
  )
  if (!file.exists(modelPath)) {
    fail(paste("TWD_land primary file does not exist:", modelPath))
  }
  normalizePath(modelPath, winslash = "/", mustWork = TRUE)
}

fmtHasFeature <- function(featureName) {
  version <- new(FMTversion)
  result <- isTRUE(version$hasfeature(featureName))
  result
}

skipIfFMTFeatureMissing <- function(featureName) {
  if (!fmtHasFeature(featureName)) {
    skip(paste("FMT was compiled without", featureName, "support"))
  }
}
