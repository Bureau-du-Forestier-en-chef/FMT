/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTVIRTUALLINEGRAPH_H
#define FMTVIRTUALLINEGRAPH_H


#include "FMTutility.h"
#include <map>

namespace Graph
{
	class FMTlinegraph;
}


namespace Spatial
	{
	template <typename T>
	class FMTlayer;
	class FMTEXPORT FMTVirtualLineGraph
		{
		public:
			FMTVirtualLineGraph()=default;
			FMTVirtualLineGraph(const Graph::FMTlinegraph& p_LineGraph,
							std::map<Graph::FMTlinegraph, size_t>& p_mapping);
			FMTVirtualLineGraph(const FMTVirtualLineGraph& p_LineGraph);
			FMTVirtualLineGraph& operator = (const FMTVirtualLineGraph& p_LineGraph);
			bool operator == (const  FMTVirtualLineGraph& p_Element) const;
			bool operator < (const  FMTVirtualLineGraph& p_Element) const;
			size_t hash() const;
			const Graph::FMTlinegraph& getLineGraph() const;
			void setLineGraph(const Graph::FMTlinegraph& p_LineGraph,
							std::map<Graph::FMTlinegraph, size_t>& p_mapping);
			void setToGraph(std::map<Graph::FMTlinegraph, size_t>& p_mapping);
		private:
			std::map<Graph::FMTlinegraph, size_t>::iterator m_iterator;
			std::map<Graph::FMTlinegraph, size_t>::iterator m_end;
			void _insertInto(const Graph::FMTlinegraph& p_LineGraph,
							std::map<Graph::FMTlinegraph, size_t>& p_mapping);
			void _add();
			void _remove();
			void _deleteGraph(std::map<Graph::FMTlinegraph, size_t>& p_mapping);
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
