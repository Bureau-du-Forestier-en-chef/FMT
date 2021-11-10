/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTEXCEPTIONS_H_INCLUDED
#define REXPORTEXCEPTIONS_H_INCLUDED

#include "FMTdefaultexceptionhandler.hpp"
#include "FMTdebugexceptionhandler.hpp"
#include "FMTquietexceptionhandler.hpp"
#include <Rcpp.h>
#include "Rdefinitions.hpp"


RCPP_EXPOSED_WRAP(Exception::FMTexceptionhandler);
RCPP_EXPOSED_AS(Exception::FMTexceptionhandler);
RCPP_EXPOSED_WRAP(Exception::FMTdefaultexceptionhandler);
RCPP_EXPOSED_AS(Exception::FMTdefaultexceptionhandler);
RCPP_EXPOSED_WRAP(Exception::FMTquietexceptionhandler);
RCPP_EXPOSED_AS(Exception::FMTquietexceptionhandler);
RCPP_EXPOSED_WRAP(Exception::FMTdebugexceptionhandler);
RCPP_EXPOSED_AS(Exception::FMTdebugexceptionhandler);
RCPP_EXPOSED_ENUM_NODECL(Exception::FMTexc);
RCPP_DEFINEVECTOR(Exception::FMTexc);//For vector
RCPP_EXPOSED_ENUM_NODECL(Exception::FMTlev);
RCPP_EXPOSED_ENUM_NODECL(Core::FMTsection);
RCPP_EXPOSED_WRAP(Exception::FMTexception);
RCPP_EXPOSED_AS(Exception::FMTexception);
RCPP_EXPOSED_WRAP(Exception::FMTerror);
RCPP_EXPOSED_AS(Exception::FMTerror);
RCPP_EXPOSED_WRAP(Exception::FMTwarning);
RCPP_EXPOSED_AS(Exception::FMTwarning);

namespace R
{

void exportException()
    {

	Rcpp::class_<Exception::FMTexceptionhandler>("FMTexceptionhandler", "@DocString(FMTexceptionhandler)")
		.constructor("@DocString(FMTexceptionhandler())");


	Rcpp::class_<Exception::FMTdefaultexceptionhandler>("FMTdefaultexceptionhandler", "@DocString(FMTdefaultexceptionhandler)")
		.derives<Exception::FMTexceptionhandler>("FMTexceptionhandler")
		.constructor("@DocString(FMTdefaultexceptionhandler())")
        .method("raise",&Exception::FMTdefaultexceptionhandler::raise,
			"@DocString(FMTdefaultexceptionhandler::raise)");


	Rcpp::class_<Exception::FMTquietexceptionhandler>("FMTquietexceptionhandler", "@DocString(FMTquietexceptionhandler)")
		.derives<Exception::FMTexceptionhandler>("FMTexceptionhandler")
		.constructor("@DocString(FMTquietexceptionhandler())")
        .method("raise",&Exception::FMTquietexceptionhandler::raise,
			"@DocString(FMTquietexceptionhandler::raise)");


	Rcpp::class_<Exception::FMTdebugexceptionhandler>("FMTdebugexceptionhandler", "@DocString(FMTdebugexceptionhandler)")
		.derives<Exception::FMTexceptionhandler>("FMTexceptionhandler")
		.constructor("@DocString(FMTdebugexceptionhandler())")
		.method("raise", &Exception::FMTdebugexceptionhandler::raise,
			"@DocString(FMTdebugexceptionhandler::raise)");


	Rcpp::class_<Exception::FMTexception>("FMTexception")
		.constructor("@DocString(FMTdebugexceptionhandler())")
		.method("gettype", &Exception::FMTexception::gettype,
			"@DocString(FMTdebugexceptionhandler::gettype)")
		.method("getsection", &Exception::FMTexception::getsection,
			"@DocString(FMTdebugexceptionhandler::getsection)")
		.method("what", &Exception::FMTexception::what,
			"@DocString(FMTdebugexceptionhandler::what)");

	Rcpp::class_<Exception::FMTerror>("FMTerror", "@DocString(FMTerror)")
		.derives<Exception::FMTexception>("FMTexception")
		.constructor("@DocString(FMTerror())");

	Rcpp::class_<Exception::FMTwarning>("FMTwarning", "@DocString(FMTwarning)")
		.derives<Exception::FMTexception>("FMTexception")
		.constructor("@DocString(FMTwarning())");
	}

}

#endif // PYEXPORTEXCEPTIONS_H_INCLUDED
