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

#ifndef REXPORTMODEL_H_INCLUDED
#define REXPORTMODEL_H_INCLUDED

#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTsesmodel.h"
#include "FMTsamodel.h"
#include "Rdefinitions.h"
#include <Rcpp>
#include <string>

RCPP_EXPOSED_WRAP(Models::FMTmodel);
RCPP_EXPOSED_AS(Models::FMTmodel);
RCPP_DEFINEVECTOR(Models::FMTmodel);//For vector
RCPP_DEFINEMAP(std::string,double);//For map<string,double>
RCPP_EXPOSED_WRAP(Models::FMTsesmodel);
RCPP_EXPOSED_AS(Models::FMTsesmodel);
RCPP_DEFINEVECTOR(Models::FMTsesmodel);//For vector
RCPP_EXPOSED_ENUM_NODECL(Models::FMTsolverinterface);

#ifdef FMTWITHOSI
	RCPP_EXPOSED_WRAP(Models::FMTlpmodel);
	RCPP_EXPOSED_AS(Models::FMTlpmodel);
	RCPP_DEFINEVECTOR(Models::FMTlpmodel);//For vector
#endif

RCPP_EXPOSED_ENUM_NODECL(Models::FMTsawarmuptype);
RCPP_EXPOSED_WRAP(Models::FMTsamodel);
RCPP_EXPOSED_AS(Models::FMTsamodel);
RCPP_DEFINEVECTOR(Models::FMTsamodel);//For vector

namespace R
{ 

void exportModel()
{

	Rcpp::class_<Models::FMTmodel>("FMTmodel", "@DocString(FMTmodel)")
			.derives<Core::FMTobject>("FMTobject")
			.constructor("@DocString(FMTmodel())")
            .field("name",&Models::FMTmodel::name,
				"@DocString(FMTmodel::name)")
            .method("getyields",&Models::FMTmodel::getyields,
				"@DocString(FMTmodel::getyields)")
            .method("getarea",&Models::FMTmodel::getarea,
				"@DocString(FMTmodel::getarea)")
            .method("getthemes",&Models::FMTmodel::getthemes,
				"@DocString(FMTmodel::getthemes)")
            .method("getactions",&Models::FMTmodel::getactions,
				"@DocString(FMTmodel::getactions)")
			.method("getoutputs", &Models::FMTmodel::getoutputs, 
				"@DocString(FMTmodel::getoutputs)")
			.method("getconstraints", &Models::FMTmodel::getconstraints,
				"@DocString(FMTmodel::getconstraints)")
			.method("setconstraints", &Models::FMTmodel::setconstraints,
				"@DocString(FMTmodel::setconstraints)")
            .method("gettransitions",&Models::FMTmodel::gettransitions,
				"@DocString(FMTmodel::gettransitions)")
            .method("settransitions",&Models::FMTmodel::settransitions,
				"@DocString(FMTmodel::settransitions)")
            .method("getlifespan",&Models::FMTmodel::getlifespan,
				"@DocString(FMTmodel::getlifespan)")
            .method("isvalid",&Models::FMTmodel::isvalid,
				"@DocString(FMTmodel::isvalid)");



	Rcpp::class_<Models::FMTsesmodel>("FMTsesmodel", "@DocString(FMTsesmodel)")
			.derives<Models::FMTmodel>("FMTmodel")
			.constructor("@DocString(FMTsesmodel())")
			.constructor<Models::FMTmodel>("@DocString(FMTsesmodel(Models::FMTmodel))")
            .method("getmapping",&Models::FMTsesmodel::getmapping,
				"@DocString(FMTsesmodel::getmapping)")
			.method("getschedule", &Models::FMTsesmodel::getschedule,
				"@DocString(FMTsesmodel::getschedule)")
            .method("getdisturbances",&Models::FMTsesmodel::getdisturbances,
				"@DocString(FMTsesmodel::getdisturbances)")
            .method("setinitialmapping",&Models::FMTsesmodel::setinitialmapping,
				"@DocString(FMTsesmodel::setinitialmapping)")
            .method("setspactions",&Models::FMTsesmodel::setspactions,
				"@DocString(FMTsesmodel::setspactions)")
			.method("getschedule",&Models::FMTsesmodel::getschedule,
				"@DocString(FMTsesmodel::getschedule)")
			.method("getdisturbancestats", &Models::FMTsesmodel::getdisturbancestats,
				"@DocString(FMTsesmodel::getdisturbancestats)")
            .method("simulate",&Models::FMTsesmodel::simulate,
				"@DocString(FMTsesmodel::simulate)");


	#ifdef FMTWITHOSI

	Rcpp::class_<Models::FMTlpmodel>("FMTlpmodel", "@DocString(FMTlpmodel)")
		.derives<Models::FMTmodel>("FMTmodel")
		.constructor<Models::FMTmodel, Models::FMTsolverinterface>("@DocString(FMTlpmodel(Models::FMTmodel,Models::FMTsolverinterface))")
		.constructor("@DocString(FMTlpmodel())")
		.method("buildperiod", &Models::FMTlpmodel::buildperiod,
			"@DocString(FMTlpmodel::buildperiod)")
		.method("boundsolution", &Models::FMTlpmodel::boundsolution,
			"@DocString(FMTlpmodel::boundsolution)")
		.method("getsolution", &Models::FMTlpmodel::getsolution,
			"@DocString(FMTlpmodel::getsolution)")
		.method("setsolution", &Models::FMTlpmodel::setsolution,
			"@DocString(FMTlpmodel::setsolution)")
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
		.method("getoutput", &Models::FMTlpmodel::getoutput,
			"@DocString(FMTlpmodel::getoutput)")
		.method("writeLP", &Models::FMTlpmodel::writeLP,
			"@DocString(FMTlpmodel::writeLP)")
		.method("writeMPS", &Models::FMTlpmodel::writeMPS,
			"@DocString(FMTlpmodel::writeMPS)")
		.method("eq", &Models::FMTlpmodel::operator ==,
			"@DocString(FMTlpmodel::operator==)")
		.method("ne", &Models::FMTlpmodel::operator !=,
			"@DocString(FMTlpmodel::operator!=)")
		.method("getstats", &Models::FMTlpmodel::getstats,
			"@DocString(FMTlpmodel::getstats)")
		.method("getoperatingareaheuristics", &Models::FMTlpmodel::getoperatingareaheuristics,
			"@DocString(FMTlpmodel::getoperatingareaheuristics)");

	#endif


	Rcpp::class_<Models::FMTsamodel>("FMTsamodel", "@DocString(FMTsamodel)")
			.derives<Models::FMTmodel>("FMTmodel")
			.constructor<Models::FMTmodel>("@DocString(FMTsamodel(FMTmodel))")
			.constructor("@DocString(FMTsamodel())")
            .method("get_current_solution",&Models::FMTsamodel::get_current_solution,
				"@DocString(FMTsamodel::get_current_solution)")
            .method("get_new_solution",&Models::FMTsamodel::get_new_solution,
				"@DocString(FMTsamodel::get_new_solution)")
            .method("getspatialactions",&Models::FMTsamodel::getspatialactions,
				"@DocString(FMTsamodel::getspatialactions)")
            .method("evaluate",&Models::FMTsamodel::evaluate,
				"@DocString(FMTsamodel::evaluate)")
            .method("setinitial_mapping",&Models::FMTsamodel::setinitial_mapping,
				"@DocString(FMTsamodel::setinitial_mapping)")
            .method("setspactions",&Models::FMTsamodel::setspactions,
				"@DocString(FMTsamodel::setspactions)")
            .method("set_min_max_moves",&Models::FMTsamodel::set_min_max_moves,
				"@DocString(FMTsamodel::set_min_max_moves)")
            .method("cool_down",&Models::FMTsamodel::cool_down,
				"@DocString(FMTsamodel::cool_down)")
            .method("get_cool_schedule_type",&Models::FMTsamodel::getcoolingscheduletype,
				"@DocString(FMTsamodel::get_cool_schedule_type)")
            .method("setschedule",&Models::FMTsamodel::setschedule,
				"@DocString(FMTsamodel::setschedule)")
            .method("buildperiod",&Models::FMTsamodel::buildperiod,
				"@DocString(FMTsamodel::buildperiod)")
            .method("move",&Models::FMTsamodel::move_solution,
				"@DocString(FMTsamodel::move_solution)")
            .method("acceptnew",&Models::FMTsamodel::acceptnew,
				"@DocString(FMTsamodel::acceptnew)")
            .method("write_outputs_at",&Models::FMTsamodel::write_outputs_at,
				"@DocString(FMTsamodel::write_outputs_at)")
            .method("get_outputs",&Models::FMTsamodel::get_outputs,
				"@DocString(FMTsamodel::get_outputs)")
            .method("write_solutions_events",&Models::FMTsamodel::write_solutions_events,
				"@DocString(FMTsamodel::write_solutions_events)")
            .method("get_number_moves",&Models::FMTsamodel::get_number_moves,
				"@DocString(FMTsamodel::get_number_moves)")
            .method("warmup",
				"@DocString(FMTsamodel::warmup)");
    }
}
#endif // PYEXPORTMODEL_H_INCLUDED
