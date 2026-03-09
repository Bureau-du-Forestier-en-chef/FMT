/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTSOLUTIONTRACKER_H
#define FMTSOLUTIONTRACKER_H


#include <map>
#include <vector>
#include <string>
#include <memory>
#include "FMTobject.h"

namespace Graph
{
	class FMTlinegraph;
}

namespace Core
{
	class FMTconstraint;
}


namespace Spatial
	{
	class FMTGraphInfo;
	class FMTSolutionTracker: public Core::FMTobject
		{
		public:
			FMTSolutionTracker(const std::vector<size_t>& p_solution,
				const std::vector<std::vector<size_t>>&p_ConstraintsFamilies,
				size_t p_NumberOfFamilies);
			FMTSolutionTracker();
			FMTSolutionTracker(const FMTSolutionTracker& p_rhs) = default;
			FMTSolutionTracker& operator=(const FMTSolutionTracker& p_rhs) = default;
			~FMTSolutionTracker() = default;
			void AddToSolution(size_t p_family,
				std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_NewGraph,
				size_t p_LastGraphId);
			void RemoveFromSolution(size_t p_family,
				std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_OldGraph);
			const std::vector<double>& GetConstraintsValues(size_t p_Constraint)const;
			size_t GetNumberOfCells(size_t p_GraphId) const;
			size_t size() const;
			void swap(FMTSolutionTracker& p_tracker);
			size_t GetNonSpatialCellsCount() const;
			void SetConstraintValues(const std::vector<std::vector<double>>& p_constraintValues,
								const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
									size_t p_NumberOfFamilies);
			bool EnableTracking();
			void DisableTracking();
			bool IsTrackingEnable() const;
		private:
			std::vector<std::vector<double>>m_LastConstraintsEvaluation;
			std::vector<size_t>m_Solution;
			std::vector<std::vector<size_t>>m_ConstraintsFamilies;
			const static size_t m_SOLUTION_RESIZE_FACTOR = 2;
			void _ModifySolution(size_t p_family,
								std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_It,
								double p_sense);
			void _BuildConstraintsFamilies(const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
											size_t p_NumberOfFamilies);
		};
}


#endif 
