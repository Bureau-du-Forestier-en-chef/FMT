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

#include "FMTDevelopment.h"
#include "FMTDevelopmentPath.h"
#include "FMTForest.h"
//#include "FMTspatialaction.h"
//#include "FMTGraph.hpp"
#include "FMTLineGraph.h"
//#include "FMTSaSolution.h"
#include "FMTModel.h"
#include "FMTSeModel.h"
#include "FMTSrModel.h"
#include "FMTNssModel.h"
#if defined FMTWITHOSI
	#include "FMTLpSolver.h"
	#include "FMTLpModel.h"
#endif
#include "FMTSesModel.h"
#include "FMTSaSchedule.h"
//#include "FMTExponentialSchedule.h" // Comment Gab 2026-02-09
#include "FMTSaModel.h"
#include "FMTOperatingArea.h"
#include "FMTOperatingAreaScheduler.h"
#include "FMTOperatingAreaClusterBinary.h"

#include "FMTMask.h"
#include "FMTActualDevelopment.h"
#include "FMTFuturDevelopment.h"
#include "FMTAction.h"
#include "FMTBounds.hpp"
#include "FMTTimeYieldHandler.h"
#include "FMTAgeYieldHandler.h"
#include "FMTFork.h"
#include "FMTTransition.h"
#include "FMTOutputNode.h"
#include "FMTOutput.h"
#include "FMTTheme.h"
#include "FMTSchedule.h"
#include "FMTGCBMTransition.h"
#include "FMTConstraint.h"
#include "FMTCoordinate.h"
#include "FMTModelParameters.h"
#include "FMTMaskFilter.h"
#include "FMTOutputSource.h"
#include "FMTOperator.h"
#include "FMTTransitionMask.h"

//Parallel
#include "FMTTask.h"
#include "FMTTaskHandler.h"
#include "FMTReplanningTask.h"
#include "FMTPlanningTask.h"
#if defined FMTWITHOSI
	#include "FMTOpAreaSchedulerTask.h"
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
RCPP_DECLAREMAP(Spatial::FMTCoordinate,Core::FMTDevelopment);//for FMTLayer
RCPP_DECLAREVECTOR(Spatial::FMTForest);// For vector
//RCPP_DECLAREVECTOR(Spatial::FMTspatialaction);//For vector
//RCPP_DECLAREMAP(Spatial::FMTCoordinate,Graph::FMTGraph);//For FMTLayer
RCPP_DECLAREMAP(Spatial::FMTCoordinate, Graph::FMTLineGraph);
RCPP_DECLAREVECTOR(Models::FMTSesModel);//For vector
RCPP_DECLAREVECTOR(Models::FMTSeModel);//For vector
//RCPP_DECLAREVECTOR(Spatial::FMTSaSolution);//For vector
//Models
RCPP_DECLAREVECTOR(Models::FMTModel);//For vector
RCPP_DECLAREMAP(std::string, double);//For map<string,double>
RCPP_DECLAREVECTOR(Models::FMTintmodelparameters);//For vector
RCPP_DECLAREVECTOR(Models::FMTdblmodelparameters);//For vector
RCPP_DECLAREVECTOR(Models::FMTboolmodelparameters);


//Parallel
RCPP_DECLAREVECTOR(Parallel::FMTTask);//For vector
RCPP_DECLAREVECTOR(Parallel::FMTTaskHandler);//For vector
RCPP_DECLAREVECTOR(Parallel::FMTReplanningTask);//For vector
RCPP_DECLAREVECTOR(Parallel::FMTPlanningTask);//For vector
#ifdef FMTWITHOSI
	RCPP_DECLAREVECTOR(Parallel::FMTOpAreaSchedulerTask);//For vector
	RCPP_DECLAREVECTOR(Models::FMTLpSolver);//For vector
	RCPP_DECLAREVECTOR(Models::FMTSrModel);//For vector
	RCPP_DECLAREVECTOR(Models::FMTLpModel);//For vector
	RCPP_DECLAREVECTOR(Models::FMTNssModel);//For vector
#endif
RCPP_DECLAREVECTOR(Models::FMTSaModel);//For vector
//Heuristics
#ifdef FMTWITHOSI
	RCPP_DECLAREVECTOR(Heuristics::FMTOperatingArea);//For vector
	RCPP_DECLAREVECTOR(Heuristics::FMTOperatingAreaScheme);//For vector
	RCPP_DECLAREVECTOR(Heuristics::FMTOperatingAreaScheduler);//For vector
	RCPP_DECLAREVECTOR(Heuristics::FMTOperatingAreaClusterBinary);//For vector
    RCPP_DECLAREVECTOR(Heuristics::FMTOperatingAreaCluster);//For vector
    RCPP_DECLAREVECTOR(Heuristics::FMTOperatingAreaClusterer);//For vector
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
