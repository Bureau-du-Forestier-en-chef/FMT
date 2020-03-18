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

#ifndef REXPORTEXCEPTIONS_H_INCLUDED
#define REXPORTEXCEPTIONS_H_INCLUDED

#include "FMTdefaultexceptionhandler.h"
#include "FMTdebugexceptionhandler.h"
#include "FMTquietexceptionhandler.h"
#include <Rcpp.h>
#include "Rdefinitions.h"


RCPP_EXPOSED_WRAP(Exception::FMTexceptionhandler);
RCPP_EXPOSED_AS(Exception::FMTexceptionhandler);
RCPP_EXPOSED_WRAP(Exception::FMTdefaultexceptionhandler);
RCPP_EXPOSED_AS(Exception::FMTdefaultexceptionhandler);
RCPP_EXPOSED_WRAP(Exception::FMTquietexceptionhandler);
RCPP_EXPOSED_AS(Exception::FMTquietexceptionhandler);
RCPP_EXPOSED_WRAP(Exception::FMTdebugexceptionhandler);
RCPP_EXPOSED_AS(Exception::FMTdebugexceptionhandler);
RCPP_EXPOSED_ENUM_NODECL(Exception::FMTexc);
RCPP_EXPOSED_ENUM_NODECL(Exception::FMTlev);
RCPP_EXPOSED_ENUM_NODECL(Core::FMTwssect);
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
