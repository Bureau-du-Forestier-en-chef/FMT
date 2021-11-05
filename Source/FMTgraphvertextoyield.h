/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifndef FMTGRAPHTOYIELD_H_INCLUDED
#define FMTGRAPHTOYIELD_H_INCLUDED

namespace Graph
{
	template<class T1, class T2>
	class FMTgraph;
	class FMTbasevertexproperties;
	class FMTbaseedgeproperties;
	class FMTvertexproperties;
	class FMTedgeproperties;

	class FMTgraphvertextoyield
	{
		enum FMTgraphrequest
		{
			nograph = 0,
			linegraph = 1,
			fullgraph = 2,
			count = 3
		};
		const void* graph;
		const void* vertex;
		FMTgraphrequest graphtype;
	public:
		FMTgraphvertextoyield(const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>& linegraph,const void* lvertex);
		FMTgraphvertextoyield(const FMTgraph<FMTvertexproperties, FMTedgeproperties>& fullgraph,const void* lvertex);
		FMTgraphvertextoyield() = default;
		const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>* const getlinegraph() const;
		const FMTgraph<FMTvertexproperties, FMTedgeproperties>* const getfullgraph() const;
		inline const void* getvertexptr() const
		{
			return vertex;
		}
	};

}


#endif // FMTYLD_H_INCLUDED
