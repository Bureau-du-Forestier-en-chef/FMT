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
    class_<Spatial::FMTcoordinate>("FMTcoordinate",py_FMTcoord_doc)
        .def(init<unsigned int,unsigned int>())
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
    class_<Spatial::FMTforest,bases<FMTlayer<FMTdevelopment>>>("FMTforest",py_FMTforest_doc)
        .def(init<Spatial::FMTforest>())
        .def("getarea",&Spatial::FMTforest::getarea)
        .def("grow",&Spatial::FMTforest::grow);
     define_pylist<Spatial::FMTforest>();

    const char* py_FMTdiststack_doc =
        " ``FMTdisturbancestack`` class.\n"
        "\n"
        "Spatialy explicit events locations\n"
        "\n";

    class_<Spatial::FMTdisturbancestack>("FMTdisturbancestack",py_FMTdiststack_doc)
        .def("allow",&Spatial::FMTdisturbancestack::allow);


    const char* py_FMTspact_doc =
        " ``FMTspatialaction`` class.\n"
        "\n"
        "Spatialy explicit WS action\n"
        "\n";
    class_<Spatial::FMTspatialaction,bases<FMTaction>>("FMTspatialaction",py_FMTspact_doc)
        .def(init<Core::FMTaction>())
        .add_property("neighbors", make_getter(&Spatial::FMTspatialaction::neighbors, return_value_policy<return_by_value>()),
                     make_setter(&Spatial::FMTspatialaction::neighbors, return_value_policy<return_by_value>()))
        .def_readwrite("green_up",&Spatial::FMTspatialaction::green_up)
        .def_readwrite("adjacency",&Spatial::FMTspatialaction::adjacency)
        .def_readwrite("minimal_size",&Spatial::FMTspatialaction::minimal_size)
        .def_readwrite("maximal_size",&Spatial::FMTspatialaction::maximal_size)
        .def_readwrite("neighbors_size",&Spatial::FMTspatialaction::neighbors_size)
        .def_readwrite("greenup_weight",&Spatial::FMTspatialaction::greenup_weight)
        .def_readwrite("adjacency_weight",&Spatial::FMTspatialaction::adjacency_weight)
        .def_readwrite("size_weight",&Spatial::FMTspatialaction::size_weight);
    define_pylist<Spatial::FMTspatialaction>();

    class_<Spatial::FMTsaschedule, boost::noncopyable>("Abstract_FMTsaschedule",no_init);

    const char* py_FMTexposchedule_doc =
        " ``FMTlinearschedule`` class.\n"
        "\n"
        "FMTlinearschedule(double cooling_rate)\n"
        "Simulated annealing cooling schedule\n"
        "\n";

    class_<Spatial::FMTexponentialschedule,bases<FMTsaschedule>>("FMTexponentialschedule",py_FMTexposchedule_doc)
        .def(init<double>());

    const char* py_FMTsasolution_doc =
        " ``FMTsasolution`` class.\n"
        "\n"
        "FMTsasolution\n"
        "\n";

    enum_<Spatial::FMTsamovetype>("FMTsamovetype")
		.value("shotgun", Spatial::FMTsamovetype::shotgun)
		.value("cluster", Spatial::FMTsamovetype::cluster);

    define_FMTlayer<Graph::FMTgraph>();
    class_<Spatial::FMTsasolution,bases<FMTlayer<FMTgraph>>>("FMTsasolution",py_FMTsasolution_doc,no_init)
        .def("get_stats",&FMTsasolution::getsolution_stats)
        .def("getobjfvalue",&FMTsasolution::getobjfvalue)
        .def("get_forest_at_period",&FMTsasolution::getforestperiod);

    define_pylist<Spatial::FMTsasolution>();

    }


#endif // PYEXPORTSPATIAL_H_INCLUDED
