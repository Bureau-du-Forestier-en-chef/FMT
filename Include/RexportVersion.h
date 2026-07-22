/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef REXPORTVERSION_H_INCLUDED
#define REXPORTVERSION_H_INCLUDED

#include <Rcpp.h>
#include "FMTVersion.h"
#include "Rdefinitions.h"
#include <string>

RCPP_EXPOSED_WRAP(Version::FMTVersion);
RCPP_EXPOSED_AS(Version::FMTVersion);

namespace R
{
int getMajorStaticPass(Version::FMTVersion* versionobject)
	{
	return Version::FMTVersion::getMajor();
	}

int getMinorStaticPass(Version::FMTVersion* versionobject)
	{
	return Version::FMTVersion::getMinor();
	}

int getPatchStaticPass(Version::FMTVersion* versionobject)
	{
	return Version::FMTVersion::getPatch();
	}

std::string getVersionStaticPass(Version::FMTVersion* versionobject)
	{
	return Version::FMTVersion::getVersion();
	}

bool isAtLeastStaticPass(Version::FMTVersion* versionobject, int major, int minor, int patch)
	{
	return Version::FMTVersion::isAtLeast(major,minor,patch);
	}

bool hasFeatureStaticPass(Version::FMTVersion* versionobject, const std::string& feature)
	{
	return Version::FMTVersion::hasFeature(feature);
	}

std::string getLicenseStaticPass(Version::FMTVersion* versionobject, bool french)
    {
	const Version::FMTVersion version;
    return version.getLicense(french);
    }

void exportVersion()
{

	Rcpp::class_<Version::FMTVersion>("FMTversion", "@DocString(FMTVersion)")
		.constructor("@DocString(FMTVersion())")
		.method("getmajor", &getMajorStaticPass,
			"@DocString(FMTVersion::getmajor)")
		.method("getminor",&getMinorStaticPass,
			"@DocString(FMTVersion::getminor)")
		.method("getpatch", &getPatchStaticPass,
			"@DocString(FMTVersion::getpatch)")
		.method("getversion", &getVersionStaticPass,
			"@DocString(FMTVersion::getversion)")
		.method("isatleast", &isAtLeastStaticPass,
			"@DocString(FMTVersion::isatleast)")
		.method("hasfeature", &hasFeatureStaticPass,
			"@DocString(FMTVersion::hasfeature)")
        .method("getlicense",&getLicenseStaticPass,
			"@DocString(FMTVersion::getlicense)");
	}
}


#endif // PYEXPORTVERSION_H_INCLUDED

