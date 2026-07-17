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
	class FMTSpatialGraphs: public Core::FMTObject
		{
		public:
			using const_iterator = std::map<Graph::FMTLineGraph, 
				FMTGraphInfo>::const_iterator;
			FMTSpatialGraphs() = default;
			FMTSpatialGraphs(const FMTSpatialGraphs&)=default;
			FMTSpatialGraphs& operator = (const FMTSpatialGraphs&)=default;
			FMTSpatialGraphs(const Models::FMTModel& p_model, double p_CellSize);
			double getConstraintsValue(size_t NonSpatialConstraintId,
											const FMTSolutionTracker& p_solution) const;
			FMTVirtualLineGraph getVirtualGraph(const Graph::FMTLineGraph& p_LineGraph);
			FMTVirtualLineGraph setVirtualGraph(const Graph::FMTLineGraph& p_LineGraph);
			FMTSolutionTracker getBaseSolution() const;
			const_iterator getIterator(
												const Graph::FMTLineGraph& p_Graph,
															size_t p_family) const;
			const_iterator setIterator(
								const Graph::FMTLineGraph& p_Graph,
								size_t p_family);
			const_iterator setNaturalGrowthIterator(size_t p_family);
			const Models::FMTModel& getModel() const;
			std::map<std::string, double> getOutput(
				const FMTSolutionTracker& p_Solution,
				const Core::FMTOutput& p_output,
				int p_period, Core::FMToutputlevel level) const;
			void setModel(const Models::FMTModel& p_model);
			void addToSolution(FMTSolutionTracker& p_solution,
				size_t p_family,
				const_iterator p_iterator);
			void removeToSolution(FMTSolutionTracker& p_solution,
				size_t p_family,
				const_iterator p_iterator);
			bool isNotNull(size_t p_family,
				const_iterator p_iterator) const;
			std::vector<Core::FMTSchedule> getSchedules(const FMTSolutionTracker& p_Solution,
											bool WithLock = false) const;
			const_iterator getBaseIterator(size_t p_family) const;
			const_iterator getLastPeriodIterator(
				size_t p_family,
				const_iterator p_iterator) const;
			void swap(FMTSpatialGraphs& p_Other);
			size_t getNumberOfCategories(int p_themeId) const;
			size_t getCategoryOf(int p_themeId, size_t p_family) const;
			void deleteNonCompleteGraphs();
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
			std::shared_ptr<const FMTGraphValues> _internValues(const std::vector<double>& p_Values);
			void _buildConstraintsLocator(const Models::FMTModel& p_model);
			std::vector<size_t> _buildGraphs(const Models::FMTModel& p_model, double p_CellSize);
			void _buildConstraintsValues( 
				family_iterator FirstGraphFamily,
				family_iterator GraphFamilyEnd);
			void _buildConstraintsValues(
				iterator p_Graph,
				size_t p_family);
			size_t _fillValuesFromLastPeriod(
				size_t p_family,
				FMTSpatialGraphs::iterator p_Graph,
				size_t p_ConstraintId,
				std::vector<double>& p_constraintValues) const;
			static Core::FMTMask _getUseFullBits(const Models::FMTModel& p_model);
			size_t _getFamily(const Graph::FMTLineGraph& p_Graph) const;
			size_t _getNonSpatialId(size_t p_Constraint) const;
			void _fillConstraintResults(size_t p_Constraint,
				const FMTGraphInfo& p_GraphInfo,
				std::vector<double>& p_results,
				const FMTSolutionTracker& p_solution) const;
			double _getGraphCells(const FMTSolutionTracker& p_solution, const FMTGraphInfo& p_GraphInfo) const;
			size_t _getMaxGraphLength() const;
			size_t _getMinGraphLength() const;
			void _removeGraphsShorterThan(size_t p_GraphSize);
			const_iterator _getNaturalGrowthIterator(size_t p_family) const;
			std::vector<double> _getConstraintsValueFromSolution(size_t p_ConstraintId,
				const FMTSolutionTracker& p_solution) const;
			std::vector<double> _getConstraintsValueFromTracker(size_t p_ConstraintId,
				const FMTSolutionTracker& p_solution) const;
			std::vector<std::vector<double>> _getConstraintsValues(const FMTSolutionTracker& p_solution) const;
			void _buildTacker(const std::vector<size_t>& p_solution);
		};
}


#endif 
