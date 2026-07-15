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

RCPP_EXPOSED_WRAP(Models::FMTmodel);
RCPP_EXPOSED_AS(Models::FMTmodel);
RCPP_DEFINEVECTOR(Models::FMTmodel);//For vector
RCPP_DEFINEMAP(std::string,double);//For map<string,double>
RCPP_EXPOSED_WRAP(Models::FMTsemodel);
RCPP_EXPOSED_AS(Models::FMTsemodel);
RCPP_DEFINEVECTOR(Models::FMTsemodel);//For vector
RCPP_EXPOSED_WRAP(Models::FMTsesmodel);
RCPP_EXPOSED_AS(Models::FMTsesmodel);
RCPP_DEFINEVECTOR(Models::FMTsesmodel);//For vector
#ifdef FMTWITHOSI
	RCPP_EXPOSED_WRAP(Models::FMTsrmodel);
	RCPP_EXPOSED_AS(Models::FMTsrmodel);
	RCPP_DEFINEVECTOR(Models::FMTsrmodel);//For vector
	RCPP_EXPOSED_WRAP(Models::FMTnssmodel);
	RCPP_EXPOSED_AS(Models::FMTnssmodel);
	RCPP_DEFINEVECTOR(Models::FMTnssmodel);//For vector
	RCPP_EXPOSED_ENUM_NODECL(Models::FMTsolverinterface);
	RCPP_EXPOSED_WRAP(Models::FMTlpsolver);
	RCPP_EXPOSED_AS(Models::FMTlpsolver);
	RCPP_DEFINEVECTOR(Models::FMTlpsolver);//For vector
	RCPP_EXPOSED_WRAP(Models::FMTlpmodel);
	RCPP_EXPOSED_AS(Models::FMTlpmodel);
	RCPP_DEFINEVECTOR(Models::FMTlpmodel);//For vector
#endif

RCPP_EXPOSED_WRAP(Models::FMTsamodel);
RCPP_EXPOSED_AS(Models::FMTsamodel);
RCPP_DEFINEVECTOR(Models::FMTsamodel);//For vector

RCPP_EXPOSED_ENUM_NODECL(Models::FMTintmodelparameters);
RCPP_EXPOSED_ENUM_NODECL(Models::FMTdblmodelparameters);
RCPP_EXPOSED_ENUM_NODECL(Models::FMTboolmodelparameters);

namespace R
{ 

void exportModel()
{
	Rcpp::class_<Models::FMTmodel>("FMTmodel", "@DocString(FMTmodel)")
		.derives<Core::FMTobject>("FMTobject")
		.constructor<Models::FMTmodel>("@DocString(FMTmodel(Models::FMTmodel))")
		.constructor("@DocString(FMTmodel())")
           .method("getname",&Models::FMTmodel::getName,
				"@DocString(FMTmodel::getname)")
            .method("getyields",&Models::FMTmodel::getYields,
				"@DocString(FMTmodel::getyields)")
			.method("setyields", &Models::FMTmodel::setYields,
				"@DocString(FMTmodel::setyields)")
            .method("getarea",&Models::FMTmodel::getArea,
				"@DocString(FMTmodel::getarea)")
			.method("setarea", &Models::FMTmodel::setArea,
				"@DocString(FMTmodel::setarea)")
            .method("getthemes",&Models::FMTmodel::getThemes,
				"@DocString(FMTmodel::getthemes)")
			.method("setthemes", &Models::FMTmodel::setThemes,
				"@DocString(FMTmodel::setthemes)")
            .method("getactions",&Models::FMTmodel::getactions,
				"@DocString(FMTmodel::getactions)")
			.method("setactions", &Models::FMTmodel::setActions,
				"@DocString(FMTmodel::setactions)")
			.method("getoutputs", &Models::FMTmodel::getOutputs, 
				"@DocString(FMTmodel::getoutputs)")
			.method("setoutputs", &Models::FMTmodel::setOutputs,
				"@DocString(FMTmodel::setoutputs)")
			.method("getconstraints", &Models::FMTmodel::getconstraints,
				"@DocString(FMTmodel::getconstraints)")
			.method("setconstraints", &Models::FMTmodel::setConstraints,
				"@DocString(FMTmodel::setconstraints)")
            .method("gettransitions",&Models::FMTmodel::getTransitions,
				"@DocString(FMTmodel::gettransitions)")
            .method("settransitions",&Models::FMTmodel::setTransitions,
				"@DocString(FMTmodel::settransitions)")
			.method("setname", &Models::FMTmodel::setName,
				"@DocString(FMTmodel::setname)")
			.method("setareaperiod", &Models::FMTmodel::setAreaPeriod,
				"@DocString(FMTmodel::setareaperiod)")
            .method("getlifespan",&Models::FMTmodel::getLifespan,
				"@DocString(FMTmodel::getlifespan)")
			.method("setlifespan", &Models::FMTmodel::setLifespan,
				"@DocString(FMTmodel::setlifespan)")
            .method("isValid",&Models::FMTmodel::isValid,
				"@DocString(FMTmodel::isValid)")
			.method("getpostsolvefilter", &Models::FMTmodel::getPostsolveFilter,
				"@DocString(FMTmodel::getpostsolvefilter)")
			.method("isoptimal", &Models::FMTmodel::isOptimal,
				"@DocString(FMTmodel::isoptimal)")
			.method("getobjectivevalue", &Models::FMTmodel::getObjectiveValue,
				"@DocString(FMTmodel::getobjectivevalue)")
			.method("GetStaticTransitionThemes", &Models::FMTmodel::getStaticTransitionThemes,
				"@DocString(FMTmodel::GetStaticTransitionThemes)")
			.method("setboolparameter",
				static_cast<bool(Models::FMTmodel::*)(const Models::FMTboolmodelparameters&, const bool&)>(&Models::FMTmodel::setParameter),
				"@DocString(Models::FMTmodel::setparameter(const FMTboolmodelparameters,const bool))")
			.method("setintparameter",
				static_cast<bool(Models::FMTmodel::*)(const Models::FMTintmodelparameters&, const int&)>(&Models::FMTmodel::setParameter),
				"@DocString(Models::FMTmodel::setparameter(const FMTintmodelparameters,const int))")
			.method("setdblparameter",
				static_cast<bool(Models::FMTmodel::*)(const Models::FMTdblmodelparameters&, const double&)>(&Models::FMTmodel::setParameter),
				"@DocString(Models::FMTmodel::setparameter(const FMTdblmodelparameters,const int))")
			.method("showparameters",&Models::FMTmodel::showParameters,
			"@DocString(FMTmodel::showparameters)")
			.method("doplanning",&Models::FMTmodel::doPlanning,
				"@DocString(FMTmodel::doplanning)");

	Rcpp::class_<Models::FMTsemodel>("FMTsemodel", "@DocString(FMTsemodel)")
		.derives<Models::FMTmodel>("FMTmodel")
		.constructor("@DocString(FMTsemodel())")
		.constructor<Models::FMTmodel>("@DocString(FMTsemodel(Models::FMTmodel))")
		.method("getmapping", &Models::FMTsemodel::getMapping,
			"@DocString(FMTsemodel::getmapping)")
		.method("getschedule", &Models::FMTsemodel::getSchedule,
			"@DocString(FMTsemodel::getschedule)")
		.method("getspschedule", &Models::FMTsemodel::getSpSchedule,
			"@DocString(FMTsemodel::getdisturbances)")
		.method("setinitialmapping", &Models::FMTsemodel::setInitialMapping,
			"@DocString(FMTsemodel::setinitialmapping)")
		.method("getspatialoutput", &Models::FMTsemodel::getSpatialOutput,
			"@DocString(FMTsemodel::getschedule)")
		.method("getdisturbancestats", &Models::FMTsemodel::getDisturbanceStats,
			"@DocString(FMTsemodel::getdisturbancestats)");


	Rcpp::class_<Models::FMTsesmodel>("FMTsesmodel", "@DocString(FMTsesmodel)")
		.derives<Models::FMTsemodel>("FMTsemodel")
		.constructor("@DocString(FMTsesmodel())")
		.constructor<Models::FMTmodel>("@DocString(FMTsesmodel(Models::FMTmodel))")
		.method("greedyreferencebuild", &Models::FMTsesmodel::greedyReferenceBuild,
			"@DocString(FMTsesmodel:::GreedyReferenceBuild)");

	#ifdef FMTWITHOSI
	Rcpp::class_<Models::FMTlpsolver>("FMTlpsolver", "@DocString(FMTlpsolver)")
		.constructor("@DocString(FMTlpsolver())")
		.method("isProvenOptimal", &Models::FMTlpsolver::isProvenOptimal,
			"@DocString(FMTlpsolver::isProvenOptimal)")
		.method("getObjValue", &Models::FMTlpsolver::getObjValue,
			"@DocString(FMTlpsolver::getObjValue)")
		.method("writeLP", &Models::FMTlpsolver::writeLP,
			"@DocString(FMTlpsolver::writeLP)")
		.method("writeMPS", &Models::FMTlpsolver::writeMPS,
			"@DocString(FMTlpsolver::writeMPS)");


	Rcpp::class_<Models::FMTsrmodel>("FMTsrmodel", "@DocString(FMTsrmodel)")
		.derives<Models::FMTmodel>("FMTmodel")
		.constructor("@DocString(FMTsrmodel())")
		.method("buildperiod", &Models::FMTsrmodel::buildPeriod,
			"@DocString(FMTsrmodel::buildperiod)")
		.method("boundsolution", &Models::FMTsrmodel::boundSolution,
			"@DocString(FMTsrmodel::boundsolution)")
		.method("getsolution", &Models::FMTsrmodel::getSolution,
			"@DocString(FMTsrmodel::getsolution)")
		.method("setsolution", &Models::FMTsrmodel::setSolution,
			"@DocString(FMTsrmodel::setsolution)")
		.method("setsolutionbylp", &Models::FMTsrmodel::setSolutionByLp,
			"@DocString(FMTsrmodel::setsolutionbylp)")
		.method("getoutputsdataframe", &Models::FMTsrmodel::getOutputsDataFrame,
			"@DocString(FMTsrmodel::getoutputsdataframe)")
		.method("getoutput", &Models::FMTsrmodel::getOutput,
			"@DocString(FMTsrmodel::getoutput)")
		.method("getstats", &Models::FMTsrmodel::getStats,
			"@DocString(FMTsrmodel::getstats)");

	Rcpp::class_<Models::FMTnssmodel>("FMTnssmodel", "@DocString(FMTnssmodel)")
		.derives<Models::FMTsrmodel>("FMTsrmodel")
		.constructor("@DocString(FMTnssmodel())")
		.constructor<Models::FMTmodel, unsigned int>("@DocString(FMTnssmodel(Models::FMTmodel,unsigned int))")
		.method("simulate", &Models::FMTnssmodel::simulate,
			"@DocString(FMTnssmodel::simulate)");
		


	Rcpp::class_<Models::FMTlpmodel>("FMTlpmodel", "@DocString(FMTlpmodel)")
		.derives<Models::FMTsrmodel>("FMTsrmodel")
		.constructor<Models::FMTmodel, Models::FMTsolverinterface>("@DocString(FMTlpmodel(Models::FMTmodel,Models::FMTsolverinterface))")
		.constructor("@DocString(FMTlpmodel())")
		.constructor<Models::FMTlpmodel>("@DocString(FMTlpmodel(const FMTlpmodel&))")
		.method("setobjective", &Models::FMTlpmodel::setObjective,
			"@DocString(FMTlpmodel::setobjective)")
		.method("setconstraint", &Models::FMTlpmodel::setConstraint,
			"@DocString(FMTlpmodel::setconstraint)")
		.method("eraseconstraint", &Models::FMTlpmodel::eraseConstraint,
			"@DocString(FMTlpmodel::eraseconstraint)")
		.method("eraseperiod", &Models::FMTlpmodel::erasePeriod,
			"@DocString(FMTlpmodel::eraseperiod)")
		.method("resolve", &Models::FMTlpmodel::resolve,
			"@DocString(FMTlpmodel::resolve)")
		.method("initialsolve", &Models::FMTlpmodel::initialSolve,
			"@DocString(FMTlpmodel::initialsolve)")
		.method("eq", &Models::FMTlpmodel::operator ==,
			"@DocString(FMTlpmodel::operator==)")
		.method("ne", &Models::FMTlpmodel::operator !=,
			"@DocString(FMTlpmodel::operator!=)")
		.method("getoperatingareaschedulerheuristics", &Models::FMTlpmodel::getOperatingAreaSchedulerHeuristics,
			"@DocString(FMTlpmodel::getoperatingareaschedulerheuristics)")
		.method("getvariabilities", &Models::FMTlpmodel::getVariabilities,
			"@DocString(FMTlpmodel::getvariabilities)")
		.method("getareavariabilities", &Models::FMTlpmodel::getAreaVariabilities,
			"@DocString(FMTlpmodel::getareavariabilities)")
		.method("writeLP", &Models::FMTlpmodel::writeLP,
			"@DocString(FMTlpmodel::writeLP)")
		.method("setstrictlypositivesoutputsmatrix", &Models::FMTlpmodel::setStrictlyPositivesOutputsMatrix,
			"@DocString(FMTlpmodel::setstrictlypositivesoutputsmatrix)")
		.method("getObjValue", &Models::FMTlpmodel::getObjValue,
					"@DocString(FMTlpmodel::getObjValue)")
		.method("writeMPS", &Models::FMTlpmodel::writeMPS,
			"@DocString(FMTlpmodel::writeMPS)");;
	

	#endif

	
	Rcpp::class_<Models::FMTsamodel>("FMTsamodel", "@DocString(FMTsamodel)")
		.derives<Models::FMTsemodel>("FMTmodel")
		.constructor<Models::FMTmodel>("@DocString(FMTsamodel(FMTmodel))")
		.constructor("@DocString(FMTsamodel())")
		.method("initialsolve", &Models::FMTsamodel::initialSolve,
			"@DocString(FMTsamodel::initialsolve)");
	
    }
}
#endif // PYEXPORTMODEL_H_INCLUDED
