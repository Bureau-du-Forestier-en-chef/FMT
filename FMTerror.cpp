/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTerror.h"



namespace Exception
{
	FMTerror::FMTerror() : FMTexception() {}
	FMTerror::FMTerror(const FMTexception& rhs) : FMTexception(rhs) {}
	FMTerror::FMTerror(const FMTexc lexception, const std::string message) : FMTexception(lexception, message) {}
	FMTerror::FMTerror(const FMTexc lexception, const Core::FMTsection lsection, const std::string message) : FMTexception(lexception, lsection, message) {}

}
