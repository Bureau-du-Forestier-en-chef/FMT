/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTMODEL_H_INCLUDED
#define REXPORTMODEL_H_INCLUDED

#include "FMTmodel.hpp"
#include "FMTsrmodel.hpp"
#include "FMTlpmodel.hpp"
#include "FMTsesmodel.hpp"
#include "FMTnssmodel.hpp"
#include "FMTsamodel.hpp"
#include "FMTlpsolver.hpp"
#include "Rdefinitions.hpp"
#include "FMTsolverinterface.hpp"
#include <Rcpp.h>
#include <string>

RCPP_EXPOSED_WRAP(Models::FMTmodel);
RCPP_EXPOSED_AS(Models::FMTmodel);
RCPP_DEFINEVECTOR(Models::FMTmodel);//For vector
RCPP_DEFINEMAP(std::string,double);//For map<string,double>
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

RCPP_EXPOSED_ENUM_NODECL(Models::FMTsawarmuptype);
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
           .method("getname",&Models::FMTmodel::getname,
				"@DocString(FMTmodel::getname)")
            .method("getyields",&Models::FMTmodel::getyields,
				"@DocString(FMTmodel::getyields)")
			.method("setyields", &Models::FMTmodel::setyields,
				"@DocString(FMTmodel::setyields)")
            .method("getarea",&Models::FMTmodel::getarea,
				"@DocString(FMTmodel::getarea)")
			.method("setarea", &Models::FMTmodel::setarea,
				"@DocString(FMTmodel::setarea)")
            .method("getthemes",&Models::FMTmodel::getthemes,
				"@DocString(FMTmodel::getthemes)")
			.method("setthemes", &Models::FMTmodel::setthemes,
				"@DocString(FMTmodel::setthemes)")
            .method("getactions",&Models::FMTmodel::getactions,
				"@DocString(FMTmodel::getactions)")
			.method("setactions", &Models::FMTmodel::setactions,
				"@DocString(FMTmodel::setactions)")
			.method("getoutputs", &Models::FMTmodel::getoutputs, 
				"@DocString(FMTmodel::getoutputs)")
			.method("setoutputs", &Models::FMTmodel::setoutputs,
				"@DocString(FMTmodel::setoutputs)")
			.method("getconstraints", &Models::FMTmodel::getconstraints,
				"@DocString(FMTmodel::getconstraints)")
			.method("setconstraints", &Models::FMTmodel::setconstraints,
				"@DocString(FMTmodel::setconstraints)")
            .method("gettransitions",&Models::FMTmodel::gettransitions,
				"@DocString(FMTmodel::gettransitions)")
            .method("settransitions",&Models::FMTmodel::settransitions,
				"@DocString(FMTmodel::settransitions)")
			.method("setname", &Models::FMTmodel::setname,
				"@DocString(FMTmodel::setname)")
			.method("setareaperiod", &Models::FMTmodel::setareaperiod,
				"@DocString(FMTmodel::setareaperiod)")
            .method("getlifespan",&Models::FMTmodel::getlifespan,
				"@DocString(FMTmodel::getlifespan)")
            .method("isvalid",&Models::FMTmodel::isvalid,
				"@DocString(FMTmodel::isvalid)")
			.method("isvalid", &Models::FMTmodel::getpostsolvefilter,
				"@DocString(FMTmodel::getpostsolvefilter)")
			.method("setboolparameter",
				static_cast<bool(Models::FMTmodel::*)(const Models::FMTboolmodelparameters&, const bool&)>(&Models::FMTmodel::setparameter),
				"@DocString(Models::FMTmodel::setparameter(const FMTboolmodelparameters,const bool))")
			.method("setintparameter",
				static_cast<bool(Models::FMTmodel::*)(const Models::FMTintmodelparameters&, const int&)>(&Models::FMTmodel::setparameter),
				"@DocString(Models::FMTmodel::setparameter(const FMTintmodelparameters,const int))")
			.method("setdblparameter",
				static_cast<bool(Models::FMTmodel::*)(const Models::FMTdblmodelparameters&, const double&)>(&Models::FMTmodel::setparameter),
				"@DocString(Models::FMTmodel::setparameter(const FMTdblmodelparameters,const int))")
			.method("showparameters",&Models::FMTmodel::showparameters,
			"@DocString(FMTmodel::showparameters)")
			.method("doplanning",&Models::FMTmodel::doplanning,
				"@DocString(FMTmodel::doplanning)");


	Rcpp::class_<Models::FMTsesmodel>("FMTsesmodel", "@DocString(FMTsesmodel)")
		.derives<Models::FMTmodel>("FMTmodel")
		.constructor("@DocString(FMTsesmodel())")
		.constructor<Models::FMTmodel>("@DocString(FMTsesmodel(Models::FMTmodel))")
			.method("getmapping",&Models::FMTsesmodel::getmapping,
				"@DocString(FMTsesmodel::getmapping)")
			.method("getschedule", &Models::FMTsesmodel::getschedule,
				"@DocString(FMTsesmodel::getschedule)")
            .method("getspschedule",&Models::FMTsesmodel::getspschedule,
				"@DocString(FMTsesmodel::getdisturbances)")
            .method("setinitialmapping",&Models::FMTsesmodel::setinitialmapping,
				"@DocString(FMTsesmodel::setinitialmapping)")
			.method("getschedule",&Models::FMTsesmodel::getschedule,
				"@DocString(FMTsesmodel::getschedule)")
			.method("getdisturbancestats", &Models::FMTsesmodel::getdisturbancestats,
				"@DocString(FMTsesmodel::getdisturbancestats)")
			.method("greedyreferencebuild", &Models::FMTsesmodel::greedyreferencebuild,
				"@DocString(FMTsesmodel::montecarlosimulate)");

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
		.method("buildperiod", &Models::FMTsrmodel::buildperiod,
			"@DocString(FMTsrmodel::buildperiod)")
		.method("boundsolution", &Models::FMTsrmodel::boundsolution,
			"@DocString(FMTsrmodel::boundsolution)")
		.method("getsolution", &Models::FMTsrmodel::getsolution,
			"@DocString(FMTsrmodel::getsolution)")
		.method("setsolution", &Models::FMTsrmodel::setsolution,
			"@DocString(FMTsrmodel::setsolution)")
		.method("setsolutionbylp", &Models::FMTsrmodel::setsolutionbylp,
			"@DocString(FMTsrmodel::setsolutionbylp)")
		.method("getoutputsdataframe", &Models::FMTsrmodel::getoutputsdataframe,
			"@DocString(FMTsrmodel::getoutputsdataframe)")
		.method("getoutput", &Models::FMTsrmodel::getoutput,
			"@DocString(FMTsrmodel::getoutput)")
		.method("getstats", &Models::FMTsrmodel::getstats,
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
		.method("setobjective", &Models::FMTlpmodel::setobjective,
			"@DocString(FMTlpmodel::setobjective)")
		.method("setconstraint", &Models::FMTlpmodel::setconstraint,
			"@DocString(FMTlpmodel::setconstraint)")
		.method("eraseconstraint", &Models::FMTlpmodel::eraseconstraint,
			"@DocString(FMTlpmodel::eraseconstraint)")
		.method("eraseperiod", &Models::FMTlpmodel::eraseperiod,
			"@DocString(FMTlpmodel::eraseperiod)")
		.method("resolve", &Models::FMTlpmodel::resolve,
			"@DocString(FMTlpmodel::resolve)")
		.method("initialsolve", &Models::FMTlpmodel::initialsolve,
			"@DocString(FMTlpmodel::initialsolve)")
		.method("eq", &Models::FMTlpmodel::operator ==,
			"@DocString(FMTlpmodel::operator==)")
		.method("ne", &Models::FMTlpmodel::operator !=,
			"@DocString(FMTlpmodel::operator!=)")
		.method("getoperatingareaschedulerheuristics", &Models::FMTlpmodel::getoperatingareaschedulerheuristics,
			"@DocString(FMTlpmodel::getoperatingareaschedulerheuristics)")
		.method("getvariabilities", &Models::FMTlpmodel::getvariabilities,
			"@DocString(FMTlpmodel::getvariabilities)")
		.method("getareavariabilities", &Models::FMTlpmodel::getareavariabilities,
			"@DocString(FMTlpmodel::getareavariabilities)")
		.method("writeLP", &Models::FMTlpmodel::writeLP,
			"@DocString(FMTlpmodel::writeLP)")
		.method("setstrictlypositivesoutputsmatrix", &Models::FMTlpmodel::setstrictlypositivesoutputsmatrix,
			"@DocString(FMTlpmodel::setstrictlypositivesoutputsmatrix)")
		.method("getObjValue", &Models::FMTlpmodel::getObjValue,
					"@DocString(FMTlpmodel::getObjValue)")
		.method("writeMPS", &Models::FMTlpmodel::writeMPS,
			"@DocString(FMTlpmodel::writeMPS)");;
	

	#endif

	
	Rcpp::class_<Models::FMTsamodel>("FMTsamodel", "@DocString(FMTsamodel)")
		.derives<Models::FMTmodel>("FMTmodel")
		.constructor<Models::FMTmodel>("@DocString(FMTsamodel(FMTmodel))")
		.constructor("@DocString(FMTsamodel())")
		.method("get_current_solution", &Models::FMTsamodel::get_current_solution,
			"@DocString(FMTsamodel::get_current_solution)")
		.method("get_new_solution", &Models::FMTsamodel::get_new_solution,
			"@DocString(FMTsamodel::get_new_solution)")
		.method("getspatialactions", &Models::FMTsamodel::getspatialactions,
			"@DocString(FMTsamodel::getspatialactions)")
		//.method("evaluate",&Models::FMTsamodel::evaluate,
		//	"@DocString(FMTsamodel::evaluate)")
		.method("initialsolve", &Models::FMTsamodel::initialsolve,
			"@DocString(FMTsamodel::initialsolve)")
		.method("setinitialmapping", &Models::FMTsamodel::setinitialmapping,
			"@DocString(FMTsamodel::setinitial_mapping)")
		.method("setspactions", &Models::FMTsamodel::setspactions,
			"@DocString(FMTsamodel::setspactions)")
		.method("set_min_max_moves", &Models::FMTsamodel::set_min_max_moves,
			"@DocString(FMTsamodel::set_min_max_moves)")
		.method("cool_down", &Models::FMTsamodel::cool_down,
			"@DocString(FMTsamodel::cool_down)")
		.method("get_cool_schedule_type", &Models::FMTsamodel::getcoolingscheduletype,
			"@DocString(FMTsamodel::get_cool_schedule_type)")
		.method("setschedule", &Models::FMTsamodel::setschedule,
			"@DocString(FMTsamodel::setschedule)")
		.method("buildperiod", &Models::FMTsamodel::buildperiod,
			"@DocString(FMTsamodel::buildperiod)")
		.method("move", &Models::FMTsamodel::move_solution,
			"@DocString(FMTsamodel::move_solution)")
		.method("acceptnew", &Models::FMTsamodel::acceptnew,
			"@DocString(FMTsamodel::acceptnew)")
		.method("write_outputs_at", &Models::FMTsamodel::write_outputs_at,
			"@DocString(FMTsamodel::write_outputs_at)")
		.method("get_outputs", &Models::FMTsamodel::get_outputs,
			"@DocString(FMTsamodel::get_outputs)")
		.method("write_solutions_events", &Models::FMTsamodel::write_solutions_events,
			"@DocString(FMTsamodel::write_solutions_events)")
		.method("get_number_moves", &Models::FMTsamodel::get_number_moves,
			"@DocString(FMTsamodel::get_number_moves)");
	
    }
}
#endif // PYEXPORTMODEL_H_INCLUDED
