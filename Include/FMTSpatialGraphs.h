/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTSPATIALGRAPHS_H
#define FMTSPATIALGRAPHS_H


#include <map>
//#include <boost/container/flat_map.hpp>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <boost/unordered_map.hpp>
#include "FMTObject.h"
#include "FMTGraphInfo.h"
#include "FMTMask.h"
#include "FMTSolutionTracker.h"

namespace Graph
{
	class FMTLineGraph;
}

namespace Models
{
	class FMTModel;
}

namespace Core
{
	class FMTConstraint;
	class FMTOutputNode;
	class FMTSchedule;
	class FMTOutput;
	class FMTSchedule;
}


namespace Spatial
	{
	class FMTVirtualLineGraph;
	// DocString: FMTSpatialGraphs
	/**
	@brief Container of the line graphs of a spatially explicit model, organized by constraint family, with a base solution tracker.
	@details It builds the graphs from a model, stores the constraint values of each graph and provides the outputs and schedules of a spatial solution.
	*/
	class FMTSpatialGraphs: public Core::FMTObject
		{
		public:
			using const_iterator = std::map<Graph::FMTLineGraph, 
				FMTGraphInfo>::const_iterator;
			// DocString: FMTSpatialGraphs()
			/**
			@brief Default constructor for FMTSpatialGraphs.
			*/
			FMTSpatialGraphs() = default;
			// DocString: FMTSpatialGraphs(const FMTSpatialGraphs&)
			/**
			@brief Default copy constructor for FMTSpatialGraphs.
			@param[in] p_rhs the FMTSpatialGraphs to copy.
			*/
			FMTSpatialGraphs(const FMTSpatialGraphs& p_rhs)=default;
			// DocString: FMTSpatialGraphs::operator=
			/**
			@brief Default copy assignment operator for FMTSpatialGraphs.
			@return a reference to this FMTSpatialGraphs.
			*/
			FMTSpatialGraphs& operator = (const FMTSpatialGraphs&)=default;
			// DocString: FMTSpatialGraphs(const Models::FMTModel&,double)
			/**
			@brief Construct a FMTSpatialGraphs from a model and a cell size, building all the line graphs.
			@param[in] p_model the model.
			@param[in] p_CellSize the cell size.
			*/
			FMTSpatialGraphs(const Models::FMTModel& p_model, double p_CellSize);
			// DocString: FMTSpatialGraphs::getConstraintsValue
			/**
			@brief Return the value of a non spatial constraint for a solution.
			@param[in] NonSpatialConstraintId the non spatial constraint id.
			@param[in] p_solution the solution.
			@return the constraint value.
			*/
			double getConstraintsValue(size_t NonSpatialConstraintId,
											const FMTSolutionTracker& p_solution) const;
			// DocString: FMTSpatialGraphs::getVirtualGraph
			/**
			@brief Return the virtual line graph of a line graph.
			@param[in] p_LineGraph the line graph.
			@return the virtual line graph.
			*/
			FMTVirtualLineGraph getVirtualGraph(const Graph::FMTLineGraph& p_LineGraph);
			// DocString: FMTSpatialGraphs::setVirtualGraph
			/**
			@brief Set and return the virtual line graph of a line graph.
			@param[in] p_LineGraph the line graph.
			@return the virtual line graph.
			*/
			FMTVirtualLineGraph setVirtualGraph(const Graph::FMTLineGraph& p_LineGraph);
			// DocString: FMTSpatialGraphs::getBaseSolution
			/**
			@brief Return the base solution tracker.
			@return the base solution.
			*/
			FMTSolutionTracker getBaseSolution() const;
			// DocString: FMTSpatialGraphs::getIterator
			/**
			@brief Return the iterator of a graph in a constraint family.
			@param[in] p_Graph the graph.
			@param[in] p_family the constraint family.
			@return the graph iterator.
			*/
			const_iterator getIterator(
												const Graph::FMTLineGraph& p_Graph,
															size_t p_family) const;
			// DocString: FMTSpatialGraphs::setIterator
			/**
			@brief Add a graph to a constraint family and return its iterator.
			@param[in] p_Graph the graph.
			@param[in] p_family the constraint family.
			@return the graph iterator.
			*/
			const_iterator setIterator(
								const Graph::FMTLineGraph& p_Graph,
								size_t p_family);
			// DocString: FMTSpatialGraphs::setNaturalGrowthIterator
			/**
			@brief Add the natural growth graph to a constraint family and return its iterator.
			@param[in] p_family the constraint family.
			@return the natural growth graph iterator.
			*/
			const_iterator setNaturalGrowthIterator(size_t p_family);
			// DocString: FMTSpatialGraphs::getModel
			/**
			@brief Return the model of the spatial graphs.
			@return the model.
			*/
			const Models::FMTModel& getModel() const;
			// DocString: FMTSpatialGraphs::getOutput
			/**
			@brief Return the output value of a solution for a period.
			@param[in] p_Solution the solution.
			@param[in] p_output the output.
			@param[in] p_period the period.
			@param[in] level the output level.
			@return a map of names to values.
			*/
			std::map<std::string, double> getOutput(
				const FMTSolutionTracker& p_Solution,
				const Core::FMTOutput& p_output,
				int p_period, Core::FMToutputlevel level) const;
			// DocString: FMTSpatialGraphs::setModel
			/**
			@brief Set the model of the spatial graphs.
			@param[in] p_model the model.
			*/
			void setModel(const Models::FMTModel& p_model);
			// DocString: FMTSpatialGraphs::addToSolution
			/**
			@brief Add a graph to a solution for a constraint family.
			@param[in,out] p_solution the solution.
			@param[in] p_family the constraint family.
			@param[in] p_iterator the graph iterator.
			*/
			void addToSolution(FMTSolutionTracker& p_solution,
				size_t p_family,
				const_iterator p_iterator);
			// DocString: FMTSpatialGraphs::removeToSolution
			/**
			@brief Remove a graph from a solution for a constraint family.
			@param[in,out] p_solution the solution.
			@param[in] p_family the constraint family.
			@param[in] p_iterator the graph iterator.
			*/
			void removeToSolution(FMTSolutionTracker& p_solution,
				size_t p_family,
				const_iterator p_iterator);
			// DocString: FMTSpatialGraphs::isNotNull
			/**
			@brief Return true if a graph iterator of a constraint family is not null.
			@param[in] p_family the constraint family.
			@param[in] p_iterator the graph iterator.
			@return true if the iterator is not null else false.
			*/
			bool isNotNull(size_t p_family,
				const_iterator p_iterator) const;
			// DocString: FMTSpatialGraphs::getSchedules
			/**
			@brief Return the schedules of a spatial solution.
			@param[in] p_Solution the solution.
			@param[in] WithLock if true includes the locked developments.
			@return the schedules.
			*/
			std::vector<Core::FMTSchedule> getSchedules(const FMTSolutionTracker& p_Solution,
											bool WithLock = false) const;
			// DocString: FMTSpatialGraphs::getBaseIterator
			/**
			@brief Return the base graph iterator of a constraint family.
			@param[in] p_family the constraint family.
			@return the base graph iterator.
			*/
			const_iterator getBaseIterator(size_t p_family) const;
			// DocString: FMTSpatialGraphs::getLastPeriodIterator
			/**
			@brief Return the last period graph iterator of a graph in a constraint family.
			@param[in] p_family the constraint family.
			@param[in] p_iterator the graph iterator.
			@return the last period graph iterator.
			*/
			const_iterator getLastPeriodIterator(
				size_t p_family,
				const_iterator p_iterator) const;
			// DocString: FMTSpatialGraphs::swap
			/**
			@brief Swap this FMTSpatialGraphs with another one.
			@param[in,out] p_Other the FMTSpatialGraphs to swap with.
			*/
			void swap(FMTSpatialGraphs& p_Other);
			// DocString: FMTSpatialGraphs::getNumberOfCategories
			/**
			@brief Return the number of categories of a theme.
			@param[in] p_themeId the theme id.
			@return the number of categories.
			*/
			size_t getNumberOfCategories(int p_themeId) const;
			// DocString: FMTSpatialGraphs::getCategoryOf
			/**
			@brief Return the category of a theme for a constraint family.
			@param[in] p_themeId the theme id.
			@param[in] p_family the constraint family.
			@return the category.
			*/
			size_t getCategoryOf(int p_themeId, size_t p_family) const;
			// DocString: FMTSpatialGraphs::deleteNonCompleteGraphs
			/**
			@brief Delete the graphs that are not complete.
			*/
			void deleteNonCompleteGraphs();
			// DocString: FMTSpatialGraphs::enableSolutionTracker
			/**
			@brief Enable the tracking of the constraint values on a solution tracker.
			@param[in,out] p_tracker the solution tracker.
			*/
			void enableSolutionTracker(FMTSolutionTracker& p_tracker) const;
		private:
			using iterator = std::map<Graph::FMTLineGraph,
				FMTGraphInfo>::iterator;
			using family_iterator = std::vector<std::map<Graph::FMTLineGraph, FMTGraphInfo>>::iterator;
			std::vector<std::map<Graph::FMTLineGraph, FMTGraphInfo>>m_AllGraphs;
			size_t m_LastGraphId;
			std::vector<std::vector<size_t>>m_Constraints;
			boost::unordered_map<Core::FMTMask, size_t>m_GraphsMasks;
			FMTSolutionTracker m_BaseSolution;
			std::vector<size_t>m_ConstraintsId;
			Models::FMTModel const * m_Model;
			// Run-scoped deduplication pool for graph constraint values (replaces the former
			// process-wide boost::flyweight static factory). Keyed by value-vector hash, holding
			// weak_ptr so distinct vectors are shared while alive and released with the graphs.
			std::unordered_map<std::size_t, std::vector<std::weak_ptr<const FMTGraphValues>>> m_ValuesPool;
			// DocString: FMTSpatialGraphs::_internValues
			/**
			@brief Return a shared, deduplicated pointer to a vector of constraint values.
			@param[in] p_Values the values.
			@return the shared values.
			*/
			std::shared_ptr<const FMTGraphValues> _internValues(const std::vector<double>& p_Values);
			// DocString: FMTSpatialGraphs::_buildConstraintsLocator
			/**
			@brief Build the constraints locator from a model.
			@param[in] p_model the model.
			*/
			void _buildConstraintsLocator(const Models::FMTModel& p_model);
			// DocString: FMTSpatialGraphs::_buildGraphs
			/**
			@brief Build the line graphs from a model and a cell size.
			@param[in] p_model the model.
			@param[in] p_CellSize the cell size.
			@return the base solution.
			*/
			std::vector<size_t> _buildGraphs(const Models::FMTModel& p_model, double p_CellSize);
			// DocString: FMTSpatialGraphs::_buildConstraintsValues(family_iterator,family_iterator)
			/**
			@brief Build the constraint values of a range of graph families.
			@param[in] FirstGraphFamily the first graph family.
			@param[in] GraphFamilyEnd the end of the graph families.
			*/
			void _buildConstraintsValues( 
				family_iterator FirstGraphFamily,
				family_iterator GraphFamilyEnd);
			// DocString: FMTSpatialGraphs::_buildConstraintsValues(iterator,size_t)
			/**
			@brief Build the constraint values of a graph in a constraint family.
			@param[in] p_Graph the graph.
			@param[in] p_family the constraint family.
			*/
			void _buildConstraintsValues(
				iterator p_Graph,
				size_t p_family);
			// DocString: FMTSpatialGraphs::_fillValuesFromLastPeriod
			/**
			@brief Fill the constraint values of a graph from its last period.
			@param[in] p_family the constraint family.
			@param[in] p_Graph the graph.
			@param[in] p_ConstraintId the constraint id.
			@param[in,out] p_constraintValues the constraint values.
			@return the number of filled values.
			*/
			size_t _fillValuesFromLastPeriod(
				size_t p_family,
				FMTSpatialGraphs::iterator p_Graph,
				size_t p_ConstraintId,
				std::vector<double>& p_constraintValues) const;
			// DocString: FMTSpatialGraphs::_getUseFullBits
			/**
			@brief Return the useful bits mask of a model.
			@param[in] p_model the model.
			@return the useful bits mask.
			*/
			static Core::FMTMask _getUseFullBits(const Models::FMTModel& p_model);
			// DocString: FMTSpatialGraphs::_getFamily
			/**
			@brief Return the constraint family of a graph.
			@param[in] p_Graph the graph.
			@return the constraint family.
			*/
			size_t _getFamily(const Graph::FMTLineGraph& p_Graph) const;
			// DocString: FMTSpatialGraphs::_getNonSpatialId
			/**
			@brief Return the non spatial id of a constraint.
			@param[in] p_Constraint the constraint index.
			@return the non spatial id.
			*/
			size_t _getNonSpatialId(size_t p_Constraint) const;
			// DocString: FMTSpatialGraphs::_fillConstraintResults
			/**
			@brief Fill the results of a constraint for a graph and a solution.
			@param[in] p_Constraint the constraint index.
			@param[in] p_GraphInfo the graph info.
			@param[in,out] p_results the results.
			@param[in] p_solution the solution.
			*/
			void _fillConstraintResults(size_t p_Constraint,
				const FMTGraphInfo& p_GraphInfo,
				std::vector<double>& p_results,
				const FMTSolutionTracker& p_solution) const;
			// DocString: FMTSpatialGraphs::_getGraphCells
			/**
			@brief Return the number of cells of a graph in a solution.
			@param[in] p_solution the solution.
			@param[in] p_GraphInfo the graph info.
			@return the number of cells.
			*/
			double _getGraphCells(const FMTSolutionTracker& p_solution, const FMTGraphInfo& p_GraphInfo) const;
			// DocString: FMTSpatialGraphs::_getMaxGraphLength
			/**
			@brief Return the maximal graph length.
			@return the maximal graph length.
			*/
			size_t _getMaxGraphLength() const;
			// DocString: FMTSpatialGraphs::_getMinGraphLength
			/**
			@brief Return the minimal graph length.
			@return the minimal graph length.
			*/
			size_t _getMinGraphLength() const;
			// DocString: FMTSpatialGraphs::_removeGraphsShorterThan
			/**
			@brief Remove the graphs shorter than a given size.
			@param[in] p_GraphSize the graph size.
			*/
			void _removeGraphsShorterThan(size_t p_GraphSize);
			// DocString: FMTSpatialGraphs::_getNaturalGrowthIterator
			/**
			@brief Return the natural growth graph iterator of a constraint family.
			@param[in] p_family the constraint family.
			@return the natural growth graph iterator.
			*/
			const_iterator _getNaturalGrowthIterator(size_t p_family) const;
			// DocString: FMTSpatialGraphs::_getConstraintsValueFromSolution
			/**
			@brief Return the values of a constraint computed from a solution.
			@param[in] p_ConstraintId the constraint id.
			@param[in] p_solution the solution.
			@return the constraint values.
			*/
			std::vector<double> _getConstraintsValueFromSolution(size_t p_ConstraintId,
				const FMTSolutionTracker& p_solution) const;
			// DocString: FMTSpatialGraphs::_getConstraintsValueFromTracker
			/**
			@brief Return the values of a constraint read from the solution tracker.
			@param[in] p_ConstraintId the constraint id.
			@param[in] p_solution the solution.
			@return the constraint values.
			*/
			std::vector<double> _getConstraintsValueFromTracker(size_t p_ConstraintId,
				const FMTSolutionTracker& p_solution) const;
			// DocString: FMTSpatialGraphs::_getConstraintsValues
			/**
			@brief Return the values of all the constraints for a solution.
			@param[in] p_solution the solution.
			@return the constraint values.
			*/
			std::vector<std::vector<double>> _getConstraintsValues(const FMTSolutionTracker& p_solution) const;
			// DocString: FMTSpatialGraphs::_buildTacker
			/**
			@brief Build the base solution tracker from a solution.
			@param[in] p_solution the solution.
			*/
			void _buildTacker(const std::vector<size_t>& p_solution);
		};
}


#endif 
