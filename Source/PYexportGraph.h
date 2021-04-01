/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTGRAPH_H_INCLUDED
#define PYEXPORTGRAPH_H_INCLUDED

#include "Rdefinitions.h"
#include "FMTgraphstats.h"
#include "FMTgraph.h"
#include "boost/python.hpp"

namespace Python 
{
void exportGraph()
{
	namespace bp = boost::python;
	bp::object GraphModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.Graph"))));
	bp::scope().attr("Graph") = GraphModule;
	bp::scope GraphModule_scope = GraphModule;
	bp::scope().attr("__doc__") = ".. module:: Graph\n"
		"\n"
		"   :platform: Unix, Windows\n"
		"   :synopsis: Module used to generate graph.\n"
		"\n";

	bp::enum_<Graph::FMToutputlevel>("FMToutputlevel")
		.value("standard", Graph::FMToutputlevel::standard)
		.value("totalonly", Graph::FMToutputlevel::totalonly)
		.value("developpement", Graph::FMToutputlevel::developpement)
		.export_values();

	bp::class_<Graph::FMTgraphstats>("FMTgraphstats", "@DocString(FMTgraphstats)")
		.def("__str__", &Graph::FMTgraphstats::operator std::string,
			"@DocString(FMTgraphstats::operator std::string)")
		.def_readwrite("cols", &Graph::FMTgraphstats::cols,
			"@DocString(FMTgraphstats::cols)")
		.def_readwrite("rows", &Graph::FMTgraphstats::rows,
			"@DocString(FMTgraphstats::rows)")
		.def_readwrite("vertices", &Graph::FMTgraphstats::vertices,
			"@DocString(FMTgraphstats::vertices)")
		.def_readwrite("edges", &Graph::FMTgraphstats::edges,
			"@DocString(FMTgraphstats::edges)")
		.def_readwrite("transfer_rows", &Graph::FMTgraphstats::transfer_rows,
			"@DocString(FMTgraphstats::transfer_rows)")
		.def_readwrite("output_rows", &Graph::FMTgraphstats::output_rows,
			"@DocString(FMTgraphstats::output_rows)")
		.def_readwrite("output_cols", &Graph::FMTgraphstats::output_cols,
			"@DocString(FMTgraphstats::output_cols)");

	bp::class_<Graph::FMTcarbonpredictor>("FMTcarbonpredictor", "@DocString(FMTcarbonpredictor)")
		.def("getpredictors", &Graph::FMTcarbonpredictor::getpredictors,
			"@DocString(FMTcarbonpredictor::getpredictors)")
		.def("getpredictorsmap", &Graph::FMTcarbonpredictor::getpredictorsmap,
			"@DocString(FMTcarbonpredictor::getpredictorsmap)");

	define_pylist<Graph::FMTcarbonpredictor>();

	bp::enum_<Graph::FMTgraphbuild>("FMTsolverinterface")
		.value("schedulebuild", Graph::FMTgraphbuild::schedulebuild)
		.value("fullbuild", Graph::FMTgraphbuild::fullbuild)
		.value("nobuild", Graph::FMTgraphbuild::nobuild);

	//bp::class_<Graph::FMTgraph>("FMTgraph", "@DocString(FMTgraph)")
	//	.def_pickle(FMT_pickle_suite<Graph::FMTgraph>());

}

}

#endif // PYEXPORTGRAPH_H_INCLUDED
