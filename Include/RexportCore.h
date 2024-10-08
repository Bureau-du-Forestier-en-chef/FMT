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

RCPP_EXPOSED_WRAP(Core::FMTobject);
RCPP_EXPOSED_AS(Core::FMTobject);
RCPP_EXPOSED_WRAP(Core::FMTmask);
RCPP_EXPOSED_AS(Core::FMTmask);
RCPP_DEFINEVECTOR(Core::FMTmask);//For vector
RCPP_EXPOSED_WRAP(Core::FMTdevelopment);
RCPP_EXPOSED_AS(Core::FMTdevelopment);
RCPP_EXPOSED_WRAP(Core::FMTdevelopmentpath);
RCPP_EXPOSED_AS(Core::FMTdevelopmentpath);
RCPP_DEFINEVECTOR(Core::FMTdevelopmentpath);//For vector
RCPP_DEFINEVECTOR(Core::FMTdevelopment);//For vector
RCPP_EXPOSED_WRAP(Core::FMTspec);
RCPP_EXPOSED_AS(Core::FMTspec);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTspec);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTspec);//For vector
RCPP_EXPOSED_WRAP(Core::FMTperbounds);
RCPP_EXPOSED_AS(Core::FMTperbounds);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTperbounds);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTperbounds);//For vector
RCPP_EXPOSED_WRAP(Core::FMTagebounds);
RCPP_EXPOSED_AS(Core::FMTagebounds);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTagebounds);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTagebounds);//For vector
RCPP_EXPOSED_WRAP(Core::FMTyldbounds);
RCPP_EXPOSED_AS(Core::FMTyldbounds);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTyldbounds);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTyldbounds);//For vector
RCPP_EXPOSED_WRAP(Core::FMTlockbounds);
RCPP_EXPOSED_AS(Core::FMTlockbounds);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTlockbounds);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTlockbounds);//For vector
RCPP_EXPOSED_WRAP(Core::FMToutputsource);
RCPP_EXPOSED_AS(Core::FMToutputsource);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMToutputsource);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMToutputsource);//For vector
RCPP_EXPOSED_WRAP(Core::FMToperator);
RCPP_EXPOSED_AS(Core::FMToperator);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMToperator);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMToperator);//For vector
RCPP_EXPOSED_WRAP(Core::FMTtransitionmask);
RCPP_EXPOSED_AS(Core::FMTtransitionmask);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTtransitionmask);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTtransitionmask);//For vector



RCPP_EXPOSED_WRAP(Core::FMTmaskfilter);
RCPP_EXPOSED_AS(Core::FMTmaskfilter);
RCPP_DEFINEVECTOR(Core::FMTmaskfilter);//For vector
RCPP_EXPOSED_WRAP(Core::FMTactualdevelopment);
RCPP_EXPOSED_AS(Core::FMTactualdevelopment);
RCPP_DEFINEVECTOR(Core::FMTactualdevelopment);//For vector
RCPP_EXPOSED_WRAP(Core::FMTfuturdevelopment);
RCPP_EXPOSED_AS(Core::FMTfuturdevelopment);
RCPP_DEFINEVECTOR(Core::FMTfuturdevelopment);//For vector
RCPP_EXPOSED_WRAP(Core::FMTaction);
RCPP_EXPOSED_AS(Core::FMTaction);
RCPP_DEFINEVECTOR(Core::FMTaction);//For vector
RCPP_EXPOSED_WRAP(Core::FMTlifespans);
RCPP_EXPOSED_AS(Core::FMTlifespans);
RCPP_DEFINEPAIR(Core::FMTmask, int);//pair for FMTlist
RCPP_EXPOSED_ENUM_NODECL(Core::FMTyldtype);
RCPP_EXPOSED_WRAP(Core::FMTyieldhandler);
RCPP_EXPOSED_AS(Core::FMTyieldhandler);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTyieldhandler);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTyieldhandler);//For vector
RCPP_EXPOSED_WRAP(Core::FMTtimeyieldhandler);
RCPP_EXPOSED_AS(Core::FMTtimeyieldhandler);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTtimeyieldhandler);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTtimeyieldhandler);//For vector
RCPP_EXPOSED_WRAP(Core::FMTageyieldhandler);
RCPP_EXPOSED_AS(Core::FMTageyieldhandler);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTageyieldhandler);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTageyieldhandler);//For vector
RCPP_EXPOSED_WRAP(Core::FMTyields);
RCPP_EXPOSED_AS(Core::FMTyields);
RCPP_EXPOSED_WRAP(Core::FMTfork);
RCPP_EXPOSED_AS(Core::FMTfork);
RCPP_DEFINEPAIR(Core::FMTmask, Core::FMTfork);//pair for FMTlist
RCPP_DEFINEVECTOR(Core::FMTfork);//For vector
RCPP_EXPOSED_WRAP(Core::FMTtransition);
RCPP_EXPOSED_AS(Core::FMTtransition);
RCPP_DEFINEVECTOR(Core::FMTtransition);//For vector
RCPP_EXPOSED_WRAP(Core::FMToutputnode);
RCPP_EXPOSED_AS(Core::FMToutputnode);
RCPP_DEFINEVECTOR(Core::FMToutputnode);//For vector
RCPP_EXPOSED_WRAP(Core::FMToutput);
RCPP_EXPOSED_AS(Core::FMToutput);
RCPP_DEFINEVECTOR(Core::FMToutput);//For vector
RCPP_EXPOSED_WRAP(Core::FMTtheme);
RCPP_EXPOSED_AS(Core::FMTtheme);
RCPP_DEFINEVECTOR(Core::FMTtheme);//For vector
RCPP_EXPOSED_WRAP(Core::FMTconstants);
RCPP_EXPOSED_AS(Core::FMTconstants);
RCPP_EXPOSED_WRAP(Core::FMTschedule);
RCPP_EXPOSED_AS(Core::FMTschedule);
RCPP_DEFINEVECTOR(Core::FMTschedule);//For vector
RCPP_DEFINELIST(std::vector<Core::FMTschedule>)//For vector of vector
RCPP_EXPOSED_WRAP(Core::FMTGCBMtransition);
RCPP_EXPOSED_AS(Core::FMTGCBMtransition);
RCPP_DEFINEVECTOR(Core::FMTGCBMtransition);//For vector
RCPP_EXPOSED_WRAP(Core::FMTconstraint);
RCPP_EXPOSED_AS(Core::FMTconstraint);
RCPP_DEFINEVECTOR(Core::FMTconstraint);//For vector
RCPP_DEFINEMAP(Core::FMTdevelopment, std::vector<double>);//For dev of doubles
RCPP_DEFINEMAP(std::string, std::vector<double>);//For string of double map
RCPP_DEFINEMAP(std::string, std::vector<Core::FMTdevelopment>);//For string of vector of developements
RCPP_DEFINEMAP(Core::FMTaction , std::map<Core::FMTdevelopment RCPP_COMMA std::vector<double>>);//For action of ...
RCPP_DEFINEMAP(std::string , std::map<std::string RCPP_COMMA std::vector<double>>);//For string of ...
RCPP_DEFINEMAP(std::string, std::string);//map of string definition
RCPP_EXPOSED_ENUM_NODECL(Core::FMTconstrainttype);
RCPP_EXPOSED_ENUM_NODECL(Core::FMTotar);
//RCPP_EXPOSED_ENUM_NODECL(Core::FMTsection);

namespace R
{

void exportCore()
    {
	define_FMTlist<Core::FMTspec>("FMTspeclist");
	define_FMTlist<int>("FMTintlist");
	//define_FMTlist<Core::FMTyieldhandler>("FMTyieldhandlerlist");
	define_FMTlist<Core::FMTfork>("FMTforklist");

	Rcpp::class_<Core::FMTobject>("FMTobject", "@DocString(FMTobject)")
		.constructor("@DocString(FMTobject())")
		.method("setdefaultexceptionhandler", &Core::FMTobject::setdefaultexceptionhandler,
			"@DocString(FMTobject::setdefaultexceptionhandler)")
		.method("setquietexceptionhandler", &Core::FMTobject::setquietexceptionhandler,
			"@DocString(FMTobject::setquietexceptionhandler)")
		.method("setdebugexceptionhandler", &Core::FMTobject::setdebugexceptionhandler,
			"@DocString(FMTobject::setdebugexceptionhandler)")
		.method("setfreeexceptionhandler", &Core::FMTobject::setfreeexceptionhandler,
			"@DocString(FMTobject::setfreeexceptionhandler)")
		.method("disablenestedexceptions", &Core::FMTobject::disablenestedexceptions,
			"@DocString(FMTobject::disablenestedexceptions)")
		.method("enablenestedexceptions", &Core::FMTobject::enablenestedexceptions,
			"@DocString(FMTobject::enablenestedexceptions)")
		.method("setquietlogger", &Core::FMTobject::setquietlogger,
			"@DocString(FMTobject::setquietlogger")
		.method("settasklogger", &Core::FMTobject::settasklogger,
			"@DocString(FMTobject::settasklogger")
		.method("setdefaultlogger", &Core::FMTobject::setdefaultlogger,
			"@DocString(FMTobject::setdefaultlogger")
		.method("setdebuglogger", &Core::FMTobject::setdebuglogger,
			"@DocString(FMTobject::setdebuglogger")
		.method("redirectlogtofile", &Core::FMTobject::redirectlogtofile,
			"@DocString(FMTobject::redirectlogtofile)")
		.method("seterrorstowarnings", &Core::FMTobject::seterrorstowarnings,
			"@DocString(FMTobject::seterrorstowarnings)");

		Rcpp::class_<Core::FMTmask>("FMTmask", "@DocString(FMTmask)")
				.constructor("@DocString(FMTmask())")
				.constructor<std::vector<std::string>,std::vector<Core::FMTtheme>>(
					"@DocString(FMTmask(const std::vector<std::string>&,const std::vector<Core::FMTtheme>&))")
			.constructor<std::string, std::vector<Core::FMTtheme>>(
				"@DocString(FMTmask(const std::string&,const std::vector<Core::FMTtheme>&))")
				.method("len", &Core::FMTmask::operator bool,
					"@DocString(FMTmask::operator bool)")
                .method("str",&Core::FMTmask::operator std::string,
					"@DocString(FMTmask::operator std::string)")
				.method("eq", &Core::FMTmask::operator ==,
					"@DocString(FMTmask::operator==)")
				.method("ne", &Core::FMTmask::operator !=,
					"@DocString(FMTmask::operator!=)")
				.method("lt", &Core::FMTmask::operator <,
					"@DocString(FMTmask::operator<)");

		Rcpp::class_<Core::FMTmaskfilter>("FMTmaskfilter", "@DocString(FMTmaskfilter)")
			.constructor("@DocString(FMTmaskfilter())");

		

		Rcpp::class_<Core::FMTmaskfilter>("FMTmaskfilter", "@DocString(FMTmaskfilter)")
			.constructor("@DocString(FMTmaskfilter())");


		Rcpp::class_<Core::FMTdevelopment>("FMTdevelopment", "@DocString(FMTdevelopment)")
				.constructor("@DocString(FMTdevelopment())")
				.method("getage", &Core::FMTdevelopment::getage,
					"@DocString(FMTdevelopment::getage)")
				.method("getlock", &Core::FMTdevelopment::getlock,
					"@DocString(FMTdevelopment::getlock)")
				.method("getperiod", &Core::FMTdevelopment::getperiod,
					"@DocString(FMTdevelopment::getperiod)")
				.method("getmask", &Core::FMTdevelopment::getmaskcopy,
					"@DocString(FMTdevelopment::getmask)")
				.method("setage", &Core::FMTdevelopment::setage,
					"@DocString(FMTdevelopment::setage)")
				.method("setlock", &Core::FMTdevelopment::setlock,
					"@DocString(FMTdevelopment::setlock)")
				.method("setperiod", &Core::FMTdevelopment::setperiod,
					"@DocString(FMTdevelopment::setperiod)")
				.method("setmask", &Core::FMTdevelopment::setmask,
					"@DocString(FMTdevelopment::setmask)")
				.method("eq", &Core::FMTdevelopment::operator ==,
					"@DocString(FMTdevelopment::operator==)")
				.method("ne", &Core::FMTdevelopment::operator !=,
					"@DocString(FMTdevelopment::operator!=)")
                .method("lt",&Core::FMTdevelopment::operator <,
					"@DocString(FMTdevelopment::operator<)")
                .method("grow",&Core::FMTdevelopment::grow,
					"@DocString(FMTdevelopment::grow)")
                //.method("operable",&Core::FMTdevelopment::operable,
				//	"@DocString(FMTdevelopment::operable)")
                //.method("operate",&Core::FMTdevelopment::operate,
				//	"@DocString(FMTdevelopment::operate)")
				.method("str", &Core::FMTdevelopment::operator std::string,
					"@DocString(FMTdevelopment::operate std::string)");


		Rcpp::class_<Core::FMTagebounds>("FMTagebounds", "@DocString(FMTagebounds)")
			.constructor("@DocString(FMTagebounds())")
			.constructor<Core::FMTsection,int,int>("@DocString(FMTagebounds(FMTsection,const int&,const int&))");

		Rcpp::class_<Core::FMTperbounds>("FMTperbounds", "@DocString(FMTperbounds)")
			.constructor("@DocString(FMTperbounds())")
			.constructor<Core::FMTsection, int, int>("@DocString(FMTperbounds(FMTsection,const int&,const int&))");

		Rcpp::class_<Core::FMTyldbounds>("FMTyldbounds", "@DocString(FMTyldbounds)")
			.constructor("@DocString(FMTyldbounds())")
			.constructor<Core::FMTsection,std::string,double,double>("@DocString(FMTperiodbounds(FMTsection,const std::string&,const double&,const double&))");

		Rcpp::class_<Core::FMTlockbounds>("FMTlockbounds", "@DocString(FMTlockbounds)")
			.constructor("@DocString(FMTlockbounds())")
			.constructor<Core::FMTsection, int, int>("@DocString(FMTlockbounds(FMTsection,const int&,const int&))");

		


		Rcpp::class_<Core::FMTspec>("FMTspec", "@DocString(FMTspec)")
			.constructor("@DocString(FMTspec())")
			.method("setbounds",&Core::FMTspec::setbounds,"@DocString(Core::FMTspec::setbounds())")
			.method("addbounds",
				static_cast<bool(Core::FMTspec::*)(const Core::FMTagebounds&)>(&Core::FMTspec::addbounds),
				"@DocString(Core::FMTspec::addbounds(const FMTagebounds&))")
			.method("addbounds",
				static_cast<bool(Core::FMTspec::*)(const Core::FMTyldbounds&)>(&Core::FMTspec::addbounds),
				"@DocString(Core::FMTspec::addbounds(const FMTyldbounds&))")
			.method("addbounds",
				static_cast<bool(Core::FMTspec::*)(const Core::FMTlockbounds&)>(&Core::FMTspec::addbounds),
				"@DocString(Core::FMTspec::addbounds(const FMTlockbounds&))");
		

		Rcpp::class_<Core::FMTactualdevelopment>("FMTactualdevelopment", "@DocString(FMTactualdevelopment)")
				.derives<Core::FMTdevelopment>("FMTdevelopment")
				.constructor("@DocString(FMTactualdevelopment())")
				.constructor<Core::FMTmask,int,int,double>("@DocString(FMTactualdevelopment(const FMTmask&,const int&,const int&,const int&,const double&))")
                .method("getarea", &Core::FMTactualdevelopment::getarea,
					"@DocString(FMTactualdevelopment::getarea)")
				.method("setarea", &Core::FMTactualdevelopment::setarea,
					"@DocString(FMTactualdevelopment::setarea)")
				.method("eq", &Core::FMTactualdevelopment::operator ==,
					"@DocString(FMTactualdevelopment::operator==)")
				.method("ne", &Core::FMTactualdevelopment::operator !=,
					"@DocString(FMTactualdevelopment::operator!=)")
				.method("lt", &Core::FMTactualdevelopment::operator <,
					"@DocString(FMTactualdevelopment::operator<)");

		Rcpp::class_<Core::FMTfuturdevelopment>("FMTfuturdevelopment", "@DocString(FMTfuturdevelopment)")
			.derives<Core::FMTdevelopment>("FMTdevelopment")
			.constructor("@DocString(FMTfuturdevelopment())");


		Rcpp::class_<Core::FMTaction>("FMTaction","@DocString(FMTaction)")
				.constructor("@DocString(FMTaction())")
				.constructor<std::string,bool,bool>("@DocString(FMTaction(const std::string&,const bool&,const bool&))")
				.derives<Core::FMTlist<Core::FMTspec>>("FMTspeclist")
                .method("__eq__",&Core::FMTaction::operator ==,
					"@DocString(FMTaction::operator==)")
                .method("__ne__",&Core::FMTaction::operator !=,
					"@DocString(FMTaction::operator!=)")
                .method("__lt__",&Core::FMTaction::operator <,
					"@DocString(FMTaction::operator<)")
				.method("__str__", &Core::FMTaction::operator std::string,
					"@DocString(FMTaction::operator std::string)")
                .method("getname",&Core::FMTaction::getname,
					"@DocString(FMTaction::getname)")
                .method("dorespectlock",&Core::FMTaction::dorespectlock,
					"@DocString(FMTaction::dorespectlock)")
                .method("isresetage",&Core::FMTaction::isresetage,
					"@DocString(FMTaction::isresetage)");

			

			Rcpp::class_<Core::FMTlifespans>("FMTlifespans", "@DocString(FMTlifespans)")
				.derives<Core::FMTlist<int>>("FMTintlist")
				.constructor("@DocString(FMTlifespans())");

			

			Rcpp::class_<Core::FMTtransitionmask>("FMTtransitionmask", "@DocString(FMTtransitionmask)")
				.constructor<std::string,std::vector<Core::FMTtheme>,double>("@DocString(FMTtransitionmask(const std::string&,const std::vector<FMTtheme>&,const double&))")
				.constructor("@DocString(FMTtransitionmask())");
			
			Rcpp::class_<Core::FMTfork>("FMTfork", "@DocString(FMTfork)")
				.derives<Core::FMTlist<Core::FMTspec>>("FMTspeclist")
				.method("add", &Core::FMTfork::add,
					"@DocString(FMTfork::add)")
				.constructor("@DocString(FMTfork())");

			Rcpp::class_<Core::FMTyieldhandler>("FMTyieldhandler", "@DocString(FMTyieldhandler)")
				.constructor("@DocString(FMTyieldhandler())")
				.method("push_data",
					static_cast<bool (Core::FMTyieldhandler::*)(const std::string&,const double&)>(&Core::FMTyieldhandler::push_data),
					"@DocString(FMTyieldhandler::push_data(const std::string&,const double&))")
				.method("push_base", &Core::FMTyieldhandler::push_base,
					"@DocString(FMTyieldhandler::push_base)");

			Rcpp::class_<Core::FMTageyieldhandler>("FMTageyieldhandler", "@DocString(FMTageyieldhandler)")
				.derives<Core::FMTyieldhandler>("FMTyieldhandler")
				.constructor("@DocString(FMTageyieldhandler())")
				.constructor<Core::FMTmask>("@DocString(FMTageyieldhandler(const FMTmask&))")
				.method("str", &Core::FMTageyieldhandler::operator std::string,
					"@DocString(FMTageyieldhandler::operator std::string)");
	
			Rcpp::class_<Core::FMTtimeyieldhandler>("FMTtimeyieldhandler", "@DocString(FMTtimeyieldhandler)")
				.derives<Core::FMTyieldhandler>("FMTyieldhandler")
				.constructor("@DocString(FMTtimeyieldhandler())")
				.constructor<Core::FMTmask>("@DocString(FMTtimeyieldhandler(const FMTmask&))")
				.method("str", &Core::FMTtimeyieldhandler::operator std::string,
					"@DocString(FMTtimeyieldhandler::operator std::string)");


			

			Rcpp::class_<Core::FMTyields>("FMTyields", "@DocString(FMTyields)")
				//.derives<Core::FMTlist<Core::FMTyieldhandler>>("FMTyieldhandlerlist")
				.constructor("@DocString(FMTyields())")
				.method("getallyields", &Core::FMTyields::getallyields,
					"@DocString(FMTyields::getallyields)")
				.method("getfromfactor", &Core::FMTyields::getfromfactor,
					"@DocString(FMTyields::getfromfactor)")
				.method("update", &Core::FMTyields::update,
					"@DocString(FMTyields::update)")
				.method("push_back",Core::FMTyields::push_backagehandler,
					"@DocString(Core::FMTyields::push_backagehandler")
				.method("push_back", Core::FMTyields::push_backtimehandler,
					"@DocString(Core::FMTyields::push_backtimehandler");

			
			

			Rcpp::class_<Core::FMTtransition>("FMTtransition", "@DocString(FMTtransition)")
				.derives<Core::FMTlist<Core::FMTfork>>("FMTforklist")
				.constructor("@DocString(FMTtransition())")
				.constructor<std::string>("@DocString(FMTtransition(const std::string&))")
                .method("single",&Core::FMTtransition::single,
					"@DocString(FMTtransition::single)")
				.method("main_target", &Core::FMTtransition::main_target,
					"@DocString(FMTtransition::main_target)")
				.method("age_after", &Core::FMTtransition::age_after,
					"@DocString(FMTtransition::age_after)")
				.method("attribute_targets", &Core::FMTtransition::attribute_targets,
					"@DocString(FMTtransition::attribute_targets)")
				.method("str", &Core::FMTtransition::operator std::string,
					"@DocString(FMTtransition::operator std::string)")
                .method("eq",&Core::FMTtransition::operator ==,
					"@DocString(FMTtransition::operator==)")
                .method("ne",&Core::FMTtransition::operator !=,
					"@DocString(FMTtransition::operator!=)")
                .method("lt",&Core::FMTtransition::operator <,
					"@DocString(FMTtransition::operator<)")
                .method("getname",&Core::FMTtransition::getname,
					"@DocString(FMTtransition::getname)");

			Rcpp::class_<Core::FMToutputsource>("FMToutputsource", "@DocString(FMToutputsource)")
				.constructor<Core::FMTspec, Core::FMTmask,
				Core::FMTotar, std::string,
				std::string, int, int>("@DocString(FMToutputsource())")
				.constructor("@DocString(FMToutputsource())");

			Rcpp::class_<Core::FMToperator>("FMToperator", "@DocString(FMToperator)")
				.constructor("@DocString(FMToperator())");

			Rcpp::class_<Core::FMToutputnode>("FMToutputnode", "@DocString(FMToutputnode)")
				.constructor("@DocString(FMToutputnode())")
				.method("str", &Core::FMToutputnode::operator std::string,
					"@DocString(FMToutputnode::operator std::string)");


			Rcpp::class_<Core::FMToutput>("FMToutput", "@DocString(FMToutput)")
				.constructor("@DocString(FMToutput())")
				.constructor<std::string,std::string,std::string,
					std::vector<Core::FMToutputsource>,
					std::vector<Core::FMToperator>>("@DocString(FMToutput(const std::string&,const std::string&,const int&,std::vector<FMToutputsource>&,std::vector<FMToperator>&))")
				.method("getname",&Core::FMToutput::getname,
					"@DocString(FMToutput::getname)")
				.method("getdescription",&Core::FMToutput::getdescription,
					"@DocString(FMToutput::getdescription)")
				.method("empty", &Core::FMToutput::empty,
					"@DocString(FMToutput::empty)")
				.method("containslevel",&Core::FMToutput::containslevel,
					"@DocString(FMToutput::containslevel)")
				.method("islevel", &Core::FMToutput::islevel,
					"@DocString(FMToutput::islevel)")
				.method("isonlylevel", &Core::FMToutput::isonlylevel,
					"@DocString(FMToutput::isonlylevel)")
				//.method("getnodes",&Core::FMToutput::getnodes,
				//	"@DocString(FMToutput::getnodes)")
				.method("str", &Core::FMToutput::operator std::string,
					"@DocString(FMToutput::operator std::string)")
				.method("eq",&Core::FMToutput::operator ==,
					"@DocString(FMToutput::operator==)")
				.method("ne",&Core::FMToutput::operator !=,
					"@DocString(FMToutput::operator!=)");


			Rcpp::class_<Core::FMTtheme>("FMTtheme", "@DocString(FMTtheme)")
				.constructor("@DocString(FMTtheme())")
				.constructor<std::vector<std::string>,
				std::vector<std::string>,
				std::vector<std::string>,
				std::vector<std::vector<std::string>>,
				size_t,size_t,std::string>(
					"@DocString(const std::vector<std::string>&,const std::vector<std::string>&,const std::vector<std::vector<std::string>>,const std::vector<std::map<std::string, double>>&,const size_t&, const size_t&, const std::string&)")
				.method("str", &Core::FMTtheme::operator std::string,
					"@DocString(FMTtheme::operator std::string)")
				.method("eq", &Core::FMTtheme::operator ==,
					"@DocString(FMTtheme::operator==)")
				.method("getattributes", &Core::FMTtheme::getattributes,
					"@DocString(FMTtheme::getattributes)")
				.method("getname", &Core::FMTtheme::getname,
					"@DocString(FMTtheme::getname)")
				.method("getaggregatesasdataframe", &Core::FMTtheme::getaggregatesasdataframe,
					"@DocString(FMTtheme::getaggregatesasdataframee)")
				.method("getattributesasdataframe", &Core::FMTtheme::getattributesasdataframe,
					"@DocString(FMTtheme::getattributesasdataframe)");


			Rcpp::class_<Core::FMTconstants>("FMTconstants", "@DocString(FMTconstants)")
				.constructor("@DocString(FMTconstants())");

			Rcpp::class_<Core::FMTschedule>("FMTschedule", "@DocString(FMTschedule)")
				.constructor("@DocString(FMTschedule())")
				.constructor<int, std::map<Core::FMTaction, std::map<Core::FMTdevelopment, std::vector<double>>>>(
					"@DocString(FMTschedule(int,std::map<Core::FMTaction,std::map<Core::FMTdevelopment,std::vector<double>>>))")
				.method("str", &Core::FMTschedule::operator std::string,
					"@DocString(FMTschedule::operator std::string)")
				.method("add", &Core::FMTschedule::operator +,
					"@DocString(FMTschedule::operator+)")
				.method("actionarea", &Core::FMTschedule::actionarea,
					"@DocString(FMTschedule::actionarea)")
				.method("getperiod", &Core::FMTschedule::getperiod,
					"@DocString(FMTschedule::getperiod)")
				.method("setperiod", &Core::FMTschedule::setperiod,
					"@DocString(FMTschedule::setperiod)")
				.method("eq", &Core::FMTschedule::operator ==,
					"@DocString(FMTschedule::operator==)")
				.method("ne", &Core::FMTschedule::operator !=,
					"@DocString(FMTschedule::operator!=)")
				.method("empty",&Core::FMTschedule::empty,
					"@DocString(FMTschedule::empty)");

			Rcpp::class_<Core::FMTGCBMtransition>("FMTGCBMtransition", "@DocString(FMTGCBMtransition)")
				.constructor("@DocString(FMTGCBMtransition())")
				.field("name", &Core::FMTGCBMtransition::name,
					"@DocString(FMTGCBMtransition::name)")
				.field("ageafter", &Core::FMTGCBMtransition::ageafter,
					"@DocString(FMTGCBMtransition::ageafter)")
				.field("themes",&Core::FMTGCBMtransition::themes,
					"@DocString(FMTGCBMtransition::themes)");

			Rcpp::class_<Core::FMTconstraint>("FMTconstraint", "@DocString(FMTconstraint)")
				.derives<Core::FMToutput>("FMToutput")
				.constructor("@DocString(FMTconstraint())")
				.constructor<Core::FMTconstrainttype,Core::FMToutput>("@DocString(FMTconstraint(FMTconstrainttype,const FMToutput&))")
				.method("setlength", &Core::FMTconstraint::setlength,
					"@DocString(FMTconstraint::setlength)")
				.method("realconstraintstring", &Core::FMTconstraint::operator std::string,
					"@DocString(FMTconstraint::operator std::string)")
				.method("eq", &Core::FMTconstraint::operator ==,
					"@DocString(FMTconstraint::operator==)");

    }
}

#endif // PYEXPORTCORE_H_INCLUDED
