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

using namespace boost;

namespace Graph
{

	typedef adjacency_list<listS,//vecS,
		listS,//vecS,hash_mapS
		bidirectionalS,//directedS, for in_edges crap...
		FMTvertexproperties,
		FMTedgeproperties,
		no_property,
		listS>FMTadjacency_list;//using list for fast add and remove vec for size overhead when using vector when vertex removed then vertexdescriptor are fucked
	//https://www.boost.org/doc/libs/1_47_0/libs/graph/doc/adjacency_list.html

	typedef adjacency_matrix<undirectedS,
		FMTvertexproperties,
		FMTedgeproperties>FMTadjacency_matrix;

	typedef labeled_graph<FMTadjacency_list,//FMTadjacency_matrix,//
		FMTdevelopment,
		hash_mapS>FMTgraphdata; //labeling the vertex...by hashing the FMTdeveloppement?!?!?!?!

	typedef graph_traits<FMTgraphdata>::vertex_descriptor FMTvertex_descriptor;
	typedef graph_traits<FMTgraphdata>::edge_descriptor FMTedge_descriptor;
	typedef graph_traits<FMTgraphdata>::in_edge_iterator FMTinedge_iterator;
	typedef graph_traits<FMTgraphdata>::out_edge_iterator FMToutedge_iterator;
	typedef graph_traits<FMTgraphdata>::vertex_iterator FMTvertex_iterator;
	typedef graph_traits<FMTgraphdata>::edge_iterator FMTedge_iterator;
	typedef std::pair<FMToutedge_iterator, FMToutedge_iterator> FMToutedge_pair;


}

#endif