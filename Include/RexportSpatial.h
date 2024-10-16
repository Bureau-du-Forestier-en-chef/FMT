/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTSPATIAL_H_INCLUDED
#define REXPORTSPATIAL_H_INCLUDED

#include <Rcpp.h>
#include "FMTcoordinate.h"
#include "FMTforest.h"
#include "FMTspatialschedule.h"
//#include "FMTspatialaction.h"
#include "FMTsaschedule.h"
#include "FMTgraph.hpp"
#include "FMTlinegraph.h"
//#include "FMTsasolution.h"
#include "Rdefinitions.h"
#include "FMTevent.h"
#include "FMTdevelopment.h"

//RCPP_EXPOSED_ENUM_NODECL(Spatial::FMTsamovetype);

RCPP_EXPOSED_WRAP(Spatial::FMTcoordinate);
RCPP_EXPOSED_AS(Spatial::FMTcoordinate);

RCPP_DEFINEMAP(Spatial::FMTcoordinate,Core::FMTdevelopment);//for FMTlayer
RCPP_EXPOSED_WRAP(Spatial::FMTlayer<Core::FMTdevelopment>);
RCPP_EXPOSED_AS(Spatial::FMTlayer<Core::FMTdevelopment>);

RCPP_EXPOSED_WRAP(Spatial::FMTforest);
RCPP_EXPOSED_AS(Spatial::FMTforest);
RCPP_DEFINEVECTOR(Spatial::FMTforest);// For vector

RCPP_EXPOSED_WRAP(Spatial::FMTspatialschedule);
RCPP_EXPOSED_AS(Spatial::FMTspatialschedule);
/*
RCPP_EXPOSED_WRAP(Spatial::FMTspatialaction);
RCPP_EXPOSED_AS(Spatial::FMTspatialaction);
RCPP_DEFINEVECTOR(Spatial::FMTspatialaction);//For vector
*/
RCPP_EXPOSED_WRAP(Graph::FMTlinegraph);
RCPP_EXPOSED_AS(Graph::FMTlinegraph);
RCPP_DEFINEMAP(Spatial::FMTcoordinate,Graph::FMTlinegraph);//For FMTlayer


RCPP_EXPOSED_WRAP(Spatial::FMTexponentialschedule);
RCPP_EXPOSED_AS(Spatial::FMTexponentialschedule);
/*
RCPP_EXPOSED_WRAP(Spatial::FMTsasolution);
RCPP_EXPOSED_AS(Spatial::FMTsasolution);
RCPP_DEFINEVECTOR(Spatial::FMTsasolution);//For vector
*/
RCPP_EXPOSED_WRAP(Spatial::FMTevent);
RCPP_EXPOSED_AS(Spatial::FMTevent);


namespace R 
{

void exportSpatial()
    {


	Rcpp::class_<Spatial::FMTcoordinate>("FMTcoordinate","@DocString(FMTcoordinate)")
		.constructor("@DocString(FMTcoordinate())")
        .constructor<unsigned int,unsigned int>("@DocString(FMTcoordinate(unsigned int,unsigned int))")
        .method("lt",&Spatial::FMTcoordinate::operator <,
			"@DocString(FMTcoordinate::operator<)")
		.method("getx",&Spatial::FMTcoordinate::getx,
			"@DocString(FMTcoordinate::getx)")
		.method("gety",&Spatial::FMTcoordinate::gety,
			"@DocString(FMTcoordinate::gety)");


	define_FMTlayer<Core::FMTdevelopment>("FMTdevelopmentlayer");


	Rcpp::class_<Spatial::FMTforest>("FMTforest", "@DocString(FMTforest)")
		.derives<Spatial::FMTlayer<Core::FMTdevelopment>>("FMTdevelopmentlayer")
		.constructor("@DocString(FMTforest())")
		.constructor<Spatial::FMTforest>("@DocString(FMTforest(Spatial::FMTforest))")
		.method("getarea",&Spatial::FMTforest::getarea,
			"@DocString(FMTforest(Spatial::getarea))")
		.method("grow",&Spatial::FMTforest::grow,
			"@DocString(FMTforest(Spatial::grow))");


	Rcpp::class_<Spatial::FMTspatialschedule>("FMTspatialschedule", "@DocString(FMTspatialschedule)")
		.constructor("@DocString(FMTspatialschedule())");

	/*
	Rcpp::class_<Spatial::FMTspatialaction>("FMTspatialaction", "@DocString(FMTspatialaction)")
		.constructor("@DocString(FMTspatialaction())")
		.derives<Core::FMTaction> ("FMTaction")
		.constructor<Core::FMTaction>("@DocString(FMTspatialaction(FMTaction))")
        .field("neighbors",&Spatial::FMTspatialaction::neighbors,
			"@DocString(FMTspatialaction(FMTspatialaction::neighbors))")
        .field("green_up",&Spatial::FMTspatialaction::green_up,
			"@DocString(FMTspatialaction(FMTspatialaction::green_up))")
        .field("adjacency",&Spatial::FMTspatialaction::adjacency,
			"@DocString(FMTspatialaction(FMTspatialaction::adjacency))")
        .field("minimal_size",&Spatial::FMTspatialaction::minimal_size,
			"@DocString(FMTspatialaction(FMTspatialaction::minimal_size))")
        .field("maximal_size",&Spatial::FMTspatialaction::maximal_size,
			"@DocString(FMTspatialaction(FMTspatialaction::maximal_size))")
        .field("neighbors_size",&Spatial::FMTspatialaction::neighbors_size,
			"@DocString(FMTspatialaction(FMTspatialaction::neighbors_size))")
        .field("greenup_weight",&Spatial::FMTspatialaction::greenup_weight,
			"@DocString(FMTspatialaction(FMTspatialaction::greenup_weight))")
        .field("adjacency_weight",&Spatial::FMTspatialaction::adjacency_weight,
			"@DocString(FMTspatialaction(FMTspatialaction::adjacency_weight))")
        .field("size_weight",&Spatial::FMTspatialaction::size_weight);
		*/


	Rcpp::class_<Spatial::FMTexponentialschedule>("FMTexponentialschedule", "@DocString(FMTexponentialschedule)")
		.constructor("@DocString(FMTexponentialschedule())")
        .constructor<double>("@DocString(FMTexponentialschedule(double))");
		

	define_FMTlayer<Graph::FMTlinegraph>("FMTgraphlayer");

	/*
    Rcpp::class_<Spatial::FMTsasolution>("FMTsasolution", "@DocString(FMTsasolution)")
		.constructor("@DocString(FMTsasolution())")
		.derives<Spatial::FMTlayer<Graph::FMTlinegraph>>("FMTgraphlayer")
        .method("get_stats",&Spatial::FMTsasolution::getsolution_stats,
			"@DocString(FMTsasolution::getsolution_stats)")
        .method("getobjfvalue",&Spatial::FMTsasolution::getobjfvalue,
			"@DocString(FMTsasolution::getobjfvalue)")
        .method("get_forest_at_period",&Spatial::FMTsasolution::getforestperiod,
			"@DocString(FMTsasolution::getforestperiod)");
	*/
    }
}

#endif // PYEXPORTSPATIAL_H_INCLUDED
