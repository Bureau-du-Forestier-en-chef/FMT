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

#ifndef PYEXPORTMODEL_H_INCLUDED
#define PYEXPORTMODEL_H_INCLUDED

#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTsesmodel.h"
#include "FMTsamodel.h"
#include "boost/python.hpp"

namespace Python
{ 
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(simulate_overloads, simulate, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(buildperiod_overloads, buildperiod, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(move_solution_overloads, move_solution, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(get_outputs_overloads, get_outputs, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getLPoutputoverloads, getoutput, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(warmup_overloads, warmup, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(evaluate_overloads, evaluate, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getoperatingareaheuristics_overloads, getoperatingareaheuristics, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getvariabilities_overloads, getvariabilities, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getareavariabilities_overloads, getareavariabilities,2,3)


void exportModel()
    {
    namespace bp = boost::python;
    bp::object ModelsModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.Models"))));
    bp::scope().attr("Models") = ModelsModule;
    bp::scope ModelsModule_scope = ModelsModule;
    bp::scope().attr("__doc__") = ".. module:: Model\n"
    "\n"
    "   :platform: Unix, Windows\n"
    "   :synopsis: Module used to generate all kind of Models (M2/M3).\n"
    "\n";

	bp::class_<Models::FMTmodel, bp::bases<Core::FMTobject>>("FMTmodel", "@DocString(FMTmodel)")
            .def_readwrite("name",&Models::FMTmodel::name,
				"@DocString(FMTmodel::name)")
			.def_pickle(FMT_pickle_suite<Models::FMTmodel>())
            .def("getyields",&Models::FMTmodel::getyields,
				"@DocString(FMTmodel::getyields)")
            .def("getarea",&Models::FMTmodel::getarea, getarea_overloads(bp::args("period"), "@DocString(FMTlpmodel::getarea)"))
            .def("getthemes",&Models::FMTmodel::getthemes,
				"@DocString(FMTmodel::getthemes)")
            .def("getactions",&Models::FMTmodel::getactions,
				"@DocString(FMTmodel::getactions)")
			.def("getoutputs", &Models::FMTmodel::getoutputs,
				"@DocString(FMTmodel::getoutputs)")
			.def("getconstraints", &Models::FMTmodel::getconstraints,
				"@DocString(FMTmodel::getconstraints)")
			.def("setconstraints", &Models::FMTmodel::setconstraints,
				"@DocString(FMTmodel::setconstraints)")
            .def("gettransitions",&Models::FMTmodel::gettransitions,
				"@DocString(FMTmodel::gettransitions)")
            .def("settransitions",&Models::FMTmodel::settransitions,
				"@DocString(FMTmodel::settransitions)")
            .def("getlifespan",&Models::FMTmodel::getlifespan,
				"@DocString(FMTmodel::getlifespan)")
            .def("isvalid",&Models::FMTmodel::isvalid,
				"@DocString(FMTmodel::isvalid)");

    define_pylist<Models::FMTmodel>();



	bp::to_python_converter<std::map<std::string, double>, MapToDict<std::string, double>>();

	bp::class_<Models::FMTsesmodel, bp::bases<Models::FMTmodel>>("FMTsesmodel", "@DocString(FMTsesmodel)")
            .def(bp::init<Models::FMTmodel>())
			.def_pickle(FMT_pickle_suite<Models::FMTsesmodel>())
            .def("getmapping",&Models::FMTsesmodel::getmapping,
				"@DocString(FMTsesmodel::getmapping)")
			.def("getschedule", &Models::FMTsesmodel::getschedule,
				"@DocString(FMTsesmodel::getschedule)")
            .def("getdisturbances",&Models::FMTsesmodel::getdisturbances,
				"@DocString(FMTsesmodel::getdisturbances)")
            .def("setinitialmapping",&Models::FMTsesmodel::setinitialmapping,
				"@DocString(FMTsesmodel::setinitialmapping)")
            .def("setspactions",&Models::FMTsesmodel::setspactions,
				"@DocString(FMTsesmodel::setspactions)")
			.def("getschedule",&Models::FMTsesmodel::getschedule,
				"@DocString(FMTsesmodel::getschedule)")
			.def("getdisturbancestats", &Models::FMTsesmodel::getdisturbancestats,
				"@DocString(FMTsesmodel::getdisturbancestats)")
            .def("simulate",&Models::FMTsesmodel::simulate,
                 simulate_overloads(bp::args("schedule", "schedule_only","seed"), "@DocString(FMTsesmodel::simulate)"));

    define_pylist<Models::FMTsesmodel>();

	#ifdef FMTWITHOSI

	bp::enum_<Models::FMTsolverinterface>("FMTsolverinterface")
		.value("CLP", Models::FMTsolverinterface::CLP)
		#ifdef  FMTWITHMOSEK
			.value("MOSEK", Models::FMTsolverinterface::MOSEK)
		#endif
		.value("CPLEX", Models::FMTsolverinterface::CPLEX)
		.value("GUROBI", Models::FMTsolverinterface::GUROBI);

	bp::class_<Models::FMTlpmodel, bp::bases<Models::FMTmodel>>("FMTlpmodel", "@DocString(FMTlpmodel)")
		.def(bp::init<Models::FMTmodel, Models::FMTsolverinterface>())
		.def_pickle(FMT_pickle_suite<Models::FMTlpmodel>())
		.def("buildperiod", &Models::FMTlpmodel::buildperiod, buildperiod_overloads(bp::args("schedule", "forcepartialbuild"), "@DocString(FMTlpmodel::buildperiod)"))
		.def("boundsolution", &Models::FMTlpmodel::boundsolution,
			"@DocString(FMTlpmodel::boundsolution)")
		.def("getsolution", &Models::FMTlpmodel::getsolution,
			"@DocString(FMTlpmodel::getsolution)")
		.def("setsolution", &Models::FMTlpmodel::setsolution,
			"@DocString(FMTlpmodel::setsolution)")
		.def("setobjective", &Models::FMTlpmodel::setobjective,
			"@DocString(FMTlpmodel::setobjective)")
		.def("setconstraint", &Models::FMTlpmodel::setconstraint,
			"@DocString(FMTlpmodel::setconstraint)")
		.def("eraseconstraint", &Models::FMTlpmodel::eraseconstraint,
			"@DocString(FMTlpmodel::eraseconstraint)")
		.def("eraseperiod", &Models::FMTlpmodel::eraseperiod,
			"@DocString(FMTlpmodel::eraseperiod)")
		.def("resolve", &Models::FMTlpmodel::resolve,
			"@DocString(FMTlpmodel::resolve)")
		.def("initialsolve", &Models::FMTlpmodel::initialsolve,
			"@DocString(FMTlpmodel::initialsolve)")
		.def("getoutput", &Models::FMTlpmodel::getoutput, getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTlpmodel::getoutput)"))
		.def("writeLP", &Models::FMTlpmodel::writeLP,
			"@DocString(FMTlpmodel::writeLP)")
		.def("writeMPS", &Models::FMTlpmodel::writeMPS,
			"@DocString(FMTlpmodel::writeMPS)")
		.def("__eq__", &Models::FMTlpmodel::operator ==,
			"@DocString(FMTlpmodel::operator==)")
		.def("__ne__", &Models::FMTlpmodel::operator !=,
			"@DocString(FMTlpmodel::operator!=)")
		.def("getstats", &Models::FMTlpmodel::getstats,
			"@DocString(FMTlpmodel::getstats)")
		.def("getoperatingareaheuristics", &Models::FMTlpmodel::getoperatingareaheuristics, getoperatingareaheuristics_overloads(bp::args("opareas", "node", "numberofheuristics", "copysolver"), "@DocString(FMTlpmodel::getoperatingareaheuristics)"))
		.def("getvariabilities", &Models::FMTlpmodel::getvariabilities, getvariabilities_overloads(bp::args("outputs", "tolerance"), "@DocString(FMTlpmodel::getvariabilities)"))
		.def("getareavariabilities", &Models::FMTlpmodel::getareavariabilities,getareavariabilities_overloads(bp::args("outputs","globalmask","tolerance"), "@DocString(FMTlpmodel::getareavariabilities)"));

	define_pylist<Models::FMTlpmodel>();
	#endif

	bp::enum_<Models::FMTsawarmuptype>("FMTsawarmuptype")
		.value("log", Models::FMTsawarmuptype::log)
		.value("delta", Models::FMTsawarmuptype::bigdelta)
		.value("logmax", Models::FMTsawarmuptype::logmax)
		.value("bootstrapmagic", Models::FMTsawarmuptype::bootstrapmagic);



	bp::class_<Models::FMTsamodel, bp::bases<Models::FMTmodel>>("FMTsamodel", "@DocString(FMTsamodel)")
            .def(bp::init<Models::FMTmodel>())
            .def("get_current_solution",&Models::FMTsamodel::get_current_solution,
				"@DocString(FMTsamodel::get_current_solution)")
            .def("get_new_solution",&Models::FMTsamodel::get_new_solution,
				"@DocString(FMTsamodel::get_new_solution)")
            .def("getspatialactions",&Models::FMTsamodel::getspatialactions,
				"@DocString(FMTsamodel::getspatialactions)")
            .def("evaluate",&Models::FMTsamodel::evaluate,evaluate_overloads(bp::args("temp", "all_data"), "@DocString(FMTsamodel::evaluate)"))
            .def("setinitial_mapping",&Models::FMTsamodel::setinitial_mapping,
				"@DocString(FMTsamodel::setinitial_mapping)")
            .def("setspactions",&Models::FMTsamodel::setspactions,
				"@DocString(FMTsamodel::setspactions)")
            .def("set_min_max_moves",&Models::FMTsamodel::set_min_max_moves,
				"@DocString(FMTsamodel::set_min_max_moves)")
            .def("cool_down",&Models::FMTsamodel::cool_down,
				"@DocString(FMTsamodel::cool_down)")
            .def("get_cool_schedule_type",&Models::FMTsamodel::getcoolingscheduletype,
				"@DocString(FMTsamodel::getcoolingscheduletype)")
            .def("setschedule",&Models::FMTsamodel::setschedule,
				"@DocString(FMTsamodel::setschedule)")
            .def("buildperiod",&Models::FMTsamodel::buildperiod,
				"@DocString(FMTsamodel::buildperiod)")
            .def("move",&Models::FMTsamodel::move_solution,
                 move_solution_overloads(bp::args("move_type"),"@DocString(FMTsamodel::move_type)"))
            .def("acceptnew",&Models::FMTsamodel::acceptnew,
				"@DocString(FMTsamodel::acceptnew)")
            .def("write_outputs_at",&Models::FMTsamodel::write_outputs_at,
				"@DocString(FMTsamodel::write_outputs_at)")
            .def("get_outputs",&Models::FMTsamodel::get_outputs,get_outputs_overloads(bp::args("addon"), "@DocString(FMTsamodel::get_outputs)"))
            .def("write_solutions_events",&Models::FMTsamodel::write_solutions_events,
				"@DocString(FMTsamodel::write_solutions_events)")
            .def("get_number_moves",&Models::FMTsamodel::get_number_moves,
				"@DocString(FMTsamodel::get_number_moves)")
            .def("warmup",&Models::FMTsamodel::warmup,warmup_overloads(bp::args("initprob","iterations","type"), "@DocString(FMTsamodel::warmup)"));

    define_pylist<Models::FMTsamodel>();

    }
}
#endif // PYEXPORTMODEL_H_INCLUDED
