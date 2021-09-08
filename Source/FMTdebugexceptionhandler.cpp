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
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
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
					if (!_specificwarningcount.count(lexception))
					{
						_specificwarningcount[lexception]=1;
					}else{
						_specificwarningcount[lexception]+=1;
					}
					FMTwarning(excp).warn(_logger,_specificwarningcount.at(lexception),maxwarningsbeforesilenced);
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
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTexceptionhandler::handelCPLerror(eErrClass, nError, pszErrorMsg);
        }catch(...)
            {
            raisefromcatch("", "FMTdebugexceptionhandler::handelCPLerror", __LINE__, __FILE__);
            }

		}
#endif

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTdebugexceptionhandler)
