/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTWarning.h"
#include "FMTDefaultLogger.h"
#include "FMTLogger.h"
#include <unordered_map>


namespace Exception
{
FMTWarning::FMTWarning() : FMTException() {}

FMTWarning::FMTWarning(FMTexc p_exception, Core::FMTsection p_section, const std::string& p_message,
	const std::string& p_method, const std::string& p_file, const int& p_line,
	const std::string& p_FrenchDescription, const std::string& p_EnglishDescription):
	FMTException(p_exception,FMTlev::FMT_Warning,p_section,p_message,
		p_method,p_file,p_line,p_FrenchDescription,p_EnglishDescription)
{

}



void FMTWarning::warn(Logging::FMTLogger& logger,std::unordered_map<int,size_t>& specificwarningcount, const size_t& maxwarning) const
	{
		std::unordered_map<int,size_t>::iterator spwit = specificwarningcount.find(getType());
		if(spwit!=specificwarningcount.end())
		{
			size_t wcount = spwit->second;
			if(wcount<=maxwarning)
			{
				logger << getMessage() << "\n";
				wcount+=1;
			}
			else if(wcount==maxwarning+1)
			{
				logger << "FMTexc("+ std::to_string(static_cast<int>(getType())) +") has reached the maximum number of times it can be raised."<< "\n";
				wcount+=1;
			}
			spwit->second=wcount;
		}else{
			logger << getMessage() << "\n";
			specificwarningcount[getType()]=1;
		}
	}
}
