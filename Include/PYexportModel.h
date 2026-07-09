/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTMODEL_H_INCLUDED
#define PYEXPORTMODEL_H_INCLUDED

#include "FMTmodel.h"
#include "FMTsrmodel.h"
#include "FMTlpmodel.h"
#include "FMTsemodel.h"
#include "FMTsesmodel.h"
#include "FMTnssmodel.h"
#include "FMTsamodel.h"
#include "FMTlpsolver.h"
#include "FMTsaschedule.h"

#include "boost/python.hpp"

namespace Python
{
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(greedyreferencebuild_overloads, greedyReferenceBuild, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(buildperiod_overloads, buildperiod, 0, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(get_outputs_overloads, get_outputs, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getsolution_overloads, getSolution, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getLPoutputoverloads, getOutput, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getSchedulesOverloads, getSchedule, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getPotentialScheduleOverloads, getPotentialSchedule,2,3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(warmup_overloads, warmup, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(evaluate_overloads, evaluate, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getoperatingareaschedulerheuristics_overloads, getOperatingAreaSchedulerHeuristics,2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getoperatingareaclustererheuristics_overloads,getOperatingAreaClustererHeuristics,4,7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getareavariabilities_overloads, getAreaVariabilities,2,3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(boundsolution_overloads, boundSolution, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(eraseperiod_overloads, erasePeriod,0,1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(setsolution_overloads,setSolution, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(setsolutionbylp_overloads,setSolutionByLp, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(addscheduletoobjective_overloads, addScheduleToObjective, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getarea_overloads, getarea, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(doplanning_overloads,doPlanning,1,2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(showparameters_overloads,showParameters,0,1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(eraseconstraint_overloads,eraseConstraint,1, 2)

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
			.def("setyields", &Models::FMTmodel::setYields,
				"@DocString(FMTmodel::setyields)")
			.def("setlifespan", &Models::FMTmodel::setLifespan,
				"@DocString(FMTmodel::setlifespan)")
			.def("addageyieldhandlers", &Models::FMTmodel::addYieldHandlers<Core::FMTageyieldhandler>,
				"@DocString(FMTmodel::addyieldhandlers)")
			.def("addtimeyieldhandlers", &Models::FMTmodel::addYieldHandlers<Core::FMTtimeyieldhandler>,
				"@DocString(FMTmodel::addyieldhandlers)")
			.def("getname", &Models::FMTmodel::getname,
				"@DocString(FMTmodel::getname)")
            .def("getarea",&Models::FMTmodel::getarea, getarea_overloads(bp::args("period","beforegrowanddeath"), "@DocString(FMTmodel::getarea)"))
			.def("setarea", &Models::FMTmodel::setArea,
				"@DocString(FMTmodel::setarea)")
			.def("getthemes",&Models::FMTmodel::getthemes,
				"@DocString(FMTmodel::getthemes)")
			.def("setthemes", &Models::FMTmodel::setThemes,
				"@DocString(FMTmodel::setthemes)")
            .def("getactions",&Models::FMTmodel::getactions,
				"@DocString(FMTmodel::getactions)")
			.def("setactions", &Models::FMTmodel::setactions,
				"@DocString(FMTmodel::setactions)")
			.def("getoutputs", &Models::FMTmodel::getoutputs,
				"@DocString(FMTmodel::getoutputs)")
			.def("setoutputs", &Models::FMTmodel::setOutputs,
				"@DocString(FMTmodel::setoutputs)")
			.def("getconstraints", &Models::FMTmodel::getconstraints,
				"@DocString(FMTmodel::getconstraints)")
			.def("setconstraints", &Models::FMTmodel::setconstraints,
				"@DocString(FMTmodel::setconstraints)")
            .def("gettransitions",&Models::FMTmodel::getTransitions,
				"@DocString(FMTmodel::gettransitions)")
            .def("settransitions",&Models::FMTmodel::setTransitions,
				"@DocString(FMTmodel::settransitions)")
			.def("setname", &Models::FMTmodel::setName,
				"@DocString(FMTmodel::setname)")
			.def("setareaperiod", &Models::FMTmodel::setAreaPeriod,
				"@DocString(FMTmodel::setareaperiod)")
            .def("getlifespan",&Models::FMTmodel::getLifespan,
				"@DocString(FMTmodel::getlifespan)")
            .def("isValid",&Models::FMTmodel::isValid,
				"@DocString(FMTmodel::isValid)")
			.def("clearcache",&Models::FMTmodel::clearCache,
				"@DocString(FMTmodel::clearcache)")
			.def("getselectedmask",&Models::FMTmodel::getSelectedMask,
				"@DocString(FMTmodel::getselectedmask)")
			.def("getpostsolvefilter",&Models::FMTmodel::getPostsolveFilter,
				"@DocString(FMTmodel::getpostsolvefilter)")
			.def("basepresolve",&Models::FMTmodel::basePresolve,
				"@DocString(FMTmodel::basepresolve)")
			.def("isoptimal", &Models::FMTmodel::isOptimal,
				"@DocString(FMTmodel::isoptimal)")
			.def("getobjectivevalue", &Models::FMTmodel::getObjectiveValue,
				"@DocString(FMTmodel::getobjectivevalue)")
			.def("getpotentialschedule", &Models::FMTmodel::getPotentialSchedule,
				getPotentialScheduleOverloads(bp::args("toremove","selection","withlock"),"@DocString(FMTmodel::getpotentialschedule)"))
				//The way to expose overload member functions with different args
			.def<bool (Models::FMTmodel::*)(const Models::FMTintmodelparameters& key, const int& value)>("setparameter", &Models::FMTmodel::setparameter,
				"@DocString(FMTmodel::setparameter(const FMTintmodelparameters,const int&))")
			.def<bool (Models::FMTmodel::*)(const Models::FMTdblmodelparameters& key, const double& value)>("setparameter", &Models::FMTmodel::setparameter,
				"@DocString(FMTmodel::setparameter(const FMTdblmodelparameters,const double))")
			.def<bool (Models::FMTmodel::*)(const Models::FMTboolmodelparameters& key, const bool& value)>("setparameter", &Models::FMTmodel::setparameter,
				"@DocString(FMTmodel::setparameter(const FMTboolmodelparameters,const bool))")
			.def<int (Models::FMTmodel::*)(const Models::FMTintmodelparameters& key)const>("getparameter", &Models::FMTmodel::getparameter,
				"@DocString(FMTmodel::getparameter(const FMTintmodelparameters))")
			.def<double (Models::FMTmodel::*)(const Models::FMTdblmodelparameters& key)const>("getparameter", &Models::FMTmodel::getparameter,
				"@DocString(FMTmodel::getparameter(const FMTdblmodelparameters))")
			.def<bool (Models::FMTmodel::*)(const Models::FMTboolmodelparameters& key)const>("getparameter", &Models::FMTmodel::getparameter,
				"@DocString(FMTmodel::getparameter(const FMTboolmodelparameters))")
			.def("setcompresstime",&Models::FMTmodel::setCompressTime,
					"@DocString(FMTmodel::setcompresstime)")
			.def("getcompresstime",&Models::FMTmodel::getCompressTime,
					"@DocString(FMTmodel::getcompresstime)")
			.def("showparameters",&Models::FMTmodel::showParameters,
					showparameters_overloads(bp::args("showhelp"),"@DocString(FMTmodel::showparameters)"))
			.def("doplanning",&Models::FMTmodel::doPlanning,
					doplanning_overloads(bp::args("solve"),"@DocString(FMTmodel::doplanning)"))
			.def("getsolution", &Models::FMTmodel::getSolution,
				getsolution_overloads(bp::args("period", "withlock"), "@DocString(FMTmodel::getsolution)"))
			.def("getoutput", &Models::FMTmodel::getOutput,
				getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTsemodel::getoutput)"));

    definePyList<Models::FMTmodel>();



	bp::to_python_converter<std::map<std::string, double>, MapToDict<std::string, double>>();
	

	bp::class_<Models::FMTsemodel, bp::bases<Models::FMTmodel>>("FMTsemodel", "@DocString(FMTsemodel)")
		.def(bp::init<Models::FMTmodel>())
		.def_pickle(FMT_pickle_suite<Models::FMTsemodel>())
		.def("getmapping", &Models::FMTsemodel::getMapping,
			"@DocString(FMTsemodel::getmapping)")
		.def("setinitialmapping", &Models::FMTsemodel::setInitialMapping,
			"@DocString(FMTsemodel::setinitialmapping)")
		.def("getschedule", &Models::FMTsemodel::getSchedule,
			getSchedulesOverloads(bp::args("withlock"), "@DocString(FMTsemodel::getschedule)"))
		.def("getdisturbancestats", &Models::FMTsemodel::getDisturbanceStats,
			"@DocString(FMTsemodel::getdisturbancestats)")
		.def("getspatialoutput", &Models::FMTsemodel::getSpatialOutput,
			"@DocString(FMTsemodel::getspatialoutput)")
		/*.def("getSolution", &Models::FMTsemodel::getSolution,
			getsolution_overloads(bp::args("period", "withlock"),"@DocString(FMTsemodel::getSolution)"))
		.def("getarea", &Models::FMTsemodel::getarea,
			getarea_overloads(bp::args("period", "beforegrowanddeath"), "@DocString(FMTsemodel::getarea)"))
		.def("getOutput", &Models::FMTsemodel::getOutput,
			getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTsemodel::getOutput)"))*/
		.def("getspschedule", &Models::FMTsemodel::getSpSchedule,
			"@DocString(FMTsemodel::getspschedule)");

	definePyList<Models::FMTsemodel>();


	bp::class_<Models::FMTsesmodel, bp::bases<Models::FMTsemodel>>("FMTsesmodel", "@DocString(FMTsesmodel)")
		.def(bp::init<Models::FMTmodel>())
		.def_pickle(FMT_pickle_suite<Models::FMTsesmodel>())
		.def("greedyreferencebuild", &Models::FMTsesmodel::greedyReferenceBuild,
			greedyreferencebuild_overloads(bp::args("schedule", "numberofiterations", "seed", "tolerance"), "@DocString(FMTsesmodel::GreedyReferenceBuild)"));

    definePyList<Models::FMTsesmodel>();

	bp::enum_<Models::FMTintmodelparameters>("FMTintmodelparameters")
		.value("LENGTH", Models::FMTintmodelparameters::LENGTH)
		.value("SEED", Models::FMTintmodelparameters::SEED)
		.value("NUMBER_OF_ITERATIONS", Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS)
		.value("PRESOLVE_ITERATIONS", Models::FMTintmodelparameters::PRESOLVE_ITERATIONS)
		.value("NUMBER_OF_THREADS", Models::FMTintmodelparameters::NUMBER_OF_THREADS)
		.value("MATRIX_TYPE", Models::FMTintmodelparameters::MATRIX_TYPE);
	
	definePyList<Models::FMTintmodelparameters>();
	
	bp::enum_<Models::FMTdblmodelparameters>("FMTdblmodelparameters")
		.value("TOLERANCE", Models::FMTdblmodelparameters::TOLERANCE)
		.value("GOALING_SCHEDULE_WEIGHT", Models::FMTdblmodelparameters::GOALING_SCHEDULE_WEIGHT);

	definePyList<Models::FMTdblmodelparameters>();

	bp::enum_<Models::FMTboolmodelparameters>("FMTboolmodelparameters")
		.value("FORCE_PARTIAL_BUILD", Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)
		.value("STRICTLY_POSITIVE", Models::FMTboolmodelparameters::STRICTLY_POSITIVE)
		.value("POSTSOLVE", Models::FMTboolmodelparameters::POSTSOLVE)
		.value("SHOW_LOCK_IN_SCHEDULES", Models::FMTboolmodelparameters::SHOW_LOCK_IN_SCHEDULES)
		.value("PRESOLVE_CAN_REMOVE_STATIC_THEMES", Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES)
		.value("DEBUG_MATRIX", Models::FMTboolmodelparameters::DEBUG_MATRIX);

	definePyList<Models::FMTboolmodelparameters>();

	

	#ifdef FMTWITHOSI

	bp::enum_<Models::FMTsolverinterface>("FMTsolverinterface")
		.value("CLP", Models::FMTsolverinterface::CLP)
		#ifdef  FMTWITHMOSEK
			.value("MOSEK", Models::FMTsolverinterface::MOSEK)
		#endif
		.value("CPLEX", Models::FMTsolverinterface::CPLEX)
		.value("GUROBI", Models::FMTsolverinterface::GUROBI);
		
	definePyList<Models::FMTsolverinterface>();

	bp::class_<Models::FMTlpsolver>("FMTlpolver", "@DocString(FMTlpsolver)")
		//.def(bp::init<Models::FMTsolverinterface>())
		.def("isProvenOptimal", &Models::FMTlpsolver::isProvenOptimal,
			"@DocString(FMTlpsolver::isProvenOptimal)")
		.def("getObjValue", &Models::FMTlpsolver::getObjValue,
			"@DocString(FMTlpsolver::getObjValue)")
		.def("getObjSense", &Models::FMTlpsolver::getObjSense,
			"@DocString(FMTlpsolver::getObjSense)")
		.def("setnumberofthreads", &Models::FMTlpsolver::setNumberOfThreads,
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
		
		.def("setsolution", &Models::FMTsrmodel::setSolution,
			setsolution_overloads(bp::args("period", "schedule", "tolerance"), "@DocString(FMTsrmodel::setsolution)"))
		.def("forcesolution", &Models::FMTsrmodel::forceSolution, "@DocString(FMTsrmodel::forcesolution)")
		.def("setsolutionbylp", &Models::FMTsrmodel::setSolutionByLp,
			setsolutionbylp_overloads(bp::args("period", "schedule", "tolerance"), "@DocString(FMTsrmodel::setsolutionbylp)"))
		//.def("getOutput", &Models::FMTsrmodel::getOutput, getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTsrmodel::getOutput)"))
		.def("cleargraphdevelopements", &Models::FMTsrmodel::clearGraphDevelopments,
			"@DocString(FMTsrmodel::cleargraphdevelopements)")
		.def("getstats", &Models::FMTsrmodel::getStats,
			"@DocString(FMTsrmodel::getstats)")
		.def("getRotations", &Models::FMTsrmodel::getRotations,
			"@DocString(FMTsrmodel::getRotations)")
		.def("getGraphStats", &Models::FMTsrmodel::getGraphStats,
			"@DocString(FMTsrmodel::getGraphStats)")
		.def("getavailablesolverinterface", &Models::FMTsrmodel::getavailablesolverinterface,
			"@DocString(FMTsrmodel::getavailablesolverinterface)").staticmethod("getavailablesolverinterface")
		.def("getscheduleproportions", &Models::FMTsrmodel::getScheduleProportions,
			"@DocString(FMTsrmodel::getscheduleproportions)");

	definePyList<Models::FMTsrmodel>();
	
	bp::class_<Models::FMTnssmodel, bp::bases<Models::FMTsrmodel>>("FMTnssmodel", "@DocString(FMTnssmodel)")
		.def(bp::init<Models::FMTmodel, unsigned int>())
		.def(bp::init<Models::FMTnssmodel>())
		.def_pickle(FMT_pickle_suite<Models::FMTnssmodel>())
		.def("simulate", &Models::FMTnssmodel::simulate);

	definePyList<Models::FMTnssmodel>();


	bp::class_<Models::FMTlpmodel, bp::bases<Models::FMTsrmodel>>("FMTlpmodel", "@DocString(FMTlpmodel)")
		.def(bp::init<Models::FMTmodel, Models::FMTsolverinterface>())
		.def(bp::init<Models::FMTlpmodel>())
		.def_pickle(FMT_pickle_suite<Models::FMTlpmodel>())
		//.def("boundSolution", &Models::FMTlpmodel::boundSolution,
		//	boundsolution_overloads(bp::args("period", "tolerance"),
		//		"@DocString(FMTlpmodel::boundSolution)"))
		.def("setobjective", &Models::FMTlpmodel::setObjective,
			"@DocString(FMTlpmodel::setobjective)")
		.def("clearcache", &Models::FMTlpmodel::clearCache,
			"@DocString(FMTlpmodel::clearcache)")
		.def("clearconstraintlocation", &Models::FMTlpmodel::clearConstraintLocation,
			"@DocString(FMTlpmodel::clearconstraintlocation)")
		.def("setstrictlypositivesoutputsmatrix", &Models::FMTlpmodel::setstrictlypositivesoutputsmatrix,
			"@DocString(FMTlpmodel::setstrictlypositivesoutputsmatrix)")
		.def("setconstraint", &Models::FMTlpmodel::setConstraint,
			"@DocString(FMTlpmodel::setconstraint)")
		.def("eraseconstraint", &Models::FMTlpmodel::eraseConstraint,
			eraseconstraint_overloads(bp::args("constraint","period"),"@DocString(FMTlpmodel::eraseconstraint)"))
		.def("eraseperiod", &Models::FMTlpmodel::erasePeriod,
			eraseperiod_overloads(bp::args("constraintsonly"), "@DocString(FMTlpmodel::eraseperiod)"))
		.def("resolve", &Models::FMTlpmodel::resolve,
			"@DocString(FMTlpmodel::resolve)")
		.def("initialsolve", &Models::FMTlpmodel::initialSolve,
			"@DocString(FMTlpmodel::initialsolve)")
		.def("__eq__", &Models::FMTlpmodel::operator ==,
			"@DocString(FMTlpmodel::operator==)")
		.def("__ne__", &Models::FMTlpmodel::operator !=,
			"@DocString(FMTlpmodel::operator!=)")
		
		.def("getObjValue", &Models::FMTlpmodel::getObjValue,
					"@DocString(FMTlpmodel::getObjValue)")
		.def("getlocalconstraints",
			&Models::FMTlpmodel::getReplanningConstraints,
			"@DocString(FMTlpmodel::getreplanningconstraints)")
		.def("writeLP",
			&Models::FMTlpmodel::writeLP,
			"@DocString(FMTlpmodel::writeLP)")
		.def("getoperatingareaschedulerheuristics", &Models::FMTlpmodel::getOperatingAreaSchedulerHeuristics, getoperatingareaschedulerheuristics_overloads(bp::args("opareas", "node", "numberofheuristics", "copysolver","updatematrixname"), "@DocString(FMTlpmodel::getoperatingareaschedulerheuristics)"))
		.def("getoperatingareaclustererheuristics", &Models::FMTlpmodel::getOperatingAreaClustererHeuristics, getoperatingareaclustererheuristics_overloads(bp::args("opareas", "statisticoutput", "areaoutput", "period", "numberofheuristics"), "@DocString(FMTlpmodel::getoperatingareaschedulerheuristics)"))
		.def("getvariabilities", &Models::FMTlpmodel::getVariabilities, "@DocString(FMTlpmodel::getvariabilities)")
		.def("getareavariabilities", &Models::FMTlpmodel::getAreaVariabilities, getareavariabilities_overloads(bp::args("outputs", "globalmask", "tolerance"), "@DocString(FMTlpmodel::getareavariabilities)"))
		.def("addscheduletoobjective", &Models::FMTlpmodel::addScheduleToObjective, addscheduletoobjective_overloads(bp::args("schedule", "weight"),"@DocString(FMTlpmodel::addscheduletoobjective)"));
		//.def("doPlanning", &Models::FMTlpmodel::doPlanning, doplanning_overloads(bp::args("solve", "schedules"),"@DocString(FMTlpmodel::doPlanning)"));
		

	definePyList<Models::FMTlpmodel>();
	#endif




	bp::class_<Models::FMTsamodel, bp::bases<Models::FMTsemodel>>("FMTsamodel", "@DocString(FMTsamodel)")
		.def(bp::init<Models::FMTmodel>())
		.def(bp::init<Models::FMTsamodel>())
		.def("initialsolve", &Models::FMTsamodel::initialSolve,
			"@DocString(FMTsamodel::initialsolve)");

    definePyList<Models::FMTsamodel>();

    }
}
#endif // PYEXPORTMODEL_H_INCLUDED
