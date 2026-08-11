/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTEXCEPTIONS_H_INCLUDED
#define PYEXPORTEXCEPTIONS_H_INCLUDED

#include "FMTDefaultExceptionHandler.h"
#include "FMTDebugExceptionHandler.h"
#include "FMTQuietExceptionHandler.h"
#include "FMTError.h"
#include "FMTWarning.h"
#include "FMTException.h"
#include "boost/python.hpp"
#include "PYdefinitions.h"

namespace Python
{

void exportException()
    {
    namespace bp = boost::python;
    bp::object ExceptionModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.Exception"))));
    bp::scope().attr("Exception") = ExceptionModule;
    bp::scope Exception_scope = ExceptionModule;
    bp::scope().attr("__doc__") = ".. module:: Exception\n"
    "\n"
    "   :platform: Unix, Windows\n"
    "   :synopsis: Module used to handel exceptions.\n"
    "\n";

	//bp::class_<Exception::FMTExceptionHandler>("FMTexceptionhandler", "@DocString(FMTExceptionHandler)");

	bp::class_<Exception::FMTDefaultExceptionHandler/*, bp::bases<Exception::FMTExceptionHandler>*/>("FMTdefaultexceptionhandler", "@DocString(FMTDefaultExceptionHandler)")
        .def("raise",&Exception::FMTDefaultExceptionHandler::raise,
			"@DocString(FMTDefaultExceptionHandler::raise)");

	bp::class_<Exception::FMTQuietExceptionHandler/*, bp::bases<Exception::FMTExceptionHandler >*/>("FMTquietexceptionhandler", "@DocString(FMTQuietExceptionHandler)")
        .def("raise",&Exception::FMTQuietExceptionHandler::raise,
			"@DocString(FMTQuietExceptionHandler::raise)");

	bp::class_<Exception::FMTDebugExceptionHandler/*, bp::bases<Exception::FMTExceptionHandler >*/>("FMTdebugexceptionhandler", "@DocString(FMTDebugExceptionHandler)")
		.def("raise", &Exception::FMTDebugExceptionHandler::raise,
			"@DocString(FMTDebugExceptionHandler::raise)");


	export_any_enum<Exception::FMTexc>("FMTexc");


	definePyList<Exception::FMTexc>();

	bp::class_<Exception::FMTException>Exceptionclass("FMTexception", "@DocString(FMTException)");
	Exceptionclass.def("gettype", &Exception::FMTException::getType,
		"@DocString(FMTException::gettype)");
	Exceptionclass.def("getsection", &Exception::FMTException::getSection,
		"@DocString(FMTException::getsection)");
	Exceptionclass.def("what", &Exception::FMTException::what,
		"@DocString(FMTException::what)");

	bp::class_<Exception::FMTError, bp::bases<Exception::FMTException>>Errorclass("FMTerror", "@DocString(FMTError)");
	bp::class_<Exception::FMTWarning, bp::bases<Exception::FMTException>>("FMTwarning", "@DocString(FMTWarning)");

	bp::register_exception_translator<Exception::FMTWarning>(&FMTtranslate_warning);
	FMTexceptiontype = Errorclass.ptr();
	bp::register_exception_translator<Exception::FMTError>(&FMTtranslate_error);

	}

	}

#endif // PYEXPORTEXCEPTIONS_H_INCLUDED
