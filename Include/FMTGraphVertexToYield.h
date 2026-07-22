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

	// DocString: FMTGraphVertexToYield
	/**
	@brief Adapter pointing to a vertex of a line graph or a full graph and its model, used to answer a generic yield request that depends on the graph.
	*/
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
		// DocString: FMTGraphVertexToYield(const Models::FMTModel&,const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>&,const void*)
		/**
		@brief Construct a FMTGraphVertexToYield from a model, a line graph and a vertex.
		@param[in] model the model.
		@param[in] linegraph the line graph.
		@param[in] lvertex the vertex.
		*/
		FMTGraphVertexToYield(const Models::FMTModel& model,const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>& linegraph,const void* lvertex);
		// DocString: FMTGraphVertexToYield(const Models::FMTModel&,const FMTGraph<FMTVertexProperties, FMTEdgeProperties>&,const void*)
		/**
		@brief Construct a FMTGraphVertexToYield from a model, a full graph and a vertex.
		@param[in] model the model.
		@param[in] fullgraph the full graph.
		@param[in] lvertex the vertex.
		*/
		FMTGraphVertexToYield(const Models::FMTModel& model,const FMTGraph<FMTVertexProperties, FMTEdgeProperties>& fullgraph,const void* lvertex);
		// DocString: FMTGraphVertexToYield()
		/**
		@brief Default constructor for FMTGraphVertexToYield.
		*/
		FMTGraphVertexToYield();
		// DocString: FMTGraphVertexToYield(const FMTGraphVertexToYield&)
		/**
		@brief Copy constructor for FMTGraphVertexToYield.
		@param[in] rhs the FMTGraphVertexToYield to copy.
		*/
		FMTGraphVertexToYield(const FMTGraphVertexToYield& rhs) = default;
		// DocString: FMTGraphVertexToYield::operator=
		/**
		@brief Copy assignment operator for FMTGraphVertexToYield.
		@param[in] rhs the FMTGraphVertexToYield to copy.
		@return a reference to this FMTGraphVertexToYield.
		*/
		FMTGraphVertexToYield& operator=(const FMTGraphVertexToYield& rhs) = default;
		// DocString: FMTGraphVertexToYield::getLineGraph
		/**
		@brief Return the line graph, or nullptr if the adapter points to a full graph.
		@return a pointer to the line graph.
		*/
		const FMTGraph<FMTBaseVertexProperties, FMTBaseEdgeProperties>* const getLineGraph() const;
		// DocString: FMTGraphVertexToYield::getFullGraph
		/**
		@brief Return the full graph, or nullptr if the adapter points to a line graph.
		@return a pointer to the full graph.
		*/
		const FMTGraph<FMTVertexProperties, FMTEdgeProperties>* const getFullGraph() const;
		// DocString: FMTGraphVertexToYield::getVertexPtr
		/**
		@brief Return a pointer to the vertex.
		@return a pointer to the vertex.
		*/
		inline const void* getVertexPtr() const
		{
			return vertex;
		}
		// DocString: FMTGraphVertexToYield::getModel
		/**
		@brief Return the model.
		@return a pointer to the model.
		*/
		const Models::FMTModel* getModel() const;
	};

}


#endif // FMTYLD_Hm_included
