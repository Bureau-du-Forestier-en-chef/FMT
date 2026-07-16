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
//#include "FMTlpheuristicmthandler.h"
#include "FMTlpsolver.h"
#include "PYdefinitions.h"
#include "boost/python.hpp"
#include "FMTpythonpickle.h"

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

	/*bp::class_<Heuristics::FMTlpheuristicmthandler>("FMTlpheuristicmthandler", "@DocString(FMTlpheuristicmthandler)")
		//.def(bp::init<std::vector<Heuristics::FMToperatingareascheduler>&>())
		//.def(bp::init<std::vector<Heuristics::FMToperatingareaclusterer>&>())
		.def(bp::init<boost::python::list&,const double&>())
		.def("initialSolve", &Heuristics::FMTlpheuristicmthandler::initialSolve,
					"@DocString(FMTlpheuristicmthandler::initialSolve)")
		.def("greedySolve", &Heuristics::FMTlpheuristicmthandler::greedySolve,
					"@DocString(FMTlpheuristicmthandler::greedySolve)");*/

	bp::class_<Heuristics::FMToperatingarea>("FMToperatingarea", "@DocString(FMToperatingarea)")
		.def(bp::init<const Core::FMTMask&, const double&>())
		.def_pickle(FMT_pickle_suite<Heuristics::FMToperatingarea>())
		.def("getneighbors", &Heuristics::FMToperatingarea::getNeighbors,
			"@DocString(FMToperatingarea::getneighbors)")
		.def("setneighbors", &Heuristics::FMToperatingarea::setNeighbors,
			"@DocString(FMToperatingarea::setneighbors)")
		.def("getmask", &Heuristics::FMToperatingarea::getMask,
			"@DocString(FMToperatingarea::getmask)")
		.def("getneihgborsperimeter", &Heuristics::FMToperatingarea::getNeighborsPerimeter,
			"@DocString(FMToperatingarea::getneihgborsperimeter)");

	definePyList<Heuristics::FMToperatingarea>();

	bp::class_<Heuristics::FMToperatingareascheme,bp::bases<Heuristics::FMToperatingarea>>("FMToperatingareascheme", "@DocString(FMToperatingareascheme)")
		.def(bp::init<const Heuristics::FMToperatingarea&,const size_t&,const size_t&,
			const size_t&, const size_t&, const size_t&, const size_t&, double>())
		.def("presolve", &Heuristics::FMToperatingareascheme::presolve,
			"@DocString(FMToperatingareascheme::presolve)");

	definePyList<Heuristics::FMToperatingareascheme>();

	bp::class_<Heuristics::FMToperatingareaclusterbinary,bp::bases<Heuristics::FMToperatingarea>>("FMToperatingareaclusterbinary", "@DocString(FMToperatingareaclusterbinary)");

	definePyList<Heuristics::FMToperatingareaclusterbinary>();

	bp::class_<Heuristics::FMToperatingareacluster>("FMToperatingareacluster", "@DocString(FMToperatingareacluster)")
        .def(bp::init<const Heuristics::FMToperatingareacluster&,const double&, const double&>())
        .def("getfilteredcluster",&Heuristics::FMToperatingareacluster::getFilteredCluster,
             "@DocString(FMToperatingareacluster::getfilteredcluster)")
        .def("getallmasks",&Heuristics::FMToperatingareacluster::getAllMasks,
             "@DocString(FMToperatingareacluster::getallmasks)")
		.def("getcentroid", &Heuristics::FMToperatingareacluster::getCentroid,
			"@DocString(FMToperatingareacluster::getcentroid)")
        .def("setminimalarea",&Heuristics::FMToperatingareacluster::setMinimalArea,
             "@DocString(FMToperatingareacluster::setminimalarea)")
        .def("setmaximalarea",&Heuristics::FMToperatingareacluster::setMaximalArea,
             "@DocString(FMToperatingareacluster::setmaximalarea)");

	definePyList<Heuristics::FMToperatingareacluster>();

	bp::class_<Heuristics::FMTlpheuristic, bp::bases<Core::FMTObject, Models::FMTlpsolver>>("FMTlpheuristic", "@DocString(Heuristics::FMTlpheuristic)");

    bp::class_<Heuristics::FMToperatingareaclusterer, bp::bases<Heuristics::FMTlpheuristic>>("Heuristics::FMToperatingareaclusterer", "@DocString(FMToperatingareaclusterer)")
		.def("initialsolve", &Heuristics::FMToperatingareaclusterer::initialSolve,
			"@DocString(FMToperatingareaclusterer::initialsolve)")
		.def("branchnboundsolve", &Heuristics::FMToperatingareaclusterer::branchNBoundSolve,
			"@DocString(FMToperatingareaclusterer::branchnboundsolve)")
		.def("setnumberofsimulationpass", &Heuristics::FMToperatingareaclusterer::setNumberOfSimulationPass,
			"@DocString(FMToperatingareaclusterer::setnumberofsimulationpass)")
		.def("getsolution", &Heuristics::FMToperatingareaclusterer::getSolution,
			"@DocString(FMToperatingareaclusterer::getsolution)");

	definePyList<Heuristics::FMToperatingareaclusterer>();

	bp::class_<Heuristics::FMToperatingareascheduler, bp::bases<Heuristics::FMTlpheuristic>>("Heuristics::FMToperatingareascheduler", "@DocString(FMToperatingareascheduler)")
		.def("initialsolve", &Heuristics::FMToperatingareascheduler::initialSolve,
			"@DocString(FMToperatingareascheduler::initialsolve)")
		.def("branchnboundsolve", &Heuristics::FMToperatingareascheduler::branchNBoundSolve,
			"@DocString(FMToperatingareascheduler::branchnboundsolve)")
		.def("getsolution", &Heuristics::FMToperatingareascheduler::getSolution,
			"@DocString(FMToperatingareaheuristic::getsolution)")
		.def("setasprimal", &Heuristics::FMToperatingareascheduler::setAsPrimal,
			"@DocString(FMToperatingareascheduler::setasprimal)")
		.def("setproportionofset", &Heuristics::FMToperatingareascheduler::setProportionOfSet,
			"@DocString(FMToperatingareascheduler::setproportionofset)");

	definePyList<Heuristics::FMToperatingareascheduler>();
	#endif
	}
}
#endif
