/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTCORE_H_INCLUDED
#define PYEXPORTCORE_H_INCLUDED

#include "FMTGCBMTransition.h"
#include "FMTObject.h"
#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTBounds.hpp"
#include "FMTDevelopment.h"
#include "FMTActualDevelopment.h"
#include "FMTFuturDevelopment.h"
#include "FMTAction.h"
#include "FMTTransition.h"
#include "FMTOutput.h"
#include "FMTConstraint.h"
#include "FMTSchedule.h"
#include "FMTYields.h"
#include "FMTConstants.h"
#include "boost/python.hpp"
//#include "FMTData.h"
#include "FMTTimeYieldHandler.h"
#include "FMTAgeYieldHandler.h"
#include "FMTMaskFilter.h"
#include "FMTTransitionMask.h"
#include "FMTDevelopmentPath.h"
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

	bp::class_<Core::FMTObject>("FMTobject", "@DocString(FMTObject)")
		.def("setdefaultexceptionhandler", &Core::FMTObject::setDefaultExceptionHandler,
			"@DocString(FMTObject::setdefaultexceptionhandler)")
		.def("setquietexceptionhandler", &Core::FMTObject::setQuietExceptionHandler,
			"@DocString(FMTObject::setquietexceptionhandler)")
		.def("setdebugexceptionhandler", &Core::FMTObject::setDebugExceptionHandler,
			"@DocString(FMTObject::setdebugexceptionhandler)")
		.def("setfreeexceptionhandler", &Core::FMTObject::setFreeExceptionHandler,
			"@DocString(FMTObject::setfreeexceptionhandler)")
		.def("enablenestedexceptions", &Core::FMTObject::enableNestedExceptions,
			"@DocString(FMTObject::enablenestedexceptions)")
		.def("disablenestedexceptions", &Core::FMTObject::disableNestedExceptions,
			"@DocString(FMTObject::disablenestedexceptions)")
		.def("setdefaultlogger", &Core::FMTObject::setDefaultLogger,
			"@DocString(FMTObject::setdefaultlogger)")
		.def("setdebuglogger", &Core::FMTObject::setDebugLogger,
			"@DocString(FMTObject::setdebuglogger)")
		.def("setquietlogger", &Core::FMTObject::setQuietLogger,
			"@DocString(FMTObject::setquietlogger)")
		.def("settasklogger", &Core::FMTObject::setTaskLogger,
			"@DocString(FMTObject::settasklogger")
		.def("redirectlogtofile", &Core::FMTObject::redirectLogToFile,
			"@DocString(FMTObject::redirectlogtofile)")
		.def("seterrorstowarnings", &Core::FMTObject::setErrorsToWarnings,
			"@DocString(FMTObject::seterrorstowarnings)");

		bp::class_<Core::FMTMaskFilter>("FMTmaskfilter", "@DocString(FMTMaskFilter)");

		definePyList<Core::FMTDevelopmentPath>();
		definePySet<Core::FMTSerie>();
		definePyList<Core::FMTSerie>();
		definePyList<Core::FMTMaskFilter>();
		definePyList<Core::FMTMask>();
        definePyList<Core::FMTDevelopment>();
        definePyList<Core::FMTFuturDevelopment>();
        definePyList<Core::FMTActualDevelopment>();
		definePyDict<std::string, std::vector<Core::FMTDevelopment>>();

        definePyList<Core::FMTTheme>();
        definePyList<Core::FMTAction>();
        definePyList<Core::FMTTransition>();
        definePyList<Core::FMTOutput>();
        definePyList<Core::FMTSchedule>();
		definePyList<std::vector<Core::FMTSchedule>>();
		definePyList<Core::FMTYieldHandler>();
		definePyList<Core::FMTOutputSource>();
		definePyList<Core::FMTOperator>();

        definePyDict<Core::FMTDevelopment, std::vector<double>>();
        definePyDict<Core::FMTAction,std::map<Core::FMTDevelopment, std::vector<double>>>();
		py_pair<Core::FMTAction, std::map<Core::FMTDevelopment, std::vector<double>>>();
		py_pair<Core::FMTAction const, std::map<Core::FMTDevelopment, std::vector<double>>>();

		define_FMTlist<Core::FMTSpec>();
		definePyList<std::vector<std::string>>();

		bool (Core::FMTMask::*issubsetof)(const Core::FMTMask&) const = &Core::FMTMask::isSubsetOf;

		bp::class_<Core::FMTMask>("FMTmask", "@DocString(FMTMask)")
				.def(bp::init<Core::FMTMask>())
				.def_pickle(FMT_pickle_suite<Core::FMTMask>())
				.def(bp::init<const std::vector<std::string>&,const std::vector<Core::FMTTheme>&>())
				.def(bp::init<const std::string&, const std::vector<Core::FMTTheme>&>())
				.def("__len__", &Core::FMTMask::operator bool,
					"@DocString(FMTMask::operator bool)")
                .def("__str__",&Core::FMTMask::operator std::string,
					"@DocString(FMTMask::operator std::string)")
				.def("__eq__", &Core::FMTMask::operator ==,
					"@DocString(FMTMask::operator==)")
				.def("__ne__", &Core::FMTMask::operator !=,
					"@DocString(FMTMask::operator!=)")
				.def("__lt__", &Core::FMTMask::operator <,
					"@DocString(FMTMask::operator<)")
                .def("__hash__",&boost::pyHash<Core::FMTMask>)
				.def("decompose", &Core::FMTMask::decompose,
					"@DocString(FMTMask::decompose)")
				.def("issubsetof",issubsetof,
					"@DocString(FMTMask::issubsetof)")
				.def("presolve",&Core::FMTMask::presolve,
					"@DocString(FMTMask::presolve)")
				.def("postsolve",&Core::FMTMask::postSolve,
					"@DocString(FMTMask::postsolve)");

		bp::class_<Core::FMTDevelopmentPath>("FMTdevelopmentpath", "@DocString(FMTDevelopmentPath)")
			.def(bp::init<Core::FMTDevelopmentPath>())
			.def("getDevelopment", &Core::FMTDevelopmentPath::getDevelopment,
				bp::return_value_policy<bp::return_by_value>(),
				"@DocString(FMTDevelopmentPath::getDevelopment)")
			.def("getProportion", &Core::FMTDevelopmentPath::getProportion,
				bp::return_value_policy<bp::return_by_value>(),
				"@DocString(FMTDevelopmentPath::getProportion)");

		bp::class_<Core::FMTSerie>("FMTSerie", "@DocString(FMTSerie)")
			.def(bp::init<Core::FMTSerie>())
			.def("getLength", &Core::FMTSerie::getLength,
				bp::return_value_policy<bp::return_by_value>(),
				"@DocString(FMTSerie::getLength)")
			.def("getSerie", &Core::FMTSerie::getSerie,
				bp::return_value_policy<bp::return_by_value>(),
				"@DocString(FMTSerie::getSerie)");

		

		bp::class_<Core::FMTDevelopment>("FMTdevelopment", "@DocString(FMTDevelopment)")
				.def_pickle(FMT_pickle_suite<Core::FMTDevelopment>())
			    .def(bp::init<Core::FMTDevelopment>())
				.def("getage", &Core::FMTDevelopment::getAge,
					"@DocString(FMTDevelopment::getage)")
				.def("getlock", &Core::FMTDevelopment::getLock,
					"@DocString(FMTDevelopment::getlock)")
				.def("getperiod", &Core::FMTDevelopment::getPeriod,
					"@DocString(FMTDevelopment::getperiod)")
				.def("getmask", &Core::FMTDevelopment::getMaskCopy,
					"@DocString(FMTDevelopment::getmask)")
				.def("setage", &Core::FMTDevelopment::setAge,
					"@DocString(FMTDevelopment::setage)")
				.def("setlock", &Core::FMTDevelopment::setLock,
					"@DocString(FMTDevelopment::setlock)")
				.def("setperiod", &Core::FMTDevelopment::setPeriod,
					"@DocString(FMTDevelopment::setperiod)")
				.def("setmask", &Core::FMTDevelopment::setMask,
					"@DocString(FMTDevelopment::setmask)")
				.def("__eq__", &Core::FMTDevelopment::operator ==,
					"@DocString(FMTDevelopment::operator==)")
				.def("__ne__", &Core::FMTDevelopment::operator !=,
					"@DocString(FMTDevelopment::operator!=)")
                .def("__lt__",&Core::FMTDevelopment::operator <,
					"@DocString(FMTDevelopment::operator<")
                .def("grow",&Core::FMTDevelopment::grow,
					"@DocString(FMTDevelopment::grow)")
                .def("operable",&Core::FMTDevelopment::operable,operable_overloads(bp::args("action,yields,yieldrequest"),"@DocString(FMTDevelopment::operable)"))
                .def("operate",&Core::FMTDevelopment::operate,
					"@DocString(FMTDevelopment::operate)")
				 .def("getyieldrequest",&Core::FMTDevelopment::getYieldRequest,getyieldrequest_overloads("@DocString(FMTDevelopment::getyieldrequest)"))
				.def("__str__", &Core::FMTDevelopment::operator std::string,
					"@DocString(FMTDevelopment::operator std::string)")
                .setattr("__hash__",&boost::pyHash<Core::FMTDevelopment>);



		bp::class_<Core::FMTAgeBounds>("FMTagebounds", "@DocString(FMTAgeBounds)")
			.def(bp::init<Core::FMTAgeBounds>())
			.def(bp::init<Core::FMTsection, int, int>());

		bp::class_<Core::FMTPerBounds>("FMTperbounds", "@DocString(FMTPerBounds)")
			.def(bp::init<Core::FMTPerBounds>())
			.def(bp::init<Core::FMTsection, int, int>());

		bp::class_<Core::FMTYldBounds>("FMTyldbounds", "@DocString(FMTYldBounds)")
			.def(bp::init<Core::FMTYldBounds>())
			.def(bp::init<Core::FMTsection, std::string, double, double>());

		bp::class_<Core::FMTLockBounds>("FMTlockbounds", "@DocString(FMTLockBounds)")
			.def(bp::init<Core::FMTLockBounds>())
			.def(bp::init<Core::FMTsection, int, int>());

		bp::class_<Core::FMTSpec>("FMTspec", "@DocString(FMTSpec)")
			.def(bp::init<Core::FMTSpec>())
			.def("getperiodlowerbound", &Core::FMTSpec::getPeriodLowerBound,
				"@DocString(FMTSpec::getperiodlowerbound)")
			.def("getperiodupperbound", &Core::FMTSpec::getPeriodUpperBound,
				"@DocString(FMTSpec::getperiodupperbound)")
			.def("setbounds", &Core::FMTSpec::setBounds, "@DocString(Core::FMTSpec::setbounds())")
			.def<bool(Core::FMTSpec::*)(const Core::FMTAgeBounds&)>("addbounds", &Core::FMTSpec::addBounds,
				"@DocString(Core::FMTSpec::addbounds(const FMTAgeBounds&))")
			.def<bool(Core::FMTSpec::*)(const Core::FMTYldBounds&)>("addbounds",&Core::FMTSpec::addBounds,
				"@DocString(Core::FMTSpec::addbounds(const FMTYldBounds&))")
			.def<bool(Core::FMTSpec::*)(const Core::FMTLockBounds&)>("addbounds",&Core::FMTSpec::addBounds,
				"@DocString(Core::FMTSpec::addbounds(const FMTLockBounds&))");

		bp::class_<Core::FMTTransitionMask>("FMTtransitionmask", "@DocString(FMTTransitionMask)")
			.def(bp::init<const std::string&, const std::vector<Core::FMTTheme>&, const double&>())
			.def(bp::init<Core::FMTTransitionMask>());

		bp::class_<Core::FMTFork, bp::bases<Core::FMTList<Core::FMTSpec>>>("FMTfork", "@DocString(FMTFork)")
			.def(bp::init<Core::FMTFork>())
			.def("add", &Core::FMTFork::add,
				"@DocString(FMTFork::add)");




		bp::class_<Core::FMTActualDevelopment, bp::bases<Core::FMTDevelopment>>("FMTactualdevelopment", "@DocString(FMTActualDevelopment)")
				.def(bp::init<const Core::FMTMask&, const int&, const int&, const double&>())
				.def(bp::init<Core::FMTActualDevelopment>())
			    .def("getarea", &Core::FMTActualDevelopment::getArea,
					"@DocString(FMTActualDevelopment::getarea)")
				.def("setarea", &Core::FMTActualDevelopment::setArea,
					"@DocString(FMTActualDevelopment::setarea)")
				.def("__eq__", &Core::FMTActualDevelopment::operator ==,
					"@DocString(FMTActualDevelopment::operator==)")
				.def("__ne__", &Core::FMTActualDevelopment::operator !=,
					"@DocString(FMTActualDevelopment::operator!=)")
				.def("__lt__", &Core::FMTActualDevelopment::operator <,
					"@DocString(FMTActualDevelopment::operator<)");

		bp::class_<Core::FMTFuturDevelopment, bp::bases<Core::FMTDevelopment>>("FMTfuturdevelopment", "@DocString(FMTFuturDevelopment)");

            //Need preprocessor here
            

			define_FMTlist<int>();
			//definePyList<string>();
			//define_FMTlist<std::unique_ptr<Core::FMTYieldHandler>>();
			define_FMTlist<Core::FMTFork>();



			bp::class_<Core::FMTAction, bp::bases<Core::FMTList<Core::FMTSpec>>>("FMTaction", "@DocString(FMTAction)")
				.def_pickle(FMT_pickle_suite<Core::FMTAction>())
				.def(bp::init<Core::FMTAction>())
				.def(bp::init<const std::string&,const bool&,const bool&>())
                .def("__eq__",&Core::FMTAction::operator ==,
					"@DocString(FMTAction::operator==)")
                .def("__ne__",&Core::FMTAction::operator !=,
					"@DocString(FMTAction::operator!=)")
                .def("__lt__",&Core::FMTAction::operator <,
					"@DocString(FMTAction::operator<)")
				.def("__str__", &Core::FMTAction::operator std::string,
					"@DocString(FMTAction::operator std::string)")
                .def("getname",&Core::FMTAction::getName,
					"@DocString(FMTAction::getname)")
                .def("dorespectlock",&Core::FMTAction::doRespectLock,
					"@DocString(FMTAction::dorespectlock)")
				.def("useyield", &Core::FMTAction::useYield,
					"@DocString(FMTAction::useyield)")
				.def("push_aggregate", &Core::FMTAction::pushAggregate,
					"@DocString(FMTAction::push_aggregate)")
				.def("getaggregates", &Core::FMTAction::getAggregates,
					"@DocString(FMTAction::getaggregates)")
				.def("update", &Core::FMTAction::update,
					"@DocString(FMTAction::update)")
                .def_readwrite("isresetage",&Core::FMTAction::isResetAge,
					"@DocString(FMTAction::isresetage)")
                .setattr("__hash__",&boost::pyHash<Core::FMTAction>);

			bp::class_<Core::FMTLifespans, bp::bases<Core::FMTList<int>>>("FMTlifespans", "@DocString(FMTLifespans)")
				.def(bp::init<Core::FMTLifespans>());


			bp::enum_<Core::FMTyldtype>("FMTyldtype")
				.value("FMTageyld", Core::FMTyldtype::FMTageyld)
				.value("FMTtimeyld", Core::FMTyldtype::FMTtimeyld)
				.value("FMTcomplexyld", Core::FMTyldtype::FMTcomplexyld)
				.export_values();

			definePyDict<std::string, std::map<std::string,std::vector<double>>>();

			definePyDict<std::string, std::vector<double>>();

			bp::class_<Core::FMTYieldHandler>("FMTyieldhandler", "@DocString(FMTYieldHandler)")
				.def(bp::init<Core::FMTYieldHandler>())
				.def<bool (Core::FMTYieldHandler::*)(const std::string& yld, const double& value)>("push_data",&Core::FMTYieldHandler::pushData,
					"@DocString(FMTYieldHandler::push_data(const std::string&,const double&))")
				.def("push_base", &Core::FMTYieldHandler::pushBase,
					"@DocString(FMTYieldHandler::push_base)");


			bp::class_<Core::FMTAgeYieldHandler,bp::bases<Core::FMTYieldHandler>>("FMTageyieldhandler", "@DocString(FMTAgeYieldHandler)")
				.def(bp::init<const Core::FMTMask&>())
				.def_pickle(FMT_pickle_suite<Core::FMTAgeYieldHandler>())
				.def("__str__", &Core::FMTAgeYieldHandler::operator std::string,
					"@DocString(FMTAgeYieldHandler::operator std::string)")
				.def("setyieldvalues", &Core::FMTAgeYieldHandler::setYieldValues,
					"@DocString(FMTAgeYieldHandler::setyieldvalues)");
				
			definePyList<Core::FMTAgeYieldHandler>();

			bp::class_<Core::FMTTimeYieldHandler,bp::bases<Core::FMTYieldHandler>>("FMTtimeyieldhandler", "@DocString(FMTTimeYieldHandler)")
				.def(bp::init<const Core::FMTMask&>())
				.def_pickle(FMT_pickle_suite<Core::FMTTimeYieldHandler>())
				.def("__str__", &Core::FMTTimeYieldHandler::operator std::string,
					"@DocString(FMTTimeYieldHandler::operator std::string)")
				.def("setyieldvalues", &Core::FMTTimeYieldHandler::setYieldValues,
					"@DocString(FMTTimeYieldHandler::setyieldvalues)");
			
			definePyList<Core::FMTTimeYieldHandler>();

			bp::class_<Core::FMTYields/*, bp::bases<Core::FMTList<std::unique_ptr<Core::FMTYieldHandler>>>*/>("FMTyields", "@DocString(FMTYields)")
				.def(bp::init<Core::FMTYields>())
				.def_pickle(FMT_pickle_suite<Core::FMTYields>())
				.def("getallyields", &Core::FMTYields::getAllYields,
					"@DocString(FMTYields::getallyields)")
				.def("get", &Core::FMTYields::get,
					"@DocString(FMTYields::get)")
				.def("push_back", &Core::FMTYields::pushBackAgeHandler,
					"@DocString(FMTYields::push_backagehandler)")
				.def("push_back", &Core::FMTYields::pushBackTimeHandler,
					"@DocString(FMTYields::push_backtimehandler)")
				.def("getallyieldnames", &Core::FMTYields::getAllYieldNames,
					"@DocString(FMTYields::getallyieldnames)")
				.def("getfromfactor",&Core::FMTYields::getFromFactor,
					"@DocString(FMTYields::getfromfactor)")
				.def("update", &Core::FMTYields::update,
					"@DocString(FMTYields::update)");



			bp::class_<Core::FMTTransition, bp::bases<Core::FMTList<Core::FMTFork>>>("FMTtransition", "@DocString(FMTTransition)")
				.def_pickle(FMT_pickle_suite<Core::FMTTransition>())
				.def(bp::init<Core::FMTTransition>())
				.def(bp::init<const std::string&>())
                .def("single",&Core::FMTTransition::single,
					"@DocString(FMTTransition::single)")
				.def("main_target", &Core::FMTTransition::mainTarget,
					"@DocString(FMTTransition::main_target)")
				.def("age_after", &Core::FMTTransition::ageAfter,
					"@DocString(FMTTransition::age_after)")
				.def("attribute_targets", &Core::FMTTransition::attributeTargets,
					"@DocString(FMTTransition::attribute_targets)")
				.def("__str__", &Core::FMTTransition::operator std::string,
					"@DocString(FMTTransition::operator std::string)")
                .def("__eq__",&Core::FMTTransition::operator ==,
					"@DocString(FMTTransition::operator==)")
                .def("__ne__",&Core::FMTTransition::operator !=,
					"@DocString(FMTTransition::operator!=)")
                .def("__lt__",&Core::FMTTransition::operator <,
					"@DocString(FMTTransition::operator<)")
                .def("getname",&Core::FMTTransition::getName,
					"@DocString(FMTTransition::getname)");


				bp::enum_<Core::FMTotar>("FMTotar")
				.value("inventory", Core::FMTotar::inventory)
				.value("actual", Core::FMTotar::actual)
				.value("val", Core::FMTotar::val)
				.value("timeyld", Core::FMTotar::timeyld)
				.value("level", Core::FMTotar::level)
				.export_values();

				bp::class_<Core::FMTOutputSource>("FMToutputsource", "@DocString(FMTOutputSource)")
					.def(bp::init<const Core::FMTSpec&, const Core::FMTMask&,
						Core::FMTotar, const std::string&,
						const std::string&, const int&, const int&>())
					.def(bp::init<Core::FMTOutputSource>());

			bp::class_<Core::FMTOperator>("FMToperator", "@DocString(FMTOperator)")
				.def(bp::init<Core::FMTOperator>());


			bp::class_<Core::FMTOutputNode>("FMToutputnode", "@DocString(FMTOutputNode)")
				.def(bp::init<const Core::FMTMask&,const std::string&>())
				.def("__str__", &Core::FMTOutputNode::operator std::string,
					"@DocString(FMTOutputNode::operator std::string)");

			define_FMTlist<Core::FMTOutputNode>();

			bp::class_<Core::FMTOutput>("FMToutput", "@DocString(FMTOutput)")
				.def(bp::init<Core::FMTOutput>())
				.def(bp::init<const std::string&, const std::string&, const std::string&,
					const std::vector<Core::FMTOutputSource>&,
					const std::vector<Core::FMTOperator>&>())
				.def_pickle(FMT_pickle_suite<Core::FMTTransition>())
                .def("getname",&Core::FMTOutput::getName,
					"@DocString(FMTOutput::getname)")
                .def("getdescription",&Core::FMTOutput::getDescription,
					"@DocString(FMTOutput::getdescription)")
				.def("empty", &Core::FMTOutput::empty,
					"@DocString(FMTOutput::empty)")
				.def("containslevel",&Core::FMTOutput::containsLevel,
					"@DocString(FMTOutput::containslevel)")
				.def("islevel", &Core::FMTOutput::isLevel,
					"@DocString(FMTOutput::islevel)")
				.def("isonlylevel", &Core::FMTOutput::isOnlyLevel,
					"@DocString(FMTOutput::isonlylevel)")
				.def("getnodes",&Core::FMTOutput::getNodes, getnodes_overloads(bp::args("multiplier"), "@DocString(FMTOutput::getnodes)"))
				.def("__str__", &Core::FMTOutput::operator std::string,
					"@DocString(FMTOutput::operator std::string)")
                .def("__eq__",&Core::FMTOutput::operator ==,
					"@DocString(FMTOutput::operator==)")
                .def("__ne__",&Core::FMTOutput::operator !=,
					"@DocString(FMTOutput::operator!=)");

			bp::class_<Core::FMTTheme>("FMTtheme", "@DocString(FMTTheme)")
				.def_pickle(FMT_pickle_suite<Core::FMTTheme>())
				.def(bp::init<const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>&,const size_t&,const size_t&, const std::string&>())
				.def(bp::init<Core::FMTTheme>())
				.def("__str__", &Core::FMTTheme::operator std::string,
					"@DocString(FMTTheme::operator std::string)")
				.def("__eq__", &Core::FMTTheme::operator ==,
					"@DocString(FMTTheme::operator==)")
				.def("getaggregates", &Core::FMTTheme::getAggregates,
					"@DocString(Core::FMTTheme::getaggregates)")
				.def("getstart", &Core::FMTTheme::getStart,
					bp::return_value_policy<bp::return_by_value>(),
					"@DocString(Core::FMTTheme::getaggregates)")
				.def("getid", &Core::FMTTheme::getId,
					bp::return_value_policy<bp::return_by_value>(),
					"@DocString(Core::FMTTheme::getid)")
				.def("getattributenames", &Core::FMTTheme::getAttributeNames,
					bp::return_value_policy<bp::return_by_value>(),
					"@DocString(Core::FMTTheme::getattributenames)")
				.def("getattributes", &Core::FMTTheme::getAttributes, getattributes_overloads(bp::args("value","aggregate_source"), "@DocString(FMTTheme::getattributes)"))
				.def("getname", &Core::FMTTheme::getName,
					"@DocString(FMTTheme::getname)");

			bp::class_<Core::FMTConstants>("FMTconstants","@DocString(FMTConstants)");

			bp::class_<Core::FMTSchedule>("FMTschedule", "@DocString(FMTSchedule)")
				.def_pickle(FMT_pickle_suite<Core::FMTSchedule>())
				.def(bp::init<Core::FMTSchedule>())
				.def(bp::init<int, std::map<Core::FMTAction, std::map<Core::FMTDevelopment, std::vector<double>>>>())
				.def("__str__", &Core::FMTSchedule::operator std::string,
					"@DocString(FMTSchedule::operator std::string)")
				.def("__add__", &Core::FMTSchedule::operator +,
					"@DocString(FMTSchedule::operator+)")
				.def("actionarea", &Core::FMTSchedule::actionArea,
					"@DocString(FMTSchedule::actionarea)")
				.def("getperiod", &Core::FMTSchedule::getPeriod,
					"@DocString(FMTSchedule::getperiod)")
				.def("setperiod", &Core::FMTSchedule::setPeriod,
					"@DocString(FMTSchedule::setperiod)")
				.def("__eq__", &Core::FMTSchedule::operator ==,
					"@DocString(FMTSchedule::operator==)")
				.def("__ne__", &Core::FMTSchedule::operator !=,
					"@DocString(FMTSchedule::operator!=)")
				.def("__iter__", boost::python::iterator<Core::FMTSchedule>())
				.def("empty",&Core::FMTSchedule::empty,
					"@DocString(FMTSchedule::empty)");

			bp::class_<Core::FMTGCBMTransition>("FMTGCBMtransition", "@DocString(FMTGCBMTransition)")
				.def_readwrite("name", &Core::FMTGCBMTransition::name,
					"@DocString(FMTGCBMTransition::name)")
				.def_readwrite("ageafter", &Core::FMTGCBMTransition::ageafter,
					"@DocString(FMTGCBMTransition::ageafter)")
				.add_property("themes", bp::make_getter(&Core::FMTGCBMTransition::themes, bp::return_value_policy<bp::return_by_value>()),
					bp::make_setter(&Core::FMTGCBMTransition::themes, bp::return_value_policy<bp::return_by_value>()));

			bp::class_<Core::FMTConstraint, bp::bases<Core::FMTOutput,Core::FMTSpec>>("FMTconstraint", "@DocString(FMTConstraint)")
				.def(bp::init<Core::FMTConstraint>())
				.def(bp::init<Core::FMTconstrainttype,const Core::FMTOutput&>())
				.def("__str__", &Core::FMTConstraint::operator std::string,
					"@DocString(FMTConstraint::operator std::string)")
				.def("__eq__", &Core::FMTConstraint::operator ==,
					"@DocString(FMTConstraint::operator==)")
				.def("isspatial",&Core::FMTConstraint::isSpatial,
					"@DocString(FMTConstraint::isspatial)")
				.def("setlength", &Core::FMTConstraint::setLength,
					"@DocString(FMTConstraint::setlength)")
				.def("issetfrom", &Core::FMTConstraint::isSetFrom,
					"@DocString(FMTConstraint::issetfrom)")
				.def("setfrom", &Core::FMTConstraint::setFrom,
					"@DocString(FMTConstraint::setfrom)")
				.def("sense",&Core::FMTConstraint::sense,
					"@DocString(FMTConstraint::sense)")
				.def("setrhs", &Core::FMTConstraint::setRhs,
					"@DocString(FMTConstraint::setrhs)")
				.def("getconstrainttype", &Core::FMTConstraint::getConstraintType,
					"@DocString(FMTConstraint::getconstrainttype)")
				.def("getfromreplicate", &Core::FMTConstraint::getFromReplicate,
					"@DocString(FMTConstraint::getfromreplicate)")
				.def("getscheduleweight", &Core::FMTConstraint::getScheduleWeight,
					"@DocString(FMTConstraint::getscheduleweight)")
				.def("setpenalties", &Core::FMTConstraint::setPenalties,
					"@DocString(FMTConstraint::setpenalties)");

			define_FMTlist<Core::FMTConstraint>();
			define_FMTlist<Core::FMTGCBMTransition>();

			bp::class_<Core::FMTYieldRequest>("FMTyieldrequest", "@DocString(FMTYieldRequest)");

			define_FMTlist<Core::FMTYieldRequest>();


    }
}

#endif // PYEXPORTCORE_H_INCLUDED
