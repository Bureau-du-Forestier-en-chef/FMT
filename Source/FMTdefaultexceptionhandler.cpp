/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdefaultexceptionhandler.h"
#include "FMTwarning.h"
#include "FMTerror.h"
#include "FMTcplhandler.h"


namespace Exception

{

	FMTexceptionhandler* FMTdefaultexceptionhandler::getCPLdata()
		{
		return this;
		}

#if defined FMTWITHGDAL
	void FMTdefaultexceptionhandler::handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg)
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTexceptionhandler::handelCPLerror(eErrClass,nError,pszErrorMsg);
        }catch(...)
            {
            raisefromcatch("", "FMTdefaultexceptionhandler::handelCPLerror", __LINE__, __FILE__);
            }
		}
#endif

	FMTdefaultexceptionhandler::FMTdefaultexceptionhandler() :FMTexceptionhandler() {}

	FMTdefaultexceptionhandler::FMTdefaultexceptionhandler(const std::unique_ptr<Logging::FMTlogger>& logger):
		FMTexceptionhandler(logger)
	{
	#if defined  FMTWITHGDAL
		Exception::FMTexceptionhandler* handler = reinterpret_cast<Exception::FMTexceptionhandler*>(CPLGetErrorHandlerUserData());
		CPLPushErrorHandlerEx(Exception::FMTCPLErrorHandler, this->getCPLdata());
	#endif
	}

	FMTexception FMTdefaultexceptionhandler::raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection,bool throwit)
	{
		
		const FMTlev LEVEL = getLevel(lexception);
		FMTexception excp = FMTexception(lexception, updatestatus(lexception, text));
		if (lsection != Core::FMTsection::Empty)
		{
			excp = FMTexception(lexception, lsection, updatestatus(lexception, text));
		}
		if (LEVEL != FMTlev::FMT_Warning)
		{
			if (lsection == Core::FMTsection::Empty)
			{
				excp = FMTexception(lexception, updatestatus(lexception, text), method,file,line);
			}
			else {
				excp = FMTexception(lexception, lsection, updatestatus(lexception, text), method, file, line);
			}
			if (throwit && (LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range) && !needtorethrow())
				{
				boost::lock_guard<boost::recursive_mutex> guard(mtx);
				std::throw_with_nested(FMTerror(excp));
				}
		}else if(throwit)
			{
			FMTwarning(excp).warn(*_logger, _specificwarningcount, maxwarningsbeforesilenced);
			}
		return excp;
	}


	std::unique_ptr <FMTexceptionhandler> FMTdefaultexceptionhandler::Clone() const
	{
		return std::unique_ptr<FMTexceptionhandler>(new FMTdefaultexceptionhandler(*this));
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTdefaultexceptionhandler)
