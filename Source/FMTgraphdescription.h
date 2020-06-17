/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTgraphdescription_H_INCLUDED
#define FMTgraphdescription_H_INCLUDED

#include "FMTvertexproperties.h"
#include "FMTedgeproperties.h"
#include "FMTdevelopment.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adj_list_serialize.hpp>


namespace Graph
{

	typedef boost::adjacency_list< boost::listS,
		boost::listS,
		boost::bidirectionalS,
		FMTvertexproperties,
		FMTedgeproperties,
		boost::no_property,
		boost::listS>FMTadjacency_list;
	//https://www.boost.org/doc/libs/1_47_0/libs/graph/doc/adjacency_list.html

	typedef boost::graph_traits<FMTadjacency_list>::vertex_descriptor FMTvertex_descriptor;
	typedef boost::graph_traits<FMTadjacency_list>::edge_descriptor FMTedge_descriptor;
	typedef boost::graph_traits<FMTadjacency_list>::in_edge_iterator FMTinedge_iterator;
	typedef boost::graph_traits<FMTadjacency_list>::out_edge_iterator FMToutedge_iterator;
	typedef boost::graph_traits<FMTadjacency_list>::vertex_iterator FMTvertex_iterator;
	typedef boost::graph_traits<FMTadjacency_list>::edge_iterator FMTedge_iterator;
	typedef std::pair<FMToutedge_iterator, FMToutedge_iterator> FMToutedge_pair;
	typedef std::pair<FMTvertex_iterator,FMTvertex_iterator> FMTvertex_pair;


}

#endif
