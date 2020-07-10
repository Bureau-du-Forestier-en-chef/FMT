/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdebugexceptionhandler_H_INCLUDED
#define FMTdebugexceptionhandler_H_INCLUDED
#include "FMTexceptionhandler.h"
#include <string>


namespace Exception
{
	// DocString: FMTdebugexceptionhandler
	/**
	The FMTdebugexceptionhandler is derived from the FMTexceptionhandler base class and is usefull when
	you want to know in which line/file of the source code the exception is comming from.
	*/
	class FMTdebugexceptionhandler final : public FMTexceptionhandler
	{
		// DocString: FMTdebugexceptionhandler::serialize
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
		// DocString: FMTdebugexceptionhandler()
		/**
		Default constructor for FMTdebugexceptionhandler.
		*/
		FMTdebugexceptionhandler();
		// DocString: ~FMTdebugexceptionhandler()
		/**
		Default destructor for FMTdebugexceptionhandler.
		*/
		~FMTdebugexceptionhandler() = default;
		// DocString: FMTdebugexceptionhandler::raise
		/**
		The function overide the base class raise function to give it a more "debug" style.
		See raise function of FMTexceptionhandler class.
		*/
		FMTexception raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;
		// DocString: FMTdebugexceptionhandler::getCPLdata
		/**
		Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
		See getCPLdata of FMTexceptionhandler class.
		*/
		FMTexceptionhandler* getCPLdata() override;
	#if defined FMTWITHGDAL
		// DocString: FMTdebugexceptionhandler::handelCPLerror
		/**
		Used has call back in gdal.
		See handelCPLerror of FMTexceptionhandler class.
		*/
		void handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg) override;
	#endif
	};

}

#endif
