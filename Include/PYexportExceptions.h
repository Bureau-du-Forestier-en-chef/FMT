/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTEXCEPTIONS_H_INCLUDED
#define PYEXPORTEXCEPTIONS_H_INCLUDED

#include "FMTdefaultexceptionhandler.h"
#include "FMTdebugexceptionhandler.h"
#include "FMTquietexceptionhandler.h"
#include "FMTerror.h"
#include "FMTwarning.h"
#include "FMTexception.h"
#include "boost/python.hpp"

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


	bp::enum_<Exception::FMTexc>("FMTexc")
		.value("FMTinvalid_number", Exception::FMTexc::FMTinvalid_number)
		.value("FMTundefined_attribute", Exception::FMTexc::FMTundefined_attribute)
		.value("FMTundefined_output", Exception::FMTexc::FMTundefined_output)
		.value("FMTundefined_action", Exception::FMTexc::FMTundefined_action)
		.value("FMTinvalid_yield", Exception::FMTexc::FMTinvalid_yield)
		.value("FMTmissingyield", Exception::FMTexc::FMTmissingyield)
		.value("FMTundefined_constant", Exception::FMTexc::FMTundefined_constant)
		.value("FMTinvalid_maskrange", Exception::FMTexc::FMTinvalid_maskrange)
		.value("FMTunsupported_yield", Exception::FMTexc::FMTunsupported_yield)
		.value("FMToutput_missing_operator", Exception::FMTexc::FMToutput_missing_operator)
		.value("FMTinvalidyield_number",Exception::FMTexc::FMTinvalidyield_number)
		.value("FMTundefinedoutput_attribute", Exception::FMTexc::FMTundefinedoutput_attribute)
		.value("FMToutput_too_much_operator", Exception::FMTexc::FMToutput_too_much_operator)
		.value("FMTinvalid_geometry", Exception::FMTexc::FMTinvalid_geometry)
		.value("FMToveridedyield", Exception::FMTexc::FMToveridedyield)
		.value("FMTdeathwithlock", Exception::FMTexc::FMTdeathwithlock)
		.value("FMTsourcetotarget_transition", Exception::FMTexc::FMTsourcetotarget_transition)
		.value("FMTsame_transitiontargets", Exception::FMTexc::FMTsame_transitiontargets)
		.export_values();

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
