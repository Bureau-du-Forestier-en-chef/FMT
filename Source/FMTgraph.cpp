/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#include "FMTgraph.h"

namespace Graph
{
    template<> std::map<int, int> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getoutvariables(const FMTvertex_descriptor& out_vertex) const
	{
		std::map<int, int> mapping;
		FMToutedge_pair edge_pair;
		for (edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
		{
			const FMTedgeproperties& edgeprop = data[*edge_pair.first];
			int actionid = edgeprop.getactionID();
			mapping[actionid] = edgeprop.getvariableID();
		}
		return mapping;
	}


 template<> std::vector<Core::FMTdevelopmentpath> FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::getpaths(const FMTvertex_descriptor& out_vertex,const int& actionID) const
 {
     std::vector<Core::FMTdevelopmentpath>paths;
     for (FMToutedge_pair edge_pair = boost::out_edges(out_vertex, data); edge_pair.first != edge_pair.second; ++edge_pair.first)
     {
         const FMTedgeproperties& edgeprop = data[*edge_pair.first];
         if (edgeprop.getactionID() == actionID)
         {
             const FMTbasevertexproperties& vertex_target = data[target(*edge_pair.first, data)];
             paths.push_back(Core::FMTdevelopmentpath(vertex_target.get(), edgeprop.getproportion()));
         }
     }
     return paths;
 }



 template<> double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::inarea(const FMTvertex_descriptor& out_vertex,const double*&  solution, bool growth) const
 {
     FMTinedge_iterator inedge_iterator, inedge_end;
     double area = 0;
     for (boost::tie(inedge_iterator, inedge_end) = boost::in_edges(out_vertex, data); inedge_iterator != inedge_end; ++inedge_iterator)
     {
         const FMTedgeproperties& edgeprop = data[*inedge_iterator];
         if (edgeprop.getactionID() < 0 || !growth)
         {
             area += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
         }
     }
     return area;
 }

 template<> double FMTgraph<Graph::FMTvertexproperties, Graph::FMTedgeproperties>::outarea(const FMTvertex_descriptor& out_vertex, const int& actionID, const double*&  solution) const
 {
     {
         FMToutedge_iterator outedge_iterator, outedge_end;
         double value = 0;
         for (boost::tie(outedge_iterator, outedge_end) = boost::out_edges(out_vertex, data); outedge_iterator != outedge_end; ++outedge_iterator)
         {
             const FMTedgeproperties& edgeprop = data[*outedge_iterator];
             if (edgeprop.getactionID() == actionID)
             {
                 value += *(solution + edgeprop.getvariableID()) * (edgeprop.getproportion() / 100);
             }
         }
         return value;
     }

 }
}

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
