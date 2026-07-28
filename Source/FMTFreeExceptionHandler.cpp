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
	FMTExceptionHandler* FMTFreeExceptionHandler::getCPLdata()
		{
		return this;
		}
	void FMTFreeExceptionHandler::handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg)
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTExceptionHandler::handelCPLerror(eErrClass,nError,pszErrorMsg);
            }catch(...)
                {
                raiseFromCatch("", "FMTFreeExceptionHandler::handelCPLerror", __LINE__, __FILE__);
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
	
	const FMTlev LEVEL = _getLevel(lexception);
	FMTException excp = FMTException(lexception, _updateStatus(lexception, text));
	if (LEVEL != FMTlev::FMT_Warning)
	{
		if (lsection == Core::FMTsection::Empty)
		{
			excp = FMTException(lexception, _updateStatus(lexception, text), method, file, line);
		}
		else {
			excp = FMTException(lexception, lsection, _updateStatus(lexception, text), method, file, line);
		}
	}
	excp.setHold(true);
	if (throwit)
	{
		if (LEVEL == FMTlev::FMT_Warning)
		{
		}
		else if (LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range && !_needToRethrow())
		{
		boost::lock_guard<boost::recursive_mutex> guard(m_mtx);
		#if defined FMTWITHR
					throw(Rcpp::exception(excp.what()));
		#else
					throw FMTError(excp);
		#endif
		}
	}

	return excp;
}


std::unique_ptr <FMTExceptionHandler> FMTFreeExceptionHandler::Clone() const
{
	return std::unique_ptr <FMTExceptionHandler>(new FMTFreeExceptionHandler(*this));
}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTFreeExceptionHandler)
