/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTwarning.h"
#include "FMTdefaultlogger.h"


namespace Exception
{
FMTwarning::FMTwarning() : FMTexception() {}
FMTwarning::FMTwarning(const FMTexception& rhs) : FMTexception(rhs) {}
FMTwarning::FMTwarning(const FMTexc lexception, const std::string message) : FMTexception(lexception, message) {}
FMTwarning::FMTwarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message) : FMTexception(lexception, lsection, message) {}

FMTwarning::FMTwarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message,
	const std::string& lmethod, const std::string& lfile, const int& lline):
	FMTexception(lexception, lsection, message, lmethod, lfile, lline) {}

void FMTwarning::warn(const std::shared_ptr<Logging::FMTlogger>logger) const
	{
		*logger << _msg << "\n";
	}
}
