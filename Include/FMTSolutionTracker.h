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
#include "FMTObject.h"

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
	// DocString: FMTSolutionTracker
	/**
	@brief Track the constraint values of a spatial solution as line graphs are added to or removed from the solution, per constraint family.
	*/
	class FMTSolutionTracker: public Core::FMTObject
		{
		public:
			// DocString: FMTSolutionTracker(const std::vector<size_t>&,const std::vector<std::vector<size_t>>&,size_t)
			/**
			@brief Construct a FMTSolutionTracker from a solution and its constraint families.
			@param[in] p_solution the solution.
			@param[in] p_ConstraintsFamilies the constraint families.
			@param[in] p_NumberOfFamilies the number of families.
			*/
			FMTSolutionTracker(const std::vector<size_t>& p_solution,
				const std::vector<std::vector<size_t>>&p_ConstraintsFamilies,
				size_t p_NumberOfFamilies);
			// DocString: FMTSolutionTracker()
			/**
			@brief Default constructor for FMTSolutionTracker.
			*/
			FMTSolutionTracker();
			// DocString: FMTSolutionTracker(const FMTSolutionTracker)
			/**
			@brief Default copy constructor for FMTSolutionTracker.
			@param[in] p_rhs the FMTSolutionTracker to copy.
			*/
			FMTSolutionTracker(const FMTSolutionTracker& p_rhs) = default;
			// DocString: FMTSolutionTracker::operator=
			/**
			@brief Default copy assignment operator for FMTSolutionTracker.
			@param[in] p_rhs the FMTSolutionTracker to copy.
			@return a reference to this FMTSolutionTracker.
			*/
			FMTSolutionTracker& operator=(const FMTSolutionTracker& p_rhs) = default;
			// DocString: ~FMTSolutionTracker()
			/**
			@brief Default destructor for FMTSolutionTracker.
			*/
			~FMTSolutionTracker() = default;
			// DocString: FMTSolutionTracker::addToSolution
			/**
			@brief Add a line graph to the solution for a constraint family, updating the tracked constraint values.
			@param[in] p_family the constraint family.
			@param[in] p_NewGraph the new graph.
			@param[in] p_LastGraphId the last graph id.
			*/
			void addToSolution(size_t p_family,
				std::map<Graph::FMTLineGraph, FMTGraphInfo>::const_iterator p_NewGraph,
				size_t p_LastGraphId);
			// DocString: FMTSolutionTracker::removeFromSolution
			/**
			@brief Remove a line graph from the solution for a constraint family, updating the tracked constraint values.
			@param[in] p_family the constraint family.
			@param[in] p_OldGraph the graph to remove.
			*/
			void removeFromSolution(size_t p_family,
				std::map<Graph::FMTLineGraph, FMTGraphInfo>::const_iterator p_OldGraph);
			// DocString: FMTSolutionTracker::getConstraintsValues
			/**
			@brief Return the tracked values of a constraint.
			@param[in] p_Constraint the constraint index.
			@return the constraint values.
			*/
			const std::vector<double>& getConstraintsValues(size_t p_Constraint)const;
			// DocString: FMTSolutionTracker::getNumberOfCells
			/**
			@brief Return the number of cells of a graph.
			@param[in] p_GraphId the graph id.
			@return the number of cells.
			*/
			size_t getNumberOfCells(size_t p_GraphId) const;
			// DocString: FMTSolutionTracker::size
			/**
			@brief Return the number of graphs in the solution.
			@return the size of the solution.
			*/
			size_t size() const;
			// DocString: FMTSolutionTracker::swap
			/**
			@brief Swap this FMTSolutionTracker with another one.
			@param[in,out] p_tracker the FMTSolutionTracker to swap with.
			*/
			void swap(FMTSolutionTracker& p_tracker);
			// DocString: FMTSolutionTracker::getNonSpatialCellsCount
			/**
			@brief Return the number of non spatial cells.
			@return the non spatial cells count.
			*/
			size_t getNonSpatialCellsCount() const;
			// DocString: FMTSolutionTracker::setConstraintValues
			/**
			@brief Set the constraint values and families.
			@param[in] p_constraintValues the constraint values.
			@param[in] p_ConstraintsFamilies the constraint families.
			@param[in] p_NumberOfFamilies the number of families.
			*/
			void setConstraintValues(const std::vector<std::vector<double>>& p_constraintValues,
								const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
									size_t p_NumberOfFamilies);
			// DocString: FMTSolutionTracker::enableTracking
			/**
			@brief Enable the tracking of the constraint values.
			@return true if the tracking is enabled else false.
			*/
			bool enableTracking();
			// DocString: FMTSolutionTracker::disableTracking
			/**
			@brief Disable the tracking of the constraint values.
			*/
			void disableTracking();
			// DocString: FMTSolutionTracker::isTrackingEnable
			/**
			@brief Return true if the tracking is enabled.
			@return true if the tracking is enabled else false.
			*/
			bool isTrackingEnable() const;
		private:
			std::vector<std::vector<double>>m_LastConstraintsEvaluation;
			std::vector<size_t>m_Solution;
			std::vector<std::vector<size_t>>m_ConstraintsFamilies;
			const static size_t m_SOLUTION_RESIZE_FACTOR = 2;
			// DocString: FMTSolutionTracker::_modifySolution
			/**
			@brief Modify the tracked constraint values for a family by adding or removing a graph, according to the sense.
			@param[in] p_family the constraint family.
			@param[in] p_It the graph iterator.
			@param[in] p_sense the sense (positive to add, negative to remove).
			*/
			void _modifySolution(size_t p_family,
								std::map<Graph::FMTLineGraph, FMTGraphInfo>::const_iterator p_It,
								double p_sense);
			// DocString: FMTSolutionTracker::_buildConstraintsFamilies
			/**
			@brief Build the constraint families structure.
			@param[in] p_ConstraintsFamilies the constraint families.
			@param[in] p_NumberOfFamilies the number of families.
			*/
			void _buildConstraintsFamilies(const std::vector<std::vector<size_t>>& p_ConstraintsFamilies,
											size_t p_NumberOfFamilies);
		};
}


#endif 
