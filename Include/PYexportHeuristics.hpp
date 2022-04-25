/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTHEURISTICS_H_INCLUDED
#define PYEXPORTHEURISTICS_H_INCLUDED


#include "FMToperatingarea.hpp"
#include "FMToperatingareascheduler.hpp"
#include "FMToperatingareaclusterer.hpp"
#include "FMToperatingareascheme.hpp"
#include "FMToperatingareacluster.hpp"
#include "FMTlpheuristicmthandler.hpp"
#include "FMTlpsolver.hpp"
#include "PYdefinitions.hpp"
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

	bp::class_<Heuristics::FMTlpheuristicmthandler>("FMTlpheuristicmthandler", "@DocString(FMTlpheuristicmthandler)")
		//.def(bp::init<std::vector<Heuristics::FMToperatingareascheduler>&>())
		//.def(bp::init<std::vector<Heuristics::FMToperatingareaclusterer>&>())
		.def(bp::init<boost::python::list&,const double&>())
		.def("initialsolve", &Heuristics::FMTlpheuristicmthandler::initialsolve,
					"@DocString(FMTlpheuristicmthandler::initialsolve)")
		.def("greedysolve", &Heuristics::FMTlpheuristicmthandler::greedysolve,
					"@DocString(FMTlpheuristicmthandler::greedysolve)");

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
			const size_t&, const size_t&, const size_t&>())
		.def("presolve", &Heuristics::FMToperatingareascheme::presolve,
			"@DocString(FMToperatingareascheme::presolve)");

	define_pylist<Heuristics::FMToperatingareascheme>();

	bp::class_<Heuristics::FMToperatingareaclusterbinary,bp::bases<Heuristics::FMToperatingarea>>("FMToperatingareaclusterbinary", "@DocString(FMToperatingareaclusterbinary)");

	define_pylist<Heuristics::FMToperatingareaclusterbinary>();

	bp::class_<Heuristics::FMToperatingareacluster>("FMToperatingareacluster", "@DocString(FMToperatingareacluster)")
        .def(bp::init<const Heuristics::FMToperatingareacluster&,const double&, const double&>())
        .def("getfilteredcluster",&Heuristics::FMToperatingareacluster::getfilteredcluster,
             "@DocString(FMToperatingareacluster::getfilteredcluster)")
        .def("getallmasks",&Heuristics::FMToperatingareacluster::getallmasks,
             "@DocString(FMToperatingareacluster::getallmasks)")
		.def("getcentroid", &Heuristics::FMToperatingareacluster::getcentroid,
			"@DocString(FMToperatingareacluster::getcentroid)")
        .def("setminimalarea",&Heuristics::FMToperatingareacluster::setminimalarea,
             "@DocString(FMToperatingareacluster::setminimalarea)")
        .def("setmaximalarea",&Heuristics::FMToperatingareacluster::setmaximalarea,
             "@DocString(FMToperatingareacluster::setmaximalarea)");

	define_pylist<Heuristics::FMToperatingareacluster>();

	bp::class_<Heuristics::FMTlpheuristic, bp::bases<Core::FMTobject, Models::FMTlpsolver>>("FMTlpheuristic", "@DocString(Heuristics::FMTlpheuristic)");

    bp::class_<Heuristics::FMToperatingareaclusterer, bp::bases<Heuristics::FMTlpheuristic>>("Heuristics::FMToperatingareaclusterer", "@DocString(FMToperatingareaclusterer)")
		.def("initialsolve", &Heuristics::FMToperatingareaclusterer::initialsolve,
			"@DocString(FMToperatingareaclusterer::initialsolve)")
		.def("branchnboundsolve", &Heuristics::FMToperatingareaclusterer::branchnboundsolve,
			"@DocString(FMToperatingareaclusterer::branchnboundsolve)")
		.def("setnumberofsimulationpass", &Heuristics::FMToperatingareaclusterer::setnumberofsimulationpass,
			"@DocString(FMToperatingareaclusterer::setnumberofsimulationpass)")
		.def("getsolution", &Heuristics::FMToperatingareaclusterer::getsolution,
			"@DocString(FMToperatingareaclusterer::getsolution)");

	define_pylist<Heuristics::FMToperatingareaclusterer>();

	bp::class_<Heuristics::FMToperatingareascheduler, bp::bases<Heuristics::FMTlpheuristic>>("Heuristics::FMToperatingareascheduler", "@DocString(FMToperatingareascheduler)")
		.def("initialsolve", &Heuristics::FMToperatingareascheduler::initialsolve,
			"@DocString(FMToperatingareascheduler::initialsolve)")
		.def("branchnboundsolve", &Heuristics::FMToperatingareascheduler::branchnboundsolve,
			"@DocString(FMToperatingareascheduler::branchnboundsolve)")
		.def("getsolution", &Heuristics::FMToperatingareascheduler::getsolution,
			"@DocString(FMToperatingareaheuristic::getsolution)")
		.def("setasprimal", &Heuristics::FMToperatingareascheduler::setasprimal,
			"@DocString(FMToperatingareascheduler::setasprimal)")
		.def("setproportionofset", &Heuristics::FMToperatingareascheduler::setproportionofset,
			"@DocString(FMToperatingareascheduler::setproportionofset)");

	define_pylist<Heuristics::FMToperatingareascheduler>();
	#endif
	}
}
#endif
