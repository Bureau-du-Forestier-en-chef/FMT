fmtSourceDirectory <- function() {
  sourceDirectory <- Sys.getenv("FMT_SOURCE_DIR")

  if (!nzchar(sourceDirectory)) {
    stop("FMT_SOURCE_DIR is not defined")
  }

  normalizePath(sourceDirectory, winslash = "/", mustWork = TRUE)
}

fmtModelPath <- function() {
  modelPath <- file.path(
    fmtSourceDirectory(),
    "Examples",
    "Models",
    "TWD_land",
    "TWD_land.pri"
  )

  if (!file.exists(modelPath)) {
    stop(paste("TWD_land primary file does not exist:", modelPath))
  }

  normalizePath(modelPath, winslash = "/", mustWork = TRUE)
}

fmtHasFeature <- function(featureName) {
  version <- new(FMTversion)
  isTRUE(version$hasfeature(featureName))
}

skipIfFMTFeatureMissing <- function(featureName) {
  if (!fmtHasFeature(featureName)) {
    skip(paste("FMT was compiled without", featureName, "support"))
  }
}

fmtPackageNamespace <- function() {
  asNamespace("FMT")
}

fmtFindExport <- function(candidateNames) {
  namespace <- fmtPackageNamespace()

  for (candidateName in candidateNames) {
    if (exists(candidateName, envir = namespace, inherits = FALSE)) {
      return(get(candidateName, envir = namespace, inherits = FALSE))
    }
  }

  NULL
}

fmtSolverInterface <- function() {
  fmtFindExport(
    c(
      "FMTsolverinterface",
      "FMTSolverInterface",
      "FMTsolverInterface"
    )
  )
}

skipIfSolverInterfaceMissing <- function() {
  if (is.null(fmtSolverInterface())) {
    skip(
      paste(
        "FMT was compiled with OSI, but the solver interface",
        "enum is not exported to R"
      )
    )
  }
}
