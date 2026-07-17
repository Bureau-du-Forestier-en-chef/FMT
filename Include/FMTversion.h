
/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTVERSION_Hm_included
#define FMTVERSION_Hm_included

#include <string>
#include <vector>

#ifndef FMT_MAJOR
    #define FMT_MAJOR 0
#endif

#ifndef FMT_MINOR
    #define FMT_MINOR 0
#endif

#ifndef FMT_PATCH
    #define FMT_PATCH 0
#endif

#include "FMTobject.h"

/// Namespace for handling FMT versions and features.
namespace Version
{
// DocString: FMTVersion
/**
The only purpose of this class is to let the user see which version of FMT he's using.
It also gives some information about the build date and the actual date.
It also informs the user about which features are avalaible in FMT.
*/



class FMTEXPORT FMTVersion : public Core::FMTObject
	{
	public:
		// DocString: FMTVersion()
		/**
		Default constructor of FMTVersion.
		*/
		FMTVersion()=default;
		// DocString: ~FMTVersion()
		/**
		Destructor of FMTVersion.
		*/
		~FMTVersion()=default;
		// DocString: FMTVersion::getMajor
		/**
		Getter of the major version of FMT.
		*/
		static int getMajor();
		// DocString: FMTVersion::getMinor
		/**
		Getter of the minor version of FMT.
		*/
		static int getMinor();
		// DocString: FMTVersion::getPatch
		/**
		Getter of the patch version of FMT.
		*/
		static int getPatch();
		// DocString: FMTVersion::getVersion
		/**
		Getter of the string of the version : MAjor.Minor.PATCH
		*/
		static std::string getVersion();
		// DocString: FMTVersion::getDatenow
		/**
		Getter of the actual date: Day Month date
		*/
		static std::string getDatenow();
		// DocString: FMTVersion::getBuildDate
		/**
		Getter of the build date: date
		*/
		static std::string getBuildDate();
		// DocString: FMTVersion::isAtLeast
		/**
		Test if the FMT version is at least the given major/minor/patch parameters.
		*/
		static bool isAtLeast(int major, int minor, int patch);
		// DocString: FMTVersion::hasFeature
		/**
		Test if the FMT version have the named feature.
		Features GDAL,OSI,MOSEK,PYTHON,R.
		*/
		static bool hasFeature(const std::string& name);
		// DocString: FMTVersion::getLicense
		/**
		Returns thes license has a regular string if french = true the returned license will be in french
		*/
		static std::string getLicense(bool french=false);
	};
}

#endif
