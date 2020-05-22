/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTHEURISTICS_H_INCLUDED
#define REXPORTHEURISTICS_H_INCLUDED


#include "FMToperatingarea.h"
#include "FMToperatingareaheuristic.h"
#include "FMTlpsolver.h"
#include "Rdefinitions.h"
#include <Rcpp.h>

#ifdef FMTWITHOSI
	RCPP_EXPOSED_WRAP(Heuristics::FMToperatingarea);
	RCPP_EXPOSED_AS(Heuristics::FMToperatingarea);
	RCPP_DEFINEVECTOR(Heuristics::FMToperatingarea);//For vector
	RCPP_EXPOSED_WRAP(Heuristics::FMToperatingareaheuristic);
	RCPP_EXPOSED_AS(Heuristics::FMToperatingareaheuristic);
	RCPP_DEFINEVECTOR(Heuristics::FMToperatingareaheuristic);//For vector
#endif

namespace R
{
void exportHeuristics()
{
	
	#ifdef FMTWITHOSI

	Rcpp::class_<Heuristics::FMToperatingarea>("FMToperatingarea", "@DocString(FMToperatingarea)")
		.constructor("@DocString(FMToperatingarea())")
		.constructor<Core::FMTmask, size_t, size_t, size_t,size_t,double,double>(
			"@DocString(FMToperatingarea(const Core::FMTmask&,const size_t&,const size_t&,const size_t&,const size_t&,const double&,const double&))")
		.method("getneighbors", &Heuristics::FMToperatingarea::getneighbors,
			"@DocString(FMToperatingarea::getneighbors)")
		.method("setneighbors", &Heuristics::FMToperatingarea::setneighbors,
			"@DocString(FMToperatingarea::setneighbors)")
		.method("getmask", &Heuristics::FMToperatingarea::getmask,
			"@DocString(FMToperatingarea::getmask)")
		.method("getneihgborsperimeter", &Heuristics::FMToperatingarea::getneihgborsperimeter,
			"@DocString(FMToperatingarea::getneihgborsperimeter)");
		


	Rcpp::class_<Heuristics::FMToperatingareaheuristic>("FMToperatingareaheuristic","@DocString(FMToperatingareaheuristic)")
		.derives<Core::FMTobject>("FMTobject")
		//.derives<Models::FMTlpsolver>("FMTlpsolver")
		.constructor("@DocString(FMToperatingareaheuristic())")
		.method("initialsolve", &Heuristics::FMToperatingareaheuristic::initialsolve,
			"@DocString(FMToperatingareaheuristic::initialsolve)")
		.method("branchnboundsolve", &Heuristics::FMToperatingareaheuristic::branchnboundsolve,
			"@DocString(FMToperatingareaheuristic::branchnboundsolve)")
		.method("getsolution", &Heuristics::FMToperatingareaheuristic::getsolution,
			"@DocString(FMToperatingareaheuristic::getsolution)");

	#endif 
	}
}
#endif 
