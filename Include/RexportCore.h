/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTCORE_H_INCLUDED
#define REXPORTCORE_H_INCLUDED

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
#include "Rdefinitions.h"
#include "FMTyieldhandler.h"
#include "FMTtimeyieldhandler.h"
#include "FMTageyieldhandler.h"
#include "FMTdevelopmentpath.h"
#include "FMTdata.h"
#include "FMTmaskfilter.h"
#include "FMTtransitionmask.h"

#include <vector>
#include <Rcpp.h>

RCPP_EXPOSED_WRAP(Core::FMTObject);
RCPP_EXPOSED_AS(Core::FMTObject);
RCPP_EXPOSED_WRAP(Core::FMTMask);
RCPP_EXPOSED_AS(Core::FMTMask);
RCPP_DEFINEVECTOR(Core::FMTMask);//For vector
RCPP_EXPOSED_WRAP(Core::FMTDevelopment);
RCPP_EXPOSED_AS(Core::FMTDevelopment);
RCPP_EXPOSED_WRAP(Core::FMTDevelopmentPath);
RCPP_EXPOSED_AS(Core::FMTDevelopmentPath);
RCPP_DEFINEVECTOR(Core::FMTDevelopmentPath);//For vector
RCPP_DEFINEVECTOR(Core::FMTDevelopment);//For vector
RCPP_EXPOSED_WRAP(Core::FMTSpec);
RCPP_EXPOSED_AS(Core::FMTSpec);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTSpec);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTSpec);//For vector
RCPP_EXPOSED_WRAP(Core::FMTPerBounds);
RCPP_EXPOSED_AS(Core::FMTPerBounds);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTPerBounds);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTPerBounds);//For vector
RCPP_EXPOSED_WRAP(Core::FMTAgeBounds);
RCPP_EXPOSED_AS(Core::FMTAgeBounds);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTAgeBounds);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTAgeBounds);//For vector
RCPP_EXPOSED_WRAP(Core::FMTYldBounds);
RCPP_EXPOSED_AS(Core::FMTYldBounds);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTYldBounds);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTYldBounds);//For vector
RCPP_EXPOSED_WRAP(Core::FMTLockBounds);
RCPP_EXPOSED_AS(Core::FMTLockBounds);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTLockBounds);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTLockBounds);//For vector
RCPP_EXPOSED_WRAP(Core::FMTOutputSource);
RCPP_EXPOSED_AS(Core::FMTOutputSource);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTOutputSource);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTOutputSource);//For vector
RCPP_EXPOSED_WRAP(Core::FMTOperator);
RCPP_EXPOSED_AS(Core::FMTOperator);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTOperator);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTOperator);//For vector
RCPP_EXPOSED_WRAP(Core::FMTTransitionMask);
RCPP_EXPOSED_AS(Core::FMTTransitionMask);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTTransitionMask);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTTransitionMask);//For vector



RCPP_EXPOSED_WRAP(Core::FMTMaskFilter);
RCPP_EXPOSED_AS(Core::FMTMaskFilter);
RCPP_DEFINEVECTOR(Core::FMTMaskFilter);//For vector
RCPP_EXPOSED_WRAP(Core::FMTActualDevelopment);
RCPP_EXPOSED_AS(Core::FMTActualDevelopment);
RCPP_DEFINEVECTOR(Core::FMTActualDevelopment);//For vector
RCPP_EXPOSED_WRAP(Core::FMTFuturDevelopment);
RCPP_EXPOSED_AS(Core::FMTFuturDevelopment);
RCPP_DEFINEVECTOR(Core::FMTFuturDevelopment);//For vector
RCPP_EXPOSED_WRAP(Core::FMTAction);
RCPP_EXPOSED_AS(Core::FMTAction);
RCPP_DEFINEVECTOR(Core::FMTAction);//For vector
RCPP_EXPOSED_WRAP(Core::FMTLifespans);
RCPP_EXPOSED_AS(Core::FMTLifespans);
RCPP_DEFINEPAIR(Core::FMTMask, int);//pair for FMTList
RCPP_EXPOSED_ENUM_NODECL(Core::FMTyldtype);
RCPP_EXPOSED_WRAP(Core::FMTYieldHandler);
RCPP_EXPOSED_AS(Core::FMTYieldHandler);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTYieldHandler);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTYieldHandler);//For vector
RCPP_EXPOSED_WRAP(Core::FMTTimeYieldHandler);
RCPP_EXPOSED_AS(Core::FMTTimeYieldHandler);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTTimeYieldHandler);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTTimeYieldHandler);//For vector
RCPP_EXPOSED_WRAP(Core::FMTAgeYieldHandler);
RCPP_EXPOSED_AS(Core::FMTAgeYieldHandler);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTAgeYieldHandler);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTAgeYieldHandler);//For vector
RCPP_EXPOSED_WRAP(Core::FMTYields);
RCPP_EXPOSED_AS(Core::FMTYields);
RCPP_EXPOSED_WRAP(Core::FMTFork);
RCPP_EXPOSED_AS(Core::FMTFork);
RCPP_DEFINEPAIR(Core::FMTMask, Core::FMTFork);//pair for FMTList
RCPP_DEFINEVECTOR(Core::FMTFork);//For vector
RCPP_EXPOSED_WRAP(Core::FMTTransition);
RCPP_EXPOSED_AS(Core::FMTTransition);
RCPP_DEFINEVECTOR(Core::FMTTransition);//For vector
RCPP_EXPOSED_WRAP(Core::FMTOutputNode);
RCPP_EXPOSED_AS(Core::FMTOutputNode);
RCPP_DEFINEVECTOR(Core::FMTOutputNode);//For vector
RCPP_EXPOSED_WRAP(Core::FMTOutput);
RCPP_EXPOSED_AS(Core::FMTOutput);
RCPP_DEFINEVECTOR(Core::FMTOutput);//For vector
RCPP_EXPOSED_WRAP(Core::FMTTheme);
RCPP_EXPOSED_AS(Core::FMTTheme);
RCPP_DEFINEVECTOR(Core::FMTTheme);//For vector
RCPP_EXPOSED_WRAP(Core::FMTConstants);
RCPP_EXPOSED_AS(Core::FMTConstants);
RCPP_EXPOSED_WRAP(Core::FMTSchedule);
RCPP_EXPOSED_AS(Core::FMTSchedule);
RCPP_DEFINEVECTOR(Core::FMTSchedule);//For vector
RCPP_DEFINELIST(std::vector<Core::FMTSchedule>)//For vector of vector
RCPP_EXPOSED_WRAP(Core::FMTGCBMTransition);
RCPP_EXPOSED_AS(Core::FMTGCBMTransition);
RCPP_DEFINEVECTOR(Core::FMTGCBMTransition);//For vector
RCPP_EXPOSED_WRAP(Core::FMTConstraint);
RCPP_EXPOSED_AS(Core::FMTConstraint);
RCPP_DEFINEVECTOR(Core::FMTConstraint);//For vector
RCPP_DEFINEMAP(Core::FMTDevelopment, std::vector<double>);//For dev of doubles
RCPP_DEFINEMAP(std::string, std::vector<double>);//For string of double map
RCPP_DEFINEMAP(std::string, std::vector<Core::FMTDevelopment>);//For string of vector of developments
RCPP_DEFINEMAP(Core::FMTAction , std::map<Core::FMTDevelopment RCPP_COMMA std::vector<double>>);//For action of ...
RCPP_DEFINEMAP(std::string , std::map<std::string RCPP_COMMA std::vector<double>>);//For string of ...
RCPP_DEFINEMAP(std::string, std::string);//map of string definition
RCPP_EXPOSED_ENUM_NODECL(Core::FMTconstrainttype);
RCPP_EXPOSED_ENUM_NODECL(Core::FMTotar);
//RCPP_EXPOSED_ENUM_NODECL(Core::FMTsection);

namespace R
{

void exportCore()
    {
	define_FMTlist<Core::FMTSpec>("FMTspeclist");
	define_FMTlist<int>("FMTintlist");
	//define_FMTlist<Core::FMTYieldHandler>("FMTyieldhandlerlist");
	define_FMTlist<Core::FMTFork>("FMTforklist");

	Rcpp::class_<Core::FMTObject>("FMTobject", "@DocString(FMTObject)")
		.constructor("@DocString(FMTObject())")
		.method("setdefaultexceptionhandler", &Core::FMTObject::setDefaultExceptionHandler,
			"@DocString(FMTObject::setdefaultexceptionhandler)")
		.method("setquietexceptionhandler", &Core::FMTObject::setQuietExceptionHandler,
			"@DocString(FMTObject::setquietexceptionhandler)")
		.method("setdebugexceptionhandler", &Core::FMTObject::setDebugExceptionHandler,
			"@DocString(FMTObject::setdebugexceptionhandler)")
		.method("setfreeexceptionhandler", &Core::FMTObject::setFreeExceptionHandler,
			"@DocString(FMTObject::setfreeexceptionhandler)")
		.method("disablenestedexceptions", &Core::FMTObject::disableNestedExceptions,
			"@DocString(FMTObject::disablenestedexceptions)")
		.method("enablenestedexceptions", &Core::FMTObject::enableNestedExceptions,
			"@DocString(FMTObject::enablenestedexceptions)")
		.method("setquietlogger", &Core::FMTObject::setQuietLogger,
			"@DocString(FMTObject::setquietlogger)")
		.method("settasklogger", &Core::FMTObject::setTaskLogger,
			"@DocString(FMTObject::settasklogger)")
		.method("setdefaultlogger", &Core::FMTObject::setDefaultLogger,
			"@DocString(FMTObject::setdefaultlogger)")
		.method("setdebuglogger", &Core::FMTObject::setDebugLogger,
			"@DocString(FMTObject::setdebuglogger)")
		.method("redirectlogtofile", &Core::FMTObject::redirectLogToFile,
			"@DocString(FMTObject::redirectlogtofile)")
		.method("seterrorstowarnings", &Core::FMTObject::setErrorsToWarnings,
			"@DocString(FMTObject::seterrorstowarnings)")
		.method("setmaxwarningsbeforesilenced", &Core::FMTObject::setMaxWarningsBeforeSilenced,
			"@DocString(FMTObject::setmaxwarningsbeforesilenced)");

		Rcpp::class_<Core::FMTMask>("FMTmask", "@DocString(FMTMask)")
				.constructor("@DocString(FMTMask())")
				.constructor<std::vector<std::string>,std::vector<Core::FMTTheme>>(
					"@DocString(FMTMask(const std::vector<std::string>&,const std::vector<Core::FMTTheme>&))")
			.constructor<std::string, std::vector<Core::FMTTheme>>(
				"@DocString(FMTMask(const std::string&,const std::vector<Core::FMTTheme>&))")
				.method("len", &Core::FMTMask::operator bool,
					"@DocString(FMTMask::operator bool)")
                .method("str",&Core::FMTMask::operator std::string,
					"@DocString(FMTMask::operator std::string)")
				.method("eq", &Core::FMTMask::operator ==,
					"@DocString(FMTMask::operator==)")
				.method("ne", &Core::FMTMask::operator !=,
					"@DocString(FMTMask::operator!=)")
				.method("lt", &Core::FMTMask::operator <,
					"@DocString(FMTMask::operator<)");

		Rcpp::class_<Core::FMTMaskFilter>("FMTmaskfilter", "@DocString(FMTMaskFilter)")
			.constructor("@DocString(FMTMaskFilter())");

		

		Rcpp::class_<Core::FMTMaskFilter>("FMTmaskfilter", "@DocString(FMTMaskFilter)")
			.constructor("@DocString(FMTMaskFilter())");


		Rcpp::class_<Core::FMTDevelopment>("FMTdevelopment", "@DocString(FMTDevelopment)")
				.constructor("@DocString(FMTDevelopment())")
				.method("getage", &Core::FMTDevelopment::getAge,
					"@DocString(FMTDevelopment::getage)")
				.method("getlock", &Core::FMTDevelopment::getLock,
					"@DocString(FMTDevelopment::getlock)")
				.method("getperiod", &Core::FMTDevelopment::getPeriod,
					"@DocString(FMTDevelopment::getperiod)")
				.method("getmask", &Core::FMTDevelopment::getMaskCopy,
					"@DocString(FMTDevelopment::getmask)")
				.method("setage", &Core::FMTDevelopment::setAge,
					"@DocString(FMTDevelopment::setage)")
				.method("setlock", &Core::FMTDevelopment::setLock,
					"@DocString(FMTDevelopment::setlock)")
				.method("setperiod", &Core::FMTDevelopment::setPeriod,
					"@DocString(FMTDevelopment::setperiod)")
				.method("setmask", &Core::FMTDevelopment::setMask,
					"@DocString(FMTDevelopment::setmask)")
				.method("eq", &Core::FMTDevelopment::operator ==,
					"@DocString(FMTDevelopment::operator==)")
				.method("ne", &Core::FMTDevelopment::operator !=,
					"@DocString(FMTDevelopment::operator!=)")
                .method("lt",&Core::FMTDevelopment::operator <,
					"@DocString(FMTDevelopment::operator<)")
                .method("grow",&Core::FMTDevelopment::grow,
					"@DocString(FMTDevelopment::grow)")
                //.method("operable",&Core::FMTDevelopment::operable,
				//	"@DocString(FMTDevelopment::operable)")
                //.method("operate",&Core::FMTDevelopment::operate,
				//	"@DocString(FMTDevelopment::operate)")
				.method("str", &Core::FMTDevelopment::operator std::string,
					"@DocString(FMTDevelopment::operate std::string)");


		Rcpp::class_<Core::FMTAgeBounds>("FMTagebounds", "@DocString(FMTAgeBounds)")
			.constructor("@DocString(FMTAgeBounds())")
			.constructor<Core::FMTsection,int,int>("@DocString(FMTAgeBounds(FMTsection,const int&,const int&))");

		Rcpp::class_<Core::FMTPerBounds>("FMTperbounds", "@DocString(FMTPerBounds)")
			.constructor("@DocString(FMTPerBounds())")
			.constructor<Core::FMTsection, int, int>("@DocString(FMTPerBounds(FMTsection,const int&,const int&))");

		Rcpp::class_<Core::FMTYldBounds>("FMTyldbounds", "@DocString(FMTYldBounds)")
			.constructor("@DocString(FMTYldBounds())")
			.constructor<Core::FMTsection,std::string,double,double>("@DocString(FMTperiodbounds(FMTsection,const std::string&,const double&,const double&))");

		Rcpp::class_<Core::FMTLockBounds>("FMTlockbounds", "@DocString(FMTLockBounds)")
			.constructor("@DocString(FMTLockBounds())")
			.constructor<Core::FMTsection, int, int>("@DocString(FMTLockBounds(FMTsection,const int&,const int&))");

		


		Rcpp::class_<Core::FMTSpec>("FMTspec", "@DocString(FMTSpec)")
			.constructor("@DocString(FMTSpec())")
			.method("setbounds",&Core::FMTSpec::setBounds,"@DocString(Core::FMTSpec::setbounds())")
			.method("addbounds",
				static_cast<bool(Core::FMTSpec::*)(const Core::FMTAgeBounds&)>(&Core::FMTSpec::addBounds),
				"@DocString(Core::FMTSpec::addbounds(const FMTAgeBounds&))")
			.method("addbounds",
				static_cast<bool(Core::FMTSpec::*)(const Core::FMTYldBounds&)>(&Core::FMTSpec::addBounds),
				"@DocString(Core::FMTSpec::addbounds(const FMTYldBounds&))")
			.method("addbounds",
				static_cast<bool(Core::FMTSpec::*)(const Core::FMTLockBounds&)>(&Core::FMTSpec::addBounds),
				"@DocString(Core::FMTSpec::addbounds(const FMTLockBounds&))");
		

		Rcpp::class_<Core::FMTActualDevelopment>("FMTactualdevelopment", "@DocString(FMTActualDevelopment)")
				.derives<Core::FMTDevelopment>("FMTdevelopment")
				.constructor("@DocString(FMTActualDevelopment())")
				.constructor<Core::FMTMask,int,int,double>("@DocString(FMTActualDevelopment(const FMTMask&,const int&,const int&,const int&,const double&))")
                .method("getarea", &Core::FMTActualDevelopment::getArea,
					"@DocString(FMTActualDevelopment::getarea)")
				.method("setarea", &Core::FMTActualDevelopment::setArea,
					"@DocString(FMTActualDevelopment::setarea)")
				.method("eq", &Core::FMTActualDevelopment::operator ==,
					"@DocString(FMTActualDevelopment::operator==)")
				.method("ne", &Core::FMTActualDevelopment::operator !=,
					"@DocString(FMTActualDevelopment::operator!=)")
				.method("lt", &Core::FMTActualDevelopment::operator <,
					"@DocString(FMTActualDevelopment::operator<)");

		Rcpp::class_<Core::FMTFuturDevelopment>("FMTfuturdevelopment", "@DocString(FMTFuturDevelopment)")
			.derives<Core::FMTDevelopment>("FMTdevelopment")
			.constructor("@DocString(FMTFuturDevelopment())");


		Rcpp::class_<Core::FMTAction>("FMTaction","@DocString(FMTAction)")
				.constructor("@DocString(FMTAction())")
				.constructor<std::string,bool,bool>("@DocString(FMTAction(const std::string&,const bool&,const bool&))")
				.derives<Core::FMTList<Core::FMTSpec>>("FMTspeclist")
                .method("__eq__",&Core::FMTAction::operator ==,
					"@DocString(FMTAction::operator==)")
                .method("__ne__",&Core::FMTAction::operator !=,
					"@DocString(FMTAction::operator!=)")
                .method("__lt__",&Core::FMTAction::operator <,
					"@DocString(FMTAction::operator<)")
				.method("__str__", &Core::FMTAction::operator std::string,
					"@DocString(FMTAction::operator std::string)")
                .method("getname",&Core::FMTAction::getName,
					"@DocString(FMTAction::getname)")
                .method("dorespectlock",&Core::FMTAction::doRespectLock,
					"@DocString(FMTAction::dorespectlock)")
                .method("isresetage",&Core::FMTAction::isResetAge,
					"@DocString(FMTAction::isresetage)");

			

			Rcpp::class_<Core::FMTLifespans>("FMTlifespans", "@DocString(FMTLifespans)")
				.derives<Core::FMTList<int>>("FMTintlist")
				.constructor("@DocString(FMTLifespans())");

			

			Rcpp::class_<Core::FMTTransitionMask>("FMTtransitionmask", "@DocString(FMTTransitionMask)")
				.constructor<std::string,std::vector<Core::FMTTheme>,double>("@DocString(FMTTransitionMask(const std::string&,const std::vector<FMTTheme>&,const double&))")
				.constructor("@DocString(FMTTransitionMask())");
			
			Rcpp::class_<Core::FMTFork>("FMTfork", "@DocString(FMTFork)")
				.derives<Core::FMTList<Core::FMTSpec>>("FMTspeclist")
				.method("add", &Core::FMTFork::add,
					"@DocString(FMTFork::add)")
				.constructor("@DocString(FMTFork())");

			Rcpp::class_<Core::FMTYieldHandler>("FMTyieldhandler", "@DocString(FMTYieldHandler)")
				.constructor("@DocString(FMTYieldHandler())")
				.method("push_data",
					static_cast<bool (Core::FMTYieldHandler::*)(const std::string&,const double&)>(&Core::FMTYieldHandler::pushData),
					"@DocString(FMTYieldHandler::push_data(const std::string&,const double&))")
				.method("push_base", &Core::FMTYieldHandler::pushBase,
					"@DocString(FMTYieldHandler::push_base)");

			Rcpp::class_<Core::FMTAgeYieldHandler>("FMTageyieldhandler", "@DocString(FMTAgeYieldHandler)")
				.derives<Core::FMTYieldHandler>("FMTyieldhandler")
				.constructor("@DocString(FMTAgeYieldHandler())")
				.constructor<Core::FMTMask>("@DocString(FMTAgeYieldHandler(const FMTMask&))")
				.method("str", &Core::FMTAgeYieldHandler::operator std::string,
					"@DocString(FMTAgeYieldHandler::operator std::string)");
	
			Rcpp::class_<Core::FMTTimeYieldHandler>("FMTtimeyieldhandler", "@DocString(FMTTimeYieldHandler)")
				.derives<Core::FMTYieldHandler>("FMTyieldhandler")
				.constructor("@DocString(FMTTimeYieldHandler())")
				.constructor<Core::FMTMask>("@DocString(FMTTimeYieldHandler(const FMTMask&))")
				.method("str", &Core::FMTTimeYieldHandler::operator std::string,
					"@DocString(FMTTimeYieldHandler::operator std::string)");


			

			Rcpp::class_<Core::FMTYields>("FMTyields", "@DocString(FMTYields)")
				//.derives<Core::FMTList<Core::FMTYieldHandler>>("FMTyieldhandlerlist")
				.constructor("@DocString(FMTYields())")
				.method("getallyields", &Core::FMTYields::getAllYields,
					"@DocString(FMTYields::getallyields)")
				.method("getfromfactor", &Core::FMTYields::getFromFactor,
					"@DocString(FMTYields::getfromfactor)")
				.method("update", &Core::FMTYields::update,
					"@DocString(FMTYields::update)")
				.method("push_back",Core::FMTYields::pushBackAgeHandler,
					"@DocString(Core::FMTYields::push_backagehandler")
				.method("push_back", Core::FMTYields::pushBackTimeHandler,
					"@DocString(Core::FMTYields::push_backtimehandler");

			
			

			Rcpp::class_<Core::FMTTransition>("FMTtransition", "@DocString(FMTTransition)")
				.derives<Core::FMTList<Core::FMTFork>>("FMTforklist")
				.constructor("@DocString(FMTTransition())")
				.constructor<std::string>("@DocString(FMTTransition(const std::string&))")
                .method("single",&Core::FMTTransition::single,
					"@DocString(FMTTransition::single)")
				.method("main_target", &Core::FMTTransition::mainTarget,
					"@DocString(FMTTransition::main_target)")
				.method("age_after", &Core::FMTTransition::ageAfter,
					"@DocString(FMTTransition::age_after)")
				.method("attribute_targets", &Core::FMTTransition::attributeTargets,
					"@DocString(FMTTransition::attribute_targets)")
				.method("str", &Core::FMTTransition::operator std::string,
					"@DocString(FMTTransition::operator std::string)")
                .method("eq",&Core::FMTTransition::operator ==,
					"@DocString(FMTTransition::operator==)")
                .method("ne",&Core::FMTTransition::operator !=,
					"@DocString(FMTTransition::operator!=)")
                .method("lt",&Core::FMTTransition::operator <,
					"@DocString(FMTTransition::operator<)")
                .method("getname",&Core::FMTTransition::getName,
					"@DocString(FMTTransition::getname)");

			Rcpp::class_<Core::FMTOutputSource>("FMToutputsource", "@DocString(FMTOutputSource)")
				.constructor<Core::FMTSpec, Core::FMTMask,
				Core::FMTotar, std::string,
				std::string, int, int>("@DocString(FMTOutputSource())")
				.constructor("@DocString(FMTOutputSource())");

			Rcpp::class_<Core::FMTOperator>("FMToperator", "@DocString(FMTOperator)")
				.constructor("@DocString(FMTOperator())");

			Rcpp::class_<Core::FMTOutputNode>("FMToutputnode", "@DocString(FMTOutputNode)")
				.constructor("@DocString(FMTOutputNode())")
				.method("str", &Core::FMTOutputNode::operator std::string,
					"@DocString(FMTOutputNode::operator std::string)");


			Rcpp::class_<Core::FMTOutput>("FMToutput", "@DocString(FMTOutput)")
				.constructor("@DocString(FMTOutput())")
				.constructor<std::string,std::string,std::string,
					std::vector<Core::FMTOutputSource>,
					std::vector<Core::FMTOperator>>("@DocString(FMTOutput(const std::string&,const std::string&,const int&,std::vector<FMTOutputSource>&,std::vector<FMTOperator>&))")
				.method("getname",&Core::FMTOutput::getName,
					"@DocString(FMTOutput::getname)")
				.method("getdescription",&Core::FMTOutput::getDescription,
					"@DocString(FMTOutput::getdescription)")
				.method("empty", &Core::FMTOutput::empty,
					"@DocString(FMTOutput::empty)")
				.method("containslevel",&Core::FMTOutput::containsLevel,
					"@DocString(FMTOutput::containslevel)")
				.method("islevel", &Core::FMTOutput::isLevel,
					"@DocString(FMTOutput::islevel)")
				.method("isonlylevel", &Core::FMTOutput::isOnlyLevel,
					"@DocString(FMTOutput::isonlylevel)")
				//.method("getNodes",&Core::FMTOutput::getNodes,
				//	"@DocString(FMTOutput::getNodes)")
				.method("str", &Core::FMTOutput::operator std::string,
					"@DocString(FMTOutput::operator std::string)")
				.method("eq",&Core::FMTOutput::operator ==,
					"@DocString(FMTOutput::operator==)")
				.method("ne",&Core::FMTOutput::operator !=,
					"@DocString(FMTOutput::operator!=)");


			Rcpp::class_<Core::FMTTheme>("FMTtheme", "@DocString(FMTTheme)")
				.constructor("@DocString(FMTTheme())")
				.constructor<std::vector<std::string>,
				std::vector<std::string>,
				std::vector<std::string>,
				std::vector<std::vector<std::string>>,
				size_t,size_t,std::string>(
					"@DocString(const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>,const std::vector<std::map<std::string, double>>&,const size_t&, const size_t&, const std::string&)")
				.method("str", &Core::FMTTheme::operator std::string,
					"@DocString(FMTTheme::operator std::string)")
				.method("eq", &Core::FMTTheme::operator ==,
					"@DocString(FMTTheme::operator==)")
				.method("getattributes", &Core::FMTTheme::getAttributes,
					"@DocString(FMTTheme::getattributes)")
				.method("getname", &Core::FMTTheme::getName,
					"@DocString(FMTTheme::getname)")
				.method("getaggregatesasdataframe", &Core::FMTTheme::getAggregatesAsDataFrame,
					"@DocString(FMTTheme::getaggregatesasdataframee)")
				.method("getattributesasdataframe", &Core::FMTTheme::getAttributesAsDataFrame,
					"@DocString(FMTTheme::getattributesasdataframe)");


			Rcpp::class_<Core::FMTConstants>("FMTconstants", "@DocString(FMTConstants)")
				.constructor("@DocString(FMTConstants())");

			Rcpp::class_<Core::FMTSchedule>("FMTschedule", "@DocString(FMTSchedule)")
				.constructor("@DocString(FMTSchedule())")
				.constructor<int, std::map<Core::FMTAction, std::map<Core::FMTDevelopment, std::vector<double>>>>(
					"@DocString(FMTSchedule(int,std::map<Core::FMTAction,std::map<Core::FMTDevelopment,std::vector<double>>>))")
				.method("str", &Core::FMTSchedule::operator std::string,
					"@DocString(FMTSchedule::operator std::string)")
				.method("add", &Core::FMTSchedule::operator +,
					"@DocString(FMTSchedule::operator+)")
				.method("actionarea", &Core::FMTSchedule::actionArea,
					"@DocString(FMTSchedule::actionarea)")
				.method("getperiod", &Core::FMTSchedule::getPeriod,
					"@DocString(FMTSchedule::getperiod)")
				.method("setperiod", &Core::FMTSchedule::setPeriod,
					"@DocString(FMTSchedule::setperiod)")
				.method("eq", &Core::FMTSchedule::operator ==,
					"@DocString(FMTSchedule::operator==)")
				.method("ne", &Core::FMTSchedule::operator !=,
					"@DocString(FMTSchedule::operator!=)")
				.method("empty",&Core::FMTSchedule::empty,
					"@DocString(FMTSchedule::empty)");

			Rcpp::class_<Core::FMTGCBMTransition>("FMTGCBMtransition", "@DocString(FMTGCBMTransition)")
				.constructor("@DocString(FMTGCBMTransition())")
				.field("name", &Core::FMTGCBMTransition::name,
					"@DocString(FMTGCBMTransition::name)")
				.field("ageafter", &Core::FMTGCBMTransition::ageafter,
					"@DocString(FMTGCBMTransition::ageafter)")
				.field("themes",&Core::FMTGCBMTransition::themes,
					"@DocString(FMTGCBMTransition::themes)");

			Rcpp::class_<Core::FMTConstraint>("FMTconstraint", "@DocString(FMTConstraint)")
				.derives<Core::FMTOutput>("FMToutput")
				.constructor("@DocString(FMTConstraint())")
				.constructor<Core::FMTconstrainttype,Core::FMTOutput>("@DocString(FMTConstraint(FMTconstrainttype,const FMTOutput&))")
				.method("setlength", &Core::FMTConstraint::setLength,
					"@DocString(FMTConstraint::setlength)")
				.method("realconstraintstring", &Core::FMTConstraint::operator std::string,
					"@DocString(FMTConstraint::operator std::string)")
				.method("eq", &Core::FMTConstraint::operator ==,
					"@DocString(FMTConstraint::operator==)");

    }
}

#endif // PYEXPORTCORE_H_INCLUDED
