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

	bp::class_<Exception::FMTexceptionhandler>("FMTexceptionhandler", "@DocString(FMTexceptionhandler)");

	bp::class_<Exception::FMTdefaultexceptionhandler, bp::bases<Exception::FMTexceptionhandler>>("FMTdefaultexceptionhandler", "@DocString(FMTdefaultexceptionhandler)")
        .def("raise",&Exception::FMTdefaultexceptionhandler::raise,
			"@DocString(FMTdefaultexceptionhandler::raise)");

	bp::class_<Exception::FMTquietexceptionhandler, bp::bases<Exception::FMTexceptionhandler>>("FMTquietexceptionhandler", "@DocString(FMTquietexceptionhandler)")
        .def("raise",&Exception::FMTquietexceptionhandler::raise,
			"@DocString(FMTquietexceptionhandler::raise)");

	bp::class_<Exception::FMTdebugexceptionhandler, bp::bases<Exception::FMTexceptionhandler>>("FMTdebugexceptionhandler", "@DocString(FMTdebugexceptionhandler)")
		.def("raise", &Exception::FMTdebugexceptionhandler::raise,
			"@DocString(FMTdebugexceptionhandler::raise)");


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

	define_pylist<Exception::FMTexc>();

	bp::class_<Exception::FMTexception>Exceptionclass("FMTexception", "@DocString(FMTexception)");
	Exceptionclass.def("gettype", &Exception::FMTexception::gettype,
		"@DocString(FMTexception::gettype)");
	Exceptionclass.def("getsection", &Exception::FMTexception::getsection,
		"@DocString(FMTexception::getsection)");
	Exceptionclass.def("what", &Exception::FMTexception::what,
		"@DocString(FMTexception::what)");

	bp::class_<Exception::FMTerror, bp::bases<Exception::FMTexception>>Errorclass("FMTerror", "@DocString(FMTerror)");
	bp::class_<Exception::FMTwarning, bp::bases<Exception::FMTexception>>("FMTwarning", "@DocString(FMTwarning)");

	bp::register_exception_translator<Exception::FMTwarning>(&FMTtranslate_warning);
	FMTexceptiontype = Errorclass.ptr();
	bp::register_exception_translator<Exception::FMTerror>(&FMTtranslate_error);

	}

	}

#endif // PYEXPORTEXCEPTIONS_H_INCLUDED
