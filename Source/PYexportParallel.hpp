/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTParallel_H_INCLUDED
#define PYEXPORTParallel_H_INCLUDED

#include "FMTtask.hpp"
#include "FMTtaskhandler.hpp"
#include "FMTreplanningtask.hpp"
#include "FMTmodel.hpp"
#include "FMToutput.hpp"
#include "FMTutility.hpp"
#include <string>
#include "boost/python.hpp"

namespace Python
{

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


	bp::class_<Parallel::FMTtask, bp::bases<Core::FMTobject>>("FMTtask", "@DocString(FMTtask)")
			.def(bp::init<Parallel::FMTtask>());

	define_pylist<Parallel::FMTtask>();

	bp::class_<Parallel::FMTreplanningtask, bp::bases<Parallel::FMTtask>>("FMTreplanningtask", "@DocString(FMTreplanningtask)")
			.def(bp::init<Parallel::FMTreplanningtask>())
			.def(bp::init<const Models::FMTmodel&, const Models::FMTmodel&, const Models::FMTmodel&,const std::vector<Core::FMToutput>&,
				const std::string&, const std::string&,const std::vector<std::string>&,
				const int&,const int&,const double&,double,Core::FMToutputlevel>(
					bp::args("Strategic model", "stochastic model","tactic model","selected outputs","Output folder","GDAL driver name","GDAL driver creation options","Number of replicates","Number of replanning periods","Strategic schedule weight","minimaldrift","output level"), "@DocString(FMTreplanningtask::FMTreplanningtask)"));

	define_pylist<Parallel::FMTreplanningtask>();

	bp::class_<Parallel::FMTtaskhandler, bp::bases<Core::FMTobject>>("FMTtaskhandler", "@DocString(FMTtaskhandler)")
		.def(bp::init<Parallel::FMTtaskhandler>())
		.def(bp::init<const Parallel::FMTtask&, unsigned int>(bp::args("Task to provide to the handler", "Number of threads"), "@DocString(FMTtaskhandler::FMTtaskhandler(const Parallel::FMTtask&,unsigned int))"))
		.def("conccurentrun", &Parallel::FMTtaskhandler::conccurentrun,"@DocString(FMTreplanningtask::conccurentrun)")
		.def("ondemandrun", &Parallel::FMTtaskhandler::ondemandrun, "@DocString(FMTreplanningtask::ondemandrun)");

	define_pylist<Parallel::FMTtaskhandler>();

    }
}
#endif // PYEXPORTParallel_H_INCLUDED
