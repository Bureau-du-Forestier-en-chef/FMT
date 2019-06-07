#ifndef PYEXPORTEXCEPTIONS_H_INCLUDED
#define PYEXPORTEXCEPTIONS_H_INCLUDED

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
    class_<Exception::FMTexceptionhandler>("FMTexceptionhandler",py_FMTexceptionhandler);
    const char* py_FMTdefaultexceptionhandler =
        " ``FMTdefaultexceptionhandler`` class.\n"
        "\n"
        "Default exception handler for FMT\n"
        "\n";
    class_<Exception::FMTdefaultexceptionhandler,bases<Exception::FMTexceptionhandler>>("FMTdefaultexceptionhandler",py_FMTdefaultexceptionhandler)
        .def("raise",&Exception::FMTdefaultexceptionhandler::raise);
    const char* py_FMTquietexceptionhandler =
        " ``FMTquietexceptionhandler`` class.\n"
        "\n"
        "Quiet exception handler for FMT\n"
        "\n";
    class_<Exception::FMTquietexceptionhandler,bases<Exception::FMTexceptionhandler>>("FMTquietexceptionhandler",py_FMTquietexceptionhandler)
        .def("raise",&Exception::FMTquietexceptionhandler::raise);
	const char* py_FMTdebugexceptionhandler =
		" ``FMTdebugexceptionhandler`` class.\n"
		"\n"
		"Debug exception handler for FMT\n"
		"\n";
	class_<Exception::FMTdebugexceptionhandler, bases<Exception::FMTexceptionhandler>>("FMTdebugexceptionhandler", py_FMTdebugexceptionhandler)
		.def("raise", &Exception::FMTdebugexceptionhandler::raise);

    const char* py_FMTobject =
        " ``FMTobject`` class.\n"
        "\n"
        "Base FMTobjectect able to carry exception\n"
        "\n";
	class_<Exception::FMTobject>("FMTobject", py_FMTobject)
		.def("setdefaultexceptionhandler", &Exception::FMTobject::setdefaultexceptionhandler)
		.def("setquietexceptionhandler", &Exception::FMTobject::setquietexceptionhandler)
		.def("setdebugexceptionhandler", &Exception::FMTobject::setdebugexceptionhandler)
		.def("setfreeexceptionhandler", &Exception::FMTobject::setfreeexceptionhandler);

	enum_<Exception::FMTexc>("FMTexc")
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

	enum_<FMTwssect>("FMTwssect")
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

	class_<Exception::FMTexception>Exceptionclass("FMTexception");
	Exceptionclass.def("gettype", &Exception::FMTexception::gettype);
	Exceptionclass.def("getsection", &Exception::FMTexception::getsection);
	Exceptionclass.def("what", &Exception::FMTexception::what);

	class_<Exception::FMTerror, bases<Exception::FMTexception>>Errorclass("FMTerror");
	class_<Exception::FMTwarning, bases<Exception::FMTexception>>("FMTwarning");

    register_exception_translator<Exception::FMTwarning>(&FMTtranslate_warning);
	FMTexceptiontype = Errorclass.ptr();
    register_exception_translator<Exception::FMTerror>(&FMTtranslate_error);



    }

#endif // PYEXPORTEXCEPTIONS_H_INCLUDED
