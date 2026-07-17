/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTSPATIAL_H_INCLUDED
#define PYEXPORTSPATIAL_H_INCLUDED

#include "FMTCoordinate.h"
#include "FMTForest.h"
#include "FMTSpatialSchedule.h"
//#include "FMTspatialaction.h"
#include "FMTEventContainer.h"
//#include "FMTSaSchedule.h"
#include "FMTGraph.hpp"
//#include "FMTSaSolution.h"
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

    bp::class_<Spatial::FMTCoordinate>("FMTcoordinate", "@DocString(FMTCoordinate)")
        .def(bp::init<unsigned int,unsigned int>())
        .def("__lt__",&Spatial::FMTCoordinate::operator <,
			"@DocString(FMTCoordinate::operator<)")
        .def("getx",&Spatial::FMTCoordinate::getX,
			"@DocString(FMTCoordinate::getx)")
        .def("gety",&Spatial::FMTCoordinate::getY,
			"@DocString(FMTCoordinate::gety)")
        .def("getSpatialCoordinate",&Spatial::FMTCoordinate::getSpatialCoordinate,
			"@DocString(FMTCoordinate::getSpatialCoordinate)")
        .setattr("__hash__",&boost::pyHash<Spatial::FMTCoordinate>);

    define_FMTlayer<Core::FMTDevelopment>();
    //Mainly to iter over FMTForest
    definePyPair<Spatial::FMTCoordinate,Core::FMTDevelopment>();
    definePyPair<Spatial::FMTCoordinate const,Core::FMTDevelopment>();
    definePyPair<Spatial::FMTCoordinate,Graph::FMTLineGraph>();
    definePyPair<Spatial::FMTCoordinate const,Graph::FMTLineGraph>();



	bp::class_<Spatial::FMTForest, bp::bases<Spatial::FMTLayer<Core::FMTDevelopment>>>("FMTforest", "@DocString(FMTForest)")
        .def(bp::init<Spatial::FMTForest>())
        .def("getarea",&Spatial::FMTForest::getArea,
			"@DocString(FMTForest::getarea)")
        .def("grow",&Spatial::FMTForest::grow,
			"@DocString(FMTForest::grow)")
        .def("getage",&Spatial::FMTForest::getAge,
			"@DocString(FMTForest::getage)")
         .def("setperiod",&Spatial::FMTForest::setPeriod,
			"@DocString(FMTForest::setperiod)");


     definePyList<Spatial::FMTForest>();

	bp::class_<Spatial::FMTEventContainer>("FMTeventcontainer", "@DocString(FMTEventContainer)")
		.def(bp::init<Spatial::FMTEventContainer>());

	definePyList<Spatial::FMTEventContainer>();

    /*
	bp::class_<Spatial::FMTspatialaction, bp::bases<Core::FMTAction>>("FMTspatialaction", "@DocString(FMTspatialaction)")
        .def(bp::init<Core::FMTAction>())
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

    
	//bp::class_<Spatial::FMTSaSchedule, boost::noncopyable>("Abstract_FMTsaschedule","@DocString(FMTSaSchedule)", bp::no_init);


	//bp::class_<Spatial::FMTExponentialSchedule, bp::bases<Spatial::FMTSaSchedule>>("FMTexponentialschedule", "@DocString(FMTExponentialSchedule)")
     //   .def(bp::init<double>());

	/*bp::enum_<Spatial::FMTsamovetype>("FMTsamovetype")
		.value("shotgun", Spatial::FMTsamovetype::shotgun)
		.value("cluster", Spatial::FMTsamovetype::cluster)
		.value("opt1", Spatial::FMTsamovetype::opt1);

    bp::class_<Spatial::FMTSaSolution, bp::bases<Spatial::FMTLayer<Graph::FMTLineGraph>>>("FMTsasolution", "@DocString(FMTSaSolution)")
        .def("get_stats",&Spatial::FMTSaSolution::getsolution_stats, "@DocString(FMTSaSolution::getsolution_stats)")
        .def("getobjfvalue",&Spatial::FMTSaSolution::getobjfvalue, "@DocString(FMTSaSolution::getobjfvalue)")
        .def("get_forest_at_period",&Spatial::FMTSaSolution::getForestPeriod, "@DocString(FMTSaSolution::getForestPeriod)")
		.def("get_graphs_outputs", &Spatial::FMTSaSolution::getgraphsoutputs);

    definePyList<Spatial::FMTSaSolution>();
    */

    define_FMTlayer<Graph::FMTLineGraph>();

	bp::class_<Spatial::FMTSpatialSchedule, bp::bases<Spatial::FMTLayer<Graph::FMTLineGraph>>>("FMTSpatialSchedule", "@DocString(FMTSpatialSchedule)")
		.def(bp::init<Spatial::FMTSpatialSchedule>())
		.def("getforestperiod", &Spatial::FMTSpatialSchedule::getForestPeriod, getforestperiod_overloads(bp::args("period","periodstart"),"@DocString(FMTSpatialSchedule::getforestperiod)"))
        .def("getoutputbycoordinate", &Spatial::FMTSpatialSchedule::getOutputByCoordinate, "@DocString(FMTSpatialSchedule::getoutputbycoordinate)")
        .def("getbindingactions", &Spatial::FMTSpatialSchedule::getBindingActions, "@DocString(FMTSpatialSchedule::getbindingactions)")
        .def("operatecoord", &Spatial::FMTSpatialSchedule::operateCoord, "@DocString(FMTSpatialSchedule::operatecoord)")
        .def("getconstraintevaluation", &Spatial::FMTSpatialSchedule::getConstraintEvaluation, getconstraintevaluation_overloads(bp::args("constraint","model"),"@DocString(FMTSpatialSchedule::getconstraintevaluation)"))
        .def("grow", &Spatial::FMTSpatialSchedule::grow, "@DocString(FMTSpatialSchedule::grow)");
	definePyList<Spatial::FMTSpatialSchedule>();

    bp::class_<Spatial::FMTBindingSpatialAction>("FMTbindingspatialaction", "@DocString(FMTBindingSpatialAction)")
        .def("getminimaladjacency", &Spatial::FMTBindingSpatialAction::getMinimalAdjacency,boost::python::return_value_policy<boost::python::copy_const_reference>(),"@DocString(FMTSpatialSchedule::getminimaladjacency)")
        .def("getminimalsize",&Spatial::FMTBindingSpatialAction::getMinimalSize,boost::python::return_value_policy<boost::python::copy_const_reference>(),"@DocString(FMTSpatialSchedule::getminimalsize)")
        .def("getmaximalsize",&Spatial::FMTBindingSpatialAction::getMaximalSize,boost::python::return_value_policy<boost::python::copy_const_reference>(), "@DocString(FMTSpatialSchedule::getmaximalsize)");

	definePyList<Spatial::FMTBindingSpatialAction>();

    }
}

#endif // PYEXPORTSPATIAL_H_INCLUDED
