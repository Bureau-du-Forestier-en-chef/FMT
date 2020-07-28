
/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTVERSION_H_INCLUDED
#define FMTVERSION_H_INCLUDED

#include <string>

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


namespace Version
{
// DocString: FMTversion
/**
The only purpose of this class is to let the user see which version of FMT he's using.
It also gives some information about the build date and the actual date.
It also informs the user about which features are avalaible in FMT.
*/

class FMTversion : public Core::FMTobject
	{
	public:
		// DocString: FMTversion()
		/**
		Default constructor of FMTversion.
		*/
		FMTversion();
		// DocString: ~FMTversion()
		/**
		Destructor of FMTversion.
		*/
		~FMTversion()=default;
		// DocString: FMTversion::getmajor
		/**
		Getter of the major version of FMT.
		*/
		static int getmajor();
		// DocString: FMTversion::getminor
		/**
		Getter of the minor version of FMT.
		*/
		static int getminor();
		// DocString: FMTversion::getpatch
		/**
		Getter of the patch version of FMT.
		*/
		static int getpatch();
		// DocString: FMTversion::getversion
		/**
		Getter of the string of the version : MAjor.Minor.PATCH
		*/
		static std::string getversion();
		// DocString: FMTversion::getdatenow
		/**
		Getter of the actual date: Day Month date
		*/
		static std::string getdatenow();
		// DocString: FMTversion::getbuilddate
		/**
		Getter of the build date: date
		*/
		static std::string getbuilddate();
		// DocString: FMTversion::isatleast
		/**
		Test if the FMT version is at least the given major/minor/patch parameters.
		*/
		static bool isatleast(int major, int minor, int patch);
		// DocString: FMTversion::hasfeature
		/**
		Test if the FMT version have the named feature.
		Features GDAL,OSI,MOSEK,PYTHON,R.
		*/
		static bool hasfeature(const std::string& name);
		// DocString: FMTversion::getlicense
		/**
		Returns thes license has a regular string if french = true the returned license will be in french
		*/
		std::string getlicense(bool french=false) const;

	};
}

#endif
