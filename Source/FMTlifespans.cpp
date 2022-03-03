/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlifespans.hpp"

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

void FMTlifespans::swap(FMTlifespans& rhs)
	{
	FMTlist<int>::swap(rhs);
	}

void FMTlifespans::update()
	{
	try {
		FMTlist<int>::update();
	}catch (...)
		{
		_exhandler->raisefromcatch(
			"","FMTlifespans::update", __LINE__, __FILE__, Core::FMTsection::Lifespan);
		}
	}

FMTlifespans FMTlifespans::presolve(const FMTmaskfilter& filter,
	const std::vector<FMTtheme>& originalthemes,
	std::vector<FMTtheme>& newthemes,bool compressdata) const
	{
	FMTlifespans newlifespans(*this);
	try {
		newlifespans.presolvelist(filter, originalthemes,newthemes);
		if (compressdata)
			{
			newlifespans.compressmasks(newthemes);
			}
		newlifespans.update();
	}catch (...)
		{
		_exhandler->raisefromcatch("","FMTlifespans::presolve", __LINE__, __FILE__, Core::FMTsection::Lifespan);
		}
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
