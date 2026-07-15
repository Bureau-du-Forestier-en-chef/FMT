/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTSPATIAL_H_INCLUDED
#define PYEXPORTSPATIAL_H_INCLUDED

#include "FMTcoordinate.h"
#include "FMTforest.h"
#include "FMTSpatialSchedule.h"
//#include "FMTspatialaction.h"
#include "FMTeventcontainer.h"
//#include "FMTsaschedule.h"
#include "FMTgraph.hpp"
//#include "FMTsasolution.h"
#include "boost/python.hpp"

namespace Python 
{
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getforestperiod_overloads, getForestPeriod, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getconstraintevaluation_overloads,getConstraintEvaluation,2,2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getobjectivevalue_overloads,getObjectiveValue,2,2)
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
        .def("getx",&Spatial::FMTcoordinate::getX,
			"@DocString(FMTcoordinate::getx)")
        .def("gety",&Spatial::FMTcoordinate::getY,
			"@DocString(FMTcoordinate::gety)")
        .def("getSpatialCoordinate",&Spatial::FMTcoordinate::getSpatialCoordinate,
			"@DocString(FMTcoordinate::getSpatialCoordinate)")
        .setattr("__hash__",&boost::pyHash<Spatial::FMTcoordinate>);

    define_FMTlayer<Core::FMTdevelopment>();
    //Mainly to iter over FMTforest
    definePyPair<Spatial::FMTcoordinate,Core::FMTdevelopment>();
    definePyPair<Spatial::FMTcoordinate const,Core::FMTdevelopment>();
    definePyPair<Spatial::FMTcoordinate,Graph::FMTlinegraph>();
    definePyPair<Spatial::FMTcoordinate const,Graph::FMTlinegraph>();



	bp::class_<Spatial::FMTforest, bp::bases<Spatial::FMTlayer<Core::FMTdevelopment>>>("FMTforest", "@DocString(FMTforest)")
        .def(bp::init<Spatial::FMTforest>())
        .def("getarea",&Spatial::FMTforest::getArea,
			"@DocString(FMTforest::getarea)")
        .def("grow",&Spatial::FMTforest::grow,
			"@DocString(FMTforest::grow)")
        .def("getage",&Spatial::FMTforest::getAge,
			"@DocString(FMTforest::getage)")
         .def("setperiod",&Spatial::FMTforest::setPeriod,
			"@DocString(FMTforest::setperiod)");


     definePyList<Spatial::FMTforest>();

	bp::class_<Spatial::FMTeventcontainer>("FMTeventcontainer", "@DocString(FMTeventcontainer)")
		.def(bp::init<Spatial::FMTeventcontainer>());

	definePyList<Spatial::FMTeventcontainer>();

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
    definePyList<Spatial::FMTspatialaction>();*/

    
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
        .def("get_forest_at_period",&Spatial::FMTsasolution::getForestPeriod, "@DocString(FMTsasolution::getForestPeriod)")
		.def("get_graphs_outputs", &Spatial::FMTsasolution::getgraphsoutputs);

    definePyList<Spatial::FMTsasolution>();
    */

    define_FMTlayer<Graph::FMTlinegraph>();

	bp::class_<Spatial::FMTSpatialSchedule, bp::bases<Spatial::FMTlayer<Graph::FMTlinegraph>>>("FMTSpatialSchedule", "@DocString(FMTSpatialSchedule)")
		.def(bp::init<Spatial::FMTSpatialSchedule>())
		.def("getforestperiod", &Spatial::FMTSpatialSchedule::getForestPeriod, getforestperiod_overloads(bp::args("period","periodstart"),"@DocString(FMTSpatialSchedule::getforestperiod)"))
        .def("getoutputbycoordinate", &Spatial::FMTSpatialSchedule::getOutputByCoordinate, "@DocString(FMTSpatialSchedule::getoutputbycoordinate)")
        .def("getbindingactions", &Spatial::FMTSpatialSchedule::getBindingActions, "@DocString(FMTSpatialSchedule::getbindingactions)")
        .def("operatecoord", &Spatial::FMTSpatialSchedule::operateCoord, "@DocString(FMTSpatialSchedule::operatecoord)")
        .def("getconstraintevaluation", &Spatial::FMTSpatialSchedule::getConstraintEvaluation, getconstraintevaluation_overloads(bp::args("constraint","model"),"@DocString(FMTSpatialSchedule::getconstraintevaluation)"))
        .def("grow", &Spatial::FMTSpatialSchedule::grow, "@DocString(FMTSpatialSchedule::grow)");
	definePyList<Spatial::FMTSpatialSchedule>();

    bp::class_<Spatial::FMTbindingspatialaction>("FMTbindingspatialaction", "@DocString(FMTbindingspatialaction)")
        .def("getminimaladjacency", &Spatial::FMTbindingspatialaction::getMinimalAdjacency,boost::python::return_value_policy<boost::python::copy_const_reference>(),"@DocString(FMTSpatialSchedule::getminimaladjacency)")
        .def("getminimalsize",&Spatial::FMTbindingspatialaction::getMinimalSize,boost::python::return_value_policy<boost::python::copy_const_reference>(),"@DocString(FMTSpatialSchedule::getminimalsize)")
        .def("getmaximalsize",&Spatial::FMTbindingspatialaction::getMaximalSize,boost::python::return_value_policy<boost::python::copy_const_reference>(), "@DocString(FMTSpatialSchedule::getmaximalsize)");

	definePyList<Spatial::FMTbindingspatialaction>();

    }
}

#endif // PYEXPORTSPATIAL_H_INCLUDED
