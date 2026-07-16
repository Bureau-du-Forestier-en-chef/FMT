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


	FMTDebugExceptionHandler::FMTDebugExceptionHandler()
	{

	}


	FMTException FMTDebugExceptionHandler::raise(FMTexc lexception, std::string text,
		const std::string& method,const int& line, const std::string& file, Core::FMTsection lsection, bool throwit)
	{
		
		const FMTlev LEVEL = getLevel(lexception);
		FMTException excp;
		if (lsection == Core::FMTsection::Empty)
			{
			excp = FMTException(lexception, updateStatus(lexception, text), method, file, line);
			}else {
			excp = FMTException(lexception, lsection, updateStatus(lexception, text),method, file,line);
			}


		if (throwit)
			{
			if (LEVEL == FMTlev::FMT_Warning || LEVEL == FMTlev::FMT_Debug)
				{
				FMTWarning(excp).warn(*_logger, _specificwarningcount, maxwarningsbeforesilenced);
				}else if(LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range)
				{
					boost::lock_guard<boost::recursive_mutex> guard(mtx);
					std::throw_with_nested(FMTError(excp));
				}

			}
		return excp;
	}
#ifdef FMTWITHGDAL

	FMTExceptionHandler* FMTDebugExceptionHandler::getCPLdata()
		{
		return this;
		}
	void FMTDebugExceptionHandler::handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg)
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTExceptionHandler::handelCPLerror(eErrClass, nError, pszErrorMsg);
        }catch(...)
            {
            raiseFromCatch("", "FMTDebugExceptionHandler::handelCPLerror", __LINE__, __FILE__);
            }

		}
#endif

	std::unique_ptr <FMTExceptionHandler> FMTDebugExceptionHandler::Clone() const
	{
		return std::unique_ptr <FMTExceptionHandler>(new FMTDebugExceptionHandler(*this));
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTDebugExceptionHandler)
