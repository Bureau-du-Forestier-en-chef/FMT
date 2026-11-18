/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTHEURISTICS_H_INCLUDED
#define PYEXPORTHEURISTICS_H_INCLUDED


#include "FMTOperatingArea.h"
#include "FMTOperatingAreaScheduler.h"
#include "FMTOperatingAreaClusterer.h"
#include "FMTOperatingAreaScheme.h"
#include "FMTOperatingAreaCluster.h"
//#include "FMTLpHeuristicMtHandler.h"
#include "FMTLpSolver.h"
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

	bp::class_<Heuristics::FMTOperatingArea>("FMToperatingarea", "@DocString(FMTOperatingArea)")
		.def(bp::init<const Core::FMTMask&, const double&>())
		.def_pickle(FMT_pickle_suite<Heuristics::FMTOperatingArea>())
		.def("getneighbors", &Heuristics::FMTOperatingArea::getNeighbors,
			"@DocString(FMTOperatingArea::getneighbors)")
		.def("setneighbors", &Heuristics::FMTOperatingArea::setNeighbors,
			"@DocString(FMTOperatingArea::setneighbors)")
		.def("getmask", &Heuristics::FMTOperatingArea::getMask,
			"@DocString(FMTOperatingArea::getmask)")
		.def("getneihgborsperimeter", &Heuristics::FMTOperatingArea::getNeighborsPerimeter,
			"@DocString(FMTOperatingArea::getneihgborsperimeter)");

	definePyList<Heuristics::FMTOperatingArea>();

	bp::class_<Heuristics::FMTOperatingAreaScheme,bp::bases<Heuristics::FMTOperatingArea>>(
		"FMToperatingareascheme", "@DocString(FMTOperatingAreaScheme)")
		.def_pickle(FMT_pickle_suite<Heuristics::FMTOperatingAreaScheme>())
		.def(bp::init<const Heuristics::FMTOperatingArea&,const size_t&,const size_t&,
			const size_t&, const size_t&, const size_t&, const size_t&, double>())
		.def("presolve", &Heuristics::FMTOperatingAreaScheme::preSolve,
			"@DocString(FMTOperatingAreaScheme::preSolve)");

	definePyList<Heuristics::FMTOperatingAreaScheme>();

	bp::class_<Heuristics::FMTOperatingAreaClusterBinary,bp::bases<Heuristics::FMTOperatingArea>>
		("FMToperatingareaclusterbinary", "@DocString(FMTOperatingAreaClusterBinary)")
		.def_pickle(FMT_pickle_suite<Heuristics::FMTOperatingAreaClusterBinary>());

	definePyList<Heuristics::FMTOperatingAreaClusterBinary>();

	bp::class_<Heuristics::FMTOperatingAreaCluster>("FMToperatingareacluster", "@DocString(FMTOperatingAreaCluster)")
        .def(bp::init<const Heuristics::FMTOperatingAreaCluster&,const double&, const double&>())
		.def_pickle(FMT_pickle_suite<Heuristics::FMTOperatingAreaCluster>())
        .def("getfilteredcluster",&Heuristics::FMTOperatingAreaCluster::getFilteredCluster,
             "@DocString(FMTOperatingAreaCluster::getfilteredcluster)")
        .def("getallmasks",&Heuristics::FMTOperatingAreaCluster::getAllMasks,
             "@DocString(FMTOperatingAreaCluster::getallmasks)")
		.def("getcentroid", &Heuristics::FMTOperatingAreaCluster::getCentroid,
			"@DocString(FMTOperatingAreaCluster::getcentroid)")
        .def("setminimalarea",&Heuristics::FMTOperatingAreaCluster::setMinimalArea,
             "@DocString(FMTOperatingAreaCluster::setminimalarea)")
        .def("setmaximalarea",&Heuristics::FMTOperatingAreaCluster::setMaximalArea,
             "@DocString(FMTOperatingAreaCluster::setmaximalarea)");

	definePyList<Heuristics::FMTOperatingAreaCluster>();

	bp::class_<Heuristics::FMTLpHeuristic, bp::bases<Core::FMTObject, Models::FMTLpSolver>>(
		"FMTlpheuristic", "@DocString(Heuristics::FMTLpHeuristic)")
		.def_pickle(FMT_pickle_suite<Heuristics::FMTLpHeuristic>());

    bp::class_<Heuristics::FMTOperatingAreaClusterer, bp::bases<Heuristics::FMTLpHeuristic>>(
		"Heuristics::FMToperatingareaclusterer", "@DocString(FMTOperatingAreaClusterer)")
		.def_pickle(FMT_pickle_suite<Heuristics::FMTOperatingAreaClusterer>())
		.def("initialsolve", &Heuristics::FMTOperatingAreaClusterer::initialSolve,
			"@DocString(FMTOperatingAreaClusterer::initialsolve)")
		.def("branchnboundsolve", &Heuristics::FMTOperatingAreaClusterer::branchNBoundSolve,
			"@DocString(FMTOperatingAreaClusterer::branchnboundsolve)")
		.def("setnumberofsimulationpass", &Heuristics::FMTOperatingAreaClusterer::setNumberOfSimulationPass,
			"@DocString(FMTOperatingAreaClusterer::setnumberofsimulationpass)")
		.def("getsolution", &Heuristics::FMTOperatingAreaClusterer::getSolution,
			"@DocString(FMTOperatingAreaClusterer::getsolution)");

	definePyList<Heuristics::FMTOperatingAreaClusterer>();

	bp::class_<Heuristics::FMTOperatingAreaScheduler, bp::bases<Heuristics::FMTLpHeuristic>>(
		"Heuristics::FMToperatingareascheduler", "@DocString(FMTOperatingAreaScheduler)")
		.def_pickle(FMT_pickle_suite<Heuristics::FMTOperatingAreaScheduler>())
		.def("initialsolve", &Heuristics::FMTOperatingAreaScheduler::initialSolve,
			"@DocString(FMTOperatingAreaScheduler::initialsolve)")
		.def("branchnboundsolve", &Heuristics::FMTOperatingAreaScheduler::branchNBoundSolve,
			"@DocString(FMTOperatingAreaScheduler::branchnboundsolve)")
		.def("getsolution", &Heuristics::FMTOperatingAreaScheduler::getSolution,
			"@DocString(FMToperatingareaheuristic::getsolution)")
		.def("setasprimal", &Heuristics::FMTOperatingAreaScheduler::setAsPrimal,
			"@DocString(FMTOperatingAreaScheduler::setasprimal)")
		.def("setproportionofset", &Heuristics::FMTOperatingAreaScheduler::setProportionOfSet,
			"@DocString(FMTOperatingAreaScheduler::setproportionofset)");

	definePyList<Heuristics::FMTOperatingAreaScheduler>();
	#endif
	}
}
#endif
