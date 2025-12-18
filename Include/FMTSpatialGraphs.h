/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTSPATIALGRAPHS_H
#define FMTSPATIALGRAPHS_H


#include <map>
#include <vector>
#include <string>
#include <memory>
#include <boost/unordered_map.hpp>
#include "FMTobject.h"
#include "FMTGraphInfo.h"
#include "FMTmask.h"

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
			FMTSpatialGraphs() = default;
			FMTSpatialGraphs(const FMTSpatialGraphs&)=default;
			FMTSpatialGraphs& operator = (const FMTSpatialGraphs&)=default;
			FMTSpatialGraphs(const Models::FMTmodel& p_model, double p_CellSize);
			double GetConstraintsValue(size_t NonSpatialConstraintId,
											const std::vector<size_t>& p_solution) const;
			FMTVirtualLineGraph GetGraphNaturalGrowth(const Graph::FMTlinegraph& p_LineGraph);
			std::vector<size_t>GetNaturalGrowthSolution() const;
			std::vector<size_t>GetBaseSolution(size_t p_SolutionSize) const;
			std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator GetIterator(
												const Graph::FMTlinegraph& p_Graph,
															size_t p_family) const;
			std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator SetIterator(
								const Graph::FMTlinegraph& p_Graph,
								size_t p_family);
			const Models::FMTmodel& GetModel() const;
			std::map<std::string, double> GetOutput(
				const std::vector<size_t>& p_Solution,
				const Core::FMToutput& p_output,
				int p_period, Core::FMToutputlevel level) const;
			void SetModel(const Models::FMTmodel& p_model);
			void AddToSolution(std::vector<size_t>& p_solution,
				size_t p_family,
				std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_iterator);
			void RemoveToSolution(std::vector<size_t>& p_solution,
				size_t p_family,
				std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_iterator);
			bool IsNotNull(size_t p_family,
				std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_iterator) const;
			std::vector<Core::FMTschedule> GetSchedules(const std::vector<size_t>& p_Solution,
											bool WithLock = false) const;

		private:
			const static size_t SOLUTION_RESIZE_FACTOR = 2;
			const static size_t MAX_ASYNC_JOBS = 5;
			std::vector<std::map<Graph::FMTlinegraph, FMTGraphInfo>>m_AllGraphs;
			size_t m_LastGraphId;
			std::vector<std::vector<size_t>>m_Constraints;
			boost::unordered_map<Core::FMTmask, size_t>m_GraphsMasks;
			std::vector<size_t>m_NaturalGrowth;
			std::vector<size_t>m_ConstraintsId;
			Models::FMTmodel const * m_Model;
			void _BuildConstraintsLocator(const Models::FMTmodel& p_model);
			void _BuildGraphs(const Models::FMTmodel& p_model, double p_CellSize);
			void _BuildConstraintsValues( 
					std::vector<std::map<Graph::FMTlinegraph, FMTGraphInfo>>::iterator FirstGraphFamily,
				std::vector<std::map<Graph::FMTlinegraph, FMTGraphInfo>>::iterator GraphFamilyEnd);
			void _BuildConstraintsValues(
				std::map<Graph::FMTlinegraph, FMTGraphInfo>::iterator p_Graph,
				size_t p_family);
			static Core::FMTmask _GetUseFullBits(const Models::FMTmodel& p_model);
			size_t _GetFamily(const Graph::FMTlinegraph& p_Graph) const;
			size_t _GetNonSpatialId(size_t p_Constraint) const;
			void _FillConstraintResults(size_t p_Constraint,
				const FMTGraphInfo& p_GraphInfo,
				std::vector<double>& p_results,
				const std::vector<size_t>& p_solution) const;
			double _GetGraphCells(const std::vector<size_t>& p_solution, const FMTGraphInfo& p_GraphInfo) const;
			size_t _GetMaxGraphLength(const std::vector<size_t>& p_solution) const;
			size_t _GetMinGraphLength(const std::vector<size_t>& p_solution) const;
			

		};
}


#endif 
