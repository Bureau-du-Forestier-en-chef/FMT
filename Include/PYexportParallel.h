/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTParallel_H_INCLUDED
#define PYEXPORTParallel_H_INCLUDED

#include "FMTtask.h"
#include "FMTtaskhandler.h"
#include "FMTreplanningtask.h"
#include "FMTplanningtask.h"
#include "FMTopareaschedulertask.h"
#include "FMTmodel.h"
#include "FMToutput.h"
#include "FMTutility.h"
#include <string>
#include "boost/python.hpp"

namespace Python
{
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(push_back_overloads, push_back, 1, 3)

void exportParallel()
    {
    namespace bp = boost::python;
    bp::object ParallelModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.Parallel"))));
    bp::scope().attr("Parallel") = ParallelModule;
    bp::scope ParallelModule_scope = ParallelModule;
    bp::scope().attr("__doc__") = ".. module:: Parallel\n"
    "\n"
    "   :platform: Unix, Windows\n"
    "   :synopsis: Module used for FMT parallel tasks handling.\n"
    "\n";


	bp::class_<Parallel::FMTTask, bp::bases<Core::FMTObject>>("FMTtask", "@DocString(FMTTask)")
			.def(bp::init<Parallel::FMTTask>());

	definePyList<Parallel::FMTTask>();

	bp::class_<Parallel::FMTReplanningTask, bp::bases<Parallel::FMTTask>>("FMTreplanningtask", "@DocString(FMTReplanningTask)")
			.def(bp::init<Parallel::FMTReplanningTask>())
			.def(bp::init<const Models::FMTModel&, const Models::FMTModel&, const Models::FMTModel&,const std::vector<Core::FMTOutput>&,
				const std::string&, const std::string&,const std::vector<std::string>&,
				const int&,const int&,double,Core::FMToutputlevel,const bool>(
					bp::args("Strategic model", "stochastic model","tactic model","selected outputs","Output folder","GDAL driver name","GDAL driver creation options","Number of replicates","Number of replanning periods","minimaldrift","output level","Write schedule"), "@DocString(FMTReplanningTask::FMTReplanningTask(const Models::FMTModel&,const Models::FMTModel&,const Models::FMTModel&))"));

	definePyList<Parallel::FMTReplanningTask>();


	bp::class_<Parallel::FMTPlanningTask, bp::bases<Parallel::FMTTask>>("FMTplanningtask", "@DocString(FMTPlanningTask)")
		.def(bp::init<Parallel::FMTPlanningTask>())
		.def(bp::init<const int&, const int&, const std::string&, std::string,
			std::vector<std::string>, Core::FMToutputlevel, std::string>(
				bp::args("Min output period", "Max output period", "Output folder", "GDAL driver name", "GDAL driver creation options", "output level", "primary file localisation"), "@DocString(FMTPlanningTask::FMTPlanningTask(...))"))
		.def("push_back", &Parallel::FMTPlanningTask::push_back, push_back_overloads(bp::args("model", "schedules", "outputs"), "@DocString(FMTPlanningTask::push_back)"));

	definePyList<Parallel::FMTPlanningTask>();

	bp::class_<Parallel::FMTOpAreaSchedulerTask, bp::bases<Parallel::FMTTask>>("FMTopareaschedulertask", "@DocString(FMTOpAreaSchedulerTask")
		.def(bp::init<Parallel::FMTOpAreaSchedulerTask>())
		.def(bp::init<const Models::FMTLpModel&,const std::vector<Heuristics::FMTOperatingAreaScheme>&,
			const Core::FMTOutputNode&,const std::string&,
			const std::string&,const unsigned int&,
			const double&,Core::FMTOutput>(
				bp::args("model", "operating areas", "node", "output location", "output yield name", "maxiterations","maxtime","returntime_output"), "@DocString(FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask(...))"));

	definePyList<Parallel::FMTOpAreaSchedulerTask>();


	bp::class_<Parallel::FMTTaskHandler, bp::bases<Core::FMTObject>>("FMTtaskhandler", "@DocString(FMTTaskHandler)")
		.def(bp::init<Parallel::FMTTaskHandler>())
		.def(bp::init<const Parallel::FMTTask&, unsigned int>(bp::args("Task to provide to the handler", "Number of threads"), "@DocString(FMTTaskHandler::FMTTaskHandler(const Parallel::FMTTask&,unsigned int))"))
		.def("conccurentrun", &Parallel::FMTTaskHandler::conccurentRun,"@DocString(FMTTaskHandler::conccurentrun)")
		.def("ondemandrun", &Parallel::FMTTaskHandler::onDemandRun, "@DocString(FMTTaskHandler::ondemandrun)");

	definePyList<Parallel::FMTTaskHandler>();

    }
}
#endif // PYEXPORTParallel_H_INCLUDED
