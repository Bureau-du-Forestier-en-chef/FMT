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

#ifndef REXPORTSPATIAL_H_INCLUDED
#define REXPORTSPATIAL_H_INCLUDED

#include <Rcpp.h>
#include "FMTcoordinate.h"
#include "FMTforest.h"
#include "FMTdisturbancestack.h"
#include "FMTspatialaction.h"
#include "FMTsaschedule.h"
#include "FMTgraph.h"
#include "FMTsasolution.h"
#include "Rdefinitions.h"
#include "FMTevent.h"
#include "FMTdevelopment.h"

RCPP_EXPOSED_ENUM_NODECL(Spatial::FMTsamovetype);

RCPP_EXPOSED_WRAP(Spatial::FMTcoordinate);
RCPP_EXPOSED_AS(Spatial::FMTcoordinate);

RCPP_DEFINEMAP(Spatial::FMTcoordinate,Core::FMTdevelopment);//for FMTlayer
RCPP_EXPOSED_WRAP(Spatial::FMTlayer<Core::FMTdevelopment>);
RCPP_EXPOSED_AS(Spatial::FMTlayer<Core::FMTdevelopment>);

RCPP_EXPOSED_WRAP(Spatial::FMTforest);
RCPP_EXPOSED_AS(Spatial::FMTforest);
RCPP_DEFINEVECTOR(Spatial::FMTforest);// For vector

RCPP_EXPOSED_WRAP(Spatial::FMTdisturbancestack);
RCPP_EXPOSED_AS(Spatial::FMTdisturbancestack);

RCPP_EXPOSED_WRAP(Spatial::FMTspatialaction);
RCPP_EXPOSED_AS(Spatial::FMTspatialaction);
RCPP_DEFINEVECTOR(Spatial::FMTspatialaction);//For vector

RCPP_DEFINEMAP(Spatial::FMTcoordinate,Graph::FMTgraph);//For FMTlayer

RCPP_EXPOSED_WRAP(Spatial::FMTexponentialschedule);
RCPP_EXPOSED_AS(Spatial::FMTexponentialschedule);

RCPP_EXPOSED_WRAP(Spatial::FMTsasolution);
RCPP_EXPOSED_AS(Spatial::FMTsasolution);
RCPP_DEFINEVECTOR(Spatial::FMTsasolution);//For vector

RCPP_EXPOSED_WRAP(Spatial::FMTevent<Core::FMTdevelopment>);
RCPP_EXPOSED_AS(Spatial::FMTevent<Core::FMTdevelopment>);


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


    define_FMTlayer<Core::FMTdevelopment>();


	Rcpp::class_<Spatial::FMTforest>("FMTforest", "@DocString(FMTforest)")
		.derives<Spatial::FMTlayer<Core::FMTdevelopment>>("FMTlayer")
		.constructor("@DocString(FMTforest())")
		.constructor<Spatial::FMTforest>("@DocString(FMTforest(Spatial::FMTforest))")
		.method("getarea",&Spatial::FMTforest::getarea,
			"@DocString(FMTforest(Spatial::getarea))")
		.method("grow",&Spatial::FMTforest::grow,
			"@DocString(FMTforest(Spatial::grow))");


	Rcpp::class_<Spatial::FMTdisturbancestack>("FMTdisturbancestack", "@DocString(FMTdisturbancestack)")
		.constructor("@DocString(FMTdisturbancestack())")
        .method("allow",&Spatial::FMTdisturbancestack::allow,
			"@DocString(FMTdisturbancestack::allow)");


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



	Rcpp::class_<Spatial::FMTexponentialschedule>("FMTexponentialschedule", "@DocString(FMTexponentialschedule)")
		.constructor("@DocString(FMTexponentialschedule())")
        .constructor<double>("@DocString(FMTexponentialschedule(double))");


    define_FMTlayer<Graph::FMTgraph>();

    Rcpp::class_<Spatial::FMTsasolution>("FMTsasolution", "@DocString(FMTsasolution)")
		.constructor("@DocString(FMTsasolution())")
		.derives<Spatial::FMTlayer<Graph::FMTgraph>>("FMTgraphlayer")
        .method("get_stats",&Spatial::FMTsasolution::getsolution_stats,
			"@DocString(FMTsasolution::getsolution_stats)")
        .method("getobjfvalue",&Spatial::FMTsasolution::getobjfvalue,
			"@DocString(FMTsasolution::getobjfvalue)")
        .method("get_forest_at_period",&Spatial::FMTsasolution::getforestperiod,
			"@DocString(FMTsasolution::getforestperiod)");

    }
}

#endif // PYEXPORTSPATIAL_H_INCLUDED
