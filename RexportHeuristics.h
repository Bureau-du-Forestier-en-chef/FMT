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

#ifndef REXPORTHEURISTICS_H_INCLUDED
#define REXPORTHEURISTICS_H_INCLUDED


#include "FMToperatingarea.h"
#include "FMToperatingareaheuristic.h"
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
		.constructor("@DocString(FMToperatingareaheuristic())")
		.method("initialsolve", &Heuristics::FMToperatingareaheuristic::initialsolve,
			"@DocString(FMToperatingareaheuristic::initialsolve)")
		.method("branchnboundsolve", &Heuristics::FMToperatingareaheuristic::branchnboundsolve,
			"@DocString(FMToperatingareaheuristic::branchnboundsolve)")
		.method("isfeasible", &Heuristics::FMToperatingareaheuristic::isfeasible,
			"@DocString(FMToperatingareaheuristic::isfeasible)")
		.method("getobjective", &Heuristics::FMToperatingareaheuristic::getobjective,
			"@DocString(FMToperatingareaheuristic::getobjective)")
		.method("getsolution", &Heuristics::FMToperatingareaheuristic::getsolution,
			"@DocString(FMToperatingareaheuristic::getsolution)");

	#endif 
	}
}
#endif 
