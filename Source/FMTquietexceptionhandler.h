/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTquietexceptionhandler_H_INCLUDED
#define FMTquietexceptionhandler_H_INCLUDED

#include "FMTexceptionhandler.h"
#include "FMTexception.h"
#include <string>

namespace Exception
{
	// DocString: FMTquietexceptionhandler
	/**
	The FMTquietexceptionhandler is derived from the FMTexceptionhandler base class.
	This class is a silent class it does not throw any kind of logging but throw errors and no warnings.
	*/
	class FMTquietexceptionhandler final : public FMTexceptionhandler
	{
		// DocString: FMTquietexceptionhandler::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("parent_handler", boost::serialization::base_object<FMTexceptionhandler>(*this));
		}
	public:
		// DocString: FMTquietexceptionhandler()
		/**
		Default constructor for FMTquietexceptionhandler
		*/
		FMTquietexceptionhandler();
		// DocString: FMTquietexceptionhandler()
		/**
		Default destructor for FMTquietexceptionhandler
		*/
		~FMTquietexceptionhandler() = default;
		// DocString: FMTquietexceptionhandler::raise
		/**
		The function overide the base class raise function.
		See raise function of FMTexceptionhandler class.
		*/
		FMTexception raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;

		// DocString: FMTquietexceptionhandler::getCPLdata
		/**
		Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
		See getCPLdata of FMTexceptionhandler class.
		*/
		FMTexceptionhandler* getCPLdata() override;
		#if defined FMTWITHGDAL
		// DocString: FMTquietexceptionhandler::handelCPLerror
		/**
		Used has call back in gdal.
		See handelCPLerror of FMTexceptionhandler class.
		*/
		void handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg) override;
		#endif
	};

}
#endif

