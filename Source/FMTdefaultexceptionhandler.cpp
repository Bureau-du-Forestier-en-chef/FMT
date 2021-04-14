/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdefaultexceptionhandler.h"


namespace Exception

{

	FMTexceptionhandler* FMTdefaultexceptionhandler::getCPLdata()
		{
		return this;
		}

#if defined FMTWITHGDAL
	void FMTdefaultexceptionhandler::handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg)
		{
		std::lock_guard<std::recursive_mutex> guard(mtx);
        try{
            FMTexceptionhandler::handelCPLerror(eErrClass,nError,pszErrorMsg);
        }catch(...)
            {
            raisefromcatch("", "FMTdefaultexceptionhandler::handelCPLerror", __LINE__, __FILE__);
            }
		}
#endif

	FMTdefaultexceptionhandler::FMTdefaultexceptionhandler() :FMTexceptionhandler() {}

	FMTexception FMTdefaultexceptionhandler::raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection,bool throwit)
	{
		std::lock_guard<std::recursive_mutex> guard(mtx);
		FMTexception excp = FMTexception(lexception, updatestatus(lexception, text));
		if (_level != FMTlev::FMT_Warning)
		{
			if (lsection == Core::FMTsection::Empty)
			{
				excp = FMTexception(lexception, updatestatus(lexception, text), method,file,line);
			}
			else {
				excp = FMTexception(lexception, lsection, updatestatus(lexception, text), method, file, line);
			}
		}
		if (throwit && !needtorethrow())
			{
			if (_level == FMTlev::FMT_Warning)
			{
				FMTwarning(excp).warn(_logger);
			}else if (_level == FMTlev::FMT_logic || _level == FMTlev::FMT_range)
				{
					std::throw_with_nested(FMTerror(excp));
				}
			}

		return excp;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTdefaultexceptionhandler)
