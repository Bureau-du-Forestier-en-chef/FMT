/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTquietexceptionhandler.h"
#include "FMTerror.h"
#include "FMTwarning.h"
#include "FMTcplhandler.h"

namespace Exception
{

	FMTquietexceptionhandler::FMTquietexceptionhandler() :FMTexceptionhandler() {}

	FMTexception FMTquietexceptionhandler::raise(FMTexc lexception, std::string text,
		const std::string& method, const int& line, const std::string& file, Core::FMTsection lsection, bool throwit)
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
				excp = FMTexception(lexception, updatestatus(lexception, text), method, file, line);
			}
			else {
				excp = FMTexception(lexception, lsection, updatestatus(lexception, text), method, file, line);
			}
		}
		if (throwit)
		{
			if (LEVEL == FMTlev::FMT_logic || LEVEL == FMTlev::FMT_range)
			{
				boost::lock_guard<boost::recursive_mutex> guard(mtx);
				if (!needtorethrow()) {
					std::throw_with_nested(FMTerror(excp));
				}
			}
		}
		return excp;
	}
	#ifdef FMTWITHGDAL

		FMTexceptionhandler* FMTquietexceptionhandler::getCPLdata()
			{
			return this;
			}
		void FMTquietexceptionhandler::handelCPLerror(int eErrClass,int nError, const char * pszErrorMsg)
			{
			//boost::lock_guard<boost::recursive_mutex> guard(mtx);
            try{
                FMTexceptionhandler::handelCPLerror(eErrClass,nError,pszErrorMsg);
            }catch(...)
                {
                raisefromcatch("", "FMTquietexceptionhandler::handelCPLerror", __LINE__, __FILE__);
                }
			}
	#endif

	std::unique_ptr <FMTexceptionhandler>  FMTquietexceptionhandler::Clone() const
		{
			return std::unique_ptr<FMTexceptionhandler>(new FMTquietexceptionhandler(*this));
		}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Exception::FMTquietexceptionhandler)
