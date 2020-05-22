/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTfreeexceptionhandler.h"

namespace Exception
{

#ifdef FMTWITHGDAL
	FMTexceptionhandler* FMTfreeexceptionhandler::getCPLdata()
		{
		return this;
		}
	void FMTfreeexceptionhandler::handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg)
		{

		}
#endif

FMTfreeexceptionhandler::FMTfreeexceptionhandler()
{
	this->disablenestedexceptions();
}


FMTlev FMTfreeexceptionhandler::raise(FMTexc lexception, Core::FMTsection lsection, std::string text,
	const int& line, const std::string& file)
{
	FMTexception excp;
	if (lsection == Core::FMTsection::Empty)
	{
		excp = FMTexception(lexception, updatestatus(lexception, text));
	}
	else {
		excp = FMTexception(lexception, lsection, updatestatus(lexception, text));
	}
	excp.sethold(true);
	if (!needtorethrow())
		{
		if (_level == FMTlev::FMT_Warning)
		{

		}else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range) 
			{
			//std::throw_with_nested(FMTerror(excp));
			throw FMTerror(excp);
			}
		}
	
	return _level;
}

}