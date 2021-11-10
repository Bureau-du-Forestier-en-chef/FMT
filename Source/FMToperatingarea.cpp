/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMToperatingarea.hpp"
#include "CoinPackedVector.hpp"
#include "FMTmatrixbuild.hpp"

namespace Heuristics
{

FMToperatingarea& FMToperatingarea::operator += (const double& value)
	{
	this->_area += value;
	return *this;
	}


double FMToperatingarea::getarea() const
	{
	return _area;
	}


std::vector<Core::FMTmask>FMToperatingarea::getneighbors() const
	{
	return neighbors;
	}

FMToperatingarea::FMToperatingarea(const Core::FMTmask& lmask,const double& lneihgborsperimeter):
	mask(lmask),
	neighbors(),
	neihgborsperimeter(lneihgborsperimeter),
	_area()
	{

	}

void FMToperatingarea::setarea(const double& newarea)
    {
    _area = newarea;
    }

void FMToperatingarea::setneighbors(const std::vector<Core::FMTmask>& lneighbors)
	{
	neighbors = lneighbors;
	}

double FMToperatingarea::getneihgborsperimeter() const
	{
	return neihgborsperimeter;
	}

Core::FMTmask FMToperatingarea::getmask() const
	{
	return mask;
	}

bool FMToperatingarea::operator == (const FMToperatingarea& rhs) const
	{
	return (mask == rhs.mask);
	}
bool FMToperatingarea::operator != (const FMToperatingarea& rhs) const
	{
	return !(*this == rhs);
	}

FMToperatingareacomparator::FMToperatingareacomparator(const Core::FMTmask& lmask):mask(lmask)
	{

	}

FMToperatingareacomparator::FMToperatingareacomparator(const FMToperatingarea& oparea):mask(oparea.getmask())
    {

    }


bool FMToperatingareacomparator::operator()(const FMToperatingarea& oparea) const
	{
	return (oparea.getmask() == mask);
	}

FMToperatingarea FMToperatingarea::presolveoperatingarea(const Core::FMTmask& selectedmask, const std::vector<Core::FMTtheme>& presolvedthemes) const
{
	FMToperatingarea presolvedoparea(*this);
	presolvedoparea.mask = this->mask.presolve(selectedmask, presolvedthemes);
	for(size_t i=0; i < presolvedoparea.neighbors.size(); ++i)
	{
		presolvedoparea.neighbors[i]=this->neighbors[i].presolve(selectedmask, presolvedthemes);
	}
	return presolvedoparea;
}

FMToperatingarea FMToperatingarea::postsolveoperatingarea(const Core::FMTmask& selectedmask, const std::vector<Core::FMTtheme>&basethemes) const
{
	FMToperatingarea postsolvedoparea(*this);
	postsolvedoparea.mask = postsolvedoparea.mask.postsolve(selectedmask, basethemes);
	return postsolvedoparea;
}


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMToperatingarea)
#endif
