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


FMTexception FMTfreeexceptionhandler::raise(FMTexc lexception, std::string text,
	const std::string& method,const int& line, const std::string& file, Core::FMTsection lsection,bool throwit)
{
	FMTexception excp = FMTexception(lexception, updatestatus(lexception, text));
	if (_level != FMTlev::FMT_Warning)
	{
		if (lsection == Core::FMTsection::Empty)
		{
			excp = FMTexception(lexception, updatestatus(lexception, text), method, file, line);
		}
		else {
			excp = FMTexception(lexception, lsection, updatestatus(lexception, text), method, file, line);
		}
	}
	excp.sethold(true);
	if (throwit&&!needtorethrow())
		{
		if (_level == FMTlev::FMT_Warning)
		{

		}else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range) 
			{
			//std::throw_with_nested(FMTerror(excp));
			throw FMTerror(excp);
			}
		}
	
	return excp;
}

}