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

#ifndef PYEXPORTEXCEPTIONS_H_INCLUDED
#define PYEXPORTEXCEPTIONS_H_INCLUDED

#include "FMTdefaultexceptionhandler.h"
#include "FMTdebugexceptionhandler.h"
#include "FMTquietexceptionhandler.h"

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
    const char* py_FMTexceptionhandler =
        " ``FMTexceptionhandler`` class.\n"
        "\n"
        "Base Exception class for FMT\n"
        "\n";
	bp::class_<Exception::FMTexceptionhandler>("FMTexceptionhandler",py_FMTexceptionhandler);
    const char* py_FMTdefaultexceptionhandler =
        " ``FMTdefaultexceptionhandler`` class.\n"
        "\n"
        "Default exception handler for FMT\n"
        "\n";
	bp::class_<Exception::FMTdefaultexceptionhandler, bp::bases<Exception::FMTexceptionhandler>>("FMTdefaultexceptionhandler",py_FMTdefaultexceptionhandler)
        .def("raise",&Exception::FMTdefaultexceptionhandler::raise);
    const char* py_FMTquietexceptionhandler =
        " ``FMTquietexceptionhandler`` class.\n"
        "\n"
        "Quiet exception handler for FMT\n"
        "\n";
	bp::class_<Exception::FMTquietexceptionhandler, bp::bases<Exception::FMTexceptionhandler>>("FMTquietexceptionhandler",py_FMTquietexceptionhandler)
        .def("raise",&Exception::FMTquietexceptionhandler::raise);
	const char* py_FMTdebugexceptionhandler =
		" ``FMTdebugexceptionhandler`` class.\n"
		"\n"
		"Debug exception handler for FMT\n"
		"\n";
	bp::class_<Exception::FMTdebugexceptionhandler, bp::bases<Exception::FMTexceptionhandler>>("FMTdebugexceptionhandler", py_FMTdebugexceptionhandler)
		.def("raise", &Exception::FMTdebugexceptionhandler::raise);


	bp::enum_<Exception::FMTexc>("FMTexc")
		.value("WSinvalid_number", Exception::FMTexc::WSinvalid_number)
		.value("WSundefined_attribute", Exception::FMTexc::WSundefined_attribute)
		.value("WSundefined_output", Exception::FMTexc::WSundefined_output)
		.value("WSundefined_action", Exception::FMTexc::WSundefined_action)
		.value("WSinvalid_yield", Exception::FMTexc::WSinvalid_yield)
		.value("FMTmissingyield", Exception::FMTexc::FMTmissingyield)
		.value("WSundefined_constant", Exception::FMTexc::WSundefined_constant)
		.value("WSinvalid_maskrange", Exception::FMTexc::WSinvalid_maskrange)
		.value("WSunsupported_yield", Exception::FMTexc::WSunsupported_yield)
		.export_values();

	bp::enum_<FMTwssect>("FMTwssect")
		.value("Control", FMTwssect::Control)
		.value("Landscape", FMTwssect::Landscape)
		.value("Area", FMTwssect::Area)
		.value("Action", FMTwssect::Action)
		.value("Transition", FMTwssect::Transition)
		.value("Yield", FMTwssect::Yield)
		.value("Outputs", FMTwssect::Outputs)
		.value("Optimize", FMTwssect::Optimize)
		.value("Constants", FMTwssect::Constants)
		.value("Schedule", FMTwssect::Schedule)
		.value("Empty", FMTwssect::Empty)
		.export_values();

	bp::class_<Exception::FMTexception>Exceptionclass("FMTexception");
	Exceptionclass.def("gettype", &Exception::FMTexception::gettype);
	Exceptionclass.def("getsection", &Exception::FMTexception::getsection);
	Exceptionclass.def("what", &Exception::FMTexception::what);

	bp::class_<Exception::FMTerror, bp::bases<Exception::FMTexception>>Errorclass("FMTerror");
	bp::class_<Exception::FMTwarning, bp::bases<Exception::FMTexception>>("FMTwarning");

	bp::register_exception_translator<Exception::FMTwarning>(&FMTtranslate_warning);
	FMTexceptiontype = Errorclass.ptr();
	bp::register_exception_translator<Exception::FMTerror>(&FMTtranslate_error);



    }

#endif // PYEXPORTEXCEPTIONS_H_INCLUDED
