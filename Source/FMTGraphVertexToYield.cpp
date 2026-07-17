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
		graph(reinterpret_cast<const void* const>(&linegraph)),
		vertex(lvertex),
		graphtype(FMTgraphrequest::linegraph),
		modelptr(&model)
	{
		
	}
	FMTGraphVertexToYield::FMTGraphVertexToYield():
		graph(nullptr),
		vertex(nullptr),
		graphtype(FMTgraphrequest::nograph),
		modelptr(nullptr)
	{

	}

	FMTGraphVertexToYield::FMTGraphVertexToYield(const Models::FMTModel& model,const FMTGraph<FMTVertexProperties, FMTEdgeProperties>& fullgraph,const void* lvertex) :
		graph(reinterpret_cast<const void* const>(&fullgraph)),
		vertex(lvertex),
		graphtype(FMTgraphrequest::fullgraph),
		modelptr(&model)
	{
		 
	}

	const FMTGraph<FMTBaseVertexProperties,FMTBaseEdgeProperties>* const FMTGraphVertexToYield::getLineGraph() const
	{
		if (graphtype == FMTgraphrequest::linegraph)
		{
			return reinterpret_cast<const Graph::FMTGraph<FMTBaseVertexProperties,FMTBaseEdgeProperties>* const>(graph);
		}
		return nullptr;
	}
	const FMTGraph<FMTVertexProperties,FMTEdgeProperties>* const FMTGraphVertexToYield::getFullGraph() const
	{
		if (graphtype == FMTgraphrequest::fullgraph)
		{
			return reinterpret_cast<const FMTGraph<FMTVertexProperties,FMTEdgeProperties>* const>(graph);
		}
		return nullptr;
	}

	const Models::FMTModel* FMTGraphVertexToYield::getModel() const
	{
		return modelptr;
	}

}