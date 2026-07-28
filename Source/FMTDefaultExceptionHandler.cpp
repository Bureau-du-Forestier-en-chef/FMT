/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTDefaultExceptionHandler.h"
#include "FMTWarning.h"
#include "FMTError.h"
#include "FMTcplhandler.h"


namespace Exception

{

	FMTExceptionHandler* FMTDefaultExceptionHandler::getCPLdata()
		{
		return this;
		}

#if defined FMTWITHGDAL
	void FMTDefaultExceptionHandler::handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg)
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTExceptionHandler::handelCPLerror(eErrClass,nError,pszErrorMsg);
        }catch(...)
            {
            raiseFromCatch("", "FMTDefaultExceptionHandler::handelCPLerror", __LINE__, __FILE__);
            }
		}
#endif

	FMTDefaultExceptionHandler::FMTDefaultExceptionHandler() :FMTExceptionHandler() {}

	FMTDefaultExceptionHandler::FMTDefaultExceptionHandler(const std::unique_ptr<Logging::FMTLogger>& logger):
		FMTExceptionHandler(logger)
	{
	#if defined  FMTWITHGDAL
		Exception::FMTExceptionHandler* handler = reinterpret_cast<Exception::FMTExceptionHandler*>(CPLGetErrorHandlerUserData());
		CPLPushErrorHandlerEx(Exception::FMTCPLErrorHandler, this->getCPLdata());
	#endif
	}

	FMTException FMTDefaultExceptionHandler::raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection,bool throwit)
	{
		const FMTlev LEVEL = _getLevel(lexception);
		FMTException excp = FMTException(lexception, _updateStatus(lexception, text));
		if (lsection != Core::FMTsection::Empty)
		{
			excp = FMTException(lexception, lsection, _updateStatus(lexception, text));
		}
		if (LEVEL != FMTlev::FMT_Warning)
		{
			if (lsection == Core::FMTsection::Empty)
			{
				excp = FMTException(lexception, _updateStatus(lexception, text), method,file,line);
			}
			else {
				excp = FMTException(lexception, lsection, _updateStatus(lexception, text), method, file, line);
			}
			if (throwit && (LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range) && !_needToRethrow())
				{
				boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
				std::throw_with_nested(FMTError(excp));
				}
		}else if(throwit)
			{
			FMTWarning(excp).warn(*_logger, _specificwarningcount, m_maxwarningsbeforesilenced);
			}
		return excp;
	}


	std::unique_ptr <FMTExceptionHandler> FMTDefaultExceptionHandler::Clone() const
	{
		return std::unique_ptr<FMTExceptionHandler>(new FMTDefaultExceptionHandler(*this));
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTDefaultExceptionHandler)
