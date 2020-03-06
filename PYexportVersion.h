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

#ifndef PYEXPORTVERSION_H_INCLUDED
#define PYEXPORTVERSION_H_INCLUDED

#include <boost/python/docstring_options.hpp>
#include <boost/python/args.hpp>
#include "FMTversion.h"
#include "boost/python.hpp"


namespace Python
{
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
			"@DocString(FMTversion::hasfeature)").staticmethod("hasfeature");
	}
}


#endif // PYEXPORTVERSION_H_INCLUDED

