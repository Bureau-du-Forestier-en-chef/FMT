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
	const char* py_FMToperatingarea_doc =
		" ``FMToperatingarea`` class.\n"
		"\n"
		"Operating area for FMTlpmodel\n"
		"\n";
	class_<Heuristics::FMToperatingarea>("FMToperatingarea", py_FMToperatingarea_doc)
		.def(init<const FMTmask&, const size_t&, const size_t&, const size_t&, const size_t&, const double&, const double&>())
		.def_pickle(FMT_pickle_suite<Heuristics::FMToperatingarea>())
		.def("getneighbors", &Heuristics::FMToperatingarea::getneighbors)
		.def("setneighbors", &Heuristics::FMToperatingarea::setneighbors)
		.def("getmask", &Heuristics::FMToperatingarea::getmask)
		.def("getneihgborsperimeter", &Heuristics::FMToperatingarea::getneihgborsperimeter);
		
	define_pylist<Heuristics::FMToperatingarea>();

	const char* py_FMToperatingareaheuristic_doc =
		" ``FMToperatingareaheuristic`` class.\n"
		"\n"
		"Model class used to do operating area opening optimization\n"
		"Uses schedule to optimize operating area schedule\n"
		"\n";

	class_<Heuristics::FMToperatingareaheuristic, bases<Core::FMTobject>>("FMToperatingareaheuristic", py_FMToperatingareaheuristic_doc)
		.def_pickle(FMT_pickle_suite<Heuristics::FMToperatingareaheuristic>())
		.def("initialsolve", &Heuristics::FMToperatingareaheuristic::initialsolve)
		.def("branchnboundsolve", &Heuristics::FMToperatingareaheuristic::branchnboundsolve)
		.def("isfeasible", &Heuristics::FMToperatingareaheuristic::isfeasible)
		.def("getobjective", &Heuristics::FMToperatingareaheuristic::getobjective)
		.def("getsolution", &Heuristics::FMToperatingareaheuristic::getsolution);

	define_pylist<Heuristics::FMToperatingareaheuristic>();


}

#endif 