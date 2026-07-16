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
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(buildperiod_overloads, buildPeriod, 0, 3)
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
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getarea_overloads, getArea, 0, 2)
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


	bp::class_<Models::FMTModel, bp::bases<Core::FMTObject>>("FMTmodel", "@DocString(FMTModel)")
			.def_pickle(FMT_pickle_suite<Models::FMTModel>())
			.def(bp::init<Models::FMTModel>())
            .def("getyields",&Models::FMTModel::getYields,
				"@DocString(FMTModel::getyields)")
			.def("setyields", &Models::FMTModel::setYields,
				"@DocString(FMTModel::setyields)")
			.def("setlifespan", &Models::FMTModel::setLifespan,
				"@DocString(FMTModel::setlifespan)")
			.def("addageyieldhandlers", &Models::FMTModel::addYieldHandlers<Core::FMTAgeYieldHandler>,
				"@DocString(FMTModel::addyieldhandlers)")
			.def("addtimeyieldhandlers", &Models::FMTModel::addYieldHandlers<Core::FMTTimeYieldHandler>,
				"@DocString(FMTModel::addyieldhandlers)")
			.def("getname", &Models::FMTModel::getName,
				"@DocString(FMTModel::getname)")
            .def("getarea",&Models::FMTModel::getArea, getarea_overloads(bp::args("period","beforegrowanddeath"), "@DocString(FMTModel::getarea)"))
			.def("setarea", &Models::FMTModel::setArea,
				"@DocString(FMTModel::setarea)")
			.def("getthemes",&Models::FMTModel::getThemes,
				"@DocString(FMTModel::getthemes)")
			.def("setthemes", &Models::FMTModel::setThemes,
				"@DocString(FMTModel::setthemes)")
            .def("getactions",&Models::FMTModel::getactions,
				"@DocString(FMTModel::getactions)")
			.def("setactions", &Models::FMTModel::setActions,
				"@DocString(FMTModel::setactions)")
			.def("getoutputs", &Models::FMTModel::getOutputs,
				"@DocString(FMTModel::getoutputs)")
			.def("setoutputs", &Models::FMTModel::setOutputs,
				"@DocString(FMTModel::setoutputs)")
			.def("getconstraints", &Models::FMTModel::getconstraints,
				"@DocString(FMTModel::getconstraints)")
			.def("setconstraints", &Models::FMTModel::setConstraints,
				"@DocString(FMTModel::setconstraints)")
            .def("gettransitions",&Models::FMTModel::getTransitions,
				"@DocString(FMTModel::gettransitions)")
            .def("settransitions",&Models::FMTModel::setTransitions,
				"@DocString(FMTModel::settransitions)")
			.def("setname", &Models::FMTModel::setName,
				"@DocString(FMTModel::setname)")
			.def("setareaperiod", &Models::FMTModel::setAreaPeriod,
				"@DocString(FMTModel::setareaperiod)")
            .def("getlifespan",&Models::FMTModel::getLifespan,
				"@DocString(FMTModel::getlifespan)")
            .def("isValid",&Models::FMTModel::isValid,
				"@DocString(FMTModel::isValid)")
			.def("clearcache",&Models::FMTModel::clearCache,
				"@DocString(FMTModel::clearcache)")
			.def("getselectedmask",&Models::FMTModel::getSelectedMask,
				"@DocString(FMTModel::getselectedmask)")
			.def("getpostsolvefilter",&Models::FMTModel::getPostsolveFilter,
				"@DocString(FMTModel::getpostsolvefilter)")
			.def("basepresolve",&Models::FMTModel::basePresolve,
				"@DocString(FMTModel::basepresolve)")
			.def("isoptimal", &Models::FMTModel::isOptimal,
				"@DocString(FMTModel::isoptimal)")
			.def("getobjectivevalue", &Models::FMTModel::getObjectiveValue,
				"@DocString(FMTModel::getobjectivevalue)")
			.def("getpotentialschedule", &Models::FMTModel::getPotentialSchedule,
				getPotentialScheduleOverloads(bp::args("toremove","selection","withlock"),"@DocString(FMTModel::getpotentialschedule)"))
				//The way to expose overload member functions with different args
			.def<bool (Models::FMTModel::*)(const Models::FMTintmodelparameters& key, const int& value)>("setparameter", &Models::FMTModel::setParameter,
				"@DocString(FMTModel::setparameter(const FMTintmodelparameters,const int&))")
			.def<bool (Models::FMTModel::*)(const Models::FMTdblmodelparameters& key, const double& value)>("setparameter", &Models::FMTModel::setParameter,
				"@DocString(FMTModel::setparameter(const FMTdblmodelparameters,const double))")
			.def<bool (Models::FMTModel::*)(const Models::FMTboolmodelparameters& key, const bool& value)>("setparameter", &Models::FMTModel::setParameter,
				"@DocString(FMTModel::setparameter(const FMTboolmodelparameters,const bool))")
			.def<int (Models::FMTModel::*)(const Models::FMTintmodelparameters& key)const>("getparameter", &Models::FMTModel::getParameter,
				"@DocString(FMTModel::getparameter(const FMTintmodelparameters))")
			.def<double (Models::FMTModel::*)(const Models::FMTdblmodelparameters& key)const>("getparameter", &Models::FMTModel::getParameter,
				"@DocString(FMTModel::getparameter(const FMTdblmodelparameters))")
			.def<bool (Models::FMTModel::*)(const Models::FMTboolmodelparameters& key)const>("getparameter", &Models::FMTModel::getParameter,
				"@DocString(FMTModel::getparameter(const FMTboolmodelparameters))")
			.def("setcompresstime",&Models::FMTModel::setCompressTime,
					"@DocString(FMTModel::setcompresstime)")
			.def("getcompresstime",&Models::FMTModel::getCompressTime,
					"@DocString(FMTModel::getcompresstime)")
			.def("showparameters",&Models::FMTModel::showParameters,
					showparameters_overloads(bp::args("showhelp"),"@DocString(FMTModel::showparameters)"))
			.def("doplanning",&Models::FMTModel::doPlanning,
					doplanning_overloads(bp::args("solve"),"@DocString(FMTModel::doplanning)"))
			.def("getsolution", &Models::FMTModel::getSolution,
				getsolution_overloads(bp::args("period", "withlock"), "@DocString(FMTModel::getsolution)"))
			.def("getoutput", &Models::FMTModel::getOutput,
				getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTSeModel::getoutput)"));

    definePyList<Models::FMTModel>();



	bp::to_python_converter<std::map<std::string, double>, MapToDict<std::string, double>>();
	

	bp::class_<Models::FMTSeModel, bp::bases<Models::FMTModel>>("FMTsemodel", "@DocString(FMTSeModel)")
		.def(bp::init<Models::FMTModel>())
		.def_pickle(FMT_pickle_suite<Models::FMTSeModel>())
		.def("getmapping", &Models::FMTSeModel::getMapping,
			"@DocString(FMTSeModel::getmapping)")
		.def("setinitialmapping", &Models::FMTSeModel::setInitialMapping,
			"@DocString(FMTSeModel::setinitialmapping)")
		.def("getschedule", &Models::FMTSeModel::getSchedule,
			getSchedulesOverloads(bp::args("withlock"), "@DocString(FMTSeModel::getschedule)"))
		.def("getdisturbancestats", &Models::FMTSeModel::getDisturbanceStats,
			"@DocString(FMTSeModel::getdisturbancestats)")
		.def("getspatialoutput", &Models::FMTSeModel::getSpatialOutput,
			"@DocString(FMTSeModel::getspatialoutput)")
		/*.def("getSolution", &Models::FMTSeModel::getSolution,
			getsolution_overloads(bp::args("period", "withlock"),"@DocString(FMTSeModel::getSolution)"))
		.def("getarea", &Models::FMTSeModel::getArea,
			getarea_overloads(bp::args("period", "beforegrowanddeath"), "@DocString(FMTSeModel::getarea)"))
		.def("getOutput", &Models::FMTSeModel::getOutput,
			getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTSeModel::getOutput)"))*/
		.def("getspschedule", &Models::FMTSeModel::getSpSchedule,
			"@DocString(FMTSeModel::getspschedule)");

	definePyList<Models::FMTSeModel>();


	bp::class_<Models::FMTSesModel, bp::bases<Models::FMTSeModel>>("FMTsesmodel", "@DocString(FMTSesModel)")
		.def(bp::init<Models::FMTModel>())
		.def_pickle(FMT_pickle_suite<Models::FMTSesModel>())
		.def("greedyreferencebuild", &Models::FMTSesModel::greedyReferenceBuild,
			greedyreferencebuild_overloads(bp::args("schedule", "numberofiterations", "seed", "tolerance"), "@DocString(FMTSesModel::GreedyReferenceBuild)"));

    definePyList<Models::FMTSesModel>();

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

	bp::class_<Models::FMTLpSolver>("FMTlpolver", "@DocString(FMTLpSolver)")
		//.def(bp::init<Models::FMTsolverinterface>())
		.def("isProvenOptimal", &Models::FMTLpSolver::isProvenOptimal,
			"@DocString(FMTLpSolver::isProvenOptimal)")
		.def("getObjValue", &Models::FMTLpSolver::getObjValue,
			"@DocString(FMTLpSolver::getObjValue)")
		.def("getObjSense", &Models::FMTLpSolver::getObjSense,
			"@DocString(FMTLpSolver::getObjSense)")
		.def("setnumberofthreads", &Models::FMTLpSolver::setNumberOfThreads,
			"@DocString(FMTLpSolver::setnumberofthreads)")
		.def("setMIPgaptolerance", &Models::FMTLpSolver::setMIPgaptolerance,
			"@DocString(FMTLpSolver::setMIPgaptolerance)")
		.def("setoptimizerMAXtime", &Models::FMTLpSolver::setoptimizerMAXtime,
			"@DocString(FMTLpSolver::setoptimizerMAXtime)")
		.def("writeLP", &Models::FMTLpSolver::writeLP,
			"@DocString(FMTLpSolver::writeLP)")
		.def("writeMPS", &Models::FMTLpSolver::writeMPS,
			"@DocString(FMTLpSolver::writeMPS)");

	bp::class_<Models::FMTSrModel, bp::bases<Models::FMTModel>>("FMTsrmodel", "@DocString(FMTSrModel)")
		.def_pickle(FMT_pickle_suite<Models::FMTSrModel>())
		.def("buildperiod", &Models::FMTSrModel::buildPeriod, buildperiod_overloads(bp::args("schedule", "forcepartialbuild", "compressageclass"), "@DocString(FMTSrModel::buildperiod)"))
		
		.def("setsolution", &Models::FMTSrModel::setSolution,
			setsolution_overloads(bp::args("period", "schedule", "tolerance"), "@DocString(FMTSrModel::setsolution)"))
		.def("forcesolution", &Models::FMTSrModel::forceSolution, "@DocString(FMTSrModel::forcesolution)")
		.def("setsolutionbylp", &Models::FMTSrModel::setSolutionByLp,
			setsolutionbylp_overloads(bp::args("period", "schedule", "tolerance"), "@DocString(FMTSrModel::setsolutionbylp)"))
		//.def("getOutput", &Models::FMTSrModel::getOutput, getLPoutputoverloads(bp::args("output", "period", "level"), "@DocString(FMTSrModel::getOutput)"))
		.def("cleargraphdevelopements", &Models::FMTSrModel::clearGraphDevelopments,
			"@DocString(FMTSrModel::cleargraphdevelopements)")
		.def("getstats", &Models::FMTSrModel::getStats,
			"@DocString(FMTSrModel::getstats)")
		.def("getRotations", &Models::FMTSrModel::getRotations,
			"@DocString(FMTSrModel::getRotations)")
		.def("getGraphStats", &Models::FMTSrModel::getGraphStats,
			"@DocString(FMTSrModel::getGraphStats)")
		.def("getavailablesolverinterface", &Models::FMTSrModel::getAvailableSolverInterface,
			"@DocString(FMTSrModel::getavailablesolverinterface)").staticmethod("getavailablesolverinterface")
		.def("getscheduleproportions", &Models::FMTSrModel::getScheduleProportions,
			"@DocString(FMTSrModel::getscheduleproportions)");

	definePyList<Models::FMTSrModel>();
	
	bp::class_<Models::FMTNssModel, bp::bases<Models::FMTSrModel>>("FMTnssmodel", "@DocString(FMTNssModel)")
		.def(bp::init<Models::FMTModel, unsigned int>())
		.def(bp::init<Models::FMTNssModel>())
		.def_pickle(FMT_pickle_suite<Models::FMTNssModel>())
		.def("simulate", &Models::FMTNssModel::simulate);

	definePyList<Models::FMTNssModel>();


	bp::class_<Models::FMTLpModel, bp::bases<Models::FMTSrModel>>("FMTlpmodel", "@DocString(FMTLpModel)")
		.def(bp::init<Models::FMTModel, Models::FMTsolverinterface>())
		.def(bp::init<Models::FMTLpModel>())
		.def_pickle(FMT_pickle_suite<Models::FMTLpModel>())
		//.def("boundSolution", &Models::FMTLpModel::boundSolution,
		//	boundsolution_overloads(bp::args("period", "tolerance"),
		//		"@DocString(FMTLpModel::boundSolution)"))
		.def("setobjective", &Models::FMTLpModel::setObjective,
			"@DocString(FMTLpModel::setobjective)")
		.def("clearcache", &Models::FMTLpModel::clearCache,
			"@DocString(FMTLpModel::clearcache)")
		.def("clearconstraintlocation", &Models::FMTLpModel::clearConstraintLocation,
			"@DocString(FMTLpModel::clearconstraintlocation)")
		.def("setstrictlypositivesoutputsmatrix", &Models::FMTLpModel::setStrictlyPositivesOutputsMatrix,
			"@DocString(FMTLpModel::setstrictlypositivesoutputsmatrix)")
		.def("setconstraint", &Models::FMTLpModel::setConstraint,
			"@DocString(FMTLpModel::setconstraint)")
		.def("eraseconstraint", &Models::FMTLpModel::eraseConstraint,
			eraseconstraint_overloads(bp::args("constraint","period"),"@DocString(FMTLpModel::eraseconstraint)"))
		.def("eraseperiod", &Models::FMTLpModel::erasePeriod,
			eraseperiod_overloads(bp::args("constraintsonly"), "@DocString(FMTLpModel::eraseperiod)"))
		.def("resolve", &Models::FMTLpModel::resolve,
			"@DocString(FMTLpModel::resolve)")
		.def("initialsolve", &Models::FMTLpModel::initialSolve,
			"@DocString(FMTLpModel::initialsolve)")
		.def("__eq__", &Models::FMTLpModel::operator ==,
			"@DocString(FMTLpModel::operator==)")
		.def("__ne__", &Models::FMTLpModel::operator !=,
			"@DocString(FMTLpModel::operator!=)")
		
		.def("getObjValue", &Models::FMTLpModel::getObjValue,
					"@DocString(FMTLpModel::getObjValue)")
		.def("getlocalconstraints",
			&Models::FMTLpModel::getReplanningConstraints,
			"@DocString(FMTLpModel::getreplanningconstraints)")
		.def("writeLP",
			&Models::FMTLpModel::writeLP,
			"@DocString(FMTLpModel::writeLP)")
		.def("getoperatingareaschedulerheuristics", &Models::FMTLpModel::getOperatingAreaSchedulerHeuristics, getoperatingareaschedulerheuristics_overloads(bp::args("opareas", "node", "numberofheuristics", "copysolver","updatematrixname"), "@DocString(FMTLpModel::getoperatingareaschedulerheuristics)"))
		.def("getoperatingareaclustererheuristics", &Models::FMTLpModel::getOperatingAreaClustererHeuristics, getoperatingareaclustererheuristics_overloads(bp::args("opareas", "statisticoutput", "areaoutput", "period", "numberofheuristics"), "@DocString(FMTLpModel::getoperatingareaschedulerheuristics)"))
		.def("getvariabilities", &Models::FMTLpModel::getVariabilities, "@DocString(FMTLpModel::getvariabilities)")
		.def("getareavariabilities", &Models::FMTLpModel::getAreaVariabilities, getareavariabilities_overloads(bp::args("outputs", "globalmask", "tolerance"), "@DocString(FMTLpModel::getareavariabilities)"))
		.def("addscheduletoobjective", &Models::FMTLpModel::addScheduleToObjective, addscheduletoobjective_overloads(bp::args("schedule", "weight"),"@DocString(FMTLpModel::addscheduletoobjective)"));
		//.def("doPlanning", &Models::FMTLpModel::doPlanning, doplanning_overloads(bp::args("solve", "schedules"),"@DocString(FMTLpModel::doPlanning)"));
		

	definePyList<Models::FMTLpModel>();
	#endif




	bp::class_<Models::FMTSaModel, bp::bases<Models::FMTSeModel>>("FMTsamodel", "@DocString(FMTSaModel)")
		.def(bp::init<Models::FMTModel>())
		.def(bp::init<Models::FMTSaModel>())
		.def("initialsolve", &Models::FMTSaModel::initialSolve,
			"@DocString(FMTSaModel::initialsolve)");

    definePyList<Models::FMTSaModel>();

    }
}
#endif // PYEXPORTMODEL_H_INCLUDED
