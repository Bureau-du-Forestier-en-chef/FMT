/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTVERSION_H_INCLUDED
#define PYEXPORTVERSION_H_INCLUDED

#include <boost/python/docstring_options.hpp>
#include <boost/python/args.hpp>
#include "FMTversion.h"
#include "boost/python.hpp"


namespace Python
{
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(getlicense_overloads,getlicense, 0, 1)


void exportVersion()
{
	namespace bp = boost::python;
	bp::object SpatialModule(bp::handle<>(bp::borrowed(PyImport_AddModule("FMT.Version"))));
	bp::scope().attr("Version") = SpatialModule;
	bp::scope Spatial_scope = SpatialModule;
	bp::scope().attr("__doc__") = ".. module:: Version\n"
		"\n"
		"   :platform: Unix, Windows\n"
		"   :synopsis: Version module.\n"
		"\n";

	boost::python::class_<Version::FMTversion>("FMTversion", "@DocString(FMTversion)")
		.def("getmajor", &Version::FMTversion::getmajor,
			"@DocString(FMTversion::getmajor)").staticmethod("getmajor")
		.def("getminor",&Version::FMTversion::getminor,
			"@DocString(FMTversion::getminor)").staticmethod("getminor")
		.def("getpatch", &Version::FMTversion::getpatch,
			"@DocString(FMTversion::getpatch)").staticmethod("getpatch")
		.def("getversion", &Version::FMTversion::getversion,
			"@DocString(FMTversion::getversion)").staticmethod("getversion")
		.def("isatleast", &Version::FMTversion::isatleast,
			"@DocString(FMTversion::isatleast)").staticmethod("isatleast")
		.def("hasfeature", &Version::FMTversion::hasfeature,
			"@DocString(FMTversion::hasfeature)").staticmethod("hasfeature")
        .def("getlicense",&Version::FMTversion::getlicense,getlicense_overloads(bp::args("french"),"@DocString(FMTversion::getlicense)"));
	}
}


#endif // PYEXPORTVERSION_H_INCLUDED

