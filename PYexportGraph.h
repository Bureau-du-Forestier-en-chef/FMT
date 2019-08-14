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

	enum_<Graph::FMToutputlevel>("FMToutputlevel")
		.value("standard", Graph::FMToutputlevel::standard)
		.value("totalonly", Graph::FMToutputlevel::totalonly)
		.value("developpement", Graph::FMToutputlevel::developpement)
		.export_values();

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
