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

#ifndef PYEXPORTHEURISTICS_H_INCLUDED
#define PYEXPORTHEURISTICS_H_INCLUDED


#include "FMToperatingarea.h"
#include "FMToperatingareaheuristic.h"
#include "PYdefinitions.h"
#include "boost/python.hpp"

namespace Python
{
void exportHeuristics()
{
	namespace bp = boost::python;
	bp::object HeuristicsModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.Heuristics"))));
	bp::scope().attr("Heuristics") = HeuristicsModule;
	bp::scope HeuristicsModule_scope = HeuristicsModule;
	bp::scope().attr("__doc__") = ".. module:: Heuristics\n"
		"\n"
		"   :platform: Unix, Windows\n"
		"   :synopsis: Module used for Heuristics.\n"
		"\n";
	#ifdef FMTWITHOSI

	bp::class_<Heuristics::FMToperatingarea>("FMToperatingarea", "@DocString(FMToperatingarea)")
		.def(bp::init<const Core::FMTmask&, const size_t&, const size_t&, const size_t&, const size_t&, const double&, const double&>())
		.def_pickle(FMT_pickle_suite<Heuristics::FMToperatingarea>())
		.def("getneighbors", &Heuristics::FMToperatingarea::getneighbors,
			"@DocString(FMToperatingarea::getneighbors)")
		.def("setneighbors", &Heuristics::FMToperatingarea::setneighbors,
			"@DocString(FMToperatingarea::setneighbors)")
		.def("getmask", &Heuristics::FMToperatingarea::getmask,
			"@DocString(FMToperatingarea::getmask)")
		.def("getneihgborsperimeter", &Heuristics::FMToperatingarea::getneihgborsperimeter,
			"@DocString(FMToperatingarea::getneihgborsperimeter)");
		
	define_pylist<Heuristics::FMToperatingarea>();

	bp::class_<Heuristics::FMToperatingareaheuristic, bp::bases<Core::FMTobject>>("FMToperatingareaheuristic", "@DocString(FMToperatingareaheuristic)")
		.def_pickle(FMT_pickle_suite<Heuristics::FMToperatingareaheuristic>())
		.def("initialsolve", &Heuristics::FMToperatingareaheuristic::initialsolve,
			"@DocString(FMToperatingareaheuristic::initialsolve)")
		.def("branchnboundsolve", &Heuristics::FMToperatingareaheuristic::branchnboundsolve,
			"@DocString(FMToperatingareaheuristic::branchnboundsolve)")
		.def("isfeasible", &Heuristics::FMToperatingareaheuristic::isfeasible,
			"@DocString(FMToperatingareaheuristic::isfeasible)")
		.def("getobjective", &Heuristics::FMToperatingareaheuristic::getobjective,
			"@DocString(FMToperatingareaheuristic::getobjective)")
		.def("getsolution", &Heuristics::FMToperatingareaheuristic::getsolution,
			"@DocString(FMToperatingareaheuristic::getsolution)");

	define_pylist<Heuristics::FMToperatingareaheuristic>();
	#endif 
	}
}
#endif 
