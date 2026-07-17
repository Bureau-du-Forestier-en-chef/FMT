/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTGRAPH_H_INCLUDED
#define PYEXPORTGRAPH_H_INCLUDED

#include "Rdefinitions.h"
#include "FMTgraphstats.h"
#include "FMTgraph.hpp"
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

	bp::class_<Graph::FMTGraphStats>("FMTgraphstats", "@DocString(FMTGraphStats)")
		.def("__str__", &Graph::FMTGraphStats::operator std::string,
			"@DocString(FMTGraphStats::operator std::string)")
		.def_readwrite("cols", &Graph::FMTGraphStats::cols,
			"@DocString(FMTGraphStats::cols)")
		.def_readwrite("rows", &Graph::FMTGraphStats::rows,
			"@DocString(FMTGraphStats::rows)")
		.def_readwrite("vertices", &Graph::FMTGraphStats::vertices,
			"@DocString(FMTGraphStats::vertices)")
		.def_readwrite("edges", &Graph::FMTGraphStats::edges,
			"@DocString(FMTGraphStats::edges)")
		.def_readwrite("transfer_rows", &Graph::FMTGraphStats::transfer_rows,
			"@DocString(FMTGraphStats::transfer_rows)")
		.def_readwrite("output_rows", &Graph::FMTGraphStats::output_rows,
			"@DocString(FMTGraphStats::output_rows)")
		.def_readwrite("output_cols", &Graph::FMTGraphStats::output_cols,
			"@DocString(FMTGraphStats::output_cols)");

	bp::class_<Graph::FMTPredictor>("FMTpredictor", "@DocString(FMTPredictor)")
		.def("getpredictors", &Graph::FMTPredictor::getPredictors,
			"@DocString(FMTPredictor::getpredictors)")
		.def("getpredictornames", &Graph::FMTPredictor::getPredictorNames,
			"@DocString(FMTPredictor::getpredictornames)");

	definePyList<Graph::FMTPredictor>();
	definePyDict<int, int>();
	definePyList<std::vector<Graph::FMTPredictor>>();
	bp::enum_<Graph::FMTgraphbuild>("FMTsolverinterface")
		.value("schedulebuild", Graph::FMTgraphbuild::schedulebuild)
		.value("fullbuild", Graph::FMTgraphbuild::fullbuild)
		.value("nobuild", Graph::FMTgraphbuild::nobuild);

	//bp::class_<Graph::FMTGraph>("FMTgraph", "@DocString(FMTGraph)")
	//	.def_pickle(FMT_pickle_suite<Graph::FMTGraph>());

	bp::class_<Graph::FMTLineGraph>("FMTlinegraph","@DocString(FMTGraph)")
		.def("getperiodactionids",&Graph::FMTLineGraph::getPeriodActionIds,"@DocString(FMTGraphStats::getperiodactionids)");

}

}

#endif // PYEXPORTGRAPH_H_INCLUDED
