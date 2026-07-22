/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTQuietExceptionHandler.h"
#include "FMTError.h"
#include "FMTWarning.h"
#include "FMTcplhandler.h"

namespace Exception
{

	FMTQuietExceptionHandler::FMTQuietExceptionHandler() :FMTExceptionHandler() {}

	FMTException FMTQuietExceptionHandler::raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection, bool throwit)
	{
		
		const FMTlev LEVEL = getLevel(lexception);
		FMTException excp = FMTException(lexception, updateStatus(lexception, text));
		if (lsection != Core::FMTsection::Empty)
		{
			excp = FMTException(lexception, lsection, updateStatus(lexception, text));
		}
		if (LEVEL != FMTlev::FMT_Warning)
		{
			if (lsection == Core::FMTsection::Empty)
			{
				excp = FMTException(lexception, updateStatus(lexception, text), method, file, line);
			}
			else {
				excp = FMTException(lexception, lsection, updateStatus(lexception, text), method, file, line);
			}
		}
		if (throwit)
		{
			if (LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range)
			{
				boost::lock_guard<boost::recursive_mutex> guard(mtx);
				if (!needToRethrow()) {
					std::throw_with_nested(FMTError(excp));
				}
			}
		}
		return excp;
	}
	#ifdef FMTWITHGDAL

		FMTExceptionHandler* FMTQuietExceptionHandler::getCPLdata()
			{
			return this;
			}
		void FMTQuietExceptionHandler::handelCPLerror(int eErrClass,int nError, const char * pszErrorMsg)
			{
			//boost::lock_guard<boost::recursive_mutex> guard(mtx);
            try{
                FMTExceptionHandler::handelCPLerror(eErrClass,nError,pszErrorMsg);
            }catch(...)
                {
                raiseFromCatch("", "FMTQuietExceptionHandler::handelCPLerror", __LINE__, __FILE__);
                }
			}
	#endif

	std::unique_ptr <FMTExceptionHandler>  FMTQuietExceptionHandler::Clone() const
		{
			return std::unique_ptr<FMTExceptionHandler>(new FMTQuietExceptionHandler(*this));
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTQuietExceptionHandler)
