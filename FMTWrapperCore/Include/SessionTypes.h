/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTWRAPPERCORE_SESSIONTYPES_HEADER
#define FMTWRAPPERCORE_SESSIONTYPES_HEADER

namespace FMTWrapper::Backend
{
    /**
    @brief Warnings tolerated before the exception handler stops repeating them.

    Applies when the interface does not ask for a number of its own: setErrorsToWarnings
    falls back on this value when it receives zero or less.
    */
    inline constexpr int DEFAULT_MAX_WARNINGS = 10;
}

#endif // FMTWRAPPERCORE_SESSIONTYPES_HEADER
