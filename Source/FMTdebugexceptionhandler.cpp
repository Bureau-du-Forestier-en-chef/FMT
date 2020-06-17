/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdebugexceptionhandler.h"
#include "FMTlogger.h"

namespace Exception
{


	FMTdebugexceptionhandler::FMTdebugexceptionhandler()
	{

	}


	FMTexception FMTdebugexceptionhandler::raise(FMTexc lexception, std::string text,
		const std::string& method,const int& line, const std::string& file, Core::FMTsection lsection, bool throwit)
	{
		FMTexception excp;
		if (lsection == Core::FMTsection::Empty)
			{
			excp = FMTexception(lexception, updatestatus(lexception, text), method, file, line);
			}else {
			excp = FMTexception(lexception, lsection, updatestatus(lexception, text),method, file,line);
			}
		if (throwit && !needtorethrow())
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
		
		return excp;
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
