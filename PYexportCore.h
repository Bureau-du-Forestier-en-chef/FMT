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

#include "PYexportCore.h"
#include "FMTGCBMtransition.h"

namespace Python
{

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

	const char* py_FMTobject =
		" ``FMTobject`` class.\n"
		"\n"
		"Base FMTobjectect able to carry exception and logger\n"
		"\n";
	bp::class_<Core::FMTobject>("FMTobject", py_FMTobject)
		.def("setdefaultexceptionhandler", &Core::FMTobject::setdefaultexceptionhandler)
		.def("setquietexceptionhandler", &Core::FMTobject::setquietexceptionhandler)
		.def("setdebugexceptionhandler", &Core::FMTobject::setdebugexceptionhandler)
		.def("setfreeexceptionhandler", &Core::FMTobject::setfreeexceptionhandler);


		define_pylist<Core::FMTmask>();
        define_pylist<Core::FMTdevelopment>();
        define_pylist<Core::FMTfuturdevelopment>();
        define_pylist<Core::FMTactualdevelopment>();
		define_pydict<std::string, std::vector<Core::FMTdevelopment>>();

        define_pylist<Core::FMTtheme>();
        define_pylist<Core::FMTaction>();
        define_pylist<Core::FMTtransition>();
        define_pylist<Core::FMToutput>();
        define_pylist<Core::FMTschedule>();
		define_pylist<std::vector<Core::FMTschedule>>();

        define_pydict<Core::FMTdevelopment, std::vector<double>>();
        define_pydict<Core::FMTaction,std::map<Core::FMTdevelopment, std::vector<double>>>();

		bp::class_<Core::FMTmask>("FMTmask")
				.def_pickle(FMT_pickle_suite<Core::FMTmask>())
				.def(bp::init<const std::vector<std::string>&,const std::vector<Core::FMTtheme>&>())
				.def("__len__", &Core::FMTmask::operator bool)
                .def("__str__",&Core::FMTmask::operator std::string)
				.def("__eq__", &Core::FMTmask::operator ==)
				.def("__ne__", &Core::FMTmask::operator !=)
				.def("__lt__", &Core::FMTmask::operator <)
                .def("__hash__",&boost::pyhash<Core::FMTmask>);
		bp::class_<Core::FMTdevelopment>("FMTdevelopment")
				.def_pickle(FMT_pickle_suite<Core::FMTdevelopment>())
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
				.def("__str__", &Core::FMTdevelopment::operator std::string)
                .setattr("__hash__",&boost::pyhash<Core::FMTdevelopment>);
		bp::class_<Core::FMTspec>("FMTspec");
		bp::class_<Core::FMTactualdevelopment, bp::bases<Core::FMTdevelopment>>("FMTactualdevelopment")
                .def("getarea", &Core::FMTactualdevelopment::getarea)
				.def("setarea", &Core::FMTactualdevelopment::setarea)
				.def("__eq__", &Core::FMTactualdevelopment::operator ==)
				.def("__ne__", &Core::FMTactualdevelopment::operator !=)
				.def("__lt__", &Core::FMTactualdevelopment::operator <);
		bp::class_<Core::FMTfuturdevelopment, bp::bases<Core::FMTdevelopment>>("FMTfuturdevelopment");

            //Need preprocessor here
            define_FMTlist<Core::FMTspec>();

			define_FMTlist<int>();
			//define_pylist<string>();
			define_FMTlist<Core::FMTyieldhandler>();
			define_FMTlist<Core::FMTfork>();



			bp::class_<Core::FMTaction, bp::bases<Core::FMTlist<Core::FMTspec>>>("FMTaction")
				.def_pickle(FMT_pickle_suite<Core::FMTaction>())
                .def("__eq__",&Core::FMTaction::operator ==)
                .def("__ne__",&Core::FMTaction::operator !=)
                .def("__lt__",&Core::FMTaction::operator <)
				.def("__str__", &Core::FMTaction::operator std::string)
                .def("getname",&Core::FMTaction::getname)
                .def("dorespectlock",&Core::FMTaction::dorespectlock)
                .def_readwrite("isresetage",&Core::FMTaction::isresetage)
                .setattr("__hash__",&boost::pyhash<Core::FMTaction>);
			bp::class_<Core::FMTlifespans, bp::bases<Core::FMTlist<int>>>("FMTlifespans");


			bp::enum_<Core::FMTyldwstype>("FMTyldwstype")
				.value("FMTageyld", Core::FMTyldwstype::FMTageyld)
				.value("FMTtimeyld", Core::FMTyldwstype::FMTtimeyld)
				.value("FMTcomplexyld", Core::FMTyldwstype::FMTcomplexyld)
				.export_values();

			define_pydict<std::string, std::map<std::string,std::vector<double>>>();
			define_pydict<std::string, std::vector<double>>();

			bp::class_<Core::FMTyieldhandler>("FMTyieldhandler")
				.def_pickle(FMT_pickle_suite<Core::FMTyieldhandler>())
				.def("__str__", &Core::FMTyieldhandler::operator std::string);

			bp::class_<Core::FMTyields, bp::bases<Core::FMTlist<Core::FMTyieldhandler>>>("FMTyields")
				.def_pickle(FMT_pickle_suite<Core::FMTyields>())
				.def("getallyields", &Core::FMTyields::getallyields)
				.def("getnullyldsnames", &Core::FMTyields::getnullyldsnames);



			bp::class_<Core::FMTtransition, bp::bases<Core::FMTlist<Core::FMTfork>>>("FMTtransition")
				.def_pickle(FMT_pickle_suite<Core::FMTtransition>())
                .def("single",&Core::FMTtransition::single)
				.def("main_target", &Core::FMTtransition::main_target)
				.def("age_after", &Core::FMTtransition::age_after)
				.def("attribute_targets", &Core::FMTtransition::attribute_targets)
				.def("__str__", &Core::FMTtransition::operator std::string)
                .def("__eq__",&Core::FMTtransition::operator ==)
                .def("__ne__",&Core::FMTtransition::operator !=)
                .def("__lt__",&Core::FMTtransition::operator <)
                .def("getname",&Core::FMTtransition::getname);


			bp::class_<Core::FMToutputnode>("FMToutputnode")
				.def("__str__", &Core::FMToutputnode::operator std::string);

			define_FMTlist<Core::FMToutputnode>();

			bp::class_<Core::FMToutput>("FMToutput")
				.def_pickle(FMT_pickle_suite<Core::FMTtransition>())
                .def("getname",&Core::FMToutput::getname)
                .def("getdescription",&Core::FMToutput::getdescription)
				.def("empty", &Core::FMToutput::empty)
				.def("containslevel",&Core::FMToutput::containslevel)
				.def("islevel", &Core::FMToutput::islevel)
				.def("getnodes",&Core::FMToutput::getnodes, getnodes_overloads())
				.def("__str__", &Core::FMToutput::operator std::string)
                .def("__eq__",&Core::FMToutput::operator ==)
                .def("__ne__",&Core::FMToutput::operator !=);
			bp::class_<Core::FMTtheme>("FMTtheme")
				.def_pickle(FMT_pickle_suite<Core::FMTtheme>())
				.def("__str__", &Core::FMTtheme::operator std::string)
				.def("__eq__", &Core::FMTtheme::operator ==)
				.def("getattributes", &Core::FMTtheme::getattributes, getattributes_overloads())
				.def("getname", &Core::FMTtheme::getname);
			bp::class_<Core::FMTconstants>("FMTconstants");
			bp::class_<Core::FMTschedule>("FMTschedule")
				.def_pickle(FMT_pickle_suite<Core::FMTschedule>())
				.def(bp::init<int, std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>>())
				.def("__str__", &Core::FMTschedule::operator std::string)
				.def("__add__", &Core::FMTschedule::operator +)
				.def("actionarea", &Core::FMTschedule::actionarea)
				.def("getperiod", &Core::FMTschedule::getperiod)
				.def("__eq__", &Core::FMTschedule::operator ==)
				.def("__ne__", &Core::FMTschedule::operator !=)
				.def("__iter__", boost::python::iterator<Core::FMTschedule>())
				.def("empty",&Core::FMTschedule::empty);
			bp::class_<Core::FMTGCBMtransition>("FMTGCBMtransition")
				.def_readwrite("name", &Core::FMTGCBMtransition::name)
				.def_readwrite("ageafter", &Core::FMTGCBMtransition::ageafter)
				.add_property("themes", bp::make_getter(&Core::FMTGCBMtransition::themes, bp::return_value_policy<bp::return_by_value>()),
					bp::make_setter(&Core::FMTGCBMtransition::themes, bp::return_value_policy<bp::return_by_value>()));
			bp::class_<Core::FMTconstraint, bp::bases<Core::FMToutput>>("FMTconstraint")
				.def("__str__", &Core::FMTconstraint::operator std::string)
				.def("__eq__", &Core::FMTconstraint::operator ==);

			define_FMTlist<Core::FMTconstraint>();
			define_FMTlist<Core::FMTGCBMtransition>();
    }
}

#endif // PYEXPORTCORE_H_INCLUDED
