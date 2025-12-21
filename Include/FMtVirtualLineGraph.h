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
	class FMTmaskfilter;
}


namespace Spatial
	{
	template <typename T>
	class FMTlayer;
	class FMTSpatialGraphs;
	class FMTEXPORT FMTVirtualLineGraph
		{
		public:
			FMTVirtualLineGraph()=default;
			FMTVirtualLineGraph(FMTSpatialGraphs& p_Graphs,
							std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator p_Iterator,
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
								std::vector<size_t>& p_solution);
			double GetOutput(const Models::FMTmodel& p_model,
				const std::vector<size_t>& p_solution,
				const Core::FMToutput& p_output, int p_period) const;
			void SetBaseGraph(std::vector<size_t>& p_solution);
			void SetLastPeriod(std::vector<size_t>& p_solution);
			FMTVirtualLineGraph PostSolve(const Core::FMTmaskfilter& p_Filter,
					const std::vector<int>& p_actionMapping,
					FMTSpatialGraphs& p_Graphs,
					std::vector<size_t>& p_solution) const;
		private:
			std::map<Graph::FMTlinegraph, FMTGraphInfo>::const_iterator m_Iterator;
			size_t m_GraphFamily;
			FMTSpatialGraphs* m_Graphs;
			void _insertInto(const Graph::FMTlinegraph& p_LineGraph,
							std::vector<size_t>& p_solution);
			void _add(std::vector<size_t>& p_solution);
			void _remove(std::vector<size_t>& p_solution);
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
