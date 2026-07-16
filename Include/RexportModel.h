/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTMODEL_H_INCLUDED
#define REXPORTMODEL_H_INCLUDED

#include "FMTmodel.h"
#include "FMTsrmodel.h"
#include "FMTlpmodel.h"
#include "FMTsemodel.h"
#include "FMTsesmodel.h"
#include "FMTnssmodel.h"
#include "FMTsamodel.h"
#include "FMTlpsolver.h"
#include "Rdefinitions.h"
#include "FMTsolverinterface.h"
#include <Rcpp.h>
#include <string>

RCPP_EXPOSED_WRAP(Models::FMTModel);
RCPP_EXPOSED_AS(Models::FMTModel);
RCPP_DEFINEVECTOR(Models::FMTModel);//For vector
RCPP_DEFINEMAP(std::string,double);//For map<string,double>
RCPP_EXPOSED_WRAP(Models::FMTSeModel);
RCPP_EXPOSED_AS(Models::FMTSeModel);
RCPP_DEFINEVECTOR(Models::FMTSeModel);//For vector
RCPP_EXPOSED_WRAP(Models::FMTSesModel);
RCPP_EXPOSED_AS(Models::FMTSesModel);
RCPP_DEFINEVECTOR(Models::FMTSesModel);//For vector
#ifdef FMTWITHOSI
	RCPP_EXPOSED_WRAP(Models::FMTSrModel);
	RCPP_EXPOSED_AS(Models::FMTSrModel);
	RCPP_DEFINEVECTOR(Models::FMTSrModel);//For vector
	RCPP_EXPOSED_WRAP(Models::FMTNssModel);
	RCPP_EXPOSED_AS(Models::FMTNssModel);
	RCPP_DEFINEVECTOR(Models::FMTNssModel);//For vector
	RCPP_EXPOSED_ENUM_NODECL(Models::FMTsolverinterface);
	RCPP_EXPOSED_WRAP(Models::FMTLpSolver);
	RCPP_EXPOSED_AS(Models::FMTLpSolver);
	RCPP_DEFINEVECTOR(Models::FMTLpSolver);//For vector
	RCPP_EXPOSED_WRAP(Models::FMTLpModel);
	RCPP_EXPOSED_AS(Models::FMTLpModel);
	RCPP_DEFINEVECTOR(Models::FMTLpModel);//For vector
#endif

RCPP_EXPOSED_WRAP(Models::FMTSaModel);
RCPP_EXPOSED_AS(Models::FMTSaModel);
RCPP_DEFINEVECTOR(Models::FMTSaModel);//For vector

RCPP_EXPOSED_ENUM_NODECL(Models::FMTintmodelparameters);
RCPP_EXPOSED_ENUM_NODECL(Models::FMTdblmodelparameters);
RCPP_EXPOSED_ENUM_NODECL(Models::FMTboolmodelparameters);

namespace R
{ 

void exportModel()
{
	Rcpp::class_<Models::FMTModel>("FMTmodel", "@DocString(FMTModel)")
		.derives<Core::FMTObject>("FMTobject")
		.constructor<Models::FMTModel>("@DocString(FMTModel(Models::FMTModel))")
		.constructor("@DocString(FMTModel())")
           .method("getname",&Models::FMTModel::getName,
				"@DocString(FMTModel::getname)")
            .method("getyields",&Models::FMTModel::getYields,
				"@DocString(FMTModel::getyields)")
			.method("setyields", &Models::FMTModel::setYields,
				"@DocString(FMTModel::setyields)")
            .method("getarea",&Models::FMTModel::getArea,
				"@DocString(FMTModel::getarea)")
			.method("setarea", &Models::FMTModel::setArea,
				"@DocString(FMTModel::setarea)")
            .method("getthemes",&Models::FMTModel::getThemes,
				"@DocString(FMTModel::getthemes)")
			.method("setthemes", &Models::FMTModel::setThemes,
				"@DocString(FMTModel::setthemes)")
            .method("getactions",&Models::FMTModel::getactions,
				"@DocString(FMTModel::getactions)")
			.method("setactions", &Models::FMTModel::setActions,
				"@DocString(FMTModel::setactions)")
			.method("getoutputs", &Models::FMTModel::getOutputs, 
				"@DocString(FMTModel::getoutputs)")
			.method("setoutputs", &Models::FMTModel::setOutputs,
				"@DocString(FMTModel::setoutputs)")
			.method("getconstraints", &Models::FMTModel::getconstraints,
				"@DocString(FMTModel::getconstraints)")
			.method("setconstraints", &Models::FMTModel::setConstraints,
				"@DocString(FMTModel::setconstraints)")
            .method("gettransitions",&Models::FMTModel::getTransitions,
				"@DocString(FMTModel::gettransitions)")
            .method("settransitions",&Models::FMTModel::setTransitions,
				"@DocString(FMTModel::settransitions)")
			.method("setname", &Models::FMTModel::setName,
				"@DocString(FMTModel::setname)")
			.method("setareaperiod", &Models::FMTModel::setAreaPeriod,
				"@DocString(FMTModel::setareaperiod)")
            .method("getlifespan",&Models::FMTModel::getLifespan,
				"@DocString(FMTModel::getlifespan)")
			.method("setlifespan", &Models::FMTModel::setLifespan,
				"@DocString(FMTModel::setlifespan)")
            .method("isValid",&Models::FMTModel::isValid,
				"@DocString(FMTModel::isValid)")
			.method("getpostsolvefilter", &Models::FMTModel::getPostsolveFilter,
				"@DocString(FMTModel::getpostsolvefilter)")
			.method("isoptimal", &Models::FMTModel::isOptimal,
				"@DocString(FMTModel::isoptimal)")
			.method("getobjectivevalue", &Models::FMTModel::getObjectiveValue,
				"@DocString(FMTModel::getobjectivevalue)")
			.method("GetStaticTransitionThemes", &Models::FMTModel::getStaticTransitionThemes,
				"@DocString(FMTModel::GetStaticTransitionThemes)")
			.method("setboolparameter",
				static_cast<bool(Models::FMTModel::*)(const Models::FMTboolmodelparameters&, const bool&)>(&Models::FMTModel::setParameter),
				"@DocString(Models::FMTModel::setparameter(const FMTboolmodelparameters,const bool))")
			.method("setintparameter",
				static_cast<bool(Models::FMTModel::*)(const Models::FMTintmodelparameters&, const int&)>(&Models::FMTModel::setParameter),
				"@DocString(Models::FMTModel::setparameter(const FMTintmodelparameters,const int))")
			.method("setdblparameter",
				static_cast<bool(Models::FMTModel::*)(const Models::FMTdblmodelparameters&, const double&)>(&Models::FMTModel::setParameter),
				"@DocString(Models::FMTModel::setparameter(const FMTdblmodelparameters,const int))")
			.method("showparameters",&Models::FMTModel::showParameters,
			"@DocString(FMTModel::showparameters)")
			.method("doplanning",&Models::FMTModel::doPlanning,
				"@DocString(FMTModel::doplanning)");

	Rcpp::class_<Models::FMTSeModel>("FMTsemodel", "@DocString(FMTSeModel)")
		.derives<Models::FMTModel>("FMTmodel")
		.constructor("@DocString(FMTSeModel())")
		.constructor<Models::FMTModel>("@DocString(FMTSeModel(Models::FMTModel))")
		.method("getmapping", &Models::FMTSeModel::getMapping,
			"@DocString(FMTSeModel::getmapping)")
		.method("getschedule", &Models::FMTSeModel::getSchedule,
			"@DocString(FMTSeModel::getschedule)")
		.method("getspschedule", &Models::FMTSeModel::getSpSchedule,
			"@DocString(FMTSeModel::getdisturbances)")
		.method("setinitialmapping", &Models::FMTSeModel::setInitialMapping,
			"@DocString(FMTSeModel::setinitialmapping)")
		.method("getspatialoutput", &Models::FMTSeModel::getSpatialOutput,
			"@DocString(FMTSeModel::getschedule)")
		.method("getdisturbancestats", &Models::FMTSeModel::getDisturbanceStats,
			"@DocString(FMTSeModel::getdisturbancestats)");


	Rcpp::class_<Models::FMTSesModel>("FMTsesmodel", "@DocString(FMTSesModel)")
		.derives<Models::FMTSeModel>("FMTsemodel")
		.constructor("@DocString(FMTSesModel())")
		.constructor<Models::FMTModel>("@DocString(FMTSesModel(Models::FMTModel))")
		.method("greedyreferencebuild", &Models::FMTSesModel::greedyReferenceBuild,
			"@DocString(FMTSesModel:::GreedyReferenceBuild)");

	#ifdef FMTWITHOSI
	Rcpp::class_<Models::FMTLpSolver>("FMTlpsolver", "@DocString(FMTLpSolver)")
		.constructor("@DocString(FMTLpSolver())")
		.method("isProvenOptimal", &Models::FMTLpSolver::isProvenOptimal,
			"@DocString(FMTLpSolver::isProvenOptimal)")
		.method("getObjValue", &Models::FMTLpSolver::getObjValue,
			"@DocString(FMTLpSolver::getObjValue)")
		.method("writeLP", &Models::FMTLpSolver::writeLP,
			"@DocString(FMTLpSolver::writeLP)")
		.method("writeMPS", &Models::FMTLpSolver::writeMPS,
			"@DocString(FMTLpSolver::writeMPS)");


	Rcpp::class_<Models::FMTSrModel>("FMTsrmodel", "@DocString(FMTSrModel)")
		.derives<Models::FMTModel>("FMTmodel")
		.constructor("@DocString(FMTSrModel())")
		.method("buildperiod", &Models::FMTSrModel::buildPeriod,
			"@DocString(FMTSrModel::buildperiod)")
		.method("boundsolution", &Models::FMTSrModel::boundSolution,
			"@DocString(FMTSrModel::boundsolution)")
		.method("getsolution", &Models::FMTSrModel::getSolution,
			"@DocString(FMTSrModel::getsolution)")
		.method("setsolution", &Models::FMTSrModel::setSolution,
			"@DocString(FMTSrModel::setsolution)")
		.method("setsolutionbylp", &Models::FMTSrModel::setSolutionByLp,
			"@DocString(FMTSrModel::setsolutionbylp)")
		.method("getoutputsdataframe", &Models::FMTSrModel::getOutputsDataFrame,
			"@DocString(FMTSrModel::getoutputsdataframe)")
		.method("getoutput", &Models::FMTSrModel::getOutput,
			"@DocString(FMTSrModel::getoutput)")
		.method("getstats", &Models::FMTSrModel::getStats,
			"@DocString(FMTSrModel::getstats)");

	Rcpp::class_<Models::FMTNssModel>("FMTnssmodel", "@DocString(FMTNssModel)")
		.derives<Models::FMTSrModel>("FMTsrmodel")
		.constructor("@DocString(FMTNssModel())")
		.constructor<Models::FMTModel, unsigned int>("@DocString(FMTNssModel(Models::FMTModel,unsigned int))")
		.method("simulate", &Models::FMTNssModel::simulate,
			"@DocString(FMTNssModel::simulate)");
		


	Rcpp::class_<Models::FMTLpModel>("FMTlpmodel", "@DocString(FMTLpModel)")
		.derives<Models::FMTSrModel>("FMTsrmodel")
		.constructor<Models::FMTModel, Models::FMTsolverinterface>("@DocString(FMTLpModel(Models::FMTModel,Models::FMTsolverinterface))")
		.constructor("@DocString(FMTLpModel())")
		.constructor<Models::FMTLpModel>("@DocString(FMTLpModel(const FMTLpModel&))")
		.method("setobjective", &Models::FMTLpModel::setObjective,
			"@DocString(FMTLpModel::setobjective)")
		.method("setconstraint", &Models::FMTLpModel::setConstraint,
			"@DocString(FMTLpModel::setconstraint)")
		.method("eraseconstraint", &Models::FMTLpModel::eraseConstraint,
			"@DocString(FMTLpModel::eraseconstraint)")
		.method("eraseperiod", &Models::FMTLpModel::erasePeriod,
			"@DocString(FMTLpModel::eraseperiod)")
		.method("resolve", &Models::FMTLpModel::resolve,
			"@DocString(FMTLpModel::resolve)")
		.method("initialsolve", &Models::FMTLpModel::initialSolve,
			"@DocString(FMTLpModel::initialsolve)")
		.method("eq", &Models::FMTLpModel::operator ==,
			"@DocString(FMTLpModel::operator==)")
		.method("ne", &Models::FMTLpModel::operator !=,
			"@DocString(FMTLpModel::operator!=)")
		.method("getoperatingareaschedulerheuristics", &Models::FMTLpModel::getOperatingAreaSchedulerHeuristics,
			"@DocString(FMTLpModel::getoperatingareaschedulerheuristics)")
		.method("getvariabilities", &Models::FMTLpModel::getVariabilities,
			"@DocString(FMTLpModel::getvariabilities)")
		.method("getareavariabilities", &Models::FMTLpModel::getAreaVariabilities,
			"@DocString(FMTLpModel::getareavariabilities)")
		.method("writeLP", &Models::FMTLpModel::writeLP,
			"@DocString(FMTLpModel::writeLP)")
		.method("setstrictlypositivesoutputsmatrix", &Models::FMTLpModel::setStrictlyPositivesOutputsMatrix,
			"@DocString(FMTLpModel::setstrictlypositivesoutputsmatrix)")
		.method("getObjValue", &Models::FMTLpModel::getObjValue,
					"@DocString(FMTLpModel::getObjValue)")
		.method("writeMPS", &Models::FMTLpModel::writeMPS,
			"@DocString(FMTLpModel::writeMPS)");;
	

	#endif

	
	Rcpp::class_<Models::FMTSaModel>("FMTsamodel", "@DocString(FMTSaModel)")
		.derives<Models::FMTSeModel>("FMTmodel")
		.constructor<Models::FMTModel>("@DocString(FMTSaModel(FMTModel))")
		.constructor("@DocString(FMTSaModel())")
		.method("initialsolve", &Models::FMTSaModel::initialSolve,
			"@DocString(FMTSaModel::initialsolve)");
	
    }
}
#endif // PYEXPORTMODEL_H_INCLUDED
