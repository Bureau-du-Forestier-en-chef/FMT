/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTGraphVertexToYield.h"
#include "FMTGraph.hpp"
#include "FMTModel.h"

namespace Graph
{

	FMTGraphVertexToYield::FMTGraphVertexToYield(const Models::FMTModel& model,const FMTGraph<FMTBaseVertexProperties,FMTBaseEdgeProperties>& linegraph,const void* lvertex) :
		m_graph(reinterpret_cast<const void* const>(&linegraph)),
		m_vertex(lvertex),
		m_graphtype(FMTgraphrequest::linegraph),
		m_modelptr(&model)
	{
		
	}
	FMTGraphVertexToYield::FMTGraphVertexToYield():
		m_graph(nullptr),
		m_vertex(nullptr),
		m_graphtype(FMTgraphrequest::nograph),
		m_modelptr(nullptr)
	{

	}

	FMTGraphVertexToYield::FMTGraphVertexToYield(const Models::FMTModel& model,const FMTGraph<FMTVertexProperties, FMTEdgeProperties>& fullgraph,const void* lvertex) :
		m_graph(reinterpret_cast<const void* const>(&fullgraph)),
		m_vertex(lvertex),
		m_graphtype(FMTgraphrequest::fullgraph),
		m_modelptr(&model)
	{
		 
	}

	const FMTGraph<FMTBaseVertexProperties,FMTBaseEdgeProperties>* const FMTGraphVertexToYield::getLineGraph() const
	{
		if (m_graphtype == FMTgraphrequest::linegraph)
		{
			return reinterpret_cast<const Graph::FMTGraph<FMTBaseVertexProperties,FMTBaseEdgeProperties>* const>(m_graph);
		}
		return nullptr;
	}
	const FMTGraph<FMTVertexProperties,FMTEdgeProperties>* const FMTGraphVertexToYield::getFullGraph() const
	{
		if (m_graphtype == FMTgraphrequest::fullgraph)
		{
			return reinterpret_cast<const FMTGraph<FMTVertexProperties,FMTEdgeProperties>* const>(m_graph);
		}
		return nullptr;
	}

	const Models::FMTModel* FMTGraphVertexToYield::getModel() const
	{
		return m_modelptr;
	}

}