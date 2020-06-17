/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTGCBMTRANSITION_H_INCLUDED
#define FMTGCBMTRANSITION_H_INCLUDED

#include <map>
#include <string>


namespace Core
{
	class FMTGCBMtransition
	{
	public:
		int ageafter;
		std::map<std::string, std::string>themes;
		std::string name;
		FMTGCBMtransition();
		FMTGCBMtransition(const int& lageafter, const std::map<std::string, std::string>& lthemes, const std::string& lname);
		FMTGCBMtransition(const FMTGCBMtransition& rhs);
		FMTGCBMtransition& operator = (const FMTGCBMtransition& rhs);
		~FMTGCBMtransition()=default;
	};
}
#endif
