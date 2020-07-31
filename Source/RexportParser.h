/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTParser_H_INCLUDED
#define REXPORTParser_H_INCLUDED

#include "FMTobject.h"
#include "FMTparser.h"
#include "FMTlandscapeparser.h"
#include "FMTconstantparser.h"
#include "FMTyieldparser.h"
#include "FMTareaparser.h"
#include "FMTactionparser.h"
#include "FMTtransitionparser.h"
#include "FMToutputparser.h"
#include "FMTscheduleparser.h"
#include "FMToptimizationparser.h"
#include "FMTmodelparser.h"
#include <Rcpp.h>
#include <string>
#include "Rdefinitions.h"

RCPP_EXPOSED_WRAP(Parser::FMTparser);
RCPP_EXPOSED_AS(Parser::FMTparser);
RCPP_EXPOSED_WRAP(Parser::FMTareaparser);
RCPP_EXPOSED_AS(Parser::FMTareaparser);
//RCPP_DEFINEMAP(std::string, std::string); //map of string definition
RCPP_EXPOSED_WRAP(Parser::FMTlandscapeparser);
RCPP_EXPOSED_AS(Parser::FMTlandscapeparser);
RCPP_EXPOSED_WRAP(Parser::FMTactionparser);
RCPP_EXPOSED_AS(Parser::FMTactionparser);
RCPP_EXPOSED_WRAP(Parser::FMTtransitionparser);
RCPP_EXPOSED_AS(Parser::FMTtransitionparser);
RCPP_EXPOSED_WRAP(Parser::FMTconstantparser);
RCPP_EXPOSED_AS(Parser::FMTconstantparser);
RCPP_EXPOSED_WRAP(Parser::FMTlifespanparser);
RCPP_EXPOSED_AS(Parser::FMTlifespanparser);
RCPP_EXPOSED_WRAP(Parser::FMTyieldparser);
RCPP_EXPOSED_AS(Parser::FMTyieldparser);
RCPP_EXPOSED_WRAP(Parser::FMToutputparser);
RCPP_EXPOSED_AS(Parser::FMToutputparser);
RCPP_EXPOSED_WRAP(Parser::FMToptimizationparser);
RCPP_EXPOSED_AS(Parser::FMToptimizationparser);
RCPP_EXPOSED_WRAP(Parser::FMTscheduleparser);
RCPP_EXPOSED_AS(Parser::FMTscheduleparser);
RCPP_EXPOSED_WRAP(Parser::FMTmodelparser);
RCPP_EXPOSED_AS(Parser::FMTmodelparser);



namespace R
{
void exportParser()
    {
	Rcpp::class_<Parser::FMTparser>("FMTparser", "@DocString(FMTparser)")
		.derives<Core::FMTobject>("FMTobject")
		.constructor("@DocString(FMTparser())");
	Rcpp::class_<Parser::FMTareaparser>("FMTareaparser", "@DocString(FMTareaparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMTareaparser())")
                .method("read", &Parser::FMTareaparser::read,
					"@DocString(FMTareaparser::read)")
				#ifdef FMTWITHGDAL
					.method("readvectors", &Parser::FMTareaparser::readvectors,
						"@DocString(FMTareaparser::readvectors)")
					.method("readrasters", &Parser::FMTareaparser::readrasters,
						"@DocString(FMTareaparser::readrasters)")
					.method("writeforest",&Parser::FMTareaparser::writeforest,
						"@DocString(FMTareaparser::writeforest)")
					.method("writedisturbances",&Parser::FMTareaparser::writedisturbances,
						"@DocString(FMTareaparser::writedisturbances)")
					#ifdef FMTWITHOSI
						.method("getschemeneighbors",&Parser::FMTareaparser::getschemeneighbors,
							"@DocString(FMTareaparser::getschemeneighbors)")
                        .method("getclusters",&Parser::FMTareaparser::getclusters,
                             "@DocString(FMTareaparser::getclusters)")
					#endif
				#endif
				.method("write", &Parser::FMTareaparser::write,
					"@DocString(FMTareaparser::write)");
	Rcpp::class_<Parser::FMTlandscapeparser>("FMTlandscapeparser","@DocString(FMTlandscapeparser)")
				.constructor("@DocString(FMTlandscapeparser())")
				.derives<Parser::FMTparser>("FMTparser")
                .method("read",&Parser::FMTlandscapeparser::read,
					"@DocString(FMTlandscapeparser::read)")
				#ifdef FMTWITHGDAL
					.method("readvectors", &Parser::FMTlandscapeparser::readvectors,
						"@DocString(FMTlandscapeparser::readvectors)")
					.method("readrasters",&Parser::FMTlandscapeparser::readrasters,
						"@DocString(FMTlandscapeparser::readrasters)")
				#endif
                .method("write",&Parser::FMTlandscapeparser::write,
					"@DocString(FMTlandscapeparser::write)");
	Rcpp::class_<Parser::FMTactionparser>("FMTactionparser", "@DocString(FMTactionparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMTactionparser())")
                .method("read",&Parser::FMTactionparser::read,
					"@DocString(FMTactionparser::read)")
                .method("write",&Parser::FMTactionparser::write,
					"@DocString(FMTactionparser::write)");
	Rcpp::class_<Parser::FMTtransitionparser>("FMTtransitionparser", "@DocString(FMTtransitionparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMTtransitionparser())")
                .method("read",&Parser::FMTtransitionparser::read,
					"@DocString(FMTtransitionparser::read)")
                .method("write",&Parser::FMTtransitionparser::write,
					"@DocString(FMTtransitionparser::write)");
	Rcpp::class_<Parser::FMTconstantparser>("FMTconstantparser", "@DocString(FMTconstantparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMTconstantparser())")
                .method("read",&Parser::FMTconstantparser::read,
					"@DocString(FMTconstantparser::read)")
                .method("write",&Parser::FMTconstantparser::write,
					"@DocString(FMTconstantparser::write)");
	Rcpp::class_<Parser::FMTlifespanparser>("FMTlifespanparser", "@DocString(FMTlifespanparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMTlifespanparser())")
                .method("read",&Parser::FMTlifespanparser::read,
					"@DocString(FMTlifespanparser::read)")
                .method("write",&Parser::FMTlifespanparser::write,
					"@DocString(FMTlifespanparser::write)");
	Rcpp::class_<Parser::FMTyieldparser>("FMTyieldparser", "@DocString(FMTyieldparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMTyieldparser())")
                .method("read",&Parser::FMTyieldparser::read,
					"@DocString(FMTyieldparser::read)")
                .method("write",&Parser::FMTyieldparser::write,
					"@DocString(FMTyieldparser::write)");
	Rcpp::class_<Parser::FMToutputparser>("FMToutputparser", "@DocString(FMToutputparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMToutputparser())")
                .method("read",&Parser::FMToutputparser::read,
					"@DocString(FMToutputparser::read)")
                .method("write",&Parser::FMToutputparser::write,
					"@DocString(FMToutputparser::write)");
	Rcpp::class_<Parser::FMToptimizationparser>("FMToptimizationparser", "@DocString(FMToptimizationparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMToptimizationparser())")
				.method("read", &Parser::FMToptimizationparser::read,
					"@DocString(FMToptimizationparser::read)")
				.method("write", &Parser::FMToptimizationparser::write,
					"@DocString(FMToptimizationparser::write)");
	Rcpp::class_<Parser::FMTmodelparser>("FMTmodelparser", "@DocString(FMTmodelparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMTmodelparser())")
                .method("read",&Parser::FMTmodelparser::read,
					"@DocString(FMTmodelparser::read)")
				.method("readproject", &Parser::FMTmodelparser::readproject,
					"@DocString(FMTmodelparser::readproject)")
				.method("readschedules", &Parser::FMTmodelparser::readschedules,
					"@DocString(FMTmodelparser::readschedules)")
                .method("write",&Parser::FMTmodelparser::write,
					"@DocString(FMTmodelparser::write)");
	Rcpp::class_<Parser::FMTscheduleparser>("FMTscheduleparser", "@DocString(FMTscheduleparser)")
				.derives<Parser::FMTparser>("FMTparser")
				.constructor("@DocString(FMTscheduleparser())")
                .method("read",&Parser::FMTscheduleparser::read,
					"@DocString(FMTscheduleparser::read)")
                .method("write",&Parser::FMTscheduleparser::write,
					"@DocString(FMTscheduleparser::write)");
    }

}

#endif // PYEXPORTParser_H_INCLUDED
