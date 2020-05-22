/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdebugexceptionhandler.h"
#include "FMTlogger.h"

namespace Exception
{

	std::string FMTdebugexceptionhandler::getsrcinfo(const int& line, const std::string& file) const
	{
		return "In File(" + file + ") At Line(" + std::to_string(line) + ")";
	}

	FMTdebugexceptionhandler::FMTdebugexceptionhandler()
	{

	}

	FMTlev FMTdebugexceptionhandler::raise(FMTexc lexception, Core::FMTsection lsection, std::string text,
		const int& line, const std::string& file)
	{
		FMTexception excp;
		text += " " + getsrcinfo(line, file);
		if (lsection == Core::FMTsection::Empty)
		{
			excp = FMTexception(lexception, updatestatus(lexception, text));
		}
		else {
			excp = FMTexception(lexception, lsection, updatestatus(lexception, text));
		}
		if (!needtorethrow())
			{
			if (_level == FMTlev::FMT_Warning)
				{
				FMTwarning(excp).warn(_logger);
				}
			else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range) 
				{
				std::throw_with_nested(FMTerror(excp));
				}
			}
		
		return _level;
	}
#ifdef FMTWITHGDAL

	FMTexceptionhandler* FMTdebugexceptionhandler::getCPLdata()
		{
		return this;
		}
	void FMTdebugexceptionhandler::handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg)
		{
		FMTexceptionhandler::handelCPLerror(eErrClass, nError, pszErrorMsg);
		}
#endif

}
