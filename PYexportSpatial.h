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
    const char* py_FMTcoord_doc =
        " ``FMTcoordinate`` class.\n"
        "\n"
        "This class is used for coordinate hashing\n"
        "\n";
    bp::class_<Spatial::FMTcoordinate>("FMTcoordinate",py_FMTcoord_doc)
        .def(bp::init<unsigned int,unsigned int>())
        .def("__lt__",&Spatial::FMTcoordinate::operator <)
        .def("getx",&Spatial::FMTcoordinate::getx)
        .def("gety",&Spatial::FMTcoordinate::gety)
        .setattr("__hash__",&boost::pyhash<Spatial::FMTcoordinate>);


    define_FMTlayer<Core::FMTdevelopment>();
    const char* py_FMTforest_doc =
        " ``FMTforest`` class.\n"
        "\n"
        "Spatialy explicit forest definition\n"
        "\n";
	bp::class_<Spatial::FMTforest, bp::bases<Spatial::FMTlayer<Core::FMTdevelopment>>>("FMTforest",py_FMTforest_doc)
        .def(bp::init<Spatial::FMTforest>())
        .def("getarea",&Spatial::FMTforest::getarea)
        .def("grow",&Spatial::FMTforest::grow);
     define_pylist<Spatial::FMTforest>();

    const char* py_FMTdiststack_doc =
        " ``FMTdisturbancestack`` class.\n"
        "\n"
        "Spatialy explicit events locations\n"
        "\n";

	bp::class_<Spatial::FMTdisturbancestack>("FMTdisturbancestack",py_FMTdiststack_doc)
        .def("allow",&Spatial::FMTdisturbancestack::allow);


    const char* py_FMTspact_doc =
        " ``FMTspatialaction`` class.\n"
        "\n"
        "Spatialy explicit WS action\n"
        "\n";
	bp::class_<Spatial::FMTspatialaction, bp::bases<Core::FMTaction>>("FMTspatialaction",py_FMTspact_doc)
        .def(bp::init<Core::FMTaction>())
        .add_property("neighbors", bp::make_getter(&Spatial::FMTspatialaction::neighbors, bp::return_value_policy<bp::return_by_value>()),
                     make_setter(&Spatial::FMTspatialaction::neighbors, bp::return_value_policy<bp::return_by_value>()))
        .def_readwrite("green_up",&Spatial::FMTspatialaction::green_up)
        .def_readwrite("adjacency",&Spatial::FMTspatialaction::adjacency)
        .def_readwrite("minimal_size",&Spatial::FMTspatialaction::minimal_size)
        .def_readwrite("maximal_size",&Spatial::FMTspatialaction::maximal_size)
        .def_readwrite("neighbors_size",&Spatial::FMTspatialaction::neighbors_size)
        .def_readwrite("greenup_weight",&Spatial::FMTspatialaction::greenup_weight)
        .def_readwrite("adjacency_weight",&Spatial::FMTspatialaction::adjacency_weight)
        .def_readwrite("size_weight",&Spatial::FMTspatialaction::size_weight);
    define_pylist<Spatial::FMTspatialaction>();

	bp::class_<Spatial::FMTsaschedule, boost::noncopyable>("Abstract_FMTsaschedule", bp::no_init);

    const char* py_FMTexposchedule_doc =
        " ``FMTlinearschedule`` class.\n"
        "\n"
        "FMTlinearschedule(double cooling_rate)\n"
        "Simulated annealing cooling schedule\n"
        "\n";

	bp::class_<Spatial::FMTexponentialschedule, bp::bases<Spatial::FMTsaschedule>>("FMTexponentialschedule",py_FMTexposchedule_doc)
        .def(bp::init<double>());

    const char* py_FMTsasolution_doc =
        " ``FMTsasolution`` class.\n"
        "\n"
        "FMTsasolution\n"
        "\n";

	bp::enum_<Spatial::FMTsamovetype>("FMTsamovetype")
		.value("shotgun", Spatial::FMTsamovetype::shotgun)
		.value("cluster", Spatial::FMTsamovetype::cluster);

    define_FMTlayer<Graph::FMTgraph>();
    bp::class_<Spatial::FMTsasolution, bp::bases<Spatial::FMTlayer<Graph::FMTgraph>>>("FMTsasolution",py_FMTsasolution_doc, bp::no_init)
        .def("get_stats",&Spatial::FMTsasolution::getsolution_stats)
        .def("getobjfvalue",&Spatial::FMTsasolution::getobjfvalue)
        .def("get_forest_at_period",&Spatial::FMTsasolution::getforestperiod);

    define_pylist<Spatial::FMTsasolution>();

    }


#endif // PYEXPORTSPATIAL_H_INCLUDED
