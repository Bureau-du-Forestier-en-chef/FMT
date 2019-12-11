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

#ifndef PYEXPORTCORE_H_INCLUDED
#define PYEXPORTCORE_H_INCLUDED



BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getattributes_overloads, getattributes, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getnodes_overloads, getnodes, 0, 1)

void exportCore()
    {
    namespace bp = boost::python;
    bp::object CoreModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.Core"))));
    bp::scope().attr("Core") = CoreModule;
    bp::scope Core_scope = CoreModule;
    bp::scope().attr("__doc__") = ".. module:: Core\n"
    "\n"
    "   :platform: Unix, Windows\n"
    "   :synopsis: Core class for generating Forest Models.\n"
    "\n";

		define_pylist<Core::FMTmask>();
        define_pylist<Core::FMTdevelopment>();
        define_pylist<Core::FMTfuturdevelopment>();
        define_pylist<Core::FMTactualdevelopment>();
		define_pydict<string, vector<FMTdevelopment>>();

        define_pylist<Core::FMTtheme>();
        define_pylist<Core::FMTaction>();
        define_pylist<Core::FMTtransition>();
        define_pylist<Core::FMToutput>();
        define_pylist<Core::FMTschedule>();
		define_pylist<vector<Core::FMTschedule>>();

        define_pydict<Core::FMTdevelopment,vector<double>>();
        define_pydict<Core::FMTaction,map<Core::FMTdevelopment,vector<double>>>();

            class_<Core::FMTmask>("FMTmask")
				.def_pickle(FMT_pickle_suite<FMTmask>())
				.def(init<const vector<string>&,const vector<FMTtheme>&>())
				.def("__len__", &Core::FMTmask::operator bool)
                .def("__str__",&Core::FMTmask::operator string)
				.def("__eq__", &Core::FMTmask::operator ==)
				.def("__ne__", &Core::FMTmask::operator !=)
                .def("to_string",&Core::FMTmask::to_string)
                .def("__hash__",&boost::pyhash<Core::FMTmask>);
			class_<Core::FMTdevelopment>("FMTdevelopment")
				.def_pickle(FMT_pickle_suite<FMTdevelopment>())
                .def_readwrite("age", &Core::FMTdevelopment::age)
				.def_readwrite("period", &Core::FMTdevelopment::period)
                .def_readwrite("lock", &Core::FMTdevelopment::lock)
                .def_readwrite("mask", &Core::FMTdevelopment::mask)
				.def("__eq__", &Core::FMTdevelopment::operator ==)
				.def("__ne__", &Core::FMTdevelopment::operator !=)
                .def("__lt__",&Core::FMTdevelopment::operator <)
                .def("grow",&Core::FMTdevelopment::grow)
                .def("operable",&Core::FMTdevelopment::operable)
                .def("operate",&Core::FMTdevelopment::operate)
				.def("__str__", &Core::FMTdevelopment::operator string)
                .setattr("__hash__",&boost::pyhash<Core::FMTdevelopment>);
            class_<Core::FMTspec>("FMTspec");
            class_<Core::FMTactualdevelopment,bases<Core::FMTdevelopment>>("FMTactualdevelopment")
                .def_readwrite("area", &Core::FMTactualdevelopment::area)
				.def("__eq__", &Core::FMTactualdevelopment::operator ==)
				.def("__ne__", &Core::FMTactualdevelopment::operator !=)
				.def("__lt__", &Core::FMTactualdevelopment::operator <);
            class_<Core::FMTfuturdevelopment,bases<Core::FMTdevelopment>>("FMTfuturdevelopment");

            //Need preprocessor here
            define_FMTlist<Core::FMTspec>();

			define_FMTlist<int>();
			//define_pylist<string>();
			define_FMTlist<Core::FMTyieldhandler>();
			define_FMTlist<Core::FMTfork>();



            class_<Core::FMTaction,bases<Core::FMTlist<Core::FMTspec>>>("FMTaction")
				.def_pickle(FMT_pickle_suite<FMTaction>())
                .def("__eq__",&Core::FMTaction::operator ==)
                .def("__ne__",&Core::FMTaction::operator !=)
                .def("__lt__",&Core::FMTaction::operator <)
				.def("__str__", &Core::FMTaction::operator string)
                .def_readwrite("name",&Core::FMTaction::name)
                .def_readwrite("lock",&Core::FMTaction::lock)
                .def_readwrite("reset",&Core::FMTaction::reset)
                .setattr("__hash__",&boost::pyhash<Core::FMTaction>);
            class_<Core::FMTlifespans,bases<Core::FMTlist<int>>>("FMTlifespans");


			enum_<FMTyldwstype>("FMTyldwstype")
				.value("FMTageyld", FMTyldwstype::FMTageyld)
				.value("FMTtimeyld", FMTyldwstype::FMTtimeyld)
				.value("FMTcomplexyld", FMTyldwstype::FMTcomplexyld)
				.export_values();

			define_pydict<string,map<string,vector<double>>>();
			define_pydict<string, vector<double>>();

			class_<Core::FMTyields, bases<Core::FMTlist<Core::FMTyieldhandler>>>("FMTyields")
				.def_pickle(FMT_pickle_suite<FMTyields>())
				.def("getallyields", &Core::FMTyields::getallyields)
				.def("getnullyldsnames", &Core::FMTyields::getnullyldsnames);



            class_<Core::FMTtransition,bases<Core::FMTlist<Core::FMTfork>>>("FMTtransition")
				.def_pickle(FMT_pickle_suite<FMTtransition>())
                .def("single",&Core::FMTtransition::single)
				.def("main_target", &Core::FMTtransition::main_target)
				.def("age_after", &Core::FMTtransition::age_after)
				.def("attribute_targets", &Core::FMTtransition::attribute_targets)
				.def("__str__", &Core::FMTtransition::operator string)
                .def("__eq__",&Core::FMTtransition::operator ==)
                .def("__ne__",&Core::FMTtransition::operator !=)
                .def("__lt__",&Core::FMTtransition::operator <)
                .def_readwrite("name",&Core::FMTtransition::name);


			class_<Core::FMToutputnode>("FMToutputnode")
				.def("__str__", &Core::FMToutputnode::operator string);

			define_FMTlist<Core::FMToutputnode>();

            class_<Core::FMToutput>("FMToutput")
				.def_pickle(FMT_pickle_suite<FMTtransition>())
                .def_readwrite("name",&Core::FMToutput::name)
                .def_readwrite("description",&Core::FMToutput::description)
				.def("empty", &Core::FMToutput::empty)
				.def("containslevel",&Core::FMToutput::containslevel)
				.def("islevel", &Core::FMToutput::islevel)
				.def("getnodes",&Core::FMToutput::getnodes, getnodes_overloads())
				.def("__str__", &Core::FMToutput::operator string)
                .def("__eq__",&Core::FMToutput::operator ==)
                .def("__ne__",&Core::FMToutput::operator !=);
            class_<Core::FMTtheme>("FMTtheme")
				.def_pickle(FMT_pickle_suite<FMTtheme>())
				.def("__str__", &Core::FMTtheme::operator string)
				.def("__eq__", &Core::FMTtheme::operator ==)
				.def("getattributes", &Core::FMTtheme::getattributes, getattributes_overloads())
                .def_readwrite("name",&Core::FMTtheme::name);
            class_<Core::FMTconstants>("FMTconstants");
            class_<Core::FMTschedule>("FMTschedule")
				.def_pickle(FMT_pickle_suite<FMTschedule>())
				.def(init<int, map<FMTaction, map<FMTdevelopment, vector<double>>>>())
				.def("__str__", &Core::FMTschedule::operator string)
				.def("__add__", &Core::FMTschedule::operator +)
                .def("getfaction",&Core::FMTschedule::getfaction)
				.def("actionarea", &Core::FMTschedule::actionarea)
				.def("getperiod", &Core::FMTschedule::getperiod)
				.def("__eq__", &Core::FMTschedule::operator ==)
				.def("__ne__", &Core::FMTschedule::operator !=)
				.def("empty",&Core::FMTschedule::empty)
                .def("get",&Core::FMTschedule::get);
			class_<Core::FMTGCBMtransition>("FMTGCBMtransition")
				.def_readwrite("name", &Core::FMTGCBMtransition::name)
				.def_readwrite("ageafter", &Core::FMTGCBMtransition::ageafter)
				.add_property("themes", make_getter(&Core::FMTGCBMtransition::themes, return_value_policy<return_by_value>()),
					make_setter(&Core::FMTGCBMtransition::themes, return_value_policy<return_by_value>()));
			class_<Core::FMTconstraint, bases<Core::FMToutput>>("FMTconstraint")
				.def("__str__", &Core::FMTconstraint::operator string)
				.def("__eq__", &Core::FMTconstraint::operator ==);

			define_FMTlist<Core::FMTconstraint>();
			define_FMTlist<Core::FMTGCBMtransition>();
    }


#endif // PYEXPORTCORE_H_INCLUDED
