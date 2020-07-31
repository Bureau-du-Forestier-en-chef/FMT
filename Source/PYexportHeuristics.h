/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTHEURISTICS_H_INCLUDED
#define PYEXPORTHEURISTICS_H_INCLUDED


#include "FMToperatingarea.h"
#include "FMToperatingareascheduler.h"
#include "FMToperatingareaclusterer.h"
#include "FMToperatingareascheme.h"
#include "FMToperatingareacluster.h"
#include "FMTlpsolver.h"
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
		.def(bp::init<const Core::FMTmask&, const double&>())
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

	bp::class_<Heuristics::FMToperatingareascheme,bp::bases<Heuristics::FMToperatingarea>>("FMToperatingareascheme", "@DocString(FMToperatingareascheme)")
		.def(bp::init<const Heuristics::FMToperatingarea&,const size_t&,const size_t&,
			const size_t&, const size_t&, const size_t&>());

	define_pylist<Heuristics::FMToperatingareascheme>();

	bp::class_<Heuristics::FMToperatingareacluster>("FMToperatingareacluster", "@DocString(FMToperatingareacluster)")
        .def(bp::init<const Heuristics::FMToperatingareacluster&,const double&, const double&>())
        .def("getfilteredcluster",&Heuristics::FMToperatingareacluster::getfilteredcluster,
             "@DocString(FMToperatingareaheuristic::getfilteredcluster)")
        .def("getallmasks",&Heuristics::FMToperatingareacluster::getallmasks,
             "@DocString(FMToperatingareaheuristic::getallmasks)")
        .def("setminimalarea",&Heuristics::FMToperatingareacluster::setminimalarea,
             "@DocString(FMToperatingareaheuristic::setminimalarea)")
        .def("setmaximalarea",&Heuristics::FMToperatingareacluster::setmaximalarea,
             "@DocString(FMToperatingareaheuristic::setmaximalarea)");

	define_pylist<Heuristics::FMToperatingareacluster>();

	bp::class_<Heuristics::FMTlpheuristic, bp::bases<Core::FMTobject, Models::FMTlpsolver>>("FMTlpheuristic", "@DocString(Heuristics::FMTlpheuristic)");

    bp::class_<Heuristics::FMToperatingareaclusterer, bp::bases<Heuristics::FMTlpheuristic>>("Heuristics::FMToperatingareaclusterer", "@DocString(FMToperatingareaclusterer)")
		.def("initialsolve", &Heuristics::FMToperatingareaclusterer::initialsolve,
			"@DocString(FMToperatingareaclusterer::initialsolve)")
		.def("branchnboundsolve", &Heuristics::FMToperatingareaclusterer::branchnboundsolve,
			"@DocString(FMToperatingareaclusterer::branchnboundsolve)")
		.def("getsolution", &Heuristics::FMToperatingareaclusterer::getsolution,
			"@DocString(FMToperatingareaclusterer::getsolution)");


	bp::class_<Heuristics::FMToperatingareascheduler, bp::bases<Heuristics::FMTlpheuristic>>("Heuristics::FMToperatingareascheduler", "@DocString(FMToperatingareascheduler)")
		.def("initialsolve", &Heuristics::FMToperatingareascheduler::initialsolve,
			"@DocString(FMToperatingareaheuristic::initialsolve)")
		.def("branchnboundsolve", &Heuristics::FMToperatingareascheduler::branchnboundsolve,
			"@DocString(FMToperatingareaheuristic::branchnboundsolve)")
		.def("getsolution", &Heuristics::FMToperatingareascheduler::getsolution,
			"@DocString(FMToperatingareaheuristic::getsolution)");

	define_pylist<Heuristics::FMToperatingareascheduler>();
	#endif
	}
}
#endif
