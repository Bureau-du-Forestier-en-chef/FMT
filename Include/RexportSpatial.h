/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTSPATIAL_H_INCLUDED
#define REXPORTSPATIAL_H_INCLUDED

#include <Rcpp.h>
#include "FMTCoordinate.h"
#include "FMTForest.h"
#include "FMTSpatialSchedule.h"
//#include "FMTspatialaction.h"
#include "FMTSaSchedule.h"
#include "FMTGraph.hpp"
#include "FMTLineGraph.h"
//#include "FMTSaSolution.h"
#include "Rdefinitions.h"
#include "FMTEvent.h"
#include "FMTDevelopment.h"

//RCPP_EXPOSED_ENUM_NODECL(Spatial::FMTsamovetype);

RCPP_EXPOSED_WRAP(Spatial::FMTCoordinate);
RCPP_EXPOSED_AS(Spatial::FMTCoordinate);

RCPP_DEFINEMAP(Spatial::FMTCoordinate,Core::FMTDevelopment);//for FMTLayer
RCPP_EXPOSED_WRAP(Spatial::FMTLayer<Core::FMTDevelopment>);
RCPP_EXPOSED_AS(Spatial::FMTLayer<Core::FMTDevelopment>);

RCPP_EXPOSED_WRAP(Spatial::FMTForest);
RCPP_EXPOSED_AS(Spatial::FMTForest);
RCPP_DEFINEVECTOR(Spatial::FMTForest);// For vector

RCPP_EXPOSED_WRAP(Spatial::FMTSpatialSchedule);
RCPP_EXPOSED_AS(Spatial::FMTSpatialSchedule);
/*
RCPP_EXPOSED_WRAP(Spatial::FMTspatialaction);
RCPP_EXPOSED_AS(Spatial::FMTspatialaction);
RCPP_DEFINEVECTOR(Spatial::FMTspatialaction);//For vector
*/
RCPP_EXPOSED_WRAP(Graph::FMTLineGraph);
RCPP_EXPOSED_AS(Graph::FMTLineGraph);
RCPP_DEFINEMAP(Spatial::FMTCoordinate,Graph::FMTLineGraph);//For FMTLayer


//RCPP_EXPOSED_WRAP(Spatial::FMTExponentialSchedule); / Comment Gab 2026-02-09
// RCPP_EXPOSED_AS(Spatial::FMTExponentialSchedule); / Comment Gab 2026-02-09
/*
RCPP_EXPOSED_WRAP(Spatial::FMTSaSolution);
RCPP_EXPOSED_AS(Spatial::FMTSaSolution);
RCPP_DEFINEVECTOR(Spatial::FMTSaSolution);//For vector
*/
RCPP_EXPOSED_WRAP(Spatial::FMTEvent);
RCPP_EXPOSED_AS(Spatial::FMTEvent);


namespace R 
{

void exportSpatial()
    {


	Rcpp::class_<Spatial::FMTCoordinate>("FMTcoordinate","@DocString(FMTCoordinate)")
		.constructor("@DocString(FMTCoordinate())")
        .constructor<unsigned int,unsigned int>("@DocString(FMTCoordinate(unsigned int,unsigned int))")
        .method("lt",&Spatial::FMTCoordinate::operator <,
			"@DocString(FMTCoordinate::operator<)")
		.method("getx",&Spatial::FMTCoordinate::getX,
			"@DocString(FMTCoordinate::getx)")
		.method("gety",&Spatial::FMTCoordinate::getY,
			"@DocString(FMTCoordinate::gety)");


	define_FMTlayer<Core::FMTDevelopment>("FMTdevelopmentlayer");


	Rcpp::class_<Spatial::FMTForest>("FMTforest", "@DocString(FMTForest)")
		.derives<Spatial::FMTLayer<Core::FMTDevelopment>>("FMTdevelopmentlayer")
		.constructor("@DocString(FMTForest())")
		.constructor<Spatial::FMTForest>("@DocString(FMTForest(Spatial::FMTForest))")
		.method("getarea",&Spatial::FMTForest::getArea,
			"@DocString(FMTForest(Spatial::getarea))")
		.method("grow",&Spatial::FMTForest::grow,
			"@DocString(FMTForest(Spatial::grow))");


	Rcpp::class_<Spatial::FMTSpatialSchedule>("FMTSpatialSchedule", "@DocString(FMTSpatialSchedule)")
		.constructor("@DocString(FMTSpatialSchedule())");

	/*
	Rcpp::class_<Spatial::FMTspatialaction>("FMTspatialaction", "@DocString(FMTspatialaction)")
		.constructor("@DocString(FMTspatialaction())")
		.derives<Core::FMTAction> ("FMTaction")
		.constructor<Core::FMTAction>("@DocString(FMTspatialaction(FMTAction))")
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

// comment Gab 2026-02-09
//	Rcpp::class_<Spatial::FMTExponentialSchedule>("FMTexponentialschedule", "@DocString(FMTExponentialSchedule)")
//		.constructor("@DocString(FMTExponentialSchedule())")
//       .constructor<double>("@DocString(FMTExponentialSchedule(double))");
		

	define_FMTlayer<Graph::FMTLineGraph>("FMTgraphlayer");

	/*
    Rcpp::class_<Spatial::FMTSaSolution>("FMTsasolution", "@DocString(FMTSaSolution)")
		.constructor("@DocString(FMTSaSolution())")
		.derives<Spatial::FMTLayer<Graph::FMTLineGraph>>("FMTgraphlayer")
        .method("get_stats",&Spatial::FMTSaSolution::getsolution_stats,
			"@DocString(FMTSaSolution::getsolution_stats)")
        .method("getobjfvalue",&Spatial::FMTSaSolution::getobjfvalue,
			"@DocString(FMTSaSolution::getobjfvalue)")
        .method("get_forest_at_period",&Spatial::FMTSaSolution::getForestPeriod,
			"@DocString(FMTSaSolution::getForestPeriod)");
	*/
    }
}

#endif // PYEXPORTSPATIAL_H_INCLUDED
