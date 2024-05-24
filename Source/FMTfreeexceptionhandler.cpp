/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTfreeexceptionhandler.h"
#include "FMTwarning.h"
#include "FMTerror.h"
#include "FMTcplhandler.h"

#if defined FMTWITHR
	#include <Rcpp.h>
#endif

namespace Exception
{

#ifdef FMTWITHGDAL
	FMTexceptionhandler* FMTfreeexceptionhandler::getCPLdata()
		{
		return this;
		}
	void FMTfreeexceptionhandler::handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg)
		{
		//boost::lock_guard<boost::recursive_mutex> guard(mtx);
        try{
            FMTexceptionhandler::handelCPLerror(eErrClass,nError,pszErrorMsg);
            }catch(...)
                {
                raisefromcatch("", "FMTfreeexceptionhandler::handelCPLerror", __LINE__, __FILE__);
                }
		}
#endif

FMTfreeexceptionhandler::FMTfreeexceptionhandler()
{
	this->disablenestedexceptions();
}


FMTexception FMTfreeexceptionhandler::raise(FMTexc lexception, std::string text,
	const std::string& method,const int& line, const std::string& file, Core::FMTsection lsection,bool throwit)
{
	
	const FMTlev LEVEL = getLevel(lexception);
	FMTexception excp = FMTexception(lexception, updatestatus(lexception, text));
	if (LEVEL != FMTlev::FMT_Warning)
	{
		if (lsection == Core::FMTsection::Empty)
		{
			excp = FMTexception(lexception, updatestatus(lexception, text), method, file, line);
		}
		else {
			excp = FMTexception(lexception, lsection, updatestatus(lexception, text), method, file, line);
		}
	}
	excp.sethold(true);
	if (throwit)
	{
		if (LEVEL == FMTlev::FMT_Warning)
		{
		}
		else if (LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range && !needtorethrow())
		{
		boost::lock_guard<boost::recursive_mutex> guard(mtx);
		#if defined FMTWITHR
					throw(Rcpp::exception(excp.what()));
		#else
					throw FMTerror(excp);
		#endif
		}
	}

	return excp;
}


std::unique_ptr <FMTexceptionhandler> FMTfreeexceptionhandler::Clone() const
{
	return std::unique_ptr <FMTexceptionhandler>(new FMTfreeexceptionhandler(*this));
}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTfreeexceptionhandler)
