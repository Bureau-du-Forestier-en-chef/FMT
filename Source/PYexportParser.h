/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTParser_H_INCLUDED
#define PYEXPORTParser_H_INCLUDED

#include "FMTlandscapeparser.h"
#include "FMTconstantparser.h"
#include "FMTyieldparser.h"
#include "FMTareaparser.h"
#include "FMTactionparser.h"
#include "FMTtransitionparser.h"
#include "FMToutputparser.h"
#include "FMToptimizationparser.h"
#include "FMTscheduleparser.h"
#include "FMTmodelparser.h"
#include "boost/python.hpp"

namespace Python
{
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readrasters_overloads,readrasters, 3, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readvectors_overloads,readvectors, 4, 8)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getschemeneighbors_overloads, getschemeneighbors, 5, 10)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writeforest_overloads,writeforest, 5, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writedisturbances_overloads,writedisturbances, 5, 6)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readmodel_overloads, read, 8, 9)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readproject_overloads, readproject,1,5)



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

	bp::class_<Parser::FMTparser, bp::bases<Core::FMTobject>>("FMTparser", "@DocString(FMTparser)");

	define_pydict<std::string,std::string>();

	bp::class_<Parser::FMTareaparser, bp::bases<Parser::FMTparser>>("FMTareaparser", "@DocString(FMTareaparser)")
                .def("read", &Parser::FMTareaparser::read, "@DocString(FMTareaparser::read)")
				#ifdef FMTWITHGDAL
					.def("readvectors", &Parser::FMTareaparser::readvectors,readvectors_overloads(bp::args("themes", "data_vectors", "agefield", "areafield", "areafactor", "lockfield","minimalarea"), "@DocString(FMTareaparser::readvectors)"))
					.def("readrasters", &Parser::FMTareaparser::readrasters,readrasters_overloads(bp::args("themes","data_rasters","age","agefactor","areafactor","lock"), "@DocString(FMTareaparser::readrasters)"))
					.def("writeforest",&Parser::FMTareaparser::writeforest,writeforest_overloads(bp::args("for_layer", "themes", "data_rasters", "age", "lock", "mapping"), "@DocString(FMTareaparser::writeforest)"))
					.def("writedisturbances",&Parser::FMTareaparser::writedisturbances,writedisturbances_overloads(bp::args("location", "disturbances", "for_layer", "out_layer", "themes", "mapping"), "@DocString(FMTareaparser::writedisturbances)"))
					.def("writedisturbancessp", &Parser::FMTareaparser::writedisturbancessp, "@DocString(FMTareaparser::writedisturbances)")
					#ifdef FMTWITHOSI
						.def("getneighbors",&Parser::FMTareaparser::getschemeneighbors, getschemeneighbors_overloads(bp::args("operatingareaparameters", "themes", "data_vectors", "agefield", "areafield", "agefactor","areafractor","lockfield","minimal_area","buffersize"), "@DocString(FMTareaparser::getschemeneighbors)"))
					#endif
				#endif
				.def("write", &Parser::FMTareaparser::write,
					"@DocString(FMTareaparser::write)");
	bp::class_<Parser::FMTlandscapeparser, bp::bases<Parser::FMTparser>>("FMTlandscapeparser", "@DocString(FMTlandscapeparser)")
                .def("read",&Parser::FMTlandscapeparser::read,
					"@DocString(FMTlandscapeparser::read)")
				#ifdef FMTWITHGDAL
					.def("readvectors", &Parser::FMTlandscapeparser::readvectors,
						"@DocString(FMTlandscapeparser::readvectors)")
					.def("readrasters",&Parser::FMTlandscapeparser::readrasters,
						"@DocString(FMTlandscapeparser::readrasters)")
				#endif
                .def("write",&Parser::FMTlandscapeparser::write,
					"@DocString(FMTlandscapeparser::write)");
	bp::class_<Parser::FMTactionparser, bp::bases<Parser::FMTparser>>("FMTactionparser", "@DocString(FMTactionparser)")
                .def("read",&Parser::FMTactionparser::read,
					"@DocString(FMTactionparser::read)")
                .def("write",&Parser::FMTactionparser::write,
					"@DocString(FMTactionparser::write)");
	bp::class_<Parser::FMTtransitionparser, bp::bases<Parser::FMTparser>>("FMTtransitionparser", "@DocString(FMTtransitionparser)")
                .def("read",&Parser::FMTtransitionparser::read,
					"@DocString(FMTtransitionparser::read)")
                .def("write",&Parser::FMTtransitionparser::write,
					"@DocString(FMTtransitionparser::write)");
	bp::class_<Parser::FMTconstantparser, bp::bases<Parser::FMTparser>>("FMTconstantparser", "@DocString(FMTconstantparser)")
                .def("read",&Parser::FMTconstantparser::read,
					"@DocString(FMTconstantparser::read)")
                .def("write",&Parser::FMTconstantparser::write,
					"@DocString(FMTconstantparser::write)");
	bp::class_<Parser::FMTlifespanparser, bp::bases<Parser::FMTparser>>("FMTlifespanparser", "@DocString(FMTlifespanparser)")
                .def("read",&Parser::FMTlifespanparser::read,
					"@DocString(FMTlifespanparser::read)")
                .def("write",&Parser::FMTlifespanparser::write,
					"@DocString(FMTlifespanparser::write)");
	bp::class_<Parser::FMTyieldparser, bp::bases<Parser::FMTparser>>("FMTyieldparser", "@DocString(FMTyieldparser)")
                .def("read",&Parser::FMTyieldparser::read,
					"@DocString(FMTyieldparser::read)")
                .def("write",&Parser::FMTyieldparser::write,
					"@DocString(FMTyieldparser::write)");
	bp::class_<Parser::FMToutputparser, bp::bases<Parser::FMTparser>>("FMToutputparser", "@DocString(FMToutputparser)")
                .def("read",&Parser::FMToutputparser::read,
					"@DocString(FMToutputparser::read)")
                .def("write",&Parser::FMToutputparser::write,
					"@DocString(FMToutputparser::write)");
	bp::class_<Parser::FMToptimizationparser, bp::bases<Parser::FMTparser>>("FMToptimizationparser", "@DocString(FMToptimizationparser)")
				.def("read", &Parser::FMToptimizationparser::read,
					"@DocString(FMToptimizationparser::read)")
				.def("write", &Parser::FMToptimizationparser::write,
					"@DocString(FMToptimizationparser::write)");
	bp::class_<Parser::FMTmodelparser, bp::bases<Parser::FMTparser>>("FMTmodelparser", "@DocString(FMTmodelparser)")
                .def("read",&Parser::FMTmodelparser::read, readmodel_overloads(bp::args("con","lan","lif","are","yld","act","tr","out","opt"), "@DocString(FMTmodel::read)"))
				.def("readproject", &Parser::FMTmodelparser::readproject, readproject_overloads(bp::args("primary_location","scenarios","readarea","readoutputs","readoptimize"), "@DocString(FMTmodel::readproject)"))
				.def("readschedules", &Parser::FMTmodelparser::readschedules, 
					"@DocString(FMTmodelparser::readschedules)")
                .def("write",&Parser::FMTmodelparser::write,
					"@DocString(FMTmodelparser::write)");
	bp::class_<Parser::FMTscheduleparser, bp::bases<Parser::FMTparser>>("FMTscheduleparser", "@DocString(FMTscheduleparser)")
                .def("read",&Parser::FMTscheduleparser::read,
					"@DocString(FMTscheduleparser::read)")
                .def("write",&Parser::FMTscheduleparser::write,
					"@DocString(FMTscheduleparser::write)");
    }

}

#endif // PYEXPORTParser_H_INCLUDED
