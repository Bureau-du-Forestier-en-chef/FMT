/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/


#ifndef FMTGRAPHTOYIELD_Hm_included
#define FMTGRAPHTOYIELD_Hm_included

namespace Models
{
class FMTModel;
}

namespace Graph
{
	template<class T1, class T2>
	class FMTGraph;
	class FMTBaseVertexProperties;
	class FMTBaseEdgeProperties;
	class FMTVertexProperties;
	class FMTEdgeProperties;

	class FMTGraphVertexToYield
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
		Models::FMTModel const* modelptr;
	public:
		FMTGraphVertexToYield(const Models::FMTModel& model,const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>& linegraph,const void* lvertex);
		FMTGraphVertexToYield(const Models::FMTModel& model,const FMTGraph<FMTVertexProperties, FMTEdgeProperties>& fullgraph,const void* lvertex);
		FMTGraphVertexToYield();
		FMTGraphVertexToYield(const FMTGraphVertexToYield& rhs) = default;
		FMTGraphVertexToYield& operator=(const FMTGraphVertexToYield& rhs) = default;
		const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>* const getLineGraph() const;
		const FMTGraph<FMTVertexProperties, FMTEdgeProperties>* const getFullGraph() const;
		inline const void* getVertexPtr() const
		{
			return vertex;
		}
		const Models::FMTModel* getModel() const;
	};

}


#endif // FMTYLD_Hm_included
