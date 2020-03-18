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

#ifndef RDECLARATIONS_H_INCLUDED
#define RDECLARATIONS_H_INCLUDED

/*This header file needs to be included before Rcpp.h*/

#include <RcppCommon.h>
#include <vector>
#include <map>

#include "FMTdevelopment.h"
#include "FMTdevelopmentpath.h"
#include "FMTforest.h"
#include "FMTspatialaction.h"
#include "FMTgraph.h"
#include "FMTsasolution.h"
#include "FMTmodel.h"
#include "FMTsesmodel.h"
#include "FMTlpmodel.h"
#include "FMTsamodel.h"
#include "FMToperatingarea.h"
#include "FMToperatingareaheuristic.h"

#include "FMTmask.h"
#include "FMTactualdevelopment.h"
#include "FMTfuturdevelopment.h"
#include "FMTaction.h"
#include "FMTbounds.h"
#include "FMTyieldhandler.h"
#include "FMTfork.h"
#include "FMTtransition.h"
#include "FMToutputnode.h"
#include "FMToutput.h"
#include "FMTtheme.h"
#include "FMTschedule.h"
#include "FMTGCBMtransition.h"
#include "FMTconstraint.h"
#include "FMTcoordinate.h"


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



//Version

//Parser
RCPP_DECLAREMAP(std::string, std::string); //map of string definition

//Spatial
RCPP_DECLAREMAP(Spatial::FMTcoordinate,Core::FMTdevelopment);//for FMTlayer
RCPP_DECLAREVECTOR(Spatial::FMTforest);// For vector
RCPP_DECLAREVECTOR(Spatial::FMTspatialaction);//For vector
RCPP_DECLAREMAP(Spatial::FMTcoordinate,Graph::FMTgraph);//For FMTlayer
RCPP_DECLAREVECTOR(Spatial::FMTsasolution);//For vector
//Models
RCPP_DECLAREVECTOR(Models::FMTmodel);//For vector
RCPP_DECLAREMAP(std::string, double);//For map<string,double>
RCPP_DECLAREVECTOR(Models::FMTsesmodel);//For vector
#ifdef FMTWITHOSI
	RCPP_DECLAREVECTOR(Models::FMTlpmodel);//For vector
#endif
RCPP_DECLAREVECTOR(Models::FMTsamodel);//For vector
//Heuristics
#ifdef FMTWITHOSI
	RCPP_DECLAREVECTOR(Heuristics::FMToperatingarea);//For vector
	RCPP_DECLAREVECTOR(Heuristics::FMToperatingareaheuristic);//For vector
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
RCPP_DECLAREPAIR(Core::FMTmask, Core::FMTyieldhandler);//pair for FMTlist
RCPP_DECLAREVECTOR(Core::FMTyieldhandler);//For vector
RCPP_DECLAREPAIR(Core::FMTmask, Core::FMTfork);//pair for FMTlist
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
