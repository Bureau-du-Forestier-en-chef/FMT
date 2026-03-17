/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTVIRTUALLINEGRAPH_H
#define FMTVIRTUALLINEGRAPH_H


#include "FMTutility.h"
#include <vector>
#include <map>
#include <memory>
#include "FMTGraphInfo.h"
#include "FMTSpatialGraphs.h"

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
	class FMToutput;
	class FMTtheme;
	class FMTmaskfilter;
}


namespace Spatial
	{
	template <typename T>
	class FMTlayer;
	class FMTSolutionTracker;
	class FMTEXPORT FMTVirtualLineGraph
		{
		public:
			FMTVirtualLineGraph()=default;
			FMTVirtualLineGraph(FMTSpatialGraphs& p_Graphs,
							FMTSpatialGraphs::const_iterator p_Iterator,
							size_t p_Family);
			FMTVirtualLineGraph(const FMTVirtualLineGraph& p_LineGraph, FMTSpatialGraphs& p_Graphs);
			FMTVirtualLineGraph(const FMTVirtualLineGraph& p_LineGraph);
			FMTVirtualLineGraph& operator = (const FMTVirtualLineGraph& p_LineGraph);
			~FMTVirtualLineGraph();
			bool operator == (const  FMTVirtualLineGraph& p_Element) const;
			bool operator < (const  FMTVirtualLineGraph& p_Element) const;
			size_t hash() const;
			const Graph::FMTlinegraph& getLineGraph() const;
			void setLineGraph(const Graph::FMTlinegraph& p_LineGraph,
							FMTSolutionTracker& p_solution);
			double GetOutput(const Models::FMTmodel& p_model,
				const FMTSolutionTracker& p_solution,
				const Core::FMToutput& p_output, int p_period) const;
			size_t GetGraphFamily() const;
			void SetBaseGraph(FMTSolutionTracker& p_solution);
			void SetLastPeriod(FMTSolutionTracker& p_solution);
			void SetNaturalGrowth(FMTSolutionTracker& p_solution);
			FMTVirtualLineGraph PostSolve(const Core::FMTmaskfilter& p_Filter,
					const std::vector<int>& p_actionMapping,
					FMTSpatialGraphs& p_Graphs,
				FMTSolutionTracker& p_solution) const;
		private:
			FMTSpatialGraphs::const_iterator m_Iterator;
			size_t m_GraphFamily;
			FMTSpatialGraphs* m_Graphs;
			void _insertInto(const Graph::FMTlinegraph& p_LineGraph,
					FMTSolutionTracker& p_solution);
			void _add(FMTSolutionTracker& p_solution);
			void _remove(FMTSolutionTracker& p_solution);
			bool notNull() const;
		};
	}

namespace std {
	template <>
	struct hash<Spatial::FMTVirtualLineGraph>
	{
		std::size_t operator()(const Spatial::FMTVirtualLineGraph& p_object) const
		{
			return (p_object.hash());
		}
	};

}


#endif 
