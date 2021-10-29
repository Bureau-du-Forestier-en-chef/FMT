/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTgraphvertextoyield.h"
#include "FMTgraph.h"

namespace Graph
{

	FMTgraphvertextoyield::FMTgraphvertextoyield(const FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>& linegraph,const void* lvertex) :
		graph(reinterpret_cast<const void* const>(&linegraph)),
		vertex(lvertex),
		graphtype(FMTgraphrequest::linegraph)
	{

	}
	FMTgraphvertextoyield::FMTgraphvertextoyield(const FMTgraph<FMTvertexproperties, FMTedgeproperties>& fullgraph,const void* lvertex) :
		graph(reinterpret_cast<const void* const>(&fullgraph)),
		vertex(lvertex),
		graphtype(FMTgraphrequest::fullgraph)
	{

	}

	const FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>* const FMTgraphvertextoyield::getlinegraph() const
	{
		if (graphtype == FMTgraphrequest::linegraph)
		{
			return reinterpret_cast<const Graph::FMTgraph<FMTbasevertexproperties,FMTbaseedgeproperties>* const>(graph);
		}
		return nullptr;
	}
	const FMTgraph<FMTvertexproperties,FMTedgeproperties>* const FMTgraphvertextoyield::getfullgraph() const
	{
		if (graphtype == FMTgraphrequest::fullgraph)
		{
			return reinterpret_cast<const FMTgraph<FMTvertexproperties,FMTedgeproperties>* const>(graph);
		}
		return nullptr;
	}

}