/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTGRAPH_H_INCLUDED
#define REXPORTGRAPH_H_INCLUDED

#include "Rdefinitions.h"
#include "FMTgraphstats.h"
//#include "FMTgraph.hpp"
#include <Rcpp.h>

RCPP_EXPOSED_ENUM_NODECL(Core::FMToutputlevel);
RCPP_EXPOSED_WRAP(Graph::FMTgraphstats);
RCPP_EXPOSED_AS(Graph::FMTgraphstats);
RCPP_EXPOSED_ENUM_NODECL(Graph::FMTgraphbuild);
//RCPP_EXPOSED_WRAP(Graph::FMTgraph);
//RCPP_EXPOSED_AS(Graph::FMTgraph);

namespace R
{
void exportGraph()
	{
	Rcpp::class_<Graph::FMTgraphstats>("FMTgraphstats", "@DocString(FMTgraphstats)")
		.constructor("@DocString(FMTgraphstats())")
		.method("str", &Graph::FMTgraphstats::operator std::string,
			"@DocString(FMTgraphstats::operator std::string)")
		.field("cols", &Graph::FMTgraphstats::cols,
			"@DocString(FMTgraphstats::cols)")
		.field("rows", &Graph::FMTgraphstats::rows,
			"@DocString(FMTgraphstats::rows)")
		.field("vertices", &Graph::FMTgraphstats::vertices,
			"@DocString(FMTgraphstats::vertices)")
		.field("edges", &Graph::FMTgraphstats::edges,
			"@DocString(FMTgraphstats::edges)")
		.field("transfer_rows", &Graph::FMTgraphstats::transfer_rows,
			"@DocString(FMTgraphstats::transfer_rows)")
		.field("output_rows", &Graph::FMTgraphstats::output_rows,
			"@DocString(FMTgraphstats::output_rows)")
		.field("output_cols", &Graph::FMTgraphstats::output_cols,
			"@DocString(FMTgraphstats::output_cols)");

	//Rcpp::class_<Graph::FMTgraph>("FMTgraph", "@DocString(FMTgraph)")
	//	.constructor("@DocString(FMTgraph())");

	}

}

#endif // PYEXPORTGRAPH_H_INCLUDED
