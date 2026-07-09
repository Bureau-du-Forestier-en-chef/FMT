/*
Copyright (c) 2019 Gouvernement du Qubec

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
#include <boost/unordered_map.hpp>
#include "FMTobject.h"
#include "FMTGraphInfo.h"
#include "FMTmask.h"
#include "FMTSolutionTracker.h"

namespace Graph
{
	class FMTlinegraph;
}

namespace Models
{
	class FMTmodel;
}

namespace Core
{
	class FMTconstraint;
	class FMToutputnode;
	class FMTschedule;
	class FMToutput;
	class FMTschedule;
}


namespace Spatial
	{
	class FMTVirtualLineGraph;
	class FMTSpatialGraphs: public Core::FMTobject
		{
		public:
			using const_iterator = std::map<Graph::FMTlinegraph, 
				FMTGraphInfo>::const_iterator;
			FMTSpatialGraphs() = default;
			FMTSpatialGraphs(const FMTSpatialGraphs&)=default;
			FMTSpatialGraphs& operator = (const FMTSpatialGraphs&)=default;
			FMTSpatialGraphs(const Models::FMTmodel& p_model, double p_CellSize);
			double getConstraintsValue(size_t NonSpatialConstraintId,
											const FMTSolutionTracker& p_solution) const;
			FMTVirtualLineGraph getVirtualGraph(const Graph::FMTlinegraph& p_LineGraph);
			FMTVirtualLineGraph setVirtualGraph(const Graph::FMTlinegraph& p_LineGraph);
			FMTSolutionTracker getBaseSolution() const;
			const_iterator getIterator(
												const Graph::FMTlinegraph& p_Graph,
															size_t p_family) const;
			const_iterator setIterator(
								const Graph::FMTlinegraph& p_Graph,
								size_t p_family);
			const_iterator setNaturalGrowthIterator(size_t p_family);
			const Models::FMTmodel& getModel() const;
			std::map<std::string, double> getOutput(
				const FMTSolutionTracker& p_Solution,
				const Core::FMToutput& p_output,
				int p_period, Core::FMToutputlevel level) const;
			void setModel(const Models::FMTmodel& p_model);
			void addToSolution(FMTSolutionTracker& p_solution,
				size_t p_family,
				const_iterator p_iterator);
			void removeToSolution(FMTSolutionTracker& p_solution,
				size_t p_family,
				const_iterator p_iterator);
			bool isNotNull(size_t p_family,
				const_iterator p_iterator) const;
			std::vector<Core::FMTschedule> getSchedules(const FMTSolutionTracker& p_Solution,
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
			using iterator = std::map<Graph::FMTlinegraph,
				FMTGraphInfo>::iterator;
			using family_iterator = std::vector<std::map<Graph::FMTlinegraph, FMTGraphInfo>>::iterator;
			std::vector<std::map<Graph::FMTlinegraph, FMTGraphInfo>>m_AllGraphs;
			size_t m_LastGraphId;
			std::vector<std::vector<size_t>>m_Constraints;
			boost::unordered_map<Core::FMTmask, size_t>m_GraphsMasks;
			FMTSolutionTracker m_BaseSolution;
			std::vector<size_t>m_ConstraintsId;
			Models::FMTmodel const * m_Model;
			void _BuildConstraintsLocator(const Models::FMTmodel& p_model);
			std::vector<size_t> _BuildGraphs(const Models::FMTmodel& p_model, double p_CellSize);
			void _BuildConstraintsValues( 
				family_iterator FirstGraphFamily,
				family_iterator GraphFamilyEnd);
			void _BuildConstraintsValues(
				iterator p_Graph,
				size_t p_family);
			size_t _FillValuesFromLastPeriod(
				size_t p_family,
				FMTSpatialGraphs::iterator p_Graph,
				size_t p_ConstraintId,
				std::vector<double>& p_constraintValues) const;
			static Core::FMTmask _GetUseFullBits(const Models::FMTmodel& p_model);
			size_t _GetFamily(const Graph::FMTlinegraph& p_Graph) const;
			size_t _GetNonSpatialId(size_t p_Constraint) const;
			void _FillConstraintResults(size_t p_Constraint,
				const FMTGraphInfo& p_GraphInfo,
				std::vector<double>& p_results,
				const FMTSolutionTracker& p_solution) const;
			double _GetGraphCells(const FMTSolutionTracker& p_solution, const FMTGraphInfo& p_GraphInfo) const;
			size_t _GetMaxGraphLength() const;
			size_t _GetMinGraphLength() const;
			void _RemoveGraphsShorterThan(size_t p_GraphSize);
			const_iterator _GetNaturalGrowthIterator(size_t p_family) const;
			std::vector<double> _GetConstraintsValueFromSolution(size_t p_ConstraintId,
				const FMTSolutionTracker& p_solution) const;
			std::vector<double> _GetConstraintsValueFromTracker(size_t p_ConstraintId,
				const FMTSolutionTracker& p_solution) const;
			std::vector<std::vector<double>> _GetConstraintsValues(const FMTSolutionTracker& p_solution) const;
			void _BuildTacker(const std::vector<size_t>& p_solution);
		};
}


#endif 
