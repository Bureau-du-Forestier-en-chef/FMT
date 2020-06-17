#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

FILE(GLOB_RECURSE RCPP_POTENTIAL_INCLUDE $ENV{RCPP_DIR}Rcpp.h)
list(GET RCPP_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(RCPP_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)


include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(RCPP  DEFAULT_MSG
                                  RCPP_INCLUDE_DIR)

mark_as_advanced(RCPP_INCLUDE_DIR RCPP_FOUND)

