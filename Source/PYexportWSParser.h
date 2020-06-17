/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTWSPARSER_H_INCLUDED
#define PYEXPORTWSPARSER_H_INCLUDED


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readrasters_overloads,readrasters, 3, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readvectors_overloads,readvectors, 4, 8)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getneighbors_overloads, getneighbors, 5, 10)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writeforest_overloads,writeforest, 5, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writedisturbances_overloads,writedisturbances, 5, 6)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readmodel_overloads, read, 8, 9)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readproject_overloads, readproject,1,5)

void exportWSParser()
    {
    namespace bp = boost::python;
    bp::object WSParserModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.WSParser"))));
    bp::scope().attr("WSParser") = WSParserModule;
    bp::scope WSParser_scope = WSParserModule;
    bp::scope().attr("__doc__") = ".. module:: WSParser\n"
    "\n"
    "   :platform: Unix, Windows\n"
    "   :synopsis: Module used to import Woodstock models.\n"
    "\n";

	bp::class_<WSParser::FMTparser, bp::bases<Core::FMTobject>>("FMTparser");

	define_pydict<std::string,std::string>();

	bp::class_<WSParser::FMTareaparser, bp::bases<WSParser::FMTparser>>("FMTareaparser")
                .def("read", &WSParser::FMTareaparser::read)
				#ifdef FMTWITHGDAL
					.def("readvectors", &WSParser::FMTareaparser::readvectors,readvectors_overloads())
					.def("readrasters", &WSParser::FMTareaparser::readrasters,readrasters_overloads())
					.def("writeforest",&WSParser::FMTareaparser::writeforest,writeforest_overloads())
					.def("writedisturbances",&WSParser::FMTareaparser::writedisturbances,writedisturbances_overloads())
					.def("getneighbors",&WSParser::FMTareaparser::getneighbors, getneighbors_overloads())
				#endif
				.def("write", &WSParser::FMTareaparser::write);
	bp::class_<WSParser::FMTlandscapeparser, bp::bases<WSParser::FMTparser>>("FMTlandscapeparser")
                .def("read",&WSParser::FMTlandscapeparser::read)
				#ifdef FMTWITHGDAL
					.def("readvectors", &WSParser::FMTlandscapeparser::readvectors)
					.def("readrasters",&WSParser::FMTlandscapeparser::readrasters)
				#endif
                .def("write",&WSParser::FMTlandscapeparser::write);
	bp::class_<WSParser::FMTactionparser, bp::bases<WSParser::FMTparser>>("FMTactionparser")
                .def("read",&WSParser::FMTactionparser::read)
                .def("write",&WSParser::FMTactionparser::write);
	bp::class_<WSParser::FMTtransitionparser, bp::bases<WSParser::FMTparser>>("FMTtransitionparser")
                .def("read",&WSParser::FMTtransitionparser::read)
                .def("write",&WSParser::FMTtransitionparser::write);
	bp::class_<WSParser::FMTconstantparser, bp::bases<WSParser::FMTparser>>("FMTconstantparser")
                .def("read",&WSParser::FMTconstantparser::read)
                .def("write",&WSParser::FMTconstantparser::write);
	bp::class_<WSParser::FMTlifespanparser, bp::bases<WSParser::FMTparser>>("FMTlifespanparser")
                .def("read",&WSParser::FMTlifespanparser::read)
                .def("write",&WSParser::FMTlifespanparser::write);
	bp::class_<WSParser::FMTyieldparser, bp::bases<WSParser::FMTparser>>("FMTyieldparser")
                .def("read",&WSParser::FMTyieldparser::read)
                .def("write",&WSParser::FMTyieldparser::write);
	bp::class_<WSParser::FMToutputparser, bp::bases<WSParser::FMTparser>>("FMToutputparser")
                .def("read",&WSParser::FMToutputparser::read)
                .def("write",&WSParser::FMToutputparser::write);
	bp::class_<WSParser::FMTmodelparser, bp::bases<WSParser::FMTparser>>("FMTmodelparser")
                .def("read",&WSParser::FMTmodelparser::read, readmodel_overloads())
				.def("readproject", &WSParser::FMTmodelparser::readproject, readproject_overloads())
				.def("readschedules", &WSParser::FMTmodelparser::readschedules)
                .def("write",&WSParser::FMTmodelparser::write);
	bp::class_<WSParser::FMTscheduleparser, bp::bases<WSParser::FMTparser>>("FMTscheduleparser")
                .def("read",&WSParser::FMTscheduleparser::read)
                .def("write",&WSParser::FMTscheduleparser::write);
    }


#endif // PYEXPORTWSPARSER_H_INCLUDED
