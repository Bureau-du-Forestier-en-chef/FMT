/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#include <vector>
#include "FMTSolutionTracker.h"
#include "FMTGraphInfo.h"
#include "FMTlinegraph.h"


namespace Spatial
{

	FMTSolutionTracker::FMTSolutionTracker():
		m_LastConstraintsEvaluation(),
		m_Solution(),
		m_ConstraintsFamilies()
	{

	}

	FMTSolutionTracker::FMTSolutionTracker(const std::vector<size_t>& p_solution,
		const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
		size_t p_NumberOfFamilies):
		m_LastConstraintsEvaluation(),
		m_Solution(p_solution),
		m_ConstraintsFamilies()
	{
		_buildConstraintsFamilies(p_ConstraintsFamilies, p_NumberOfFamilies);
	};



	void FMTSolutionTracker::_modifySolution(
		size_t p_family,
		std::map<Graph::FMTLineGraph, FMTGraphInfo>::const_iterator p_It,
		double p_sense)
	{
		try {
			for (const size_t CONSTRAINT_Id : m_ConstraintsFamilies.at(p_family))
				{
					const std::vector<double>& VALUES = p_It->second.getValues(CONSTRAINT_Id);
					for (size_t j = 0; j < VALUES.size(); ++j)
					{
						if (j == m_LastConstraintsEvaluation.at(CONSTRAINT_Id).size())
						{
							m_LastConstraintsEvaluation.at(CONSTRAINT_Id).push_back(VALUES.at(j) * p_sense);
						}
						else {
							m_LastConstraintsEvaluation.at(CONSTRAINT_Id).at(j) += (VALUES.at(j) * p_sense);
						}
					}
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTSolutionTracker::_modifySolution", __LINE__, __FILE__);
			}
	}

	void  FMTSolutionTracker::setConstraintValues(const std::vector<std::vector<double>>& p_constraintValues,
						const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
						size_t p_NumberOfFamilies)
		{
		m_LastConstraintsEvaluation = p_constraintValues;
		_buildConstraintsFamilies(p_ConstraintsFamilies, p_NumberOfFamilies);
		}

	void FMTSolutionTracker::disableTracking()
	{
		m_LastConstraintsEvaluation.clear();
	}

	bool FMTSolutionTracker::isTrackingEnable() const
		{
		return !m_LastConstraintsEvaluation.empty();
		}	


	const std::vector<double>& FMTSolutionTracker::getConstraintsValues(size_t p_Constraint)const
		{
		return m_LastConstraintsEvaluation.at(p_Constraint);
		}


	size_t FMTSolutionTracker::getNumberOfCells(size_t p_GraphId) const
		{
		size_t numberOf = 0;
		try {
			if (p_GraphId < m_Solution.size())
				{
				numberOf = m_Solution.at(p_GraphId);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTSolutionTracker::getNumberOfCells", __LINE__, __FILE__);
			}
		return numberOf;
		}

	size_t FMTSolutionTracker::size() const
		{
		return m_Solution.size();
		}

	void FMTSolutionTracker::swap(FMTSolutionTracker& p_tracker)
		{
		m_LastConstraintsEvaluation.swap(p_tracker.m_LastConstraintsEvaluation);
		m_Solution.swap(p_tracker.m_Solution);
		m_ConstraintsFamilies.swap(p_tracker.m_ConstraintsFamilies);
		}

	size_t FMTSolutionTracker::getNonSpatialCellsCount() const
	{
		size_t totalCount = 0;
		for (size_t CELLS : m_Solution)
			{
			totalCount += CELLS;
			}
		return totalCount;
	}

	void FMTSolutionTracker::addToSolution(size_t p_family,
		std::map<Graph::FMTLineGraph, FMTGraphInfo>::const_iterator p_NewGraph,
		size_t p_LastGraphId)
	{
		try {
			if (p_LastGraphId > m_Solution.size())
				{
				m_Solution.resize(p_LastGraphId * m_SOLUTION_RESIZE_FACTOR, 0);
				}
			++m_Solution.at(p_NewGraph->second.getGraphId());
			if (isTrackingEnable())
				{
				_modifySolution(p_family, p_NewGraph, 1.0);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("",
				"FMTSolutionTracker::addToSolution", __LINE__, __FILE__);
			}
	}
	void FMTSolutionTracker::removeFromSolution(size_t p_family,
		std::map<Graph::FMTLineGraph, FMTGraphInfo>::const_iterator p_OldGraph)
	{
		try {
			--m_Solution.at(p_OldGraph->second.getGraphId());
			if (isTrackingEnable())
				{
				_modifySolution( p_family, p_OldGraph, -1.0);
				}
		}catch (...)
		{
			_exhandler->raiseFromCatch("",
				"FMTSolutionTracker::removeFromSolution", __LINE__, __FILE__);
		}
	}

	void FMTSolutionTracker::_buildConstraintsFamilies(const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
														size_t p_NumberOfFamilies)
		{
		m_ConstraintsFamilies.clear();
		m_ConstraintsFamilies.resize(p_NumberOfFamilies);
		size_t constraintId = 0;
		for (const std::vector<size_t>& FAMILIES : p_ConstraintsFamilies)
			{
			for (const size_t& FAMILY : FAMILIES)
				{
				m_ConstraintsFamilies.at(FAMILY).push_back(constraintId);
				}
			++constraintId;
			}
		}
	

}


