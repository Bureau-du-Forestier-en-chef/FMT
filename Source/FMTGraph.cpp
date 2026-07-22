/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTGraph.hpp"


    

namespace boost
{
	namespace archive
	{
		namespace detail
		{
			namespace extra_detail
			{
				template<> struct init_guid<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>>
				{
					static guid_initializer<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>> const & g;
				};
				template<> struct init_guid<Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>>
				{
					static guid_initializer<Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>> const & g;
				};

				guid_initializer<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>> const & init_guid<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>>::g = ::boost::serialization::singleton< guid_initializer<Graph::FMTGraph<Graph::FMTVertexProperties, Graph::FMTEdgeProperties>> >::get_mutable_instance().export_guid();
				guid_initializer<Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>> const & init_guid<Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>>::g = ::boost::serialization::singleton< guid_initializer<Graph::FMTGraph<Graph::FMTBaseVertexProperties, Graph::FMTBaseEdgeProperties>> >::get_mutable_instance().export_guid();
			}
		}
	}
}
