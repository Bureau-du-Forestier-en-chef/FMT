/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTGCBMtransition.hpp"

namespace Core
{
FMTGCBMtransition::FMTGCBMtransition():ageafter(), themes(),name()
	{

	}

FMTGCBMtransition::FMTGCBMtransition(const int& lageafter, const std::map<std::string, std::string>& lthemes, const std::string& lname) :
	ageafter(lageafter), themes(lthemes), name(lname)
	{

	}

FMTGCBMtransition::FMTGCBMtransition(const FMTGCBMtransition& rhs):ageafter(rhs.ageafter), themes(rhs.themes), name(rhs.name)
	{

	}
FMTGCBMtransition& FMTGCBMtransition::operator = (const FMTGCBMtransition& rhs)
	{
	if (this != &rhs)
		{
		ageafter = rhs.ageafter;
		themes = rhs.themes;
		name = rhs.name;
		}
	return *this;
	}


bool FMTGCBMtransition::operator == (const FMTGCBMtransition& rhs) const
{
	return (ageafter == rhs.ageafter && name == rhs.name && themes == rhs.themes);
}

bool FMTGCBMtransition::operator != (const FMTGCBMtransition& rhs) const
{
	return !(*this == rhs);

}

}


