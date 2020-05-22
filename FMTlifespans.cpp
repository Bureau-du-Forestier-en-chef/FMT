/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlifespans.h"

namespace Core{

FMTlifespans::FMTlifespans() : FMTlist<int>(){}
FMTlifespans::FMTlifespans(const FMTlifespans& rhs) : FMTlist<int>(rhs){}
FMTlifespans& FMTlifespans::operator = (const FMTlifespans& rhs)
    {
    if (this!=&rhs)
        {
		FMTlist<int>::operator = (rhs);
        }
    return *this;
    }

void FMTlifespans::update()
	{
	FMTlist<int>::update();
	}

FMTlifespans FMTlifespans::presolve(const FMTmask& basemask,
	const std::vector<FMTtheme>& originalthemes,
	const FMTmask& presolvedmask,
	const std::vector<FMTtheme>& newthemes) const
	{
	FMTlifespans newlifespans(*this);
	newlifespans.presolvelist(basemask, originalthemes, presolvedmask, newthemes);
	newlifespans.update();
	return newlifespans;
	}

bool FMTlifespans::operator == (const FMTlifespans& rhs) const
	{
	return (FMTlist<int>::operator ==(rhs));
	}

FMTlifespans::operator std::string() const
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

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTlifespans)
