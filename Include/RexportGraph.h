/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTGRAPH_H_INCLUDED
#define REXPORTGRAPH_H_INCLUDED

#include "Rdefinitions.h"
#include "FMTgraphstats.h"
//#include "FMTGraph.hpp"
#include <Rcpp.h>

RCPP_EXPOSED_ENUM_NODECL(Core::FMToutputlevel);
RCPP_EXPOSED_WRAP(Graph::FMTGraphStats);
RCPP_EXPOSED_AS(Graph::FMTGraphStats);
RCPP_EXPOSED_ENUM_NODECL(Graph::FMTgraphbuild);
//RCPP_EXPOSED_WRAP(Graph::FMTGraph);
//RCPP_EXPOSED_AS(Graph::FMTGraph);

namespace R
{
void exportGraph()
	{
	Rcpp::class_<Graph::FMTGraphStats>("FMTgraphstats", "@DocString(FMTGraphStats)")
		.constructor("@DocString(FMTGraphStats())")
		.method("str", &Graph::FMTGraphStats::operator std::string,
			"@DocString(FMTGraphStats::operator std::string)")
		.field("cols", &Graph::FMTGraphStats::cols,
			"@DocString(FMTGraphStats::cols)")
		.field("rows", &Graph::FMTGraphStats::rows,
			"@DocString(FMTGraphStats::rows)")
		.field("vertices", &Graph::FMTGraphStats::vertices,
			"@DocString(FMTGraphStats::vertices)")
		.field("edges", &Graph::FMTGraphStats::edges,
			"@DocString(FMTGraphStats::edges)")
		.field("transfer_rows", &Graph::FMTGraphStats::transfer_rows,
			"@DocString(FMTGraphStats::transfer_rows)")
		.field("output_rows", &Graph::FMTGraphStats::output_rows,
			"@DocString(FMTGraphStats::output_rows)")
		.field("output_cols", &Graph::FMTGraphStats::output_cols,
			"@DocString(FMTGraphStats::output_cols)");

	//Rcpp::class_<Graph::FMTGraph>("FMTgraph", "@DocString(FMTGraph)")
	//	.constructor("@DocString(FMTGraph())");

	}

}

#endif // PYEXPORTGRAPH_H_INCLUDED
