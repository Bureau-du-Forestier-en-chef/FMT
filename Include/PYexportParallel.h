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


	bp::class_<Parallel::FMTtask, bp::bases<Core::FMTObject>>("FMTtask", "@DocString(FMTtask)")
			.def(bp::init<Parallel::FMTtask>());

	definePyList<Parallel::FMTtask>();

	bp::class_<Parallel::FMTreplanningtask, bp::bases<Parallel::FMTtask>>("FMTreplanningtask", "@DocString(FMTreplanningtask)")
			.def(bp::init<Parallel::FMTreplanningtask>())
			.def(bp::init<const Models::FMTModel&, const Models::FMTModel&, const Models::FMTModel&,const std::vector<Core::FMTOutput>&,
				const std::string&, const std::string&,const std::vector<std::string>&,
				const int&,const int&,double,Core::FMToutputlevel,const bool>(
					bp::args("Strategic model", "stochastic model","tactic model","selected outputs","Output folder","GDAL driver name","GDAL driver creation options","Number of replicates","Number of replanning periods","minimaldrift","output level","Write schedule"), "@DocString(FMTreplanningtask::FMTreplanningtask(const Models::FMTModel&,const Models::FMTModel&,const Models::FMTModel&))"));

	definePyList<Parallel::FMTreplanningtask>();


	bp::class_<Parallel::FMTplanningtask, bp::bases<Parallel::FMTtask>>("FMTplanningtask", "@DocString(FMTplanningtask)")
		.def(bp::init<Parallel::FMTplanningtask>())
		.def(bp::init<const int&, const int&, const std::string&, std::string,
			std::vector<std::string>, Core::FMToutputlevel, std::string>(
				bp::args("Min output period", "Max output period", "Output folder", "GDAL driver name", "GDAL driver creation options", "output level", "primary file localisation"), "@DocString(FMTplanningtask::FMTplanningtask(...))"))
		.def("push_back", &Parallel::FMTplanningtask::push_back, push_back_overloads(bp::args("model", "schedules", "outputs"), "@DocString(FMTplanningtask::push_back)"));

	definePyList<Parallel::FMTplanningtask>();

	bp::class_<Parallel::FMTopareaschedulertask, bp::bases<Parallel::FMTtask>>("FMTopareaschedulertask", "@DocString(FMTopareaschedulertask")
		.def(bp::init<Parallel::FMTopareaschedulertask>())
		.def(bp::init<const Models::FMTLpModel&,const std::vector<Heuristics::FMToperatingareascheme>&,
			const Core::FMTOutputNode&,const std::string&,
			const std::string&,const unsigned int&,
			const double&,Core::FMTOutput>(
				bp::args("model", "operating areas", "node", "output location", "output yield name", "maxiterations","maxtime","returntime_output"), "@DocString(FMTopareaschedulertask::FMTopareaschedulertask(...))"));

	definePyList<Parallel::FMTopareaschedulertask>();


	bp::class_<Parallel::FMTtaskhandler, bp::bases<Core::FMTObject>>("FMTtaskhandler", "@DocString(FMTtaskhandler)")
		.def(bp::init<Parallel::FMTtaskhandler>())
		.def(bp::init<const Parallel::FMTtask&, unsigned int>(bp::args("Task to provide to the handler", "Number of threads"), "@DocString(FMTtaskhandler::FMTtaskhandler(const Parallel::FMTtask&,unsigned int))"))
		.def("conccurentrun", &Parallel::FMTtaskhandler::conccurentRun,"@DocString(FMTtaskhandler::conccurentrun)")
		.def("ondemandrun", &Parallel::FMTtaskhandler::onDemandRun, "@DocString(FMTtaskhandler::ondemandrun)");

	definePyList<Parallel::FMTtaskhandler>();

    }
}
#endif // PYEXPORTParallel_H_INCLUDED
