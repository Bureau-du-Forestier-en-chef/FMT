/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PYEXPORTVERSION_H_INCLUDED
#define PYEXPORTVERSION_H_INCLUDED

#include <boost/python/docstring_options.hpp>
#include <boost/python/args.hpp>
#include "FMTVersion.h"
#include "boost/python.hpp"


namespace Python
{
BOOST_PYTHON_FUNCTION_OVERLOADS(getlicense_overloads, Version::FMTVersion::getLicense, 0, 1)
BOOST_PYTHON_FUNCTION_OVERLOADS(getchangelog_overloads, Version::FMTVersion::getChangelog, 0, 1)

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


	boost::python::class_<Version::FMTVersion>("FMTversion", "@DocString(FMTVersion)")
		.def("getmajor", &Version::FMTVersion::getMajor,
			"@DocString(FMTVersion::getmajor)").staticmethod("getmajor")
		.def("getminor",&Version::FMTVersion::getMinor,
			"@DocString(FMTVersion::getminor)").staticmethod("getminor")
		.def("getpatch", &Version::FMTVersion::getPatch,
			"@DocString(FMTVersion::getpatch)").staticmethod("getpatch")
		.def("getversion", &Version::FMTVersion::getVersion,
			"@DocString(FMTVersion::getversion)").staticmethod("getversion")
		.def("isatleast", &Version::FMTVersion::isAtLeast,
			"@DocString(FMTVersion::isatleast)").staticmethod("isatleast")
		.def("hasfeature", &Version::FMTVersion::hasFeature,
			"@DocString(FMTVersion::hasfeature)").staticmethod("hasfeature")
        .def("getlicense",&Version::FMTVersion::getLicense,
			getlicense_overloads(bp::args("french"),"@DocString(FMTVersion::getlicense)")).staticmethod("getlicense")
        .def("getchangelog",&Version::FMTVersion::getChangelog,
			getchangelog_overloads(bp::args("french"),"@DocString(FMTVersion::getchangelog)")).staticmethod("getchangelog");
	}
}


#endif // PYEXPORTVERSION_H_INCLUDED

