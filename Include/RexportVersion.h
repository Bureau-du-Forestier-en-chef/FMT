/*
Copyright (c) 2019 Gouvernement du Qubec

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
int getMajorStaticPass(Version::FMTversion* versionobject)
	{
	return Version::FMTversion::getMajor();
	}

int getMinorStaticPass(Version::FMTversion* versionobject)
	{
	return Version::FMTversion::getMinor();
	}

int getPatchStaticPass(Version::FMTversion* versionobject)
	{
	return Version::FMTversion::getPatch();
	}

std::string getVersionStaticPass(Version::FMTversion* versionobject)
	{
	return Version::FMTversion::getVersion();
	}

bool isAtLeastStaticPass(Version::FMTversion* versionobject, int major, int minor, int patch)
	{
	return Version::FMTversion::isAtLeast(major,minor,patch);
	}

bool hasFeatureStaticPass(Version::FMTversion* versionobject, const std::string& feature)
	{
	return Version::FMTversion::hasFeature(feature);
	}

std::string getLicenseStaticPass(Version::FMTversion* versionobject, bool french)
    {
	const Version::FMTversion version;
    return version.getLicense(french);
    }

void exportVersion()
{

	Rcpp::class_<Version::FMTversion>("FMTversion", "@DocString(FMTversion)")
		.constructor("@DocString(FMTversion())")
		.method("getmajor", &getMajorStaticPass,
			"@DocString(FMTversion::getmajor)")
		.method("getminor",&getMinorStaticPass,
			"@DocString(FMTversion::getminor)")
		.method("getpatch", &getPatchStaticPass,
			"@DocString(FMTversion::getpatch)")
		.method("getversion", &getVersionStaticPass,
			"@DocString(FMTversion::getversion)")
		.method("isatleast", &isAtLeastStaticPass,
			"@DocString(FMTversion::isatleast)")
		.method("hasfeature", &hasFeatureStaticPass,
			"@DocString(FMTversion::hasfeature)")
        .method("getlicense",&getLicenseStaticPass,
			"@DocString(FMTversion::getlicense)");
	}
}


#endif // PYEXPORTVERSION_H_INCLUDED

