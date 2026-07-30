
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

#include "FMTObject.h"

/// Namespace for handling FMT versions and features.
namespace Version
{
// DocString: FMTVersion
/**
@brief Class exposing the version of FMT, its build date and the available features.
*/



class FMTEXPORT FMTVersion : public Core::FMTObject
	{
	public:
		// DocString: FMTVersion()
		/**
		@brief Default constructor for FMTVersion.
		*/
		FMTVersion()=default;
		// DocString: ~FMTVersion()
		/**
		@brief Default destructor for FMTVersion.
		*/
		~FMTVersion()=default;
		// DocString: FMTVersion::getMajor
		/**
		@brief Return the major version of FMT.
		@return the major version of FMT.
		*/
		static int getMajor();
		// DocString: FMTVersion::getMinor
		/**
		@brief Return the minor version of FMT.
		@return the minor version of FMT.
		*/
		static int getMinor();
		// DocString: FMTVersion::getPatch
		/**
		@brief Return the patch version of FMT.
		@return the patch version of FMT.
		*/
		static int getPatch();
		// DocString: FMTVersion::getVersion
		/**
		@brief Return the version string of FMT (Major.Minor.Patch).
		@return the version string.
		*/
		static std::string getVersion();
		// DocString: FMTVersion::getDateNow
		/**
		@brief Return the actual date (Day Month date).
		@return the actual date.
		*/
		static std::string getDateNow();
		// DocString: FMTVersion::getBuildDate
		/**
		@brief Return the build date of FMT.
		@return the build date.
		*/
		static std::string getBuildDate();
		// DocString: FMTVersion::isAtLeast
		/**
		@brief Test if the FMT version is at least the given major, minor and patch.
		@param[in] major the major version.
		@param[in] minor the minor version.
		@param[in] patch the patch version.
		@return true if the version is at least the given one else false.
		*/
		static bool isAtLeast(int major, int minor, int patch);
		// DocString: FMTVersion::hasFeature
		/**
		@brief Test if the FMT version has the named feature (GDAL, OSI, MOSEK, PYTHON, R).
		@param[in] name the feature name.
		@return true if the feature is available else false.
		*/
		static bool hasFeature(const std::string& name);
		// DocString: FMTVersion::getLicense
		/**
		@brief Return the license as a string.
		@param[in] french if true returns the license in French.
		@return the license string.
		*/
		static std::string getLicense(bool french=false);
	};
}

#endif
