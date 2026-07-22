/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
#include "FMTOperatingArea.h"
#include "CoinPackedVector.hpp"
#include "FMTMatrixBuild.h"

namespace Heuristics
{

FMTOperatingArea& FMTOperatingArea::operator += (const double& value)
	{
	this->_area += value;
	return *this;
	}


double FMTOperatingArea::getArea() const
	{
	return _area;
	}


std::vector<Core::FMTMask>FMTOperatingArea::getNeighbors() const
	{
	return neighbors;
	}

FMTOperatingArea::FMTOperatingArea(const Core::FMTMask& lmask,const double& lneighborsperimeter):
	mask(lmask),
	neighbors(),
	neighborsperimeter(lneighborsperimeter),
	_area()
	{

	}

void FMTOperatingArea::setArea(const double& newarea)
    {
    _area = newarea;
    }

void FMTOperatingArea::setNeighbors(const std::vector<Core::FMTMask>& lneighbors)
	{
	neighbors = lneighbors;
	}

double FMTOperatingArea::getNeighborsPerimeter() const
	{
	return neighborsperimeter;
	}

Core::FMTMask FMTOperatingArea::getMask() const
	{
	return mask;
	}

bool FMTOperatingArea::operator == (const FMTOperatingArea& rhs) const
	{
	return (mask == rhs.mask);
	}
bool FMTOperatingArea::operator != (const FMTOperatingArea& rhs) const
	{
	return !(*this == rhs);
	}

FMTOperatingAreaComparator::FMTOperatingAreaComparator(const Core::FMTMask& lmask):mask(lmask)
	{

	}

FMTOperatingAreaComparator::FMTOperatingAreaComparator(const FMTOperatingArea& oparea):mask(oparea.getMask())
    {

    }


bool FMTOperatingAreaComparator::operator()(const FMTOperatingArea& oparea) const
	{
	return (oparea.getMask() == mask);
	}

FMTOperatingArea FMTOperatingArea::presolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>& presolvedthemes) const
{
	FMTOperatingArea presolvedoparea(*this);
	presolvedoparea.mask = this->mask.presolve(filter, presolvedthemes);
	for(size_t i=0; i < presolvedoparea.neighbors.size(); ++i)
	{
		presolvedoparea.neighbors[i]=this->neighbors[i].presolve(filter, presolvedthemes);
	}
	return presolvedoparea;
}

FMTOperatingArea FMTOperatingArea::postsolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&basethemes) const
{
	FMTOperatingArea postsolvedoparea(*this);
	postsolvedoparea.mask = postsolvedoparea.mask.postSolve(filter,basethemes);
	return postsolvedoparea;
}


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTOperatingArea)
#endif
