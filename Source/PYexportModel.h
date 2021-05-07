/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTMODEL_H_INCLUDED
#define PYEXPORTMODEL_H_INCLUDED

#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTsesmodel.h"
#include "FMTnssmodel.h"
#include "FMTsamodel.h"
#include "FMTlpsolver.h"

#include "boost/python.hpp"

namespace Python
{
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(greedyreferencebuild_overloads,greedyreferencebuild, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(simulatenssm_overloads, simulate, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(buildperiod_overloads, buildperiod, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(get_outputs_overloads, get_outputs, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getsolution_overloads, getsolution, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getLPoutputoverloads, getoutput, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getschedulesoverloads, getschedule, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getpotentialscheduleoverloads, getpotentialschedule,2,3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(warmup_overloads, warmup, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(evaluate_overloads, evaluate, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getoperatingareaschedulerheuristics_overloads, getoperatingareaschedulerheuristics,2, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getoperatingareaclustererheuristics_overloads,getoperatingareaclustererheuristics,4,7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getvariabilities_overloads, getvariabilities, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getareavariabilities_overloads, getareavariabilities,2,3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(boundsolution_overloads, boundsolution, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(eraseperiod_overloads, eraseperiod,0,1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(setsolution_overloads,setsolution, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(basepresolve_overloads,basepresolve, 0, 1)

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
			.def_pickle(FMT_pickle_suite<Models::FMTmodel>())
			.def(bp::init<Models::FMTmodel>())
            .def("getyields",&Models::FMTmodel::getyields,
				"@DocString(FMTmodel::getyields)")
			.def("setyields", &Models::FMTmodel::setyields,
				"@DocString(FMTmodel::setyields)")
			.def("getname", &Models::FMTmodel::getname,
				"@DocString(FMTmodel::getname)")
            .def("getarea",&Models::FMTmodel::getarea, getarea_overloads(bp::args("period"), "@DocString(FMTlpmodel::getarea)"))
			.def("setarea", &Models::FMTmodel::setarea,
				"@DocString(FMTmodel::setarea)")
			.def("getthemes",&Models::FMTmodel::getthemes,
				"@DocString(FMTmodel::getthemes)")
			.def("setthemes", &Models::FMTmodel::setthemes,
				"@DocString(FMTmodel::setthemes)")
            .def("getactions",&Models::FMTmodel::getactions,
				"@DocString(FMTmodel::getactions)")
			.def("setactions", &Models::FMTmodel::setactions,
				"@DocString(FMTmodel::setactions)")
			.def("getoutputs", &Models::FMTmodel::getoutputs,
				"@DocString(FMTmodel::getoutputs)")
			.def("setoutputs", &Models::FMTmodel::setoutputs,
				"@DocString(FMTmodel::setoutputs)")
			.def("getconstraints", &Models::FMTmodel::getconstraints,
				"@DocString(FMTmodel::getconstraints)")
			.def("setconstraints", &Models::FMTmodel::setconstraints,
				"@DocString(FMTmodel::setconstraints)")
            .def("gettransitions",&Models::FMTmodel::gettransitions,
				"@DocString(FMTmodel::gettransitions)")
            .def("settransitions",&Models::FMTmodel::settransitions,
				"@DocString(FMTmodel::settransitions)")
			.def("setname", &Models::FMTmodel::setname,
				"@DocString(FMTmodel::setname)")
			.def("setareaperiod", &Models::FMTmodel::setareaperiod,
				"@DocString(FMTmodel::setareaperiod)")
            .def("getlifespan",&Models::FMTmodel::getlifespan,
				"@DocString(FMTmodel::getlifespan)")
            .def("isvalid",&Models::FMTmodel::isvalid,
				"@DocString(FMTmodel::isvalid)")
			.def("clearcache",&Models::FMTmodel::clearcache,
				"@DocString(FMTmodel::clearcache)")
			.def("getselectedmask",&Models::FMTmodel::getselectedmask,
				"@DocString(FMTmodel::getselectedmask)")
			.def("getpostsolvebasemask",&Models::FMTmodel::getpostsolvebasemask,
				"@DocString(FMTmodel::getpostsolvebasemask)")
			.def("basepresolve",&Models::FMTmodel::basepresolve,
				basepresolve_overloads(bp::args("presolvepass"),"@DocString(FMTmodel::basepresolve)"))
		.def("getpotentialschedule", &Models::FMTmodel::getpotentialschedule,
			getpotentialscheduleoverloads(bp::args("toremove","selection","withlock"),"@DocString(FMTmodel::getpotentialschedule)"));

    define_pylist<Models::FMTmodel>();



	bp::to_python_converter<std::map<std::string, double>, MapToDict<std::string, double>>();

	bp::class_<Models::FMTsesmodel, bp::bases<Models::FMTmodel>>("FMTsesmodel", "@DocString(FMTsesmodel)")
            .def(bp::init<Models::FMTmodel>())
			.def_pickle(FMT_pickle_suite<Models::FMTsesmodel>())
            .def("getmapping",&Models::FMTsesmodel::getmapping,
				"@DocString(FMTsesmodel::getmapping)")
			.def("getschedule", &Models::FMTsesmodel::getschedule,
				"@DocString(FMTsesmodel::getschedule)")
            .def("setinitialmapping",&Models::FMTsesmodel::setinitialmapping,
				"@DocString(FMTsesmodel::setinitialmapping)")
			.def("getschedule",&Models::FMTsesmodel::getschedule,
				getschedulesoverloads(bp::args("withlock"), "@DocString(FMTsesmodel::getschedule)"))
			.def("getdisturbancestats", &Models::FMTsesmodel::getdisturbancestats,
				"@DocString(FMTsesmodel::getdisturbancestats)")
			.def("getoutput", &Models::FMTsesmodel::getoutput,
				getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTsesmodel::getoutput)"))
			.def("greedyreferencebuild", &Models::FMTsesmodel::greedyreferencebuild,
				greedyreferencebuild_overloads(bp::args("schedule", "numberofiterations", "seed", "tolerance"), "@DocString(FMTsesmodel::greedyreferencebuild)"))
			.def("getspschedule", &Models::FMTsesmodel::getspschedule,
				"@DocString(FMTsesmodel::getdisturbances)");

    define_pylist<Models::FMTsesmodel>();

	bp::class_<Models::FMTnssmodel, bp::bases<Models::FMTmodel>>("FMTnssmodel", "@DocString(FMTnssmodel)")
		.def(bp::init<Models::FMTmodel,unsigned int>())
		.def_pickle(FMT_pickle_suite<Models::FMTnssmodel>())
		.def("simulate", &Models::FMTnssmodel::simulate,
			simulatenssm_overloads(bp::args( "grow", "withlock"),"@DocString(FMTnssmodel::simulate)"));

	define_pylist<Models::FMTnssmodel>();




	#ifdef FMTWITHOSI

	bp::enum_<Models::FMTsolverinterface>("FMTsolverinterface")
		.value("CLP", Models::FMTsolverinterface::CLP)
		#ifdef  FMTWITHMOSEK
			.value("MOSEK", Models::FMTsolverinterface::MOSEK)
		#endif
		.value("CPLEX", Models::FMTsolverinterface::CPLEX)
		.value("GUROBI", Models::FMTsolverinterface::GUROBI);

	bp::class_<Models::FMTlpsolver>("FMTlpolver", "@DocString(FMTlpolver)")
		.def(bp::init<Models::FMTsolverinterface>())
		.def("isProvenOptimal", &Models::FMTlpsolver::isProvenOptimal,
			"@DocString(FMTlpsolver::isProvenOptimal)")
		.def("getObjValue", &Models::FMTlpsolver::getObjValue,
			"@DocString(FMTlpsolver::getObjValue)")
		.def("getObjSense", &Models::FMTlpsolver::getObjSense,
			"@DocString(FMTlpsolver::getObjSense)")
		.def("setnumberofthreads", &Models::FMTlpsolver::setnumberofthreads,
			"@DocString(FMTlpsolver::setnumberofthreads)")
		.def("setMIPgaptolerance", &Models::FMTlpsolver::setMIPgaptolerance,
			"@DocString(FMTlpsolver::setMIPgaptolerance)")
		.def("setoptimizerMAXtime", &Models::FMTlpsolver::setoptimizerMAXtime,
			"@DocString(FMTlpsolver::setoptimizerMAXtime)")
		.def("writeLP", &Models::FMTlpsolver::writeLP,
			"@DocString(FMTlpsolver::writeLP)")
		.def("writeMPS", &Models::FMTlpsolver::writeMPS,
			"@DocString(FMTlpsolver::writeMPS)");


	bp::class_<Models::FMTlpmodel, bp::bases<Models::FMTmodel>>("FMTlpmodel", "@DocString(FMTlpmodel)")
		.def(bp::init<Models::FMTmodel, Models::FMTsolverinterface>())
		.def(bp::init<Models::FMTlpmodel>())
		.def_pickle(FMT_pickle_suite<Models::FMTlpmodel>())
		.def("buildperiod", &Models::FMTlpmodel::buildperiod, buildperiod_overloads(bp::args("schedule", "forcepartialbuild"), "@DocString(FMTlpmodel::buildperiod)"))
		.def("boundsolution", &Models::FMTlpmodel::boundsolution,
			boundsolution_overloads(bp::args("period", "tolerance"),
				"@DocString(FMTlpmodel::boundsolution)"))
		.def("getsolution", &Models::FMTlpmodel::getsolution,
			getsolution_overloads(bp::args("period", "withlock"), "@DocString(FMTlpmodel::getsolution)"))
		.def("setsolution", &Models::FMTlpmodel::setsolution,
			setsolution_overloads(bp::args("period", "schedule", "tolerance"), "@DocString(FMTlpmodel::setsolution)"))
		.def("setobjective", &Models::FMTlpmodel::setobjective,
			"@DocString(FMTlpmodel::setobjective)")
		.def("clearcache", &Models::FMTlpmodel::clearcache,
			"@DocString(FMTlpmodel::clearcache)")
		.def("clearconstraintlocation", &Models::FMTlpmodel::clearconstraintlocation,
			"@DocString(FMTlpmodel::clearconstraintlocation)")
		.def("cleargraphdevelopements", &Models::FMTlpmodel::cleargraphdevelopements,
			"@DocString(FMTlpmodel::cleargraphdevelopements)")
		.def("setstrictlypositivesoutputsmatrix", &Models::FMTlpmodel::setstrictlypositivesoutputsmatrix,
			"@DocString(FMTlpmodel::setstrictlypositivesoutputsmatrix)")
		.def("setconstraint", &Models::FMTlpmodel::setconstraint,
			"@DocString(FMTlpmodel::setconstraint)")
		.def("eraseconstraint", &Models::FMTlpmodel::eraseconstraint,
			"@DocString(FMTlpmodel::eraseconstraint)")
		.def("eraseperiod", &Models::FMTlpmodel::eraseperiod,
			eraseperiod_overloads(bp::args("constraintsonly"), "@DocString(FMTlpmodel::eraseperiod)"))
		.def("resolve", &Models::FMTlpmodel::resolve,
			"@DocString(FMTlpmodel::resolve)")
		.def("initialsolve", &Models::FMTlpmodel::initialsolve,
			"@DocString(FMTlpmodel::initialsolve)")
		.def("getoutput", &Models::FMTlpmodel::getoutput, getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTlpmodel::getoutput)"))
		.def("__eq__", &Models::FMTlpmodel::operator ==,
			"@DocString(FMTlpmodel::operator==)")
		.def("__ne__", &Models::FMTlpmodel::operator !=,
			"@DocString(FMTlpmodel::operator!=)")
		.def("getstats", &Models::FMTlpmodel::getstats,
			"@DocString(FMTlpmodel::getstats)")
		.def("getObjValue", &Models::FMTlpmodel::getObjValue,
					"@DocString(FMTlpmodel::getObjValue)")
		.def("getlocalconstraints",
			&Models::FMTlpmodel::getlocalconstraints,
			"@DocString(FMTlpmodel::getlocalconstraints)")
		.def("writeLP",
			&Models::FMTlpmodel::writeLP,
			"@DocString(FMTlpmodel::writeLP)")
		.def("getoperatingareaschedulerheuristics", &Models::FMTlpmodel::getoperatingareaschedulerheuristics, getoperatingareaschedulerheuristics_overloads(bp::args("opareas", "node", "numberofheuristics", "copysolver","updatematrixname"), "@DocString(FMTlpmodel::getoperatingareaschedulerheuristics)"))
		.def("getoperatingareaclustererheuristics", &Models::FMTlpmodel::getoperatingareaclustererheuristics, getoperatingareaclustererheuristics_overloads(bp::args("opareas", "statisticoutput", "areaoutput", "period", "numberofheuristics"), "@DocString(FMTlpmodel::getoperatingareaschedulerheuristics)"))
		.def("getvariabilities", &Models::FMTlpmodel::getvariabilities, getvariabilities_overloads(bp::args("outputs", "tolerance"), "@DocString(FMTlpmodel::getvariabilities)"))
		.def("getareavariabilities", &Models::FMTlpmodel::getareavariabilities, getareavariabilities_overloads(bp::args("outputs", "globalmask", "tolerance"), "@DocString(FMTlpmodel::getareavariabilities)"));
		

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
			.def("initialsolve", &Models::FMTsamodel::initialsolve,
				"@DocString(FMTsamodel::initialsolve)")
            //.def("evaluate",&Models::FMTsamodel::evaluate,evaluate_overloads(bp::args("temp", "all_data"), "@DocString(FMTsamodel::evaluate)"))
            .def("setinitialmapping",&Models::FMTsamodel::setinitialmapping,
				"@DocString(FMTsamodel::setinitialmapping)")
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
            .def("move",&Models::FMTsamodel::move_solution,"@DocString(FMTsamodel::move_type)")
            .def("acceptnew",&Models::FMTsamodel::acceptnew,
				"@DocString(FMTsamodel::acceptnew)")
            .def("write_outputs_at",&Models::FMTsamodel::write_outputs_at,
				"@DocString(FMTsamodel::write_outputs_at)")
            .def("get_outputs",&Models::FMTsamodel::get_outputs,get_outputs_overloads(bp::args("addon"), "@DocString(FMTsamodel::get_outputs)"))
            .def("write_solutions_events",&Models::FMTsamodel::write_solutions_events,
				"@DocString(FMTsamodel::write_solutions_events)")
            .def("get_number_moves",&Models::FMTsamodel::get_number_moves,
				"@DocString(FMTsamodel::get_number_moves)")
            //.def("warmup",&Models::FMTsamodel::warmup,warmup_overloads(bp::args("initprob","iterations","keep_best","type"), "@DocString(FMTsamodel::warmup)"))
			.def("set_movetype", &Models::FMTsamodel::set_movetype, "@DocString(FMTsamodel::set_movetype)");

    define_pylist<Models::FMTsamodel>();

    }
}
#endif // PYEXPORTMODEL_H_INCLUDED
