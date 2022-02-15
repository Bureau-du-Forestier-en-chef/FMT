/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTMODEL_H_INCLUDED
#define PYEXPORTMODEL_H_INCLUDED

#include "FMTmodel.hpp"
#include "FMTsrmodel.hpp"
#include "FMTlpmodel.hpp"
#include "FMTsesmodel.hpp"
#include "FMTnssmodel.hpp"
#include "FMTsamodel.hpp"
#include "FMTlpsolver.hpp"

#include "boost/python.hpp"

namespace Python
{
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(greedyreferencebuild_overloads,greedyreferencebuild, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(buildperiod_overloads, buildperiod, 0, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(get_outputs_overloads, get_outputs, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getsolution_overloads, getsolution, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getLPoutputoverloads, getoutput, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getschedulesoverloads, getschedule, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getpotentialscheduleoverloads, getpotentialschedule,2,3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(warmup_overloads, warmup, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(evaluate_overloads, evaluate, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getoperatingareaschedulerheuristics_overloads, getoperatingareaschedulerheuristics,2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getoperatingareaclustererheuristics_overloads,getoperatingareaclustererheuristics,4,7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getvariabilities_overloads, getvariabilities, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getareavariabilities_overloads, getareavariabilities,2,3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(boundsolution_overloads, boundsolution, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(eraseperiod_overloads, eraseperiod,0,1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(setsolution_overloads,setsolution, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(setsolutionbylp_overloads,setsolutionbylp, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(basepresolve_overloads,basepresolve, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(addscheduletoobjective_overloads, addscheduletoobjective, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getarea_overloads, getarea, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(doplanning_overloads,doplanning,1,2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(showparameters_overloads,showparameters,0,1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(eraseconstraint_overloads,eraseconstraint,1, 2)

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
			.def("addageyieldhandlers", &Models::FMTmodel::addyieldhandlers<Core::FMTageyieldhandler>,
				"@DocString(FMTmodel::addyieldhandlers)")
			.def("addtimeyieldhandlers", &Models::FMTmodel::addyieldhandlers<Core::FMTtimeyieldhandler>,
				"@DocString(FMTmodel::addyieldhandlers)")
			.def("getname", &Models::FMTmodel::getname,
				"@DocString(FMTmodel::getname)")
            .def("getarea",&Models::FMTmodel::getarea, getarea_overloads(bp::args("period","beforegrowanddeath"), "@DocString(FMTmodel::getarea)"))
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
			.def("getpostsolvefilter",&Models::FMTmodel::getpostsolvefilter,
				"@DocString(FMTmodel::getpostsolvefilter)")
			.def("basepresolve",&Models::FMTmodel::basepresolve,
				basepresolve_overloads(bp::args("presolvepass"),"@DocString(FMTmodel::basepresolve)"))
		.def("getpotentialschedule", &Models::FMTmodel::getpotentialschedule,
			getpotentialscheduleoverloads(bp::args("toremove","selection","withlock"),"@DocString(FMTmodel::getpotentialschedule)"))
			//The way to expose overload member functions with different args
			.def<bool (Models::FMTmodel::*)(const Models::FMTintmodelparameters& key, const int& value)>("setparameter",&Models::FMTmodel::setparameter,
				"@DocString(FMTmodel::setparameter)")
			.def<bool (Models::FMTmodel::*)(const Models::FMTdblmodelparameters& key, const double& value)>("setparameter",&Models::FMTmodel::setparameter,
				"@DocString(FMTmodel::setparameter)")
			.def<bool (Models::FMTmodel::*)(const Models::FMTboolmodelparameters& key, const bool& value)>("setparameter",&Models::FMTmodel::setparameter,
				"@DocString(FMTmodel::setparameter)")
			.def<int (Models::FMTmodel::*)(const Models::FMTintmodelparameters& key)const>("getparameter",&Models::FMTmodel::getparameter,
				"@DocString(FMTmodel::getparameter)")
			.def<double (Models::FMTmodel::*)(const Models::FMTdblmodelparameters& key)const>("getparameter",&Models::FMTmodel::getparameter,
				"@DocString(FMTmodel::getparameter)")
			.def<bool (Models::FMTmodel::*)(const Models::FMTboolmodelparameters& key)const>("getparameter",&Models::FMTmodel::getparameter,
				"@DocString(FMTmodel::getparameter)")
			.def("setcompresstime",&Models::FMTmodel::setcompresstime,
				"@DocString(FMTmodel::setcompresstime)")
			.def("getcompresstime",&Models::FMTmodel::getcompresstime,
				"@DocString(FMTmodel::getcompresstime)")
			.def("showparameters",&Models::FMTmodel::showparameters,
				showparameters_overloads(bp::args("showhelp"),"@DocString(FMTmodel::showparameters)"))
			.def("doplanning",&Models::FMTmodel::doplanning,
				doplanning_overloads(bp::args("solve"),"@DocString(FMTmodel::doplanning)"));

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

	bp::enum_<Models::FMTintmodelparameters>("FMTintmodelparameters")
		.value("LENGTH", Models::FMTintmodelparameters::LENGTH)
		.value("SEED", Models::FMTintmodelparameters::SEED)
		.value("NUMBER_OF_ITERATIONS", Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS)
		.value("PRESOLVE_ITERATIONS", Models::FMTintmodelparameters::PRESOLVE_ITERATIONS)
		.value("NUMBER_OF_THREADS", Models::FMTintmodelparameters::NUMBER_OF_THREADS);
	
	define_pylist<Models::FMTintmodelparameters>();
	
	bp::enum_<Models::FMTdblmodelparameters>("FMTdblmodelparameters")
		.value("TOLERANCE", Models::FMTdblmodelparameters::TOLERANCE)
		.value("GOALING_SCHEDULE_WEIGHT", Models::FMTdblmodelparameters::GOALING_SCHEDULE_WEIGHT);

	define_pylist<Models::FMTdblmodelparameters>();

	bp::enum_<Models::FMTboolmodelparameters>("FMTboolmodelparameters")
		.value("FORCE_PARTIAL_BUILD", Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)
		.value("STRICTLY_POSITIVE", Models::FMTboolmodelparameters::STRICTLY_POSITIVE)
		.value("POSTSOLVE", Models::FMTboolmodelparameters::POSTSOLVE)
		.value("SHOW_LOCK_IN_SCHEDULES", Models::FMTboolmodelparameters::SHOW_LOCK_IN_SCHEDULES);

	define_pylist<Models::FMTboolmodelparameters>();

	

	#ifdef FMTWITHOSI

	bp::enum_<Models::FMTsolverinterface>("FMTsolverinterface")
		.value("CLP", Models::FMTsolverinterface::CLP)
		#ifdef  FMTWITHMOSEK
			.value("MOSEK", Models::FMTsolverinterface::MOSEK)
		#endif
		.value("CPLEX", Models::FMTsolverinterface::CPLEX)
		.value("GUROBI", Models::FMTsolverinterface::GUROBI);
		
	define_pylist<Models::FMTsolverinterface>();

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

	bp::class_<Models::FMTsrmodel, bp::bases<Models::FMTmodel>>("FMTsrmodel", "@DocString(FMTsrmodel)")
		.def_pickle(FMT_pickle_suite<Models::FMTsrmodel>())
		.def("buildperiod", &Models::FMTsrmodel::buildperiod, buildperiod_overloads(bp::args("schedule", "forcepartialbuild", "compressageclass"), "@DocString(FMTsrmodel::buildperiod)"))
		.def("getsolution", &Models::FMTsrmodel::getsolution,
			getsolution_overloads(bp::args("period", "withlock"), "@DocString(FMTsrmodel::getsolution)"))
		.def("setsolution", &Models::FMTsrmodel::setsolution,
			setsolution_overloads(bp::args("period", "schedule", "tolerance"), "@DocString(FMTsrmodel::setsolution)"))
		.def("setsolutionbylp", &Models::FMTsrmodel::setsolutionbylp,
			setsolutionbylp_overloads(bp::args("period", "schedule", "tolerance"), "@DocString(FMTsrmodel::setsolutionbylp)"))
		.def("getoutput", &Models::FMTsrmodel::getoutput, getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTsrmodel::getoutput)"))
		.def("cleargraphdevelopements", &Models::FMTsrmodel::cleargraphdevelopements,
			"@DocString(FMTsrmodel::cleargraphdevelopements)")
		.def("getstats", &Models::FMTsrmodel::getstats,
			"@DocString(FMTlpmodel::getstats)")
		.def("getavailablesolverinterface", &Models::FMTsrmodel::getavailablesolverinterface,
			"@DocString(FMTsrmodel::getavailablesolverinterface)").staticmethod("getavailablesolverinterface");

	define_pylist<Models::FMTsrmodel>();
	
	bp::class_<Models::FMTnssmodel, bp::bases<Models::FMTsrmodel>>("FMTnssmodel", "@DocString(FMTnssmodel)")
		.def(bp::init<Models::FMTmodel, unsigned int>())
		.def(bp::init<Models::FMTnssmodel>())
		.def_pickle(FMT_pickle_suite<Models::FMTnssmodel>())
		.def("simulate", &Models::FMTnssmodel::simulate);

	define_pylist<Models::FMTnssmodel>();


	bp::class_<Models::FMTlpmodel, bp::bases<Models::FMTsrmodel>>("FMTlpmodel", "@DocString(FMTlpmodel)")
		.def(bp::init<Models::FMTmodel, Models::FMTsolverinterface>())
		.def(bp::init<Models::FMTlpmodel>())
		.def_pickle(FMT_pickle_suite<Models::FMTlpmodel>())
		.def("boundsolution", &Models::FMTlpmodel::boundsolution,
			boundsolution_overloads(bp::args("period", "tolerance"),
				"@DocString(FMTlpmodel::boundsolution)"))
		.def("setobjective", &Models::FMTlpmodel::setobjective,
			"@DocString(FMTlpmodel::setobjective)")
		.def("clearcache", &Models::FMTlpmodel::clearcache,
			"@DocString(FMTlpmodel::clearcache)")
		.def("clearconstraintlocation", &Models::FMTlpmodel::clearconstraintlocation,
			"@DocString(FMTlpmodel::clearconstraintlocation)")
		.def("setstrictlypositivesoutputsmatrix", &Models::FMTlpmodel::setstrictlypositivesoutputsmatrix,
			"@DocString(FMTlpmodel::setstrictlypositivesoutputsmatrix)")
		.def("setconstraint", &Models::FMTlpmodel::setconstraint,
			"@DocString(FMTlpmodel::setconstraint)")
		.def("eraseconstraint", &Models::FMTlpmodel::eraseconstraint,
			eraseconstraint_overloads(bp::args("constraint","period"),"@DocString(FMTlpmodel::eraseconstraint)"))
		.def("eraseperiod", &Models::FMTlpmodel::eraseperiod,
			eraseperiod_overloads(bp::args("constraintsonly"), "@DocString(FMTlpmodel::eraseperiod)"))
		.def("resolve", &Models::FMTlpmodel::resolve,
			"@DocString(FMTlpmodel::resolve)")
		.def("initialsolve", &Models::FMTlpmodel::initialsolve,
			"@DocString(FMTlpmodel::initialsolve)")
		.def("__eq__", &Models::FMTlpmodel::operator ==,
			"@DocString(FMTlpmodel::operator==)")
		.def("__ne__", &Models::FMTlpmodel::operator !=,
			"@DocString(FMTlpmodel::operator!=)")
		
		.def("getObjValue", &Models::FMTlpmodel::getObjValue,
					"@DocString(FMTlpmodel::getObjValue)")
		.def("getlocalconstraints",
			&Models::FMTlpmodel::getreplanningconstraints,
			"@DocString(FMTlpmodel::getreplanningconstraints)")
		.def("writeLP",
			&Models::FMTlpmodel::writeLP,
			"@DocString(FMTlpmodel::writeLP)")
		.def("getoperatingareaschedulerheuristics", &Models::FMTlpmodel::getoperatingareaschedulerheuristics, getoperatingareaschedulerheuristics_overloads(bp::args("opareas", "node", "numberofheuristics", "copysolver","updatematrixname"), "@DocString(FMTlpmodel::getoperatingareaschedulerheuristics)"))
		.def("getoperatingareaclustererheuristics", &Models::FMTlpmodel::getoperatingareaclustererheuristics, getoperatingareaclustererheuristics_overloads(bp::args("opareas", "statisticoutput", "areaoutput", "period", "numberofheuristics"), "@DocString(FMTlpmodel::getoperatingareaschedulerheuristics)"))
		.def("getvariabilities", &Models::FMTlpmodel::getvariabilities, getvariabilities_overloads(bp::args("outputs", "tolerance"), "@DocString(FMTlpmodel::getvariabilities)"))
		.def("getareavariabilities", &Models::FMTlpmodel::getareavariabilities, getareavariabilities_overloads(bp::args("outputs", "globalmask", "tolerance"), "@DocString(FMTlpmodel::getareavariabilities)"))
		.def("addscheduletoobjective", &Models::FMTlpmodel::addscheduletoobjective, addscheduletoobjective_overloads(bp::args("schedule", "weight"),"@DocString(FMTlpmodel::addscheduletoobjective)"));
		//.def("doplanning", &Models::FMTlpmodel::doplanning, doplanning_overloads(bp::args("solve", "schedules"),"@DocString(FMTlpmodel::doplanning)"));
		

	define_pylist<Models::FMTlpmodel>();
	#endif

	bp::enum_<Models::FMTsawarmuptype>("FMTsawarmuptype")
		.value("log", Models::FMTsawarmuptype::log)
		.value("delta", Models::FMTsawarmuptype::bigdelta)
		.value("logmax", Models::FMTsawarmuptype::logmax)
		.value("bootstrapmagic", Models::FMTsawarmuptype::bootstrapmagic);



	bp::class_<Models::FMTsamodel, bp::bases<Models::FMTmodel>>("FMTsamodel", "@DocString(FMTsamodel)")
            .def(bp::init<Models::FMTmodel>())
			.def(bp::init<Models::FMTsamodel>())	
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
