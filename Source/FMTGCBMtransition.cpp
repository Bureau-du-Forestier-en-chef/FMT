/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTGCBMtransition.h"

namespace Core
{
FMTGCBMTransition::FMTGCBMTransition():ageafter(), themes(),name()
	{

	}

FMTGCBMTransition::FMTGCBMTransition(const int& lageafter, const std::map<std::string, std::string>& lthemes, const std::string& lname) :
	ageafter(lageafter), themes(lthemes), name(lname)
	{

	}

FMTGCBMTransition::FMTGCBMTransition(const FMTGCBMTransition& rhs):ageafter(rhs.ageafter), themes(rhs.themes), name(rhs.name)
	{

	}
FMTGCBMTransition& FMTGCBMTransition::operator = (const FMTGCBMTransition& rhs)
	{
	if (this != &rhs)
		{
		ageafter = rhs.ageafter;
		themes = rhs.themes;
		name = rhs.name;
		}
	return *this;
	}


bool FMTGCBMTransition::operator == (const FMTGCBMTransition& rhs) const
{
	return (ageafter == rhs.ageafter && name == rhs.name && themes == rhs.themes);
}

bool FMTGCBMTransition::operator != (const FMTGCBMTransition& rhs) const
{
	return !(*this == rhs);

}

}


