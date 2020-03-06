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

#include "FMTGCBMtransition.h"
#include "FMTobject.h"
#include "FMTmask.h"
#include "FMTtheme.h"
#include "FMTbounds.h"
#include "FMTdevelopment.h"
#include "FMTactualdevelopment.h"
#include "FMTfuturdevelopment.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMToutput.h"
#include "FMTconstraint.h"
#include "FMTschedule.h"
#include "FMTyields.h"
#include "FMTconstants.h"
#include "boost/python.hpp"

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


	bp::class_<Core::FMTobject>("FMTobject", "@DocString(FMTobject)")
		.def("setdefaultexceptionhandler", &Core::FMTobject::setdefaultexceptionhandler,
			"@DocString(FMTobject::setdefaultexceptionhandler)")
		.def("setquietexceptionhandler", &Core::FMTobject::setquietexceptionhandler,
			"@DocString(FMTobject::setquietexceptionhandler)")
		.def("setdebugexceptionhandler", &Core::FMTobject::setdebugexceptionhandler,
			"@DocString(FMTobject::setdebugexceptionhandler)")
		.def("setfreeexceptionhandler", &Core::FMTobject::setfreeexceptionhandler,
			"@DocString(FMTobject::setfreeexceptionhandler)");


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

		bp::class_<Core::FMTmask>("FMTmask", "@DocString(FMTmask)")
				.def_pickle(FMT_pickle_suite<Core::FMTmask>())
				.def(bp::init<const std::vector<std::string>&,const std::vector<Core::FMTtheme>&>(),
					"@DocString(FMTmask(const std::vector<std::string>&,const std::vector<Core::FMTtheme>&))")
				.def("__len__", &Core::FMTmask::operator bool,
					"@DocString(FMTmask::operator bool)")
                .def("__str__",&Core::FMTmask::operator std::string,
					"@DocString(FMTmask::operator std::string)")
				.def("__eq__", &Core::FMTmask::operator ==,
					"@DocString(FMTmask::operator==)")
				.def("__ne__", &Core::FMTmask::operator !=,
					"@DocString(FMTmask::operator!=)")
				.def("__lt__", &Core::FMTmask::operator <,
					"@DocString(FMTmask::operator<)")
                .def("__hash__",&boost::pyhash<Core::FMTmask>);

		bp::class_<Core::FMTdevelopment>("FMTdevelopment", "@DocString(FMTdevelopment)")
				.def_pickle(FMT_pickle_suite<Core::FMTdevelopment>())
                .def_readwrite("age", &Core::FMTdevelopment::age,
					"@DocString(FMTdevelopment::age)")
				.def_readwrite("period", &Core::FMTdevelopment::period,
					"@DocString(FMTdevelopment::period)")
                .def_readwrite("lock", &Core::FMTdevelopment::lock,
					"@DocString(FMTdevelopment::lock)")
                .def_readwrite("mask", &Core::FMTdevelopment::mask,
					"@DocString(FMTdevelopment::mask)")
				.def("__eq__", &Core::FMTdevelopment::operator ==,
					"@DocString(FMTdevelopment::operator==)")
				.def("__ne__", &Core::FMTdevelopment::operator !=,
					"@DocString(FMTdevelopment::operator!=)")
                .def("__lt__",&Core::FMTdevelopment::operator <,
					"@DocString(FMTdevelopment::operator<")
                .def("grow",&Core::FMTdevelopment::grow,
					"@DocString(FMTdevelopment::grow)")
                .def("operable",&Core::FMTdevelopment::operable,
					"@DocString(FMTdevelopment::operable)")
                .def("operate",&Core::FMTdevelopment::operate,
					"@DocString(FMTdevelopment::operate)")
				.def("__str__", &Core::FMTdevelopment::operator std::string,
					"@DocString(FMTdevelopment::operator std::string)")
                .setattr("__hash__",&boost::pyhash<Core::FMTdevelopment>);

		bp::class_<Core::FMTspec>("FMTspec","@DocString(FMTspec)");

		bp::class_<Core::FMTactualdevelopment, bp::bases<Core::FMTdevelopment>>("FMTactualdevelopment", "@DocString(FMTactualdevelopment)")
                .def("getarea", &Core::FMTactualdevelopment::getarea,
					"@DocString(FMTactualdevelopment::getarea)")
				.def("setarea", &Core::FMTactualdevelopment::setarea,
					"@DocString(FMTactualdevelopment::setarea)")
				.def("__eq__", &Core::FMTactualdevelopment::operator ==,
					"@DocString(FMTactualdevelopment::operator==)")
				.def("__ne__", &Core::FMTactualdevelopment::operator !=,
					"@DocString(FMTactualdevelopment::operator!=)")
				.def("__lt__", &Core::FMTactualdevelopment::operator <,
					"@DocString(FMTactualdevelopment::operator<)");

		bp::class_<Core::FMTfuturdevelopment, bp::bases<Core::FMTdevelopment>>("FMTfuturdevelopment", "@DocString(FMTfuturdevelopment)");

            //Need preprocessor here
            define_FMTlist<Core::FMTspec>();

			define_FMTlist<int>();
			//define_pylist<string>();
			define_FMTlist<Core::FMTyieldhandler>();
			define_FMTlist<Core::FMTfork>();



			bp::class_<Core::FMTaction, bp::bases<Core::FMTlist<Core::FMTspec>>>("FMTaction", "@DocString(FMTaction)")
				.def_pickle(FMT_pickle_suite<Core::FMTaction>())
                .def("__eq__",&Core::FMTaction::operator ==,
					"@DocString(FMTaction::operator==)")
                .def("__ne__",&Core::FMTaction::operator !=,
					"@DocString(FMTaction::operator!=)")
                .def("__lt__",&Core::FMTaction::operator <,
					"@DocString(FMTaction::operator<)")
				.def("__str__", &Core::FMTaction::operator std::string,
					"@DocString(FMTaction::operator std::string)")
                .def("getname",&Core::FMTaction::getname,
					"@DocString(FMTaction::getname)")
                .def("dorespectlock",&Core::FMTaction::dorespectlock,
					"@DocString(FMTaction::dorespectlock)")
                .def_readwrite("isresetage",&Core::FMTaction::isresetage,
					"@DocString(FMTaction::isresetage)")
                .setattr("__hash__",&boost::pyhash<Core::FMTaction>);

			bp::class_<Core::FMTlifespans, bp::bases<Core::FMTlist<int>>>("FMTlifespans", "@DocString(FMTlifespans)");


			bp::enum_<Core::FMTyldwstype>("FMTyldwstype")
				.value("FMTageyld", Core::FMTyldwstype::FMTageyld)
				.value("FMTtimeyld", Core::FMTyldwstype::FMTtimeyld)
				.value("FMTcomplexyld", Core::FMTyldwstype::FMTcomplexyld)
				.export_values();

			define_pydict<std::string, std::map<std::string,std::vector<double>>>();

			define_pydict<std::string, std::vector<double>>();

			bp::class_<Core::FMTyieldhandler>("FMTyieldhandler", "@DocString(FMTyieldhandler)")
				.def_pickle(FMT_pickle_suite<Core::FMTyieldhandler>())
				.def("__str__", &Core::FMTyieldhandler::operator std::string,
					"@DocString(FMTyieldhandler::operator std::string)");

			bp::class_<Core::FMTyields, bp::bases<Core::FMTlist<Core::FMTyieldhandler>>>("FMTyields", "@DocString(FMTyields)")
				.def_pickle(FMT_pickle_suite<Core::FMTyields>())
				.def("getallyields", &Core::FMTyields::getallyields,
					"@DocString(FMTyields::getallyields)")
				.def("getnullyldsnames", &Core::FMTyields::getnullyldsnames,
					"@DocString(FMTyields::getnullyldsnames)");



			bp::class_<Core::FMTtransition, bp::bases<Core::FMTlist<Core::FMTfork>>>("FMTtransition", "@DocString(FMTtransition)")
				.def_pickle(FMT_pickle_suite<Core::FMTtransition>())
                .def("single",&Core::FMTtransition::single,
					"@DocString(FMTtransition::single)")
				.def("main_target", &Core::FMTtransition::main_target,
					"@DocString(FMTtransition::main_target)")
				.def("age_after", &Core::FMTtransition::age_after,
					"@DocString(FMTtransition::age_after)")
				.def("attribute_targets", &Core::FMTtransition::attribute_targets,
					"@DocString(FMTtransition::attribute_targets)")
				.def("__str__", &Core::FMTtransition::operator std::string,
					"@DocString(FMTtransition::operator std::string)")
                .def("__eq__",&Core::FMTtransition::operator ==,
					"@DocString(FMTtransition::operator==)")
                .def("__ne__",&Core::FMTtransition::operator !=,
					"@DocString(FMTtransition::operator!=)")
                .def("__lt__",&Core::FMTtransition::operator <,
					"@DocString(FMTtransition::operator<)")
                .def("getname",&Core::FMTtransition::getname,
					"@DocString(FMTtransition::getname)");


			bp::class_<Core::FMToutputnode>("FMToutputnode", "@DocString(FMToutputnode)")
				.def("__str__", &Core::FMToutputnode::operator std::string,
					"@DocString(FMToutputnode::operator std::string)");

			define_FMTlist<Core::FMToutputnode>();

			bp::class_<Core::FMToutput>("FMToutput", "@DocString(FMToutput)")
				.def_pickle(FMT_pickle_suite<Core::FMTtransition>())
                .def("getname",&Core::FMToutput::getname,
					"@DocString(FMToutput::getname)")
                .def("getdescription",&Core::FMToutput::getdescription,
					"@DocString(FMToutput::getdescription)")
				.def("empty", &Core::FMToutput::empty,
					"@DocString(FMToutput::empty)")
				.def("containslevel",&Core::FMToutput::containslevel,
					"@DocString(FMToutput::containslevel)")
				.def("islevel", &Core::FMToutput::islevel,
					"@DocString(FMToutput::islevel)")
				.def("getnodes",&Core::FMToutput::getnodes, getnodes_overloads())
				.def("__str__", &Core::FMToutput::operator std::string,
					"@DocString(FMToutput::operator std::string)")
                .def("__eq__",&Core::FMToutput::operator ==,
					"@DocString(FMToutput::operator==)")
                .def("__ne__",&Core::FMToutput::operator !=,
					"@DocString(FMToutput::operator!=)");

			bp::class_<Core::FMTtheme>("FMTtheme", "@DocString(FMTtheme)")
				.def_pickle(FMT_pickle_suite<Core::FMTtheme>())
				.def("__str__", &Core::FMTtheme::operator std::string,
					"@DocString(FMTtheme::operator std::string)")
				.def("__eq__", &Core::FMTtheme::operator ==,
					"@DocString(FMTtheme::operator==)")
				.def("getattributes", &Core::FMTtheme::getattributes, getattributes_overloads())
				.def("getname", &Core::FMTtheme::getname,
					"@DocString(FMTtheme::getname)");

			bp::class_<Core::FMTconstants>("FMTconstants","@DocString(FMTconstants)");

			bp::class_<Core::FMTschedule>("FMTschedule", "@DocString(FMTschedule)")
				.def_pickle(FMT_pickle_suite<Core::FMTschedule>())
				.def(bp::init<int, std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>>(),
					"@DocString(FMTschedule(int,std::map<Core::FMTaction,std::map<Core::FMTdevelopment,std::vector<double>>>))")
				.def("__str__", &Core::FMTschedule::operator std::string,
					"@DocString(FMTschedule::operator std::string)")
				.def("__add__", &Core::FMTschedule::operator +,
					"@DocString(FMTschedule::operator +)")
				.def("actionarea", &Core::FMTschedule::actionarea,
					"@DocString(FMTschedule::actionarea)")
				.def("getperiod", &Core::FMTschedule::getperiod,
					"@DocString(FMTschedule::getperiod)")
				.def("__eq__", &Core::FMTschedule::operator ==,
					"@DocString(FMTschedule::operator==)")
				.def("__ne__", &Core::FMTschedule::operator !=,
					"@DocString(FMTschedule::operator!=)")
				.def("__iter__", boost::python::iterator<Core::FMTschedule>())
				.def("empty",&Core::FMTschedule::empty,
					"@DocString(FMTschedule::empty)");

			bp::class_<Core::FMTGCBMtransition>("FMTGCBMtransition", "@DocString(FMTGCBMtransition)")
				.def_readwrite("name", &Core::FMTGCBMtransition::name,
					"@DocString(FMTGCBMtransition::name)")
				.def_readwrite("ageafter", &Core::FMTGCBMtransition::ageafter,
					"@DocString(FMTGCBMtransition::ageafter)")
				.add_property("themes", bp::make_getter(&Core::FMTGCBMtransition::themes, bp::return_value_policy<bp::return_by_value>()),
					bp::make_setter(&Core::FMTGCBMtransition::themes, bp::return_value_policy<bp::return_by_value>()));

			bp::class_<Core::FMTconstraint, bp::bases<Core::FMToutput>>("FMTconstraint", "@DocString(FMTconstraint)")
				.def("__str__", &Core::FMTconstraint::operator std::string,
					"@DocString(FMTconstraint::operator std::string)")
				.def("__eq__", &Core::FMTconstraint::operator ==,
					"@DocString(FMTconstraint::operator==)");

			define_FMTlist<Core::FMTconstraint>();
			define_FMTlist<Core::FMTGCBMtransition>();
    }
}

#endif // PYEXPORTCORE_H_INCLUDED
