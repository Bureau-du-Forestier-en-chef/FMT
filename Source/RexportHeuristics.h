/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTHEURISTICS_H_INCLUDED
#define REXPORTHEURISTICS_H_INCLUDED


#include "FMToperatingarea.h"
#include "FMToperatingareascheduler.h"
#include "FMToperatingareascheme.h"
#include "FMTlpsolver.h"
#include "Rdefinitions.h"
#include <Rcpp.h>

#ifdef FMTWITHOSI
	RCPP_EXPOSED_WRAP(Heuristics::FMToperatingarea);
	RCPP_EXPOSED_AS(Heuristics::FMToperatingarea);
	RCPP_DEFINEVECTOR(Heuristics::FMToperatingarea);//For vector
	RCPP_EXPOSED_WRAP(Heuristics::FMToperatingareascheme);
	RCPP_EXPOSED_AS(Heuristics::FMToperatingareascheme);
	RCPP_DEFINEVECTOR(Heuristics::FMToperatingareascheme);//For vector
	RCPP_EXPOSED_WRAP(Heuristics::FMToperatingareascheduler);
	RCPP_EXPOSED_AS(Heuristics::FMToperatingareascheduler);
	RCPP_DEFINEVECTOR(Heuristics::FMToperatingareascheduler);//For vector
#endif

namespace R
{
void exportHeuristics()
{
	
	#ifdef FMTWITHOSI

	Rcpp::class_<Heuristics::FMToperatingarea>("FMToperatingarea", "@DocString(FMToperatingarea)")
		.constructor("@DocString(FMToperatingarea())")
		.constructor<Core::FMTmask,double>("@DocString(FMToperatingarea(const Core::FMTmask&,const double&))")
		.method("getneighbors", &Heuristics::FMToperatingarea::getneighbors,
			"@DocString(FMToperatingarea::getneighbors)")
		.method("setneighbors", &Heuristics::FMToperatingarea::setneighbors,
			"@DocString(FMToperatingarea::setneighbors)")
		.method("getmask", &Heuristics::FMToperatingarea::getmask,
			"@DocString(FMToperatingarea::getmask)")
		.method("getneihgborsperimeter", &Heuristics::FMToperatingarea::getneihgborsperimeter,
			"@DocString(FMToperatingarea::getneihgborsperimeter)");

	Rcpp::class_<Heuristics::FMToperatingareascheme>("FMToperatingareascheme", "@DocString(FMToperatingareascheme)")
		.derives<Heuristics::FMToperatingarea>("FMToperatingarea")
		.constructor("@DocString(FMToperatingareascheme())")
		.constructor<Heuristics::FMToperatingarea,size_t,size_t,size_t,size_t,size_t>("@DocString(FMToperatingareascheme(const Heuristics::FMToperatingarea&,const size_t&,const size_t&,const size_t&, const size_t&, const size_t&))");
		
	Rcpp::class_<Heuristics::FMTlpheuristic>("FMTlpheuristic", "@DocString(FMTlpheuristic)")
		.derives<Core::FMTobject>("FMTobject")
		.constructor("@DocString(FMTlpheuristic())");

	Rcpp::class_<Heuristics::FMToperatingareascheduler>("FMToperatingareascheduler","@DocString(FMToperatingareascheduler)")
		.derives<Heuristics::FMTlpheuristic>("FMTlpheuristic")
		//.derives<Models::FMTlpsolver>("FMTlpsolver")
		.constructor("@DocString(FMToperatingareascheduler())")
		.method("initialsolve", &Heuristics::FMToperatingareascheduler::initialsolve,
			"@DocString(FMToperatingareascheduler::initialsolve)")
		.method("branchnboundsolve", &Heuristics::FMToperatingareascheduler::branchnboundsolve,
			"@DocString(FMToperatingareaheuristic::branchnboundsolve)")
		.method("getsolution", &Heuristics::FMToperatingareascheduler::getsolution,
			"@DocString(FMToperatingareaheuristic::getsolution)");

	#endif 
	}
}
#endif 
