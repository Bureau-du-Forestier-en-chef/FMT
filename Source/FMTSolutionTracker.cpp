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
		_BuildConstraintsFamilies(p_ConstraintsFamilies, p_NumberOfFamilies);
	};



	void FMTSolutionTracker::_ModifySolution(
		size_t p_family,
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_It,
		double p_sense)
	{
		try {
			for (const size_t CONSTRAINT_Id : m_ConstraintsFamilies.at(p_family))
				{
					const std::vector<double>& VALUES = p_It->second.GetValues(CONSTRAINT_Id);
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
			_exhandler->raisefromcatch("",
				"FMTSolutionTracker::_ModifySolution", __LINE__, __FILE__);
			}
	}

	void  FMTSolutionTracker::SetConstraintValues(const std::vector<std::vector<double>>& p_constraintValues,
						const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
						size_t p_NumberOfFamilies)
		{
		m_LastConstraintsEvaluation = p_constraintValues;
		_BuildConstraintsFamilies(p_ConstraintsFamilies, p_NumberOfFamilies);
		}

	void FMTSolutionTracker::DisableTracking()
	{
		m_LastConstraintsEvaluation.clear();
	}

	bool FMTSolutionTracker::IsTrackingEnable() const
		{
		return !m_LastConstraintsEvaluation.empty();
		}	


	const std::vector<double>& FMTSolutionTracker::GetConstraintsValues(size_t p_Constraint)const
		{
		return m_LastConstraintsEvaluation.at(p_Constraint);
		}


	size_t FMTSolutionTracker::GetNumberOfCells(size_t p_GraphId) const
		{
		size_t numberOf = 0;
		try {
			if (p_GraphId < m_Solution.size())
				{
				numberOf = m_Solution.at(p_GraphId);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTSolutionTracker::GetNumberOfCells", __LINE__, __FILE__);
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

	size_t FMTSolutionTracker::GetNonSpatialCellsCount() const
	{
		size_t totalCount = 0;
		for (size_t CELLS : m_Solution)
			{
			totalCount += CELLS;
			}
		return totalCount;
	}

	void FMTSolutionTracker::AddToSolution(size_t p_family,
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_NewGraph,
		size_t p_LastGraphId)
	{
		try {
			if (p_LastGraphId > m_Solution.size())
				{
				m_Solution.resize(p_LastGraphId * m_SOLUTION_RESIZE_FACTOR, 0);
				}
			++m_Solution.at(p_NewGraph->second.GetGraphId());
			if (IsTrackingEnable())
				{
				_ModifySolution(p_family, p_NewGraph, 1.0);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("",
				"FMTSolutionTracker::AddToSolution", __LINE__, __FILE__);
			}
	}
	void FMTSolutionTracker::RemoveFromSolution(size_t p_family,
		std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_OldGraph)
	{
		try {
			--m_Solution.at(p_OldGraph->second.GetGraphId());
			if (IsTrackingEnable())
				{
				_ModifySolution( p_family, p_OldGraph, -1.0);
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("",
				"FMTSolutionTracker::RemoveFromSolution", __LINE__, __FILE__);
		}
	}

	void FMTSolutionTracker::_BuildConstraintsFamilies(const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
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


