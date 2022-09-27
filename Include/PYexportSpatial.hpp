/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTSPATIAL_H_INCLUDED
#define PYEXPORTSPATIAL_H_INCLUDED

#include "FMTcoordinate.hpp"
#include "FMTforest.hpp"
//#include "FMTspatialschedule.hpp"
//#include "FMTspatialaction.hpp"
#include "FMTeventcontainer.hpp"
//#include "FMTsaschedule.hpp"
#include "FMTgraph.hpp"
//#include "FMTsasolution.hpp"
#include "boost/python.hpp"

namespace Python 
{
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getforestperiod_overloads, getforestperiod, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getconstraintevaluation_overloads,getconstraintevaluation,2,2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getobjectivevalue_overloads,getobjectivevalue,2,2)
void exportSpatial()
    {
    namespace bp = boost::python;
    bp::object SpatialModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.Spatial"))));
    bp::scope().attr("Spatial") = SpatialModule;
    bp::scope Spatial_scope = SpatialModule;
    bp::scope().attr("__doc__") = ".. module:: Spatial\n"
    "\n"
    "   :platform: Unix, Windows\n"
    "   :synopsis: Spatial module used for Simulation.\n"
    "\n";

    bp::class_<Spatial::FMTcoordinate>("FMTcoordinate", "@DocString(FMTcoordinate)")
        .def(bp::init<unsigned int,unsigned int>())
        .def("__lt__",&Spatial::FMTcoordinate::operator <,
			"@DocString(FMTcoordinate::operator<)")
        .def("getx",&Spatial::FMTcoordinate::getx,
			"@DocString(FMTcoordinate::getx)")
        .def("gety",&Spatial::FMTcoordinate::gety,
			"@DocString(FMTcoordinate::gety)")
        .def("getSpatialCoordinate",&Spatial::FMTcoordinate::getSpatialCoordinate,
			"@DocString(FMTcoordinate::getSpatialCoordinate)")
        .setattr("__hash__",&boost::pyhash<Spatial::FMTcoordinate>);

    define_FMTlayer<Core::FMTdevelopment>();
    //Mainly to iter over FMTforest
    define_pypair<Spatial::FMTcoordinate,Core::FMTdevelopment>();
    define_pypair<Spatial::FMTcoordinate const,Core::FMTdevelopment>();
    define_pypair<Spatial::FMTcoordinate,Graph::FMTlinegraph>();
    define_pypair<Spatial::FMTcoordinate const,Graph::FMTlinegraph>();



	bp::class_<Spatial::FMTforest, bp::bases<Spatial::FMTlayer<Core::FMTdevelopment>>>("FMTforest", "@DocString(FMTforest)")
        .def(bp::init<Spatial::FMTforest>())
        .def("getarea",&Spatial::FMTforest::getarea,
			"@DocString(FMTforest::getarea)")
        .def("grow",&Spatial::FMTforest::grow,
			"@DocString(FMTforest::grow)")
        .def("getage",&Spatial::FMTforest::getage,
			"@DocString(FMTforest::getage)");


     define_pylist<Spatial::FMTforest>();

	bp::class_<Spatial::FMTeventcontainer>("FMTeventcontainer", "@DocString(FMTeventcontainer)")
		.def(bp::init<Spatial::FMTeventcontainer>());

	define_pylist<Spatial::FMTeventcontainer>();

    /*
	bp::class_<Spatial::FMTspatialaction, bp::bases<Core::FMTaction>>("FMTspatialaction", "@DocString(FMTspatialaction)")
        .def(bp::init<Core::FMTaction>())
        .add_property("neighbors", bp::make_getter(&Spatial::FMTspatialaction::neighbors, bp::return_value_policy<bp::return_by_value>()),
                     make_setter(&Spatial::FMTspatialaction::neighbors, bp::return_value_policy<bp::return_by_value>()))
        .def_readwrite("green_up",&Spatial::FMTspatialaction::green_up,
			"@DocString(FMTspatialaction::green_up)")
        .def_readwrite("adjacency",&Spatial::FMTspatialaction::adjacency,
			"@DocString(FMTspatialaction::adjacency)")
        .def_readwrite("minimal_size",&Spatial::FMTspatialaction::minimal_size,
			"@DocString(FMTspatialaction::minimal_size)")
        .def_readwrite("maximal_size",&Spatial::FMTspatialaction::maximal_size,
			"@DocString(FMTspatialaction::maximal_size)")
        .def_readwrite("neighbors_size",&Spatial::FMTspatialaction::neighbors_size,
			"@DocString(FMTspatialaction::neighbors_size)")
        .def_readwrite("greenup_weight",&Spatial::FMTspatialaction::greenup_weight,
			"@DocString(FMTspatialaction::greenup_weight)")
        .def_readwrite("adjacency_weight",&Spatial::FMTspatialaction::adjacency_weight,
			"@DocString(FMTspatialaction::adjacency_weight)")
        .def_readwrite("size_weight",&Spatial::FMTspatialaction::size_weight,
			"@DocString(FMTspatialaction::size_weight)");
    define_pylist<Spatial::FMTspatialaction>();*/

    
	//bp::class_<Spatial::FMTsaschedule, boost::noncopyable>("Abstract_FMTsaschedule","@DocString(FMTsaschedule)", bp::no_init);


	//bp::class_<Spatial::FMTexponentialschedule, bp::bases<Spatial::FMTsaschedule>>("FMTexponentialschedule", "@DocString(FMTexponentialschedule)")
     //   .def(bp::init<double>());

	/*bp::enum_<Spatial::FMTsamovetype>("FMTsamovetype")
		.value("shotgun", Spatial::FMTsamovetype::shotgun)
		.value("cluster", Spatial::FMTsamovetype::cluster)
		.value("opt1", Spatial::FMTsamovetype::opt1);

    bp::class_<Spatial::FMTsasolution, bp::bases<Spatial::FMTlayer<Graph::FMTlinegraph>>>("FMTsasolution", "@DocString(FMTsasolution)")
        .def("get_stats",&Spatial::FMTsasolution::getsolution_stats, "@DocString(FMTsasolution::getsolution_stats)")
        .def("getobjfvalue",&Spatial::FMTsasolution::getobjfvalue, "@DocString(FMTsasolution::getobjfvalue)")
        .def("get_forest_at_period",&Spatial::FMTsasolution::getforestperiod, "@DocString(FMTsasolution::getforestperiod)")
		.def("get_graphs_outputs", &Spatial::FMTsasolution::getgraphsoutputs);

    define_pylist<Spatial::FMTsasolution>();
    */

    define_FMTlayer<Graph::FMTlinegraph>();

	bp::class_<Spatial::FMTspatialschedule, bp::bases<Spatial::FMTlayer<Graph::FMTlinegraph>>>("FMTspatialschedule", "@DocString(FMTspatialschedule)")
		.def(bp::init<Spatial::FMTspatialschedule>())
		.def("getforestperiod", &Spatial::FMTspatialschedule::getforestperiod, getforestperiod_overloads(bp::args("period","periodstart"),"@DocString(FMTspatialschedule::getforestperiod)"))
        .def("getoutputbycoordinate", &Spatial::FMTspatialschedule::getoutputbycoordinate, "@DocString(FMTspatialschedule::getoutputbycoordinate)")
        .def("getbindingactions", &Spatial::FMTspatialschedule::getbindingactions, "@DocString(FMTspatialschedule::getbindingactions)")
        .def("operatecoord", &Spatial::FMTspatialschedule::operatecoord, "@DocString(FMTspatialschedule::operatecoord)")
        .def("getconstraintevaluation", &Spatial::FMTspatialschedule::getconstraintevaluation, getconstraintevaluation_overloads(bp::args("constraint","model"),"@DocString(FMTspatialschedule::getconstraintevaluation)"))
        .def("grow", &Spatial::FMTspatialschedule::grow, "@DocString(FMTspatialschedule::grow)");
	define_pylist<Spatial::FMTspatialschedule>();

    bp::class_<Spatial::FMTbindingspatialaction>("FMTbindingspatialaction", "@DocString(FMTbindingspatialaction)")
        .def("getminimaladjacency", &Spatial::FMTbindingspatialaction::getminimaladjacency,boost::python::return_value_policy<boost::python::copy_const_reference>(),"@DocString(FMTspatialschedule::getminimaladjacency)")
        .def("getminimalsize",&Spatial::FMTbindingspatialaction::getminimalsize,boost::python::return_value_policy<boost::python::copy_const_reference>(),"@DocString(FMTspatialschedule::getminimalsize)")
        .def("getmaximalsize",&Spatial::FMTbindingspatialaction::getmaximalsize,boost::python::return_value_policy<boost::python::copy_const_reference>(), "@DocString(FMTspatialschedule::getmaximalsize)");

	define_pylist<Spatial::FMTbindingspatialaction>();

    }
}

#endif // PYEXPORTSPATIAL_H_INCLUDED
