/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTgraph.hpp"


    

namespace boost
{
	namespace archive
	{
		namespace detail
		{
			namespace extra_detail
			{
				template<> struct init_guid<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>>
				{
					static guid_initializer<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>> const & g;
				};
				template<> struct init_guid<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>>
				{
					static guid_initializer<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>> const & g;
				};

				guid_initializer<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>> const & init_guid<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>>::g = ::boost::serialization::singleton< guid_initializer<Graph::FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>> >::get_mutable_instance().export_guid();
				guid_initializer<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>> const & init_guid<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>>::g = ::boost::serialization::singleton< guid_initializer<Graph::FMTgraph<Graph::FMTbasevertexproperties, Graph::FMTbaseedgeproperties>> >::get_mutable_instance().export_guid();
			}
		}
	}
}
