/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTCORE_H_INCLUDED
#define PYEXPORTCORE_H_INCLUDED

#include "FMTGCBMtransition.h"
#include "FMTobject.h"
#include "FMTmask.h"
#include "FMTtheme.h"
#include "FMTbounds.hpp"
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
//#include "FMTdata.h"
#include "FMTtimeyieldhandler.h"
#include "FMTageyieldhandler.h"
#include "FMTmaskfilter.h"
#include "FMTtransitionmask.h"
#include "FMTdevelopmentpath.h"
#include "FMTSerie.h"

namespace Python
{

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getattributes_overloads, getAttributes, 1, 2)
//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getnodes_overloads, getNodes, 3, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getnodes_overloads, getNodes, 1, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getyieldrequest_overloads, getYieldRequest, 0, 0)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(operable_overloads, operable, 2, 3)

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

	bp::enum_<Core::FMTsection>("FMTsection")
		.value("Control", Core::FMTsection::Control)
		.value("Landscape", Core::FMTsection::Landscape)
		.value("Area", Core::FMTsection::Area)
		.value("Action", Core::FMTsection::Action)
		.value("Transition", Core::FMTsection::Transition)
		.value("Yield", Core::FMTsection::Yield)
		.value("Outputs", Core::FMTsection::Outputs)
		.value("Optimize", Core::FMTsection::Optimize)
		.value("Constants", Core::FMTsection::Constants)
		.value("Schedule", Core::FMTsection::Schedule)
		.value("Empty", Core::FMTsection::Empty)
		.export_values();

	bp::enum_<Core::FMToutputlevel>("FMToutputlevel")
		.value("standard", Core::FMToutputlevel::standard)
		.value("totalonly", Core::FMToutputlevel::totalonly)
		.value("developpement", Core::FMToutputlevel::developpement)
		.export_values();

	bp::enum_<Core::FMTconstrainttype>("FMTconstrainttype")
		.value("FMTMAXobjective", Core::FMTconstrainttype::FMTMAXobjective)
		.value("FMTMINobjective", Core::FMTconstrainttype::FMTMINobjective)
		.value("FMTMAXMINobjective", Core::FMTconstrainttype::FMTMAXMINobjective)
		.value("FMTMINMAXobjective", Core::FMTconstrainttype::FMTMINMAXobjective)
		.value("FMTevenflow", Core::FMTconstrainttype::FMTevenflow)
		.value("FMTnondeclining", Core::FMTconstrainttype::FMTnondeclining)
		.value("FMTsequence", Core::FMTconstrainttype::FMTsequence)
		.value("FMTstandard", Core::FMTconstrainttype::FMTstandard)
		.value("FMTspatialsize", Core::FMTconstrainttype::FMTspatialsize)
		.value("FMTspatialadjacency", Core::FMTconstrainttype::FMTspatialadjacency)
		.value("FMTspatialgreenup", Core::FMTconstrainttype::FMTspatialgreenup)
		.export_values();

	bp::class_<Core::FMTobject>("FMTobject", "@DocString(FMTobject)")
		.def("setdefaultexceptionhandler", &Core::FMTobject::setDefaultExceptionHandler,
			"@DocString(FMTobject::setdefaultexceptionhandler)")
		.def("setquietexceptionhandler", &Core::FMTobject::setQuietExceptionHandler,
			"@DocString(FMTobject::setquietexceptionhandler)")
		.def("setdebugexceptionhandler", &Core::FMTobject::setDebugExceptionHandler,
			"@DocString(FMTobject::setdebugexceptionhandler)")
		.def("setfreeexceptionhandler", &Core::FMTobject::setFreeExceptionHandler,
			"@DocString(FMTobject::setfreeexceptionhandler)")
		.def("enablenestedexceptions", &Core::FMTobject::enableNestedExceptions,
			"@DocString(FMTobject::enablenestedexceptions)")
		.def("disablenestedexceptions", &Core::FMTobject::disableNestedExceptions,
			"@DocString(FMTobject::disablenestedexceptions)")
		.def("setdefaultlogger", &Core::FMTobject::setDefaultLogger,
			"@DocString(FMTobject::setdefaultlogger)")
		.def("setdebuglogger", &Core::FMTobject::setDebugLogger,
			"@DocString(FMTobject::setdebuglogger)")
		.def("setquietlogger", &Core::FMTobject::setQuietLogger,
			"@DocString(FMTobject::setquietlogger)")
		.def("settasklogger", &Core::FMTobject::setTaskLogger,
			"@DocString(FMTobject::settasklogger")
		.def("redirectlogtofile", &Core::FMTobject::redirectLogToFile,
			"@DocString(FMTobject::redirectlogtofile)")
		.def("seterrorstowarnings", &Core::FMTobject::seterrorstowarnings,
			"@DocString(FMTobject::seterrorstowarnings)");

		bp::class_<Core::FMTmaskfilter>("FMTmaskfilter", "@DocString(FMTmaskfilter)");

		definePyList<Core::FMTdevelopmentpath>();
		definePySet<Core::FMTSerie>();
		definePyList<Core::FMTSerie>();
		definePyList<Core::FMTmaskfilter>();
		definePyList<Core::FMTmask>();
        definePyList<Core::FMTdevelopment>();
        definePyList<Core::FMTfuturdevelopment>();
        definePyList<Core::FMTactualdevelopment>();
		definePyDict<std::string, std::vector<Core::FMTdevelopment>>();

        definePyList<Core::FMTtheme>();
        definePyList<Core::FMTaction>();
        definePyList<Core::FMTtransition>();
        definePyList<Core::FMToutput>();
        definePyList<Core::FMTschedule>();
		definePyList<std::vector<Core::FMTschedule>>();
		definePyList<Core::FMTyieldhandler>();
		definePyList<Core::FMToutputsource>();
		definePyList<Core::FMToperator>();

        definePyDict<Core::FMTdevelopment, std::vector<double>>();
        definePyDict<Core::FMTaction,std::map<Core::FMTdevelopment, std::vector<double>>>();
		py_pair<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>();
		py_pair<Core::FMTaction const, std::map<Core::FMTdevelopment, std::vector<double>>>();

		define_FMTlist<Core::FMTspec>();
		definePyList<std::vector<std::string>>();

		bool (Core::FMTmask::*issubsetof)(const Core::FMTmask&) const = &Core::FMTmask::isSubsetOf;

		bp::class_<Core::FMTmask>("FMTmask", "@DocString(FMTmask)")
				.def(bp::init<Core::FMTmask>())
				.def_pickle(FMT_pickle_suite<Core::FMTmask>())
				.def(bp::init<const std::vector<std::string>&,const std::vector<Core::FMTtheme>&>())
				.def(bp::init<const std::string&, const std::vector<Core::FMTtheme>&>())
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
                .def("__hash__",&boost::pyHash<Core::FMTmask>)
				.def("issubsetof",issubsetof,
					"@DocString(FMTmask::issubsetof)")
				.def("presolve",&Core::FMTmask::presolve,
					"@DocString(FMTmask::presolve)")
				.def("postsolve",&Core::FMTmask::postsolve,
					"@DocString(FMTmask::postsolve)");

		bp::class_<Core::FMTdevelopmentpath>("FMTdevelopmentpath", "@DocString(FMTdevelopmentpath)")
			.def(bp::init<Core::FMTdevelopmentpath>())
			.def("getDevelopment", &Core::FMTdevelopmentpath::getDevelopment,
				bp::return_value_policy<bp::return_by_value>(),
				"@DocString(FMTdevelopmentpath::getDevelopment)")
			.def("getProportion", &Core::FMTdevelopmentpath::getProportion,
				bp::return_value_policy<bp::return_by_value>(),
				"@DocString(FMTdevelopmentpath::getProportion)");

		bp::class_<Core::FMTSerie>("FMTSerie", "@DocString(FMTSerie)")
			.def(bp::init<Core::FMTSerie>())
			.def("getLength", &Core::FMTSerie::getLength,
				bp::return_value_policy<bp::return_by_value>(),
				"@DocString(FMTSerie::getLength)")
			.def("getSerie", &Core::FMTSerie::getSerie,
				bp::return_value_policy<bp::return_by_value>(),
				"@DocString(FMTSerie::getSerie)");

		

		bp::class_<Core::FMTdevelopment>("FMTdevelopment", "@DocString(FMTdevelopment)")
				.def_pickle(FMT_pickle_suite<Core::FMTdevelopment>())
			    .def(bp::init<Core::FMTdevelopment>())
				.def("getage", &Core::FMTdevelopment::getAge,
					"@DocString(FMTdevelopment::getage)")
				.def("getlock", &Core::FMTdevelopment::getLock,
					"@DocString(FMTdevelopment::getlock)")
				.def("getperiod", &Core::FMTdevelopment::getperiod,
					"@DocString(FMTdevelopment::getperiod)")
				.def("getmask", &Core::FMTdevelopment::getMaskCopy,
					"@DocString(FMTdevelopment::getmask)")
				.def("setage", &Core::FMTdevelopment::setAge,
					"@DocString(FMTdevelopment::setage)")
				.def("setlock", &Core::FMTdevelopment::setLock,
					"@DocString(FMTdevelopment::setlock)")
				.def("setperiod", &Core::FMTdevelopment::setPeriod,
					"@DocString(FMTdevelopment::setperiod)")
				.def("setmask", &Core::FMTdevelopment::setMask,
					"@DocString(FMTdevelopment::setmask)")
				.def("__eq__", &Core::FMTdevelopment::operator ==,
					"@DocString(FMTdevelopment::operator==)")
				.def("__ne__", &Core::FMTdevelopment::operator !=,
					"@DocString(FMTdevelopment::operator!=)")
                .def("__lt__",&Core::FMTdevelopment::operator <,
					"@DocString(FMTdevelopment::operator<")
                .def("grow",&Core::FMTdevelopment::grow,
					"@DocString(FMTdevelopment::grow)")
                .def("operable",&Core::FMTdevelopment::operable,operable_overloads(bp::args("action,yields,yieldrequest"),"@DocString(FMTdevelopment::operable)"))
                .def("operate",&Core::FMTdevelopment::operate,
					"@DocString(FMTdevelopment::operate)")
				 .def("getyieldrequest",&Core::FMTdevelopment::getYieldRequest,getyieldrequest_overloads("@DocString(FMTdevelopment::getyieldrequest)"))
				.def("__str__", &Core::FMTdevelopment::operator std::string,
					"@DocString(FMTdevelopment::operator std::string)")
                .setattr("__hash__",&boost::pyHash<Core::FMTdevelopment>);



		bp::class_<Core::FMTagebounds>("FMTagebounds", "@DocString(FMTagebounds)")
			.def(bp::init<Core::FMTagebounds>())
			.def(bp::init<Core::FMTsection, int, int>());

		bp::class_<Core::FMTperbounds>("FMTperbounds", "@DocString(FMTperbounds)")
			.def(bp::init<Core::FMTperbounds>())
			.def(bp::init<Core::FMTsection, int, int>());

		bp::class_<Core::FMTyldbounds>("FMTyldbounds", "@DocString(FMTyldbounds)")
			.def(bp::init<Core::FMTyldbounds>())
			.def(bp::init<Core::FMTsection, std::string, double, double>());

		bp::class_<Core::FMTlockbounds>("FMTlockbounds", "@DocString(FMTlockbounds)")
			.def(bp::init<Core::FMTlockbounds>())
			.def(bp::init<Core::FMTsection, int, int>());

		bp::class_<Core::FMTspec>("FMTspec", "@DocString(FMTspec)")
			.def(bp::init<Core::FMTspec>())
			.def("getperiodlowerbound", &Core::FMTspec::getPeriodLowerBound,
				"@DocString(FMTspec::getperiodlowerbound)")
			.def("getperiodupperbound", &Core::FMTspec::getPeriodUpperBound,
				"@DocString(FMTspec::getperiodupperbound)")
			.def("setbounds", &Core::FMTspec::setBounds, "@DocString(Core::FMTspec::setbounds())")
			.def<bool(Core::FMTspec::*)(const Core::FMTagebounds&)>("addbounds", &Core::FMTspec::addBounds,
				"@DocString(Core::FMTspec::addbounds(const FMTagebounds&))")
			.def<bool(Core::FMTspec::*)(const Core::FMTyldbounds&)>("addbounds",&Core::FMTspec::addBounds,
				"@DocString(Core::FMTspec::addbounds(const FMTyldbounds&))")
			.def<bool(Core::FMTspec::*)(const Core::FMTlockbounds&)>("addbounds",&Core::FMTspec::addBounds,
				"@DocString(Core::FMTspec::addbounds(const FMTlockbounds&))");

		bp::class_<Core::FMTtransitionmask>("FMTtransitionmask", "@DocString(FMTtransitionmask)")
			.def(bp::init<const std::string&, const std::vector<Core::FMTtheme>&, const double&>())
			.def(bp::init<Core::FMTtransitionmask>());

		bp::class_<Core::FMTfork, bp::bases<Core::FMTlist<Core::FMTspec>>>("FMTfork", "@DocString(FMTfork)")
			.def(bp::init<Core::FMTfork>())
			.def("add", &Core::FMTfork::add,
				"@DocString(FMTfork::add)");




		bp::class_<Core::FMTactualdevelopment, bp::bases<Core::FMTdevelopment>>("FMTactualdevelopment", "@DocString(FMTactualdevelopment)")
				.def(bp::init<const Core::FMTmask&, const int&, const int&, const double&>())
				.def(bp::init<Core::FMTactualdevelopment>())
			    .def("getarea", &Core::FMTactualdevelopment::getarea,
					"@DocString(FMTactualdevelopment::getarea)")
				.def("setarea", &Core::FMTactualdevelopment::setArea,
					"@DocString(FMTactualdevelopment::setarea)")
				.def("__eq__", &Core::FMTactualdevelopment::operator ==,
					"@DocString(FMTactualdevelopment::operator==)")
				.def("__ne__", &Core::FMTactualdevelopment::operator !=,
					"@DocString(FMTactualdevelopment::operator!=)")
				.def("__lt__", &Core::FMTactualdevelopment::operator <,
					"@DocString(FMTactualdevelopment::operator<)");

		bp::class_<Core::FMTfuturdevelopment, bp::bases<Core::FMTdevelopment>>("FMTfuturdevelopment", "@DocString(FMTfuturdevelopment)");

            //Need preprocessor here
            

			define_FMTlist<int>();
			//definePyList<string>();
			//define_FMTlist<std::unique_ptr<Core::FMTyieldhandler>>();
			define_FMTlist<Core::FMTfork>();



			bp::class_<Core::FMTaction, bp::bases<Core::FMTlist<Core::FMTspec>>>("FMTaction", "@DocString(FMTaction)")
				.def_pickle(FMT_pickle_suite<Core::FMTaction>())
				.def(bp::init<Core::FMTaction>())
				.def(bp::init<const std::string&,const bool&,const bool&>())
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
                .def("dorespectlock",&Core::FMTaction::doRespectLock,
					"@DocString(FMTaction::dorespectlock)")
				.def("useyield", &Core::FMTaction::useyield,
					"@DocString(FMTaction::useyield)")
				.def("push_aggregate", &Core::FMTaction::push_aggregate,
					"@DocString(FMTaction::push_aggregate)")
				.def("getaggregates", &Core::FMTaction::getaggregates,
					"@DocString(FMTaction::getaggregates)")
				.def("update", &Core::FMTaction::update,
					"@DocString(FMTaction::update)")
                .def_readwrite("isresetage",&Core::FMTaction::isResetAge,
					"@DocString(FMTaction::isresetage)")
                .setattr("__hash__",&boost::pyHash<Core::FMTaction>);

			bp::class_<Core::FMTlifespans, bp::bases<Core::FMTlist<int>>>("FMTlifespans", "@DocString(FMTlifespans)")
				.def(bp::init<Core::FMTlifespans>());


			bp::enum_<Core::FMTyldtype>("FMTyldtype")
				.value("FMTageyld", Core::FMTyldtype::FMTageyld)
				.value("FMTtimeyld", Core::FMTyldtype::FMTtimeyld)
				.value("FMTcomplexyld", Core::FMTyldtype::FMTcomplexyld)
				.export_values();

			definePyDict<std::string, std::map<std::string,std::vector<double>>>();

			definePyDict<std::string, std::vector<double>>();

			bp::class_<Core::FMTyieldhandler>("FMTyieldhandler", "@DocString(FMTyieldhandler)")
				.def(bp::init<Core::FMTyieldhandler>())
				.def<bool (Core::FMTyieldhandler::*)(const std::string& yld, const double& value)>("push_data",&Core::FMTyieldhandler::pushData,
					"@DocString(FMTyieldhandler::push_data(const std::string&,const double&))")
				.def("push_base", &Core::FMTyieldhandler::pushBase,
					"@DocString(FMTyieldhandler::push_base)");


			bp::class_<Core::FMTageyieldhandler,bp::bases<Core::FMTyieldhandler>>("FMTageyieldhandler", "@DocString(FMTageyieldhandler)")
				.def(bp::init<const Core::FMTmask&>())
				.def_pickle(FMT_pickle_suite<Core::FMTageyieldhandler>())
				.def("__str__", &Core::FMTageyieldhandler::operator std::string,
					"@DocString(FMTageyieldhandler::operator std::string)")
				.def("setyieldvalues", &Core::FMTageyieldhandler::setYieldValues,
					"@DocString(FMTageyieldhandler::setyieldvalues)");
				
			definePyList<Core::FMTageyieldhandler>();

			bp::class_<Core::FMTtimeyieldhandler,bp::bases<Core::FMTyieldhandler>>("FMTtimeyieldhandler", "@DocString(FMTtimeyieldhandler)")
				.def(bp::init<const Core::FMTmask&>())
				.def_pickle(FMT_pickle_suite<Core::FMTtimeyieldhandler>())
				.def("__str__", &Core::FMTtimeyieldhandler::operator std::string,
					"@DocString(FMTtimeyieldhandler::operator std::string)")
				.def("setyieldvalues", &Core::FMTtimeyieldhandler::setYieldValues,
					"@DocString(FMTtimeyieldhandler::setyieldvalues)");
			
			definePyList<Core::FMTtimeyieldhandler>();

			bp::class_<Core::FMTyields/*, bp::bases<Core::FMTlist<std::unique_ptr<Core::FMTyieldhandler>>>*/>("FMTyields", "@DocString(FMTyields)")
				.def(bp::init<Core::FMTyields>())
				.def_pickle(FMT_pickle_suite<Core::FMTyields>())
				.def("getallyields", &Core::FMTyields::getAllYields,
					"@DocString(FMTyields::getallyields)")
				.def("get", &Core::FMTyields::get,
					"@DocString(FMTyields::get)")
				.def("push_back", &Core::FMTyields::pushBackAgeHandler,
					"@DocString(FMTyields::push_backagehandler)")
				.def("push_back", &Core::FMTyields::pushBackTimeHandler,
					"@DocString(FMTyields::push_backtimehandler)")
				.def("getallyieldnames", &Core::FMTyields::getallyieldnames,
					"@DocString(FMTyields::getallyieldnames)")
				.def("getfromfactor",&Core::FMTyields::getFromFactor,
					"@DocString(FMTyields::getfromfactor)")
				.def("update", &Core::FMTyields::update,
					"@DocString(FMTyields::update)");



			bp::class_<Core::FMTtransition, bp::bases<Core::FMTlist<Core::FMTfork>>>("FMTtransition", "@DocString(FMTtransition)")
				.def_pickle(FMT_pickle_suite<Core::FMTtransition>())
				.def(bp::init<Core::FMTtransition>())
				.def(bp::init<const std::string&>())
                .def("single",&Core::FMTtransition::single,
					"@DocString(FMTtransition::single)")
				.def("main_target", &Core::FMTtransition::mainTarget,
					"@DocString(FMTtransition::main_target)")
				.def("age_after", &Core::FMTtransition::ageAfter,
					"@DocString(FMTtransition::age_after)")
				.def("attribute_targets", &Core::FMTtransition::attributeTargets,
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


				bp::enum_<Core::FMTotar>("FMTotar")
				.value("inventory", Core::FMTotar::inventory)
				.value("actual", Core::FMTotar::actual)
				.value("val", Core::FMTotar::val)
				.value("timeyld", Core::FMTotar::timeyld)
				.value("level", Core::FMTotar::level)
				.export_values();

				bp::class_<Core::FMToutputsource>("FMToutputsource", "@DocString(FMToutputsource)")
					.def(bp::init<const Core::FMTspec&, const Core::FMTmask&,
						Core::FMTotar, const std::string&,
						const std::string&, const int&, const int&>())
					.def(bp::init<Core::FMToutputsource>());

			bp::class_<Core::FMToperator>("FMToperator", "@DocString(FMToperator)")
				.def(bp::init<Core::FMToperator>());


			bp::class_<Core::FMToutputnode>("FMToutputnode", "@DocString(FMToutputnode)")
				.def(bp::init<const Core::FMTmask&,const std::string&>())
				.def("__str__", &Core::FMToutputnode::operator std::string,
					"@DocString(FMToutputnode::operator std::string)");

			define_FMTlist<Core::FMToutputnode>();

			bp::class_<Core::FMToutput>("FMToutput", "@DocString(FMToutput)")
				.def(bp::init<Core::FMToutput>())
				.def(bp::init<const std::string&, const std::string&, const std::string&,
					const std::vector<Core::FMToutputsource>&,
					const std::vector<Core::FMToperator>&>())
				.def_pickle(FMT_pickle_suite<Core::FMTtransition>())
                .def("getname",&Core::FMToutput::getname,
					"@DocString(FMToutput::getname)")
                .def("getdescription",&Core::FMToutput::getDescription,
					"@DocString(FMToutput::getdescription)")
				.def("empty", &Core::FMToutput::empty,
					"@DocString(FMToutput::empty)")
				.def("containslevel",&Core::FMToutput::containsLevel,
					"@DocString(FMToutput::containslevel)")
				.def("islevel", &Core::FMToutput::isLevel,
					"@DocString(FMToutput::islevel)")
				.def("isonlylevel", &Core::FMToutput::isOnlyLevel,
					"@DocString(FMToutput::isonlylevel)")
				.def("getnodes",&Core::FMToutput::getNodes, getnodes_overloads(bp::args("multiplier"), "@DocString(FMToutput::getnodes)"))
				.def("__str__", &Core::FMToutput::operator std::string,
					"@DocString(FMToutput::operator std::string)")
                .def("__eq__",&Core::FMToutput::operator ==,
					"@DocString(FMToutput::operator==)")
                .def("__ne__",&Core::FMToutput::operator !=,
					"@DocString(FMToutput::operator!=)");

			bp::class_<Core::FMTtheme>("FMTtheme", "@DocString(FMTtheme)")
				.def_pickle(FMT_pickle_suite<Core::FMTtheme>())
				.def(bp::init<const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>&,const size_t&,const size_t&, const std::string&>())
				.def(bp::init<Core::FMTtheme>())
				.def("__str__", &Core::FMTtheme::operator std::string,
					"@DocString(FMTtheme::operator std::string)")
				.def("__eq__", &Core::FMTtheme::operator ==,
					"@DocString(FMTtheme::operator==)")
				.def("getaggregates", &Core::FMTtheme::getaggregates,
					"@DocString(Core::FMTtheme::getaggregates)")
				.def("getstart", &Core::FMTtheme::getStart,
					bp::return_value_policy<bp::return_by_value>(),
					"@DocString(Core::FMTtheme::getaggregates)")
				.def("getid", &Core::FMTtheme::getId,
					bp::return_value_policy<bp::return_by_value>(),
					"@DocString(Core::FMTtheme::getid)")
				.def("getattributenames", &Core::FMTtheme::getAttributeNames,
					bp::return_value_policy<bp::return_by_value>(),
					"@DocString(Core::FMTtheme::getattributenames)")
				.def("getattributes", &Core::FMTtheme::getAttributes, getattributes_overloads(bp::args("value","aggregate_source"), "@DocString(FMTtheme::getattributes)"))
				.def("getname", &Core::FMTtheme::getname,
					"@DocString(FMTtheme::getname)");

			bp::class_<Core::FMTconstants>("FMTconstants","@DocString(FMTconstants)");

			bp::class_<Core::FMTschedule>("FMTschedule", "@DocString(FMTschedule)")
				.def_pickle(FMT_pickle_suite<Core::FMTschedule>())
				.def(bp::init<Core::FMTschedule>())
				.def(bp::init<int, std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>>())
				.def("__str__", &Core::FMTschedule::operator std::string,
					"@DocString(FMTschedule::operator std::string)")
				.def("__add__", &Core::FMTschedule::operator +,
					"@DocString(FMTschedule::operator+)")
				.def("actionarea", &Core::FMTschedule::actionArea,
					"@DocString(FMTschedule::actionarea)")
				.def("getperiod", &Core::FMTschedule::getperiod,
					"@DocString(FMTschedule::getperiod)")
				.def("setperiod", &Core::FMTschedule::setPeriod,
					"@DocString(FMTschedule::setperiod)")
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

			bp::class_<Core::FMTconstraint, bp::bases<Core::FMToutput,Core::FMTspec>>("FMTconstraint", "@DocString(FMTconstraint)")
				.def(bp::init<Core::FMTconstraint>())
				.def(bp::init<Core::FMTconstrainttype,const Core::FMToutput&>())
				.def("__str__", &Core::FMTconstraint::operator std::string,
					"@DocString(FMTconstraint::operator std::string)")
				.def("__eq__", &Core::FMTconstraint::operator ==,
					"@DocString(FMTconstraint::operator==)")
				.def("isspatial",&Core::FMTconstraint::isSpatial,
					"@DocString(FMTconstraint::isspatial)")
				.def("setlength", &Core::FMTconstraint::setLength,
					"@DocString(FMTconstraint::setlength)")
				.def("issetfrom", &Core::FMTconstraint::isSetFrom,
					"@DocString(FMTconstraint::issetfrom)")
				.def("setfrom", &Core::FMTconstraint::setFrom,
					"@DocString(FMTconstraint::setfrom)")
				.def("sense",&Core::FMTconstraint::sense,
					"@DocString(FMTconstraint::sense)")
				.def("setrhs", &Core::FMTconstraint::setRhs,
					"@DocString(FMTconstraint::setrhs)")
				.def("getconstrainttype", &Core::FMTconstraint::getConstraintType,
					"@DocString(FMTconstraint::getconstrainttype)")
				.def("getfromreplicate", &Core::FMTconstraint::getFromReplicate,
					"@DocString(FMTconstraint::getfromreplicate)")
				.def("getscheduleweight", &Core::FMTconstraint::getScheduleWeight,
					"@DocString(FMTconstraint::getscheduleweight)")
				.def("setpenalties", &Core::FMTconstraint::setPenalties,
					"@DocString(FMTconstraint::setpenalties)");

			define_FMTlist<Core::FMTconstraint>();
			define_FMTlist<Core::FMTGCBMtransition>();

			bp::class_<Core::FMTyieldrequest>("FMTyieldrequest", "@DocString(FMTyieldrequest)");

			define_FMTlist<Core::FMTyieldrequest>();


    }
}

#endif // PYEXPORTCORE_H_INCLUDED
