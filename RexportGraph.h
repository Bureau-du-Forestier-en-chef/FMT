/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef REXPORTGRAPH_H_INCLUDED
#define REXPORTGRAPH_H_INCLUDED

#include "Rdefinitions.h"
#include "FMTgraphstats.h"
#include "FMTgraph.h"
#include <Rcpp.h>

RCPP_EXPOSED_ENUM_NODECL(Graph::FMToutputlevel);
RCPP_EXPOSED_WRAP(Graph::FMTgraphstats);
RCPP_EXPOSED_AS(Graph::FMTgraphstats);
RCPP_EXPOSED_ENUM_NODECL(Graph::FMTgraphbuild);
RCPP_EXPOSED_WRAP(Graph::FMTgraph);
RCPP_EXPOSED_AS(Graph::FMTgraph);

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

	Rcpp::class_<Graph::FMTgraph>("FMTgraph", "@DocString(FMTgraph)")
		.constructor("@DocString(FMTgraph())");

	}

}

#endif // PYEXPORTGRAPH_H_INCLUDED
