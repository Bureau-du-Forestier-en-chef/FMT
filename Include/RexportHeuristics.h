/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTHEURISTICS_H_INCLUDED
#define REXPORTHEURISTICS_H_INCLUDED


#include "FMToperatingarea.h"
#include "FMToperatingareaclusterbinary.h"
#include "FMToperatingareacluster.h"
#include "FMToperatingareaclusterer.h"
#include "FMToperatingareascheduler.h"
#include "FMToperatingareascheme.h"
#include "FMTlpsolver.h"
#include "Rdefinitions.h"
#include <Rcpp.h>

#ifdef FMTWITHOSI
	RCPP_EXPOSED_WRAP(Heuristics::FMTOperatingArea);
	RCPP_EXPOSED_AS(Heuristics::FMTOperatingArea);
	RCPP_DEFINEVECTOR(Heuristics::FMTOperatingArea);//For vector
	RCPP_EXPOSED_WRAP(Heuristics::FMTOperatingAreaScheme);
	RCPP_EXPOSED_AS(Heuristics::FMTOperatingAreaScheme);
	RCPP_DEFINEVECTOR(Heuristics::FMTOperatingAreaScheme);//For vector
	RCPP_EXPOSED_WRAP(Heuristics::FMTOperatingAreaScheduler);
	RCPP_EXPOSED_AS(Heuristics::FMTOperatingAreaScheduler);
	RCPP_DEFINEVECTOR(Heuristics::FMTOperatingAreaScheduler);//For vector
	RCPP_EXPOSED_WRAP(Heuristics::FMTOperatingAreaClusterBinary);
	RCPP_EXPOSED_AS(Heuristics::FMTOperatingAreaClusterBinary);
	RCPP_DEFINEVECTOR(Heuristics::FMTOperatingAreaClusterBinary);//For vector
	RCPP_EXPOSED_WRAP(Heuristics::FMTOperatingAreaCluster);
	RCPP_EXPOSED_AS(Heuristics::FMTOperatingAreaCluster);
	RCPP_DEFINEVECTOR(Heuristics::FMTOperatingAreaCluster);//For vector
	RCPP_EXPOSED_WRAP(Heuristics::FMTOperatingAreaClusterer);
	RCPP_EXPOSED_AS(Heuristics::FMTOperatingAreaClusterer);
	RCPP_DEFINEVECTOR(Heuristics::FMTOperatingAreaClusterer);//For vector
#endif

namespace R
{
void exportHeuristics()
{

	#ifdef FMTWITHOSI

	Rcpp::class_<Heuristics::FMTOperatingArea>("FMToperatingarea", "@DocString(FMTOperatingArea)")
		.constructor("@DocString(FMTOperatingArea())")
		.constructor<Core::FMTMask,double>("@DocString(FMTOperatingArea(const Core::FMTMask&,const double&))")
		.method("getneighbors", &Heuristics::FMTOperatingArea::getNeighbors,
			"@DocString(FMTOperatingArea::getneighbors)")
		.method("setneighbors", &Heuristics::FMTOperatingArea::setNeighbors,
			"@DocString(FMTOperatingArea::setneighbors)")
		.method("getmask", &Heuristics::FMTOperatingArea::getMask,
			"@DocString(FMTOperatingArea::getmask)")
		.method("getneihgborsperimeter", &Heuristics::FMTOperatingArea::getNeighborsPerimeter,
			"@DocString(FMTOperatingArea::getneihgborsperimeter)");

	Rcpp::class_<Heuristics::FMTOperatingAreaCluster>("FMToperatingareacluster", "@DocString(FMTOperatingAreaCluster)")
        .constructor("@DocString(FMTOperatingAreaCluster())")
        .constructor<Heuristics::FMTOperatingAreaCluster,double,double>("@DocString(FMTOperatingAreaCluster(const FMTOperatingAreaCluster&,const double&,const double&))")
        .method("getfilteredcluster",&Heuristics::FMTOperatingAreaCluster::getFilteredCluster,
             "@DocString(FMTOperatingAreaCluster::getfilteredcluster)")
        .method("getallmasks",&Heuristics::FMTOperatingAreaCluster::getAllMasks,
             "@DocString(FMTOperatingAreaCluster::getallmasks)")
        .method("setminimalarea",&Heuristics::FMTOperatingAreaCluster::setMinimalArea,
             "@DocString(FMTOperatingAreaCluster::setminimalarea)")
        .method("setmaximalarea",&Heuristics::FMTOperatingAreaCluster::setMaximalArea,
             "@DocString(FMTOperatingAreaCluster::setmaximalarea)");

	Rcpp::class_<Heuristics::FMTOperatingAreaScheme>("FMToperatingareascheme", "@DocString(FMTOperatingAreaScheme)")
		.derives<Heuristics::FMTOperatingArea>("FMToperatingarea")
		.constructor("@DocString(FMTOperatingAreaScheme())")
		.constructor<Heuristics::FMTOperatingArea,size_t,size_t,size_t,size_t,size_t,size_t>("@DocString(FMTOperatingAreaScheme(const Heuristics::FMTOperatingArea&,const size_t&,const size_t&,const size_t&, const size_t&, const size_t&,double,bool))");

	Rcpp::class_<Heuristics::FMTLpHeuristic>("FMTlpheuristic", "@DocString(FMTLpHeuristic)")
		.derives<Core::FMTObject>("FMTobject")
		.constructor("@DocString(FMTLpHeuristic())");

	Rcpp::class_<Heuristics::FMTOperatingAreaScheduler>("FMToperatingareascheduler","@DocString(FMTOperatingAreaScheduler)")
		.derives<Heuristics::FMTLpHeuristic>("FMTlpheuristic")
		//.derives<Models::FMTLpSolver>("FMTlpsolver")
		.constructor("@DocString(FMTOperatingAreaScheduler())")
		.method("initialsolve", &Heuristics::FMTOperatingAreaScheduler::initialSolve,
			"@DocString(FMTOperatingAreaScheduler::initialsolve)")
		.method("branchnboundsolve", &Heuristics::FMTOperatingAreaScheduler::branchNBoundSolve,
			"@DocString(FMTOperatingAreaScheduler::branchnboundsolve)")
		.method("getsolution", &Heuristics::FMTOperatingAreaScheduler::getSolution,
			"@DocString(FMTOperatingAreaScheduler::getsolution)");

    Rcpp::class_<Heuristics::FMTOperatingAreaClusterer>("FMToperatingareaclusterer", "@DocString(FMTOperatingAreaClusterer)")
        .derives<Heuristics::FMTLpHeuristic>("FMTlpheuristic")
        .constructor("@DocString(FMTOperatingAreaClusterer())")
		.method("initialsolve", &Heuristics::FMTOperatingAreaClusterer::initialSolve,
			"@DocString(FMTOperatingAreaClusterer::initialsolve)")
		.method("branchnboundsolve", &Heuristics::FMTOperatingAreaClusterer::branchNBoundSolve,
			"@DocString(FMTOperatingAreaClusterer::branchnboundsolve)")
		.method("getsolution", &Heuristics::FMTOperatingAreaClusterer::getSolution,
			"@DocString(FMTOperatingAreaClusterer::getsolution)");

	#endif
	}
}
#endif
