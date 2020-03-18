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

#ifndef PYEXPORTSPATIAL_H_INCLUDED
#define PYEXPORTSPATIAL_H_INCLUDED

#include "FMTcoordinate.h"
#include "FMTforest.h"
#include "FMTdisturbancestack.h"
#include "FMTspatialaction.h"
#include "FMTsaschedule.h"
#include "FMTgraph.h"
#include "FMTsasolution.h"
#include "boost/python.hpp"

namespace Python 
{

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
        .setattr("__hash__",&boost::pyhash<Spatial::FMTcoordinate>);

    define_FMTlayer<Core::FMTdevelopment>();


	bp::class_<Spatial::FMTforest, bp::bases<Spatial::FMTlayer<Core::FMTdevelopment>>>("FMTforest", "@DocString(FMTforest)")
        .def(bp::init<Spatial::FMTforest>())
        .def("getarea",&Spatial::FMTforest::getarea,
			"@DocString(FMTforest::getarea)")
        .def("grow",&Spatial::FMTforest::grow,
			"@DocString(FMTforest::grow)");


     define_pylist<Spatial::FMTforest>();


	bp::class_<Spatial::FMTdisturbancestack>("FMTdisturbancestack", "@DocString(FMTdisturbancestack)")
        .def("allow",&Spatial::FMTdisturbancestack::allow,
			"@DocString(FMTdisturbancestack::allow)");


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
    define_pylist<Spatial::FMTspatialaction>();

	bp::class_<Spatial::FMTsaschedule, boost::noncopyable>("Abstract_FMTsaschedule","@DocString(FMTsaschedule)", bp::no_init);


	bp::class_<Spatial::FMTexponentialschedule, bp::bases<Spatial::FMTsaschedule>>("FMTexponentialschedule", "@DocString(FMTexponentialschedule)")
        .def(bp::init<double>());

	bp::enum_<Spatial::FMTsamovetype>("FMTsamovetype")
		.value("shotgun", Spatial::FMTsamovetype::shotgun)
		.value("cluster", Spatial::FMTsamovetype::cluster);

    define_FMTlayer<Graph::FMTgraph>();

    bp::class_<Spatial::FMTsasolution, bp::bases<Spatial::FMTlayer<Graph::FMTgraph>>>("FMTsasolution", "@DocString(FMTsasolution)", bp::no_init)
        .def("get_stats",&Spatial::FMTsasolution::getsolution_stats, "@DocString(FMTsasolution::getsolution_stats)")
        .def("getobjfvalue",&Spatial::FMTsasolution::getobjfvalue, "@DocString(FMTsasolution::getobjfvalue)")
        .def("get_forest_at_period",&Spatial::FMTsasolution::getforestperiod, "@DocString(FMTsasolution::getforestperiod)");

    define_pylist<Spatial::FMTsasolution>();

    }
}

#endif // PYEXPORTSPATIAL_H_INCLUDED
