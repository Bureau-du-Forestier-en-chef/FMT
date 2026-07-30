/*
Copyright (c) 2019 Gouvernement du Qu�bec

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
	this->m_area += value;
	return *this;
	}


double FMTOperatingArea::getArea() const
	{
	return m_area;
	}


std::vector<Core::FMTMask>FMTOperatingArea::getNeighbors() const
	{
	return m_neighbors;
	}

FMTOperatingArea::FMTOperatingArea(const Core::FMTMask& lmask,const double& lneighborsperimeter):
	m_mask(lmask),
	m_neighbors(),
	m_neighborsperimeter(lneighborsperimeter),
	m_area()
	{

	}

void FMTOperatingArea::setArea(const double& newarea)
    {
    m_area = newarea;
    }

void FMTOperatingArea::setNeighbors(const std::vector<Core::FMTMask>& lneighbors)
	{
	m_neighbors = lneighbors;
	}

double FMTOperatingArea::getNeighborsPerimeter() const
	{
	return m_neighborsperimeter;
	}

Core::FMTMask FMTOperatingArea::getMask() const
	{
	return m_mask;
	}

bool FMTOperatingArea::operator == (const FMTOperatingArea& rhs) const
	{
	return (m_mask == rhs.m_mask);
	}
bool FMTOperatingArea::operator != (const FMTOperatingArea& rhs) const
	{
	return !(*this == rhs);
	}

FMTOperatingAreaComparator::FMTOperatingAreaComparator(const Core::FMTMask& lmask):m_mask(lmask)
	{

	}

FMTOperatingAreaComparator::FMTOperatingAreaComparator(const FMTOperatingArea& oparea):m_mask(oparea.getMask())
    {

    }


bool FMTOperatingAreaComparator::operator()(const FMTOperatingArea& oparea) const
	{
	return (oparea.getMask() == m_mask);
	}

FMTOperatingArea FMTOperatingArea::preSolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>& presolvedthemes) const
{
	FMTOperatingArea presolvedoparea(*this);
	presolvedoparea.m_mask = this->m_mask.preSolve(filter, presolvedthemes);
	for(size_t i=0; i < presolvedoparea.m_neighbors.size(); ++i)
	{
		presolvedoparea.m_neighbors[i]=this->m_neighbors[i].preSolve(filter, presolvedthemes);
	}
	return presolvedoparea;
}

FMTOperatingArea FMTOperatingArea::postSolveOperatingArea(const Core::FMTMaskFilter& filter, const std::vector<Core::FMTTheme>&basethemes) const
{
	FMTOperatingArea postsolvedoparea(*this);
	postsolvedoparea.m_mask = postsolvedoparea.m_mask.postSolve(filter,basethemes);
	return postsolvedoparea;
}


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTOperatingArea)
#endif
