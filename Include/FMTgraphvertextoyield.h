/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifndef FMTGRAPHTOYIELD_Hm_included
#define FMTGRAPHTOYIELD_Hm_included

namespace Models
{
class FMTmodel;
}

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
		void const* graph;
		void const* vertex;
		FMTgraphrequest graphtype;
		Models::FMTmodel const* modelptr;
	public:
		FMTgraphvertextoyield(const Models::FMTmodel& model,const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>& linegraph,const void* lvertex);
		FMTgraphvertextoyield(const Models::FMTmodel& model,const FMTgraph<FMTvertexproperties, FMTedgeproperties>& fullgraph,const void* lvertex);
		FMTgraphvertextoyield();
		FMTgraphvertextoyield(const FMTgraphvertextoyield& rhs) = default;
		FMTgraphvertextoyield& operator=(const FMTgraphvertextoyield& rhs) = default;
		const FMTgraph<FMTbasevertexproperties, FMTbaseedgeproperties>* const getlinegraph() const;
		const FMTgraph<FMTvertexproperties, FMTedgeproperties>* const getfullgraph() const;
		inline const void* getvertexptr() const
		{
			return vertex;
		}
		const Models::FMTmodel* getmodel() const;
	};

}


#endif // FMTYLD_Hm_included
