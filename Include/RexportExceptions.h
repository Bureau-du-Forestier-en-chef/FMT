/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTEXCEPTIONS_H_INCLUDED
#define REXPORTEXCEPTIONS_H_INCLUDED

#include "FMTdefaultexceptionhandler.h"
#include "FMTdebugexceptionhandler.h"
#include "FMTquietexceptionhandler.h"
#include "FMTexceptionhandler.h"
#include <Rcpp.h>
#include "Rdefinitions.h"
#include "FMTerror.h"
#include "FMTwarning.h"
#include "FMTexception.h"

/*
RCPP_EXPOSED_WRAP(Exception::FMTExceptionHandler);
RCPP_EXPOSED_AS(Exception::FMTExceptionHandler);*/
RCPP_EXPOSED_WRAP(Exception::FMTDefaultExceptionHandler);
RCPP_EXPOSED_AS(Exception::FMTDefaultExceptionHandler);
RCPP_EXPOSED_WRAP(Exception::FMTQuietExceptionHandler);
RCPP_EXPOSED_AS(Exception::FMTQuietExceptionHandler);
RCPP_EXPOSED_WRAP(Exception::FMTDebugExceptionHandler);
RCPP_EXPOSED_AS(Exception::FMTDebugExceptionHandler);
RCPP_EXPOSED_ENUM_NODECL(Exception::FMTexc);
RCPP_DEFINEVECTOR(Exception::FMTexc);//For vector
RCPP_EXPOSED_ENUM_NODECL(Exception::FMTlev);
RCPP_EXPOSED_ENUM_NODECL(Core::FMTsection);
RCPP_EXPOSED_WRAP(Exception::FMTException);
RCPP_EXPOSED_AS(Exception::FMTException);
RCPP_EXPOSED_WRAP(Exception::FMTError);
RCPP_EXPOSED_AS(Exception::FMTError);
RCPP_EXPOSED_WRAP(Exception::FMTWarning);
RCPP_EXPOSED_AS(Exception::FMTWarning);

namespace R
{

void exportException()
    {

	/*Rcpp::class_<Exception::FMTExceptionHandler>("FMTexceptionhandler", "@DocString(FMTExceptionHandler)")
		.constructor("@DocString(FMTExceptionHandler())");*/


	Rcpp::class_<Exception::FMTDefaultExceptionHandler>("FMTdefaultexceptionhandler", "@DocString(FMTDefaultExceptionHandler)")
		//.derives<Exception::FMTExceptionHandler>("FMTexceptionhandler")
		.constructor("@DocString(FMTDefaultExceptionHandler())")
        .method("raise",&Exception::FMTDefaultExceptionHandler::raise,
			"@DocString(FMTDefaultExceptionHandler::raise)");


	Rcpp::class_<Exception::FMTQuietExceptionHandler>("FMTquietexceptionhandler", "@DocString(FMTQuietExceptionHandler)")
		//.derives<Exception::FMTExceptionHandler>("FMTexceptionhandler")
		.constructor("@DocString(FMTQuietExceptionHandler())")
        .method("raise",&Exception::FMTQuietExceptionHandler::raise,
			"@DocString(FMTQuietExceptionHandler::raise)");


	Rcpp::class_<Exception::FMTDebugExceptionHandler>("FMTdebugexceptionhandler", "@DocString(FMTDebugExceptionHandler)")
		//.derives<Exception::FMTExceptionHandler>("FMTexceptionhandler")
		.constructor("@DocString(FMTDebugExceptionHandler())")
		.method("raise", &Exception::FMTDebugExceptionHandler::raise,
			"@DocString(FMTDebugExceptionHandler::raise)");


	Rcpp::class_<Exception::FMTException>("FMTexception")
		.constructor("@DocString(FMTDebugExceptionHandler())")
		.method("gettype", &Exception::FMTException::getType,
			"@DocString(FMTDebugExceptionHandler::gettype)")
		.method("getsection", &Exception::FMTException::getSection,
			"@DocString(FMTDebugExceptionHandler::getsection)")
		.method("what", &Exception::FMTException::what,
			"@DocString(FMTDebugExceptionHandler::what)");

	Rcpp::class_<Exception::FMTError>("FMTerror", "@DocString(FMTError)")
		.derives<Exception::FMTException>("FMTexception")
		.constructor("@DocString(FMTError())");

	Rcpp::class_<Exception::FMTWarning>("FMTwarning", "@DocString(FMTWarning)")
		.derives<Exception::FMTException>("FMTexception")
		.constructor("@DocString(FMTWarning())");
	}

}

#endif // PYEXPORTEXCEPTIONS_H_INCLUDED
