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

#include "FMTdevelopment.hpp"
#include "FMTdevelopmentpath.hpp"
#include "FMTforest.hpp"
#include "FMTspatialaction.hpp"
//#include "FMTgraph.hpp"
#include "FMTlinegraph.hpp"
#include "FMTsasolution.hpp"
#include "FMTmodel.hpp"
#include "FMTsesmodel.hpp"
#include "FMTsrmodel.hpp"
#include "FMTnssmodel.hpp"
#include "FMTlpsolver.hpp"
#include "FMTlpmodel.hpp"
#include "FMTsamodel.hpp"
#include "FMToperatingarea.hpp"
#include "FMToperatingareascheduler.hpp"
#include "FMToperatingareaclusterbinary.hpp"

#include "FMTmask.hpp"
#include "FMTactualdevelopment.hpp"
#include "FMTfuturdevelopment.hpp"
#include "FMTaction.hpp"
#include "FMTbounds.hpp"
#include "FMTtimeyieldhandler.hpp"
#include "FMTfork.hpp"
#include "FMTtransition.hpp"
#include "FMToutputnode.hpp"
#include "FMToutput.hpp"
#include "FMTtheme.hpp"
#include "FMTschedule.hpp"
#include "FMTGCBMtransition.hpp"
#include "FMTconstraint.hpp"
#include "FMTcoordinate.hpp"
#include "FMTmodelparameters.hpp"
#include "FMTmaskfilter.hpp"

//Parallel
#include "FMTtask.hpp"
#include "FMTtaskhandler.hpp"
#include "FMTreplanningtask.hpp"
#include "FMTplanningtask.hpp"
#include "FMTopareaschedulertask.hpp"

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
RCPP_DECLAREMAP(Spatial::FMTcoordinate,Core::FMTdevelopment);//for FMTlayer
RCPP_DECLAREVECTOR(Spatial::FMTforest);// For vector
RCPP_DECLAREVECTOR(Spatial::FMTspatialaction);//For vector
//RCPP_DECLAREMAP(Spatial::FMTcoordinate,Graph::FMTgraph);//For FMTlayer
RCPP_DECLAREMAP(Spatial::FMTcoordinate, Graph::FMTlinegraph);
RCPP_DECLAREVECTOR(Models::FMTsesmodel);//For vector
RCPP_DECLAREVECTOR(Spatial::FMTsasolution);//For vector
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
RCPP_DECLAREVECTOR(Parallel::FMTopareaschedulertask);//For vector

#ifdef FMTWITHOSI
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
RCPP_DECLAREVECTOR(Core::FMTmask);//For vector
RCPP_DECLAREVECTOR(Core::FMTdevelopment);//For vector
RCPP_DECLAREVECTOR(Core::FMTdevelopmentpath);//For vector
RCPP_DECLAREPAIR(Core::FMTmask, Core::FMTspec);//pair for FMTlist
RCPP_DECLAREVECTOR(Core::FMTspec);//For vector
RCPP_DECLAREVECTOR(Core::FMTactualdevelopment);//For vector
RCPP_DECLAREVECTOR(Core::FMTfuturdevelopment);//For vector
RCPP_DECLAREVECTOR(Core::FMTaction);//For vector
RCPP_DECLAREPAIR(Core::FMTmask, int);//pair for FMTlist
RCPP_DECLAREPAIR(Core::FMTmask, Core::FMTtimeyieldhandler);//pair for FMTlist
RCPP_DECLAREVECTOR(Core::FMTtimeyieldhandler);//For vector
RCPP_DECLAREPAIR(Core::FMTmask, Core::FMTfork);//pair for FMTlist
RCPP_DECLAREVECTOR(Core::FMTmaskfilter);//For vector
RCPP_DECLAREVECTOR(Core::FMTfork);//For vector
RCPP_DECLAREVECTOR(Core::FMTtransition);//For vector
RCPP_DECLAREVECTOR(Core::FMToutputnode);//For vector
RCPP_DECLAREVECTOR(Core::FMToutput);//For vector
RCPP_DECLAREVECTOR(Core::FMTtheme);//For vector
RCPP_DECLAREVECTOR(Core::FMTschedule);//For vector
RCPP_DECLARELIST(std::vector<Core::FMTschedule>)//For vector of vector
RCPP_DECLAREVECTOR(Core::FMTGCBMtransition);//For vector
RCPP_DECLAREVECTOR(Core::FMTconstraint);//For vector
RCPP_DECLAREMAP(Core::FMTdevelopment, std::vector<double>);//For dev of doubles
RCPP_DECLAREMAP(std::string, std::vector<double>);//For string of double map
RCPP_DECLAREMAP(std::string, std::vector<Core::FMTdevelopment>);//For string of vector of developements
RCPP_DECLAREMAP(Core::FMTaction , std::map<Core::FMTdevelopment RCPP_COMMA std::vector<double>>);//For action of ...
RCPP_DECLAREMAP(std::string , std::map<std::string RCPP_COMMA std::vector<double>>);//For string of ...

#endif // RDECLARATIONS_H_INCLUDED