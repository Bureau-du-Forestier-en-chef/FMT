/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTLifespans.h"

namespace Core{

FMTLifespans::FMTLifespans() : FMTList<int>(){}
FMTLifespans::FMTLifespans(const FMTLifespans& rhs) : FMTList<int>(rhs){}
FMTLifespans& FMTLifespans::operator = (const FMTLifespans& rhs)
    {
    if (this!=&rhs)
        {
		FMTList<int>::operator = (rhs);
        }
    return *this;
    }

void FMTLifespans::swap(FMTLifespans& rhs)
	{
	FMTList<int>::swap(rhs);
	}

void FMTLifespans::update()
	{
	try {
		FMTList<int>::update();
	}catch (...)
		{
		_exhandler->raiseFromCatch(
			"","FMTLifespans::update", __LINE__, __FILE__, Core::FMTsection::Lifespan);
		}
	}

FMTLifespans FMTLifespans::preSolve(const FMTMaskFilter& filter,
	const std::vector<FMTTheme>& originalthemes,
	std::vector<FMTTheme>& newthemes,bool compressdata) const
	{
	FMTLifespans newlifespans(*this);
	try {
		if (!empty())
		{
			newlifespans._preSolveList(filter, originalthemes, newthemes);
			if (compressdata)
			{
				newlifespans._compressMasks(newthemes);
			}
			newlifespans.update();
		}
	}catch (...)
		{
		_exhandler->raiseFromCatch("","FMTLifespans::preSolve", __LINE__, __FILE__, Core::FMTsection::Lifespan);
		}
	return newlifespans;
	}

bool FMTLifespans::operator == (const FMTLifespans& rhs) const
	{
	return (FMTList<int>::operator ==(rhs));
	}

bool FMTLifespans::operator != (const FMTLifespans& rhs) const
{
	return !(*this == rhs);
}

FMTLifespans::operator std::string() const
    {
	std::string line;
    for(const auto& lfobject : *this)
        {
        line+= std::string(lfobject.first)+" ";
        line+= std::to_string(lfobject.second);
        line+="\n";
        }
    return line;
    }
}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTLifespans)
