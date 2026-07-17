/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTParser_H_INCLUDED
#define REXPORTParser_H_INCLUDED

#include "FMTObject.h"
#include "FMTParser.h"
#include "FMTLandscapeParser.h"
#include "FMTConstantParser.h"
#include "FMTLifespanParser.h"
#include "FMTYieldParser.h"
#include "FMTAreaParser.h"
#include "FMTActionParser.h"
#include "FMTTransitionParser.h"
#include "FMTOutputParser.h"
#include "FMTScheduleParser.h"
#include "FMTOptimizationParser.h"
#include "FMTModelParser.h"
#include <Rcpp.h>
#include <string>
#include "Rdefinitions.h"

RCPP_EXPOSED_WRAP(Parser::FMTParser);
RCPP_EXPOSED_AS(Parser::FMTParser);
RCPP_EXPOSED_WRAP(Parser::FMTAreaParser);
RCPP_EXPOSED_AS(Parser::FMTAreaParser);
//RCPP_DEFINEMAP(std::string, std::string); //map of string definition
RCPP_EXPOSED_WRAP(Parser::FMTLandscapeParser);
RCPP_EXPOSED_AS(Parser::FMTLandscapeParser);
RCPP_EXPOSED_WRAP(Parser::FMTActionParser);
RCPP_EXPOSED_AS(Parser::FMTActionParser);
RCPP_EXPOSED_WRAP(Parser::FMTTransitionParser);
RCPP_EXPOSED_AS(Parser::FMTTransitionParser);
RCPP_EXPOSED_WRAP(Parser::FMTConstantParser);
RCPP_EXPOSED_AS(Parser::FMTConstantParser);
RCPP_EXPOSED_WRAP(Parser::FMTLifespanParser);
RCPP_EXPOSED_AS(Parser::FMTLifespanParser);
RCPP_EXPOSED_WRAP(Parser::FMTYieldParser);
RCPP_EXPOSED_AS(Parser::FMTYieldParser);
RCPP_EXPOSED_WRAP(Parser::FMTOutputParser);
RCPP_EXPOSED_AS(Parser::FMTOutputParser);
RCPP_EXPOSED_WRAP(Parser::FMTOptimizationParser);
RCPP_EXPOSED_AS(Parser::FMTOptimizationParser);
RCPP_EXPOSED_WRAP(Parser::FMTScheduleParser);
RCPP_EXPOSED_AS(Parser::FMTScheduleParser);
RCPP_EXPOSED_WRAP(Parser::FMTModelParser);
RCPP_EXPOSED_AS(Parser::FMTModelParser);



namespace R
{
void exportParser()
    {
	// writeForest est surchargé (worker chemins explicites vs. version dossier) : on fixe la surcharge
	// à exposer (le worker à 6 args) via un pointeur typé, sinon &FMTAreaParser::writeForest est ambigu.
	bool (Parser::FMTAreaParser::*writeForestPtr)(
		const Spatial::FMTForest&, const std::vector<Core::FMTTheme>&,
		const std::vector<std::string>&, const std::string&, const std::string&,
		std::vector<std::map<std::string, std::string>>) const = &Parser::FMTAreaParser::writeForest;

	Rcpp::class_<Parser::FMTParser>("FMTparser", "@DocString(FMTParser)")
		.derives<Core::FMTObject>("FMTobject")
		.constructor("@DocString(FMTParser())");
	Rcpp::class_<Parser::FMTAreaParser>("FMTareaparser", "@DocString(FMTAreaParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTAreaParser())")
                .method("read", &Parser::FMTAreaParser::read,
					"@DocString(FMTAreaParser::read)")
				#ifdef FMTWITHGDAL
					.method("readvectors", &Parser::FMTAreaParser::readVectors,
						"@DocString(FMTAreaParser::readvectors)")
					.method("readrasters", &Parser::FMTAreaParser::readRasters,
						"@DocString(FMTAreaParser::readrasters)")
					.method("vectormaptoFMTforest", &Parser::FMTAreaParser::vectormaptoFMTforest,
						"@DocString(FMTAreaParser::vectormaptoFMTforest)")
					.method("vectorfieldtoraster", &Parser::FMTAreaParser::vectorFieldToRaster,
						"@DocString(FMTAreaParser::vectorfieldtoraster)")
					.method("writeforest",writeForestPtr,
						"@DocString(FMTAreaParser::writeforest)")
					.method("writedisturbances",&Parser::FMTAreaParser::writeDisturbances,
						"@DocString(FMTAreaParser::writedisturbances)")
					#ifdef FMTWITHOSI
						.method("getschemeneighbors",&Parser::FMTAreaParser::getSchemeNeighbors,
							"@DocString(FMTAreaParser::getschemeneighbors)")
                        .method("getclusters",&Parser::FMTAreaParser::getClusters,
                             "@DocString(FMTAreaParser::getclusters)")
					#endif
				#endif
				.method("write", &Parser::FMTAreaParser::write,
					"@DocString(FMTAreaParser::write)");
	Rcpp::class_<Parser::FMTLandscapeParser>("FMTlandscapeparser","@DocString(FMTLandscapeParser)")
				.constructor("@DocString(FMTLandscapeParser())")
				.derives<Parser::FMTParser>("FMTparser")
                .method("read",&Parser::FMTLandscapeParser::read,
					"@DocString(FMTLandscapeParser::read)")
				#ifdef FMTWITHGDAL
					.method("readvectors", &Parser::FMTLandscapeParser::readVectors,
						"@DocString(FMTLandscapeParser::readvectors)")
					.method("readrasters",&Parser::FMTLandscapeParser::readRasters,
						"@DocString(FMTLandscapeParser::readrasters)")
				#endif
                .method("write",&Parser::FMTLandscapeParser::write,
					"@DocString(FMTLandscapeParser::write)");
	Rcpp::class_<Parser::FMTActionParser>("FMTactionparser", "@DocString(FMTActionParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTActionParser())")
                .method("read",&Parser::FMTActionParser::read,
					"@DocString(FMTActionParser::read)")
                .method("write",&Parser::FMTActionParser::write,
					"@DocString(FMTActionParser::write)");
	Rcpp::class_<Parser::FMTTransitionParser>("FMTtransitionparser", "@DocString(FMTTransitionParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTTransitionParser())")
                .method("read",&Parser::FMTTransitionParser::read,
					"@DocString(FMTTransitionParser::read)")
                .method("write",&Parser::FMTTransitionParser::write,
					"@DocString(FMTTransitionParser::write)");
	Rcpp::class_<Parser::FMTConstantParser>("FMTconstantparser", "@DocString(FMTConstantParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTConstantParser())")
                .method("read",&Parser::FMTConstantParser::read,
					"@DocString(FMTConstantParser::read)")
                .method("write",&Parser::FMTConstantParser::write,
					"@DocString(FMTConstantParser::write)");
	Rcpp::class_<Parser::FMTLifespanParser>("FMTlifespanparser", "@DocString(FMTLifespanParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTLifespanParser())")
                .method("read",&Parser::FMTLifespanParser::read,
					"@DocString(FMTLifespanParser::read)")
                .method("write",&Parser::FMTLifespanParser::write,
					"@DocString(FMTLifespanParser::write)");
	Rcpp::class_<Parser::FMTYieldParser>("FMTyieldparser", "@DocString(FMTYieldParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTYieldParser())")
                .method("read",&Parser::FMTYieldParser::read,
					"@DocString(FMTYieldParser::read)")
                .method("write",&Parser::FMTYieldParser::write,
					"@DocString(FMTYieldParser::write)");
	Rcpp::class_<Parser::FMTOutputParser>("FMToutputparser", "@DocString(FMTOutputParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTOutputParser())")
                .method("read",&Parser::FMTOutputParser::read,
					"@DocString(FMTOutputParser::read)")
				.method("addoutputs",&Parser::FMTOutputParser::addOutputs,
					"@DocString(FMTOutputParser::addoutputs)")
                .method("write",&Parser::FMTOutputParser::write,
					"@DocString(FMTOutputParser::write)");
	Rcpp::class_<Parser::FMTOptimizationParser>("FMToptimizationparser", "@DocString(FMTOptimizationParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTOptimizationParser())")
				.method("read", &Parser::FMTOptimizationParser::read,
					"@DocString(FMTOptimizationParser::read)")
				.method("write", &Parser::FMTOptimizationParser::write,
					"@DocString(FMTOptimizationParser::write)");
	Rcpp::class_<Parser::FMTModelParser>("FMTmodelparser", "@DocString(FMTModelParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTModelParser())")
                .method("read",&Parser::FMTModelParser::read,
					"@DocString(FMTModelParser::read)")
				.method("readproject", &Parser::FMTModelParser::readproject,
					"@DocString(FMTModelParser::readproject)")
				.method("readschedules", &Parser::FMTModelParser::readschedules,
					"@DocString(FMTModelParser::readschedules)")
                .method("write",&Parser::FMTModelParser::write,
					"@DocString(FMTModelParser::write)");
	Rcpp::class_<Parser::FMTScheduleParser>("FMTscheduleparser", "@DocString(FMTScheduleParser)")
				.derives<Parser::FMTParser>("FMTparser")
				.constructor("@DocString(FMTScheduleParser())")
                .method("read",&Parser::FMTScheduleParser::read,
					"@DocString(FMTScheduleParser::read)")
                .method("write",&Parser::FMTScheduleParser::write,
					"@DocString(FMTScheduleParser::write)");
    }

}

#endif // PYEXPORTParser_H_INCLUDED
