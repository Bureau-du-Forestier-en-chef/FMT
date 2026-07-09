
/*
Copyright (c) 2019 Gouvernement du Qubec

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
// DocString: FMTversion
/**
The only purpose of this class is to let the user see which version of FMT he's using.
It also gives some information about the build date and the actual date.
It also informs the user about which features are avalaible in FMT.
*/



class FMTEXPORT FMTversion : public Core::FMTobject
	{
	public:
		// DocString: FMTversion()
		/**
		Default constructor of FMTversion.
		*/
		FMTversion()=default;
		// DocString: ~FMTversion()
		/**
		Destructor of FMTversion.
		*/
		~FMTversion()=default;
		// DocString: FMTversion::getMajor
		/**
		Getter of the major version of FMT.
		*/
		static int getMajor();
		// DocString: FMTversion::getMinor
		/**
		Getter of the minor version of FMT.
		*/
		static int getMinor();
		// DocString: FMTversion::getPatch
		/**
		Getter of the patch version of FMT.
		*/
		static int getPatch();
		// DocString: FMTversion::getVersion
		/**
		Getter of the string of the version : MAjor.Minor.PATCH
		*/
		static std::string getVersion();
		// DocString: FMTversion::getDatenow
		/**
		Getter of the actual date: Day Month date
		*/
		static std::string getDatenow();
		// DocString: FMTversion::getBuildDate
		/**
		Getter of the build date: date
		*/
		static std::string getBuildDate();
		// DocString: FMTversion::isAtLeast
		/**
		Test if the FMT version is at least the given major/minor/patch parameters.
		*/
		static bool isAtLeast(int major, int minor, int patch);
		// DocString: FMTversion::hasFeature
		/**
		Test if the FMT version have the named feature.
		Features GDAL,OSI,MOSEK,PYTHON,R.
		*/
		static bool hasFeature(const std::string& name);
		// DocString: FMTversion::getLicense
		/**
		Returns thes license has a regular string if french = true the returned license will be in french
		*/
		static std::string getLicense(bool french=false);
	};
}

#endif
