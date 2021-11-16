
/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTVERSION_H_INCLUDED
#define FMTVERSION_H_INCLUDED

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

#include "FMTobject.hpp"
#ifdef FMTWITHOSI
	#include "FMTsolverinterface.hpp"
#endif

#ifdef FMTWITHGDAL
	#include "gdal.h"
	#include "gdal_priv.h"
#endif

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
	#ifdef FMTWITHGDAL
		// DocString: FMTversion::getallGDALdrivernames
		/**
		Return a vector of GDAL drivers for a given spatialtype (raster/vector)
		*/
		static std::vector<GDALDriver*> getallGDALdrivers(const char* spatialtype);
	#endif
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
		#ifdef FMTWITHOSI
		// DocString: FMTversion::getavailablesolverinterface
		/**
		Return a vector of solverinterface available
		*/
		static std::vector<Models::FMTsolverinterface> getavailablesolverinterface();
		#endif
		#ifdef FMTWITHGDAL
			// DocString: FMTversion::getGDALvectordrivernames
			/**
			Return a vector of GDAL vector driver names
			*/
			static std::vector<std::string>getGDALvectordrivernames();
			// DocString: FMTversion::getGDALrasterdrivernames
			/**
			Return a vector of GDAL raster driver names
			*/
			static std::vector<std::string>getGDALrasterdrivernames();
			// DocString: FMTversion::getGDALvectordriverextensions
			/**
			Return a vector of GDAL vector driver extensions
			*/
			static std::vector<std::string>getGDALvectordriverextensions();
			// DocString: FMTversion::getGDALrasterdriverextensions
			/**
			Return a vector of GDAL raster driver extensions
			*/
			static std::vector<std::string>getGDALrasterdriverextensions();
		#endif
	};
}

#endif
