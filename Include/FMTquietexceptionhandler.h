/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTquietexceptionhandler_Hm_included
#define FMTquietexceptionhandler_Hm_included

#include "FMTexceptionhandler.h"
#include "FMTexception.h"
#include <string>

namespace Exception
{
	// DocString: FMTQuietExceptionHandler
	/**
	The FMTQuietExceptionHandler is derived from the FMTExceptionHandler base class.
	This class is a silent class it does not throw any kind of logging but throw errors and no warnings.
	*/
	class FMTEXPORT FMTQuietExceptionHandler final : public FMTExceptionHandler
	{
	public:
		// DocString: FMTQuietExceptionHandler()
		/**
		Default constructor for FMTQuietExceptionHandler
		*/
		FMTQuietExceptionHandler();
		// DocString: FMTQuietExceptionHandler()
		/**
		Default destructor for FMTQuietExceptionHandler
		*/
		~FMTQuietExceptionHandler() = default;
		// DocString: FMTQuietExceptionHandler::raise
		/**
		The function overide the base class raise function.
		See raise function of FMTExceptionHandler class.
		*/
		FMTException raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;
    #if defined FMTWITHGDAL
		// DocString: FMTQuietExceptionHandler::getCPLdata
		/**
		Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
		See getCPLdata of FMTExceptionHandler class.
		*/
		FMTExceptionHandler* getCPLdata() override;
		// DocString: FMTQuietExceptionHandler::handelCPLerror
		/**
		Used has call back in gdal.
		See handelCPLerror of FMTExceptionHandler class.
		*/
		void handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg) override;
		#endif
		// DocString: FMTQuietExceptionHandler::Clone
		/**
		@brief clone the FMTQuietExceptionHandler
		@return a valid cloned FMTQuietExceptionHandler
		*/
		virtual std::unique_ptr <FMTExceptionHandler> Clone() const;
	private:
		// DocString: FMTQuietExceptionHandler::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTexceptionhandler", boost::serialization::base_object<FMTExceptionHandler>(*this));
		}
	};

}
BOOST_CLASS_EXPORT_KEY(Exception::FMTQuietExceptionHandler)
#endif

