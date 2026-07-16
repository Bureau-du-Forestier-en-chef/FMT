/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMToperatingarea.h"
#include "CoinPackedVector.hpp"
#include "FMTmatrixbuild.h"

namespace Heuristics
{

FMToperatingarea& FMToperatingarea::operator += (const double& value)
	{
	this->_area += value;
	return *this;
	}


double FMToperatingarea::getArea() const
	{
	return _area;
	}


std::vector<Core::FMTMask>FMToperatingarea::getNeighbors() const
	{
	return neighbors;
	}

FMToperatingarea::FMToperatingarea(const Core::FMTMask& lmask,const double& lneighborsperimeter):
	mask(lmask),
	neighbors(),
	neighborsperimeter(lneighborsperimeter),
	_area()
	{

	}

void FMToperatingarea::setArea(const double& newarea)
    {
    _area = newarea;
    }

void FMToperatingarea::setNeighbors(const std::vector<Core::FMTMask>& lneighbors)
	{
	neighbors = lneighbors;
	}

double FMToperatingarea::getNeighborsPerimeter() const
	{
	return neighborsperimeter;
	}

Core::FMTMask FMToperatingarea::getMask() const
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

FMTOperatingAreaComparator::FMTOperatingAreaComparator(const Core::FMTMask& lmask):mask(lmask)
	{

	}

FMTOperatingAreaComparator::FMTOperatingAreaComparator(const FMToperatingarea& oparea):mask(oparea.getMask())
    {

    }


bool FMTOperatingAreaComparator::operator()(const FMToperatingarea& oparea) const
	{
	return (oparea.getMask() == mask);
	}

FMToperatingarea FMToperatingarea::presolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>& presolvedthemes) const
{
	FMToperatingarea presolvedoparea(*this);
	presolvedoparea.mask = this->mask.presolve(filter, presolvedthemes);
	for(size_t i=0; i < presolvedoparea.neighbors.size(); ++i)
	{
		presolvedoparea.neighbors[i]=this->neighbors[i].presolve(filter, presolvedthemes);
	}
	return presolvedoparea;
}

FMToperatingarea FMToperatingarea::postsolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&basethemes) const
{
	FMToperatingarea postsolvedoparea(*this);
	postsolvedoparea.mask = postsolvedoparea.mask.postSolve(filter,basethemes);
	return postsolvedoparea;
}


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMToperatingarea)
#endif
