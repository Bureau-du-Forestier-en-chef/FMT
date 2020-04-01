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

#ifndef REXPORTVERSION_H_INCLUDED
#define REXPORTVERSION_H_INCLUDED

#include <Rcpp.h>
#include "FMTversion.h"
#include "Rdefinitions.h"
#include <string>

RCPP_EXPOSED_WRAP(Version::FMTversion);
RCPP_EXPOSED_AS(Version::FMTversion);

namespace R
{
int getmajorstaticpass(Version::FMTversion* versionobject)
	{
	return Version::FMTversion::getmajor();
	}

int getminorstaticpass(Version::FMTversion* versionobject)
	{
	return Version::FMTversion::getminor();
	}

int getpatchstaticpass(Version::FMTversion* versionobject)
	{
	return Version::FMTversion::getpatch();
	}

std::string getversionstaticpass(Version::FMTversion* versionobject)
	{
	return Version::FMTversion::getversion();
	}

bool isatleaststaticpass(Version::FMTversion* versionobject, int major, int minor, int patch)
	{
	return Version::FMTversion::isatleast(major,minor,patch);
	}

bool hasfeaturestaticpass(Version::FMTversion* versionobject, const std::string& feature)
	{
	return Version::FMTversion::hasfeature(feature);
	}

void exportVersion()
{

	Rcpp::class_<Version::FMTversion>("FMTversion", "@DocString(FMTversion)")
		.constructor("@DocString(FMTversion())")
		.method("getmajor", &getmajorstaticpass,
			"@DocString(FMTversion::getmajor)")
		.method("getminor",&getminorstaticpass,
			"@DocString(FMTversion::getminor)")
		.method("getpatch", &getpatchstaticpass,
			"@DocString(FMTversion::getpatch)")
		.method("getversion", &getversionstaticpass,
			"@DocString(FMTversion::getversion)")
		.method("isatleast", &isatleaststaticpass,
			"@DocString(FMTversion::isatleast)")
		.method("hasfeature", &hasfeaturestaticpass,
			"@DocString(FMTversion::hasfeature)");
	}
}


#endif // PYEXPORTVERSION_H_INCLUDED

