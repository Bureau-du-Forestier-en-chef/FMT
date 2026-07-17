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
	class FMTLineGraph;
}

namespace Core
{
	class FMTConstraint;
}


namespace Spatial
	{
	class FMTGraphInfo;
	class FMTSolutionTracker: public Core::FMTObject
		{
		public:
			FMTSolutionTracker(const std::vector<size_t>& p_solution,
				const std::vector<std::vector<size_t>>&p_ConstraintsFamilies,
				size_t p_NumberOfFamilies);
			FMTSolutionTracker();
			FMTSolutionTracker(const FMTSolutionTracker& p_rhs) = default;
			FMTSolutionTracker& operator=(const FMTSolutionTracker& p_rhs) = default;
			~FMTSolutionTracker() = default;
			void addToSolution(size_t p_family,
				std::map<Graph::FMTLineGraph, FMTGraphInfo>::const_iterator p_NewGraph,
				size_t p_LastGraphId);
			void removeFromSolution(size_t p_family,
				std::map<Graph::FMTLineGraph, FMTGraphInfo>::const_iterator p_OldGraph);
			const std::vector<double>& getConstraintsValues(size_t p_Constraint)const;
			size_t getNumberOfCells(size_t p_GraphId) const;
			size_t size() const;
			void swap(FMTSolutionTracker& p_tracker);
			size_t getNonSpatialCellsCount() const;
			void setConstraintValues(const std::vector<std::vector<double>>& p_constraintValues,
								const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
									size_t p_NumberOfFamilies);
			bool enableTracking();
			void disableTracking();
			bool isTrackingEnable() const;
		private:
			std::vector<std::vector<double>>m_LastConstraintsEvaluation;
			std::vector<size_t>m_Solution;
			std::vector<std::vector<size_t>>m_ConstraintsFamilies;
			const static size_t m_SOLUTION_RESIZE_FACTOR = 2;
			void _modifySolution(size_t p_family,
								std::map<Graph::FMTLineGraph, FMTGraphInfo>::const_iterator p_It,
								double p_sense);
			void _buildConstraintsFamilies(const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
											size_t p_NumberOfFamilies);
		};
}


#endif 
