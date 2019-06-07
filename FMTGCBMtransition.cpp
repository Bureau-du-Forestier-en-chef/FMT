#include "FMTGCBMtransition.h"

namespace Core
{
FMTGCBMtransition::FMTGCBMtransition():ageafter(), themes(),name()
	{

	}

FMTGCBMtransition::FMTGCBMtransition(const int& lageafter, const map<string, string>& lthemes, const string& lname) : 
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

}


