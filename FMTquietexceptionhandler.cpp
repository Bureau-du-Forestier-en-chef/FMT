/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTquietexceptionhandler.h"

namespace Exception
{

	FMTquietexceptionhandler::FMTquietexceptionhandler() :FMTexceptionhandler() {}

	FMTlev FMTquietexceptionhandler::raise(FMTexc lexception, Core::FMTsection lsection, std::string text,
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
		if (!needtorethrow())
			{
			if (_level == FMTlev::FMT_Warning)
			{

			}
			else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range) 
				{
				std::throw_with_nested(FMTerror(excp));
				}
			}
		
		return _level;
	}
	#ifdef FMTWITHGDAL

		FMTexceptionhandler* FMTquietexceptionhandler::getCPLdata()
			{
			return this;
			}
		void FMTquietexceptionhandler::handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg)
			{

			}
	#endif

}
