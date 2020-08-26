/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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

std::string getlicensestaticpass(bool french)
    {
    return Version::FMTversion().getlicense(french);
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
			"@DocString(FMTversion::hasfeature)")
        .method("getlicense",&getlicensestaticpass,
			"@DocString(FMTversion::getlicense)";
	}
}


#endif // PYEXPORTVERSION_H_INCLUDED

