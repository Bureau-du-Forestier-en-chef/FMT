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


double FMToperatingarea::getarea() const
	{
	return _area;
	}


std::vector<Core::FMTmask>FMToperatingarea::getNeighbors() const
	{
	return neighbors;
	}

FMToperatingarea::FMToperatingarea(const Core::FMTmask& lmask,const double& lneighborsperimeter):
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

void FMToperatingarea::setNeighbors(const std::vector<Core::FMTmask>& lneighbors)
	{
	neighbors = lneighbors;
	}

double FMToperatingarea::getNeighborsPerimeter() const
	{
	return neighborsperimeter;
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

FMToperatingarea FMToperatingarea::presolveOperatingArea(const Core::FMTmaskfilter& filter, const std::vector<Core::FMTtheme>& presolvedthemes) const
{
	FMToperatingarea presolvedoparea(*this);
	presolvedoparea.mask = this->mask.presolve(filter, presolvedthemes);
	for(size_t i=0; i < presolvedoparea.neighbors.size(); ++i)
	{
		presolvedoparea.neighbors[i]=this->neighbors[i].presolve(filter, presolvedthemes);
	}
	return presolvedoparea;
}

FMToperatingarea FMToperatingarea::postsolveOperatingArea(const Core::FMTmaskfilter& filter, const std::vector<Core::FMTtheme>&basethemes) const
{
	FMToperatingarea postsolvedoparea(*this);
	postsolvedoparea.mask = postsolvedoparea.mask.postSolve(filter,basethemes);
	return postsolvedoparea;
}


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMToperatingarea)
#endif
