#ifndef PYEXPORTWSPARSER_H_INCLUDED
#define PYEXPORTWSPARSER_H_INCLUDED


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readrasters_overloads,readrasters, 3, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readvectors_overloads,readvectors, 4, 8)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writeforest_overloads,writeforest, 5, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(writedisturbances_overloads,writedisturbances, 5, 6)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readmodel_overloads, read, 8, 9)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(readproject_overloads, readproject,1,2)

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

    class_<WSParser::FMTparser,bases<Exception::FMTobject>>("FMTparser");

	define_pydict<string,string>();

    class_<WSParser::FMTareaparser,bases<WSParser::FMTparser>>("FMTareaparser")
                .def("read", &WSParser::FMTareaparser::read)
                .def("readvectors", &WSParser::FMTareaparser::readvectors,readvectors_overloads())
                .def("readrasters", &WSParser::FMTareaparser::readrasters,readrasters_overloads())
                .def("writeforest",&WSParser::FMTareaparser::writeforest,writeforest_overloads())
                .def("writedisturbances",&WSParser::FMTareaparser::writedisturbances,writedisturbances_overloads())
                .def("write", &WSParser::FMTareaparser::write);
            class_<WSParser::FMTlandscapeparser,bases<WSParser::FMTparser>>("FMTlandscapeparser")
                .def("read",&WSParser::FMTlandscapeparser::read)
                .def("readvectors", &WSParser::FMTlandscapeparser::readvectors)
                .def("readrasters",&WSParser::FMTlandscapeparser::readrasters)
                .def("write",&WSParser::FMTlandscapeparser::write);
            class_<WSParser::FMTactionparser,bases<WSParser::FMTparser>>("FMTactionparser")
                .def("read",&WSParser::FMTactionparser::read)
                .def("write",&WSParser::FMTactionparser::write);
            class_<WSParser::FMTtransitionparser,bases<WSParser::FMTparser>>("FMTtransitionparser")
                .def("read",&WSParser::FMTtransitionparser::read)
                .def("write",&WSParser::FMTtransitionparser::write);
            class_<WSParser::FMTconstantparser,bases<WSParser::FMTparser>>("FMTconstantparser")
                .def("read",&WSParser::FMTconstantparser::read)
                .def("write",&WSParser::FMTconstantparser::write);
            class_<WSParser::FMTlifespanparser,bases<WSParser::FMTparser>>("FMTlifespanparser")
                .def("read",&WSParser::FMTlifespanparser::read)
                .def("write",&WSParser::FMTlifespanparser::write);
            class_<WSParser::FMTyieldparser,bases<WSParser::FMTparser>>("FMTyieldparser")
                .def("read",&WSParser::FMTyieldparser::read)
                .def("write",&WSParser::FMTyieldparser::write);
            class_<WSParser::FMToutputparser,bases<WSParser::FMTparser>>("FMToutputparser")
                .def("read",&WSParser::FMToutputparser::read)
                .def("write",&WSParser::FMToutputparser::write);
            class_<WSParser::FMTmodelparser,bases<WSParser::FMTparser>>("FMTmodelparser")
                .def("read",&WSParser::FMTmodelparser::read, readmodel_overloads())
				.def("readproject", &WSParser::FMTmodelparser::readproject, readproject_overloads())
				.def("readschedules", &WSParser::FMTmodelparser::readschedules)
                .def("write",&WSParser::FMTmodelparser::write);
            class_<WSParser::FMTscheduleparser,bases<WSParser::FMTparser>>("FMTscheduleparser")
                .def("read",&WSParser::FMTscheduleparser::read)
                .def("write",&WSParser::FMTscheduleparser::write);
			/*class_<WSParser::FMTlpmodelparser, bases<WSParser::FMTmodelparser>>("FMTlpmodelparser")
				.def("read", &WSParser::FMTlpmodelparser::read<Graph::FMTsolverinterface::CLP>);//<Graph::FMTsolverinterface::CLP>*/
    }

/*template <FMTsolverinterface  T>
void define_FMTgraph()
{
	const char* py_FMTgraph =
		" ``FMTgraph`` class.\n"
		"\n"
		"This class is used for M3 graph representation\n"
		"\n";
	class_<Graph::FMTgraph<T>>("FMTgraph", py_FMTgraph);
}*/


#endif // PYEXPORTWSPARSER_H_INCLUDED
