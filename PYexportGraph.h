#ifndef PYEXPORTGRAPH_H_INCLUDED
#define PYEXPORTGRAPH_H_INCLUDED


//#include "FMTgraph.h"

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
	const char* py_FMTgraphstats_doc =
		" ``FMTgraphstats`` class.\n"
		"\n"
		"Graph stats\n"
		"\n";

	class_<Graph::FMTgraphstats>("FMTgraphstats", py_FMTgraphstats_doc)
		.def_readwrite("cols", &Graph::FMTgraphstats::cols)
		.def_readwrite("rows", &Graph::FMTgraphstats::rows)
		.def_readwrite("vertices", &Graph::FMTgraphstats::vertices)
		.def_readwrite("edges", &Graph::FMTgraphstats::edges)
		.def_readwrite("transfer_rows", &Graph::FMTgraphstats::transfer_rows)
		.def_readwrite("output_rows", &Graph::FMTgraphstats::output_rows)
		.def_readwrite("output_cols", &Graph::FMTgraphstats::output_cols);

	enum_<Graph::FMTgraphbuild>("FMTsolverinterface")
		.value("schedulebuild", Graph::FMTgraphbuild::schedulebuild)
		.value("fullbuild", Graph::FMTgraphbuild::fullbuild)
		.value("nobuild", Graph::FMTgraphbuild::nobuild);

	class_<Graph::FMTgraph>("FMTgraph")
		.def_pickle(FMT_pickle_suite<FMTgraph>());


	//define_FMTgraph<Graph::FMTsolverinterface::CLP>();

}

#endif // PYEXPORTGRAPH_H_INCLUDED
