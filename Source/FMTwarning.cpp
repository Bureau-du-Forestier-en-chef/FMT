/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTwarning.hpp"
#include "FMTdefaultlogger.hpp"
#include "FMTlogger.hpp"


namespace Exception
{
FMTwarning::FMTwarning() : FMTexception() {}
FMTwarning::FMTwarning(const FMTexception& rhs) : FMTexception(rhs) {}
FMTwarning::FMTwarning(const FMTexc lexception, const std::string message) : FMTexception(lexception, message) {}
FMTwarning::FMTwarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message) : FMTexception(lexception, lsection, message) {}

FMTwarning::FMTwarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message,
	const std::string& lmethod, const std::string& lfile, const int& lline):
	FMTexception(lexception, lsection, message, lmethod, lfile, lline) {}

void FMTwarning::warn(const std::shared_ptr<Logging::FMTlogger>logger,boost::unordered_map<int,size_t>& specificwarningcount, const size_t& maxwarning) const
	{
		boost::unordered_map<int,size_t>::iterator spwit = specificwarningcount.find(exceptiontype);
		if(spwit!=specificwarningcount.end())
		{
			size_t wcount = spwit->second;
			if(wcount<=maxwarning)
			{
				*logger << _msg << "\n";
				wcount+=1;
			}
			else if(wcount==maxwarning+1)
			{
				*logger << "FMTexc("+ std::to_string(exceptiontype) +") has reached the maximum number of times it can be raise."<< "\n";
				wcount+=1;
			}
			spwit->second=wcount;
		}else{
			*logger << _msg << "\n";
			specificwarningcount[exceptiontype]=1;
		}
	}
}
