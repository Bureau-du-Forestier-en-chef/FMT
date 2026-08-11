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

	FMTExceptionHandler* FMTDefaultExceptionHandler::getCplData()
		{
		return this;
		}

#if defined FMTWITHGDAL
	void FMTDefaultExceptionHandler::handelCplError(int eErrClass, int nError, const char * pszErrorMsg)
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTExceptionHandler::handelCplError(eErrClass,nError,pszErrorMsg);
        }catch(...)
            {
            raiseFromCatch("", "FMTDefaultExceptionHandler::handelCplError", __LINE__, __FILE__);
            }
		}
#endif

	FMTDefaultExceptionHandler::FMTDefaultExceptionHandler() :FMTExceptionHandler() {}

	FMTDefaultExceptionHandler::FMTDefaultExceptionHandler(const std::unique_ptr<Logging::FMTLogger>& logger):
		FMTExceptionHandler(logger)
	{
	#if defined  FMTWITHGDAL
		Exception::FMTExceptionHandler* handler = reinterpret_cast<Exception::FMTExceptionHandler*>(CPLGetErrorHandlerUserData());
		CPLPushErrorHandlerEx(Exception::FMTCPLErrorHandler, this->getCplData());
	#endif
	}

	FMTException FMTDefaultExceptionHandler::raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection,bool throwit)
	{
		std::unique_ptr<FMTException> newException = _createException(lexception, lsection,
			text, method, file, line);
		_updateStatus(newException);
		if (newException->getLevel() != FMTlev::FMT_Warning)
		{
			if (throwit && (newException->isFatal()) && !_needToRethrow())
				{
				boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
				std::throw_with_nested(*newException);
				}
		}else if(throwit)
			{
			const FMTWarning* WARNING = dynamic_cast<const FMTWarning*>(newException.get());
			_updateWarningCount(*WARNING);
			}
		return *newException;
	}


	std::unique_ptr <FMTExceptionHandler> FMTDefaultExceptionHandler::clone() const
	{
		return std::unique_ptr<FMTExceptionHandler>(new FMTDefaultExceptionHandler(*this));
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTDefaultExceptionHandler)
