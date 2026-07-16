/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef RDECLARATIONS_H_INCLUDED
#define RDECLARATIONS_H_INCLUDED

/*This header file needs to be included before Rcpp.h*/

#include <RcppCommon.h>
#include <vector>
#include <map>

#include "FMTdevelopment.h"
#include "FMTdevelopmentpath.h"
#include "FMTforest.h"
//#include "FMTspatialaction.h"
//#include "FMTgraph.hpp"
#include "FMTlinegraph.h"
//#include "FMTsasolution.h"
#include "FMTmodel.h"
#include "FMTsemodel.h"
#include "FMTsrmodel.h"
#include "FMTnssmodel.h"
#if defined FMTWITHOSI
	#include "FMTlpsolver.h"
	#include "FMTlpmodel.h"
#endif
#include "FMTsesmodel.h"
#include "FMTsaschedule.h"
//#include "FMTexponentialschedule.h" // Comment Gab 2026-02-09
#include "FMTsamodel.h"
#include "FMToperatingarea.h"
#include "FMToperatingareascheduler.h"
#include "FMToperatingareaclusterbinary.h"

#include "FMTmask.h"
#include "FMTactualdevelopment.h"
#include "FMTfuturdevelopment.h"
#include "FMTaction.h"
#include "FMTbounds.hpp"
#include "FMTtimeyieldhandler.h"
#include "FMTageyieldhandler.h"
#include "FMTfork.h"
#include "FMTtransition.h"
#include "FMToutputnode.h"
#include "FMToutput.h"
#include "FMTtheme.h"
#include "FMTschedule.h"
#include "FMTGCBMtransition.h"
#include "FMTconstraint.h"
#include "FMTcoordinate.h"
#include "FMTmodelparameters.h"
#include "FMTmaskfilter.h"
#include "FMToutputsource.h"
#include "FMToperator.h"
#include "FMTtransitionmask.h"

//Parallel
#include "FMTtask.h"
#include "FMTtaskhandler.h"
#include "FMTreplanningtask.h"
#include "FMTplanningtask.h"
#if defined FMTWITHOSI
	#include "FMTopareaschedulertask.h"
#endif

#define RCPP_DECLAREVECTOR(CLASS)namespace Rcpp{\
template <> std::vector<CLASS> as(SEXP obj);\
template <> SEXP wrap(const std::vector<CLASS>& obj);\
}

#define RCPP_DECLARELIST(CLASS)namespace Rcpp{\
template <> std::vector<CLASS> as(SEXP obj);\
template <> SEXP wrap(const std::vector<CLASS>& obj);\
}

#define RCPP_DECLAREPAIR(FIRST,SECOND)namespace Rcpp{\
template <> std::pair<FIRST,SECOND> as(SEXP obj);\
template <> SEXP wrap(const std::pair<FIRST,SECOND>& obj);\
}

#define RCPP_DECLAREMAP(KEY,OBJECT)namespace Rcpp{\
template <> std::map<KEY,OBJECT> as(SEXP obj);\
template <> SEXP wrap(const std::map<KEY,OBJECT>& obj);\
}

#define RCPP_COMMA ,

//Exception
RCPP_DECLAREVECTOR(Exception::FMTexc);//For vector

//Version

//Parser
RCPP_DECLAREMAP(std::string, std::string); //map of string definition

//Spatial
RCPP_DECLAREMAP(Spatial::FMTcoordinate,Core::FMTDevelopment);//for FMTlayer
RCPP_DECLAREVECTOR(Spatial::FMTforest);// For vector
//RCPP_DECLAREVECTOR(Spatial::FMTspatialaction);//For vector
//RCPP_DECLAREMAP(Spatial::FMTcoordinate,Graph::FMTgraph);//For FMTlayer
RCPP_DECLAREMAP(Spatial::FMTcoordinate, Graph::FMTlinegraph);
RCPP_DECLAREVECTOR(Models::FMTsesmodel);//For vector
RCPP_DECLAREVECTOR(Models::FMTsemodel);//For vector
//RCPP_DECLAREVECTOR(Spatial::FMTsasolution);//For vector
//Models
RCPP_DECLAREVECTOR(Models::FMTmodel);//For vector
RCPP_DECLAREMAP(std::string, double);//For map<string,double>
RCPP_DECLAREVECTOR(Models::FMTintmodelparameters);//For vector
RCPP_DECLAREVECTOR(Models::FMTdblmodelparameters);//For vector
RCPP_DECLAREVECTOR(Models::FMTboolmodelparameters);


//Parallel
RCPP_DECLAREVECTOR(Parallel::FMTtask);//For vector
RCPP_DECLAREVECTOR(Parallel::FMTtaskhandler);//For vector
RCPP_DECLAREVECTOR(Parallel::FMTreplanningtask);//For vector
RCPP_DECLAREVECTOR(Parallel::FMTplanningtask);//For vector
#ifdef FMTWITHOSI
	RCPP_DECLAREVECTOR(Parallel::FMTopareaschedulertask);//For vector
	RCPP_DECLAREVECTOR(Models::FMTlpsolver);//For vector
	RCPP_DECLAREVECTOR(Models::FMTsrmodel);//For vector
	RCPP_DECLAREVECTOR(Models::FMTlpmodel);//For vector
	RCPP_DECLAREVECTOR(Models::FMTnssmodel);//For vector
#endif
RCPP_DECLAREVECTOR(Models::FMTsamodel);//For vector
//Heuristics
#ifdef FMTWITHOSI
	RCPP_DECLAREVECTOR(Heuristics::FMToperatingarea);//For vector
	RCPP_DECLAREVECTOR(Heuristics::FMToperatingareascheme);//For vector
	RCPP_DECLAREVECTOR(Heuristics::FMToperatingareascheduler);//For vector
	RCPP_DECLAREVECTOR(Heuristics::FMToperatingareaclusterbinary);//For vector
    RCPP_DECLAREVECTOR(Heuristics::FMToperatingareacluster);//For vector
    RCPP_DECLAREVECTOR(Heuristics::FMToperatingareaclusterer);//For vector
#endif
//Core
RCPP_DECLAREVECTOR(Core::FMTMask);//For vector
RCPP_DECLAREVECTOR(Core::FMTDevelopment);//For vector
RCPP_DECLAREVECTOR(Core::FMTDevelopmentPath);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTSpec);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTSpec);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTPerBounds);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTPerBounds);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTAgeBounds);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTAgeBounds);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTYldBounds);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTYldBounds);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTLockBounds);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTLockBounds);//For vector
RCPP_DECLAREVECTOR(Core::FMTActualDevelopment);//For vector
RCPP_DECLAREVECTOR(Core::FMTFuturDevelopment);//For vector
RCPP_DECLAREVECTOR(Core::FMTAction);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, int);//pair for FMTList
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTYieldHandler);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTYieldHandler);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTTimeYieldHandler);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTTimeYieldHandler);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTAgeYieldHandler);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTAgeYieldHandler);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTFork);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTMaskFilter);//For vector
RCPP_DECLAREVECTOR(Core::FMTFork);//For vector
RCPP_DECLAREVECTOR(Core::FMTTransition);//For vector
RCPP_DECLAREVECTOR(Core::FMTOutputNode);//For vector
RCPP_DECLAREVECTOR(Core::FMTOutput);//For vector
RCPP_DECLAREVECTOR(Core::FMTTheme);//For vector
RCPP_DECLAREVECTOR(Core::FMTSchedule);//For vector
RCPP_DECLARELIST(std::vector<Core::FMTSchedule>)//For vector of vector
RCPP_DECLAREVECTOR(Core::FMTGCBMTransition);//For vector
RCPP_DECLAREVECTOR(Core::FMTConstraint);//For vector
RCPP_DECLAREMAP(Core::FMTDevelopment, std::vector<double>);//For dev of doubles
RCPP_DECLAREMAP(std::string, std::vector<double>);//For string of double map
RCPP_DECLAREMAP(std::string, std::vector<Core::FMTDevelopment>);//For string of vector of developments
RCPP_DECLAREMAP(Core::FMTAction , std::map<Core::FMTDevelopment RCPP_COMMA std::vector<double>>);//For action of ...
RCPP_DECLAREMAP(std::string , std::map<std::string RCPP_COMMA std::vector<double>>);//For string of ...
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTOutputSource);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTOutputSource);//For vector
RCPP_DECLAREPAIR(Core::FMTMask, Core::FMTOperator);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTOperator);//For vector
RCPP_DECLAREPAIR(Core::FMTTransitionMask, Core::FMTTransitionMask);//pair for FMTList
RCPP_DECLAREVECTOR(Core::FMTTransitionMask);//For vector



#endif // RDECLARATIONS_H_INCLUDED
