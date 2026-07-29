/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTParser_H_INCLUDED
#define PYEXPORTParser_H_INCLUDED

#include "FMTLandscapeParser.h"
#include "FMTConstantParser.h"
#include "FMTYieldParser.h"
#include "FMTAreaParser.h"
#include "FMTActionParser.h"
#include "FMTTransitionParser.h"
#include "FMTOutputParser.h"
#include "FMTOptimizationParser.h"
#include "FMTScheduleParser.h"
#include "FMTModelParser.h"
#include "boost/python.hpp"
#include "FMTLifespanParser.h"

namespace Python
{
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readrasters_overloads,readRasters, 3, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readvectors_overloads,readVectors, 4, 8)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getschemeneighbors_overloads, getSchemeNeighbors, 5, 10)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getclusters_overloads,getClusters,6, 11)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(addoutputs_overloads,addOutputs,6, 7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vectormaptoFMTforest_overloads,vectormaptoFMTforest,5,11)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vectorfieldtoraster_overloads, vectorFieldToRaster,4,5)


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writeforest_overloads,writeForest, 5, 6)
//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writedisturbances_overloads,writeDisturbances, 5, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writepredictors_overloads,writePredictors, 5, 7)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readmodel_overloads, read, 8, 9)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getconstraintsfromstring_overloads,getConstraintsFromString,2,3)


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readproject_overloads, readproject,1,5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writeresults_overloads, writeResults, 5, 7)


void exportParser()
    {
    namespace bp = boost::python;
    bp::object ParserModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.Parser"))));
    bp::scope().attr("Parser") = ParserModule;
    bp::scope Parser_scope = ParserModule;
    bp::scope().attr("__doc__") = ".. module:: Parser\n"
    "\n"
    "   :platform: Unix, Windows\n"
    "   :synopsis: Module used to import Woodstock models.\n"
    "\n";

	bp::class_<Parser::FMTParser, bp::bases<Core::FMTObject>>("FMTparser", "@DocString(FMTParser)");

	definePyDict<std::string,std::string>();

	// writeForest est surchargé (worker chemins explicites vs. version dossier) : on fixe la surcharge
	// à exposer (le worker à 6 args) via un pointeur typé, sinon &FMTAreaParser::writeForest est ambigu.
	bool (Parser::FMTAreaParser::*writeForestPtr)(
		const Spatial::FMTForest&, const std::vector<Core::FMTTheme>&,
		const std::vector<std::string>&, const std::string&, const std::string&,
		std::vector<std::map<std::string, std::string>>) const = &Parser::FMTAreaParser::writeForest;

	bp::class_<Parser::FMTAreaParser, bp::bases<Parser::FMTParser>>("FMTareaparser", "@DocString(FMTAreaParser)")
                .def("read", &Parser::FMTAreaParser::read, "@DocString(FMTAreaParser::read)")
				#ifdef FMTWITHGDAL
					.def("readvectors", &Parser::FMTAreaParser::readVectors,readvectors_overloads(bp::args("themes", "data_vectors", "agefield", "areafield", "areafactor", "lockfield","minimalarea"), "@DocString(FMTAreaParser::readvectors)"))
					.def("readrasters", &Parser::FMTAreaParser::readRasters,readrasters_overloads(bp::args("themes","data_rasters","age","agefactor","areafactor","lock"), "@DocString(FMTAreaParser::readrasters)"))
					.def("writeforest",writeForestPtr,writeforest_overloads(bp::args("for_layer", "themes", "data_rasters", "age", "lock", "mapping"), "@DocString(FMTAreaParser::writeforest)"))
					//.def("writeDisturbances",&Parser::FMTAreaParser::writeDisturbances,writedisturbances_overloads(bp::args("location", "spatialschedule", "for_layer", "out_layer", "themes", "mapping"), "@DocString(FMTAreaParser::writeDisturbances)"))
					.def("writedisturbances", &Parser::FMTAreaParser::writeDisturbances)
					.def("writepredictors", &Parser::FMTAreaParser::writePredictors,writepredictors_overloads(bp::args("location", "spatialsolution", "yieldnames", "model", "period", "withGCBMid"), "@DocString(FMTAreaParser::writepredictors)"))
					.def("vectormaptoFMTforest", &Parser::FMTAreaParser::vectormaptoFMTforest,vectormaptoFMTforest_overloads(bp::args("data_vectors","resolution","themes","agefield","areafield","agefactor","areafactor","lockfield","minimalarea","writeforestfolder","fittoforel"),"@DocString(FMTAreaParser::vectormaptoFMTforest)"))
					.def("vectorfieldtoraster", &Parser::FMTAreaParser::vectorFieldToRaster, vectorfieldtoraster_overloads(bp::args("data_vectors", "tifpathandname", "resolution", "field", "fittoforel"), "@DocString(FMTAreaParser::vectorfieldtoraster)"))
					#ifdef FMTWITHOSI
						.def("getschemeneighbors",&Parser::FMTAreaParser::getSchemeNeighbors, getschemeneighbors_overloads(bp::args("operatingareaparameters", "themes", "data_vectors", "agefield", "areafield", "agefactor","areafractor","lockfield","minimal_area","buffersize"), "@DocString(FMTAreaParser::getschemeneighbors)"))
                        .def("getclusters",&Parser::FMTAreaParser::getClusters,getclusters_overloads(bp::args("operatingareaparameters", "themes", "data_vectors", "agefield", "areafield","maximaldistance","agefactor","areafractor","lockfield","minimal_area","buffersize"),"@DocString(FMTAreaParser::getclusters)"))
					#endif
				#endif
				.def("write", &Parser::FMTAreaParser::write,
					"@DocString(FMTAreaParser::write)");
	bp::class_<Parser::FMTLandscapeParser, bp::bases<Parser::FMTParser>>("FMTlandscapeparser", "@DocString(FMTLandscapeParser)")
                .def("read",&Parser::FMTLandscapeParser::read,
					"@DocString(FMTLandscapeParser::read)")
				#ifdef FMTWITHGDAL
					.def("readvectors", &Parser::FMTLandscapeParser::readVectors,
						"@DocString(FMTLandscapeParser::readvectors)")
					.def("readrasters",&Parser::FMTLandscapeParser::readRasters,
						"@DocString(FMTLandscapeParser::readrasters)")
				#endif
                .def("write",&Parser::FMTLandscapeParser::write,
					"@DocString(FMTLandscapeParser::write)");
	bp::class_<Parser::FMTActionParser, bp::bases<Parser::FMTParser>>("FMTactionparser", "@DocString(FMTActionParser)")
                .def("read",&Parser::FMTActionParser::read,
					"@DocString(FMTActionParser::read)")
                .def("write",&Parser::FMTActionParser::write,
					"@DocString(FMTActionParser::write)");
	bp::class_<Parser::FMTTransitionParser, bp::bases<Parser::FMTParser>>("FMTtransitionparser", "@DocString(FMTTransitionParser)")
                .def("read",&Parser::FMTTransitionParser::read,
					"@DocString(FMTTransitionParser::read)")
                .def("write",&Parser::FMTTransitionParser::write,
					"@DocString(FMTTransitionParser::write)")
				.def("readGCBM", &Parser::FMTTransitionParser::readGCBM,
					"@DocString(FMTTransitionParser::readGCBM)")
				.def("writeGCBM", &Parser::FMTTransitionParser::writeGCBM,
					"@DocString(FMTTransitionParser::writeGCBM)");
	bp::class_<Parser::FMTConstantParser, bp::bases<Parser::FMTParser>>("FMTconstantparser", "@DocString(FMTConstantParser)")
                .def("read",&Parser::FMTConstantParser::read,
					"@DocString(FMTConstantParser::read)")
                .def("write",&Parser::FMTConstantParser::write,
					"@DocString(FMTConstantParser::write)");
	bp::class_<Parser::FMTLifespanParser, bp::bases<Parser::FMTParser>>("FMTlifespanparser", "@DocString(FMTLifespanParser)")
                .def("read",&Parser::FMTLifespanParser::read,
					"@DocString(FMTLifespanParser::read)")
                .def("write",&Parser::FMTLifespanParser::write,
					"@DocString(FMTLifespanParser::write)");
	bp::class_<Parser::FMTYieldParser, bp::bases<Parser::FMTParser>>("FMTyieldparser", "@DocString(FMTYieldParser)")
                .def("read",&Parser::FMTYieldParser::read,
					"@DocString(FMTYieldParser::read)")
                .def("write",&Parser::FMTYieldParser::write,
					"@DocString(FMTYieldParser::write)");
	bp::class_<Parser::FMTOutputParser, bp::bases<Parser::FMTParser>>("FMToutputparser", "@DocString(FMTOutputParser)")
                .def("read",&Parser::FMTOutputParser::read,
					"@DocString(FMTOutputParser::read)")
				.def("addoutputs",&Parser::FMTOutputParser::addOutputs,addoutputs_overloads(bp::args("oldoutputs","themes","actions","ylds","constants","location","outputsname"),"@DocString(FMTOutputParser::addoutputs)"))
                .def("write",&Parser::FMTOutputParser::write,
					"@DocString(FMTOutputParser::write)");
	bp::class_<Parser::FMTOptimizationParser, bp::bases<Parser::FMTParser>>("FMToptimizationparser", "@DocString(FMTOptimizationParser)")
				.def("read", &Parser::FMTOptimizationParser::read,
					"@DocString(FMTOptimizationParser::read)")
				.def("write", &Parser::FMTOptimizationParser::write,
					"@DocString(FMTOptimizationParser::write)");
	bp::class_<Parser::FMTModelParser, bp::bases<Parser::FMTParser>>("FMTmodelparser", "@DocString(FMTModelParser)")
                .def("read",&Parser::FMTModelParser::read, readmodel_overloads(bp::args("con","lan","lif","are","yld","act","tr","out","opt"), "@DocString(FMTModel::read)"))
				.def("readproject", &Parser::FMTModelParser::readproject, readproject_overloads(bp::args("primary_location","scenarios","readarea","readoutputs","readoptimize"), "@DocString(FMTModel::readproject)"))
				.def("getconstraintsfromstring",&Parser::FMTModelParser::getConstraintsFromString, getconstraintsfromstring_overloads(bp::args("constraintstr", "model", "constants"), "@DocString(FMTModel::getconstraintsfromstring)"))
				.def("readschedules", &Parser::FMTModelParser::readSchedules,
					"@DocString(FMTModelParser::readSchedules)")
				#ifdef FMTWITHGDAL
				.def("writeresults", &Parser::FMTModelParser::writeResults,
					writeresults_overloads(bp::args("model", "theoutputs", "firstperiod", "lastperiod", "outlocation", "outputlevel", "gdaldrivername"),
						"@DocString(FMTModelParser::writeresults)"))
				#endif
                .def("write",&Parser::FMTModelParser::write,
					"@DocString(FMTModelParser::write)");
	bp::class_<Parser::FMTScheduleParser, bp::bases<Parser::FMTParser>>("FMTscheduleparser", "@DocString(FMTScheduleParser)")
                .def("read",&Parser::FMTScheduleParser::read,
					"@DocString(FMTScheduleParser::read)")
                .def("write",&Parser::FMTScheduleParser::write,
					"@DocString(FMTScheduleParser::write)");
    }

}

#endif // PYEXPORTParser_H_INCLUDED
