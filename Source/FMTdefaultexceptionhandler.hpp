/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdefaultexceptionhandler_H_INCLUDED
#define FMTdefaultexceptionhandler_H_INCLUDED

#include "FMTexceptionhandler.hpp"
#include "FMTexception.hpp"
#include <string>

namespace Exception
{
// DocString: FMTdefaultexceptionhandler
/**
The FMTdefaultexceptionhandler is derived from the FMTexceptionhandler base class and is
the default exception handler used in FMT.
*/
class FMTEXPORT FMTdefaultexceptionhandler final : public FMTexceptionhandler
	{
	// DocString: FMTdefaultexceptionhandler::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar &  boost::serialization::make_nvp("FMTexceptionhandler", boost::serialization::base_object<FMTexceptionhandler>(*this));
	}
	public:
		// DocString: FMTdefaultexceptionhandler()
		/**
		Default constructor for FMTdefaultexceptionhandler.
		*/
		FMTdefaultexceptionhandler();
		// DocString: ~FMTdefaultexceptionhandler()
		/**
		Default destructor for FMTdefaultexceptionhandler.
		*/
		~FMTdefaultexceptionhandler() = default;
		// DocString: FMTdefaultexceptionhandler::raise
		/**
		The function overide the base class raise function for a default implementation.
		See raise function of FMTexceptionhandler class.
		*/
		FMTexception raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;

		// DocString: FMTdefaultexceptionhandler::getCPLdata
		/**
		Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
		See getCPLdata of FMTexceptionhandler class.
		*/
		FMTexceptionhandler* getCPLdata() override;
		// DocString: FMTdefaultexceptionhandler::handelCPLerror
		#if defined FMTWITHGDAL
		/**
		Used has call back in gdal.
		See handelCPLerror of FMTexceptionhandler class.
		*/
		void handelCPLerror(CPLErr eErrClass, CPLErrorNum nError, const char * pszErrorMsg) override;
		#endif
	};
}

BOOST_CLASS_EXPORT_KEY(Exception::FMTdefaultexceptionhandler)

#endif

