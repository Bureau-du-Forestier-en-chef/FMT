/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTDebugExceptionHandler.h"
#include "FMTLogger.h"
#include "FMTWarning.h"
#include "FMTError.h"
#include "FMTcplhandler.h"


namespace Exception
{


	FMTDebugExceptionHandler::FMTDebugExceptionHandler()
	{

	}


	FMTException FMTDebugExceptionHandler::raise(FMTexc lexception, std::string text,
		const std::string& method,const int& line, const std::string& file, Core::FMTsection lsection, bool throwit)
	{
		
		std::unique_ptr<FMTException> newException = _createException(lexception, lsection,
			text, method, file, line);
		_updateStatus(newException);

		if (throwit)
			{
			if (newException->getLevel() == FMTlev::FMT_Warning || 
				newException->getLevel() == FMTlev::FMT_Debug)
				{
				const FMTWarning* WARNING = dynamic_cast<const FMTWarning*>(newException.get());
				_updateWarningCount(*WARNING);
				}else if(newException->isFatal())
				{
					boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
					std::throw_with_nested(*newException);
				}

			}
		return *newException;
	}
#ifdef FMTWITHGDAL

	FMTExceptionHandler* FMTDebugExceptionHandler::getCplData()
		{
		return this;
		}
	void FMTDebugExceptionHandler::handelCplError(int eErrClass, int nError, const char * pszErrorMsg)
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTExceptionHandler::handelCplError(eErrClass, nError, pszErrorMsg);
        }catch(...)
            {
            raiseFromCatch("", "FMTDebugExceptionHandler::handelCplError", __LINE__, __FILE__);
            }

		}
#endif

	std::unique_ptr <FMTExceptionHandler> FMTDebugExceptionHandler::clone() const
	{
		return std::unique_ptr <FMTExceptionHandler>(new FMTDebugExceptionHandler(*this));
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTDebugExceptionHandler)
