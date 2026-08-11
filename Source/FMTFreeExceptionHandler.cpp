/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTFreeExceptionHandler.h"
#include "FMTWarning.h"
#include "FMTError.h"
#include "FMTcplhandler.h"

#if defined FMTWITHR
	#include <Rcpp.h>
#endif

namespace Exception
{

#ifdef FMTWITHGDAL
	FMTExceptionHandler* FMTFreeExceptionHandler::getCplData()
		{
		return this;
		}
	void FMTFreeExceptionHandler::handelCplError(int eErrClass, int nError, const char * pszErrorMsg)
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTExceptionHandler::handelCplError(eErrClass,nError,pszErrorMsg);
            }catch(...)
                {
                raiseFromCatch("", "FMTFreeExceptionHandler::handelCplError", __LINE__, __FILE__);
                }
		}
#endif

FMTFreeExceptionHandler::FMTFreeExceptionHandler()
{
	this->disableNestedExceptions();
}


FMTException FMTFreeExceptionHandler::raise(FMTexc lexception, std::string text,
	const std::string& method,const int& line, const std::string& file, Core::FMTsection lsection,bool throwit)
{

	std::unique_ptr<FMTException> newException = _createException(lexception, lsection,
		text, method, file, line);
	_updateStatus(newException);
	newException->setHold(true);
	if (throwit)
	{
		if (newException->getLevel() == FMTlev::FMT_Warning)
		{

		}else if (newException->isFatal() && !_needToRethrow())
			{
			boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
			#if defined FMTWITHR
						throw(Rcpp::exception(newException->what()));
			#else
						throw *newException;
			#endif
			}
	}

	return *newException;
}


std::unique_ptr <FMTExceptionHandler> FMTFreeExceptionHandler::clone() const
{
	return std::unique_ptr <FMTExceptionHandler>(new FMTFreeExceptionHandler(*this));
}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTFreeExceptionHandler)
