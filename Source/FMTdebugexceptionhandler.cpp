/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdebugexceptionhandler.h"
#include "FMTlogger.h"
#include "FMTwarning.h"
#include "FMTerror.h"
#include "FMTcplhandler.h"


namespace Exception
{


	FMTdebugexceptionhandler::FMTdebugexceptionhandler()
	{

	}


	FMTexception FMTdebugexceptionhandler::raise(FMTexc lexception, std::string text,
		const std::string& method,const int& line, const std::string& file, Core::FMTsection lsection, bool throwit)
	{
		
		const FMTlev LEVEL = getLevel(lexception);
		FMTexception excp;
		if (lsection == Core::FMTsection::Empty)
			{
			excp = FMTexception(lexception, updatestatus(lexception, text), method, file, line);
			}else {
			excp = FMTexception(lexception, lsection, updatestatus(lexception, text),method, file,line);
			}


		if (throwit)
			{
			if (LEVEL == FMTlev::FMT_Warning || LEVEL == FMTlev::FMT_Debug)
				{
				FMTwarning(excp).warn(*_logger, _specificwarningcount, maxwarningsbeforesilenced);
				}else if(LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range)
				{
					boost::lock_guard<boost::recursive_mutex> guard(mtx);
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
	void FMTdebugexceptionhandler::handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg)
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTexceptionhandler::handelCPLerror(eErrClass, nError, pszErrorMsg);
        }catch(...)
            {
            raisefromcatch("", "FMTdebugexceptionhandler::handelCPLerror", __LINE__, __FILE__);
            }

		}
#endif

	std::unique_ptr <FMTexceptionhandler> FMTdebugexceptionhandler::Clone() const
	{
		return std::unique_ptr <FMTexceptionhandler>(new FMTdebugexceptionhandler(*this));
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTdebugexceptionhandler)
