/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdefaultexceptionhandler_Hm_included
#define FMTdefaultexceptionhandler_Hm_included

#include "FMTExceptionHandler.h"
#include "FMTException.h"
#include <string>

namespace Exception
{
// DocString: FMTDefaultExceptionHandler
/**
The FMTDefaultExceptionHandler is derived from the FMTExceptionHandler base class and is
the default exception handler used in FMT.
*/
class FMTEXPORT FMTDefaultExceptionHandler final : public FMTExceptionHandler
	{
	public:
		// DocString: FMTDefaultExceptionHandler()
		/**
		Default constructor for FMTDefaultExceptionHandler.
		*/
		FMTDefaultExceptionHandler();
		// DocString: FMTDefaultExceptionHandler(const std::unique_ptr<Logging::FMTLogger>&)
		/**
		Constructor with logger
		*/
		FMTDefaultExceptionHandler(const std::unique_ptr<Logging::FMTLogger>& logger);
		// DocString: ~FMTDefaultExceptionHandler()
		/**
		Default destructor for FMTDefaultExceptionHandler.
		*/
		~FMTDefaultExceptionHandler() = default;
		// DocString: FMTDefaultExceptionHandler::raise
		/**
		The function overide the base class raise function for a default implementation.
		See raise function of FMTExceptionHandler class.
		*/
		FMTException raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;

		// DocString: FMTDefaultExceptionHandler::getCPLdata
		/**
		Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
		See getCPLdata of FMTExceptionHandler class.
		*/
		FMTExceptionHandler* getCPLdata() override;
		// DocString: FMTDefaultExceptionHandler::handelCPLerror
		#if defined FMTWITHGDAL
		/**
		Used has call back in gdal.
		See handelCPLerror of FMTExceptionHandler class.
		*/
		void handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg) override;
		#endif
		// DocString: FMTDefaultExceptionHandler::Clone
		/**
		@brief clone the FMTdefaulexceptionhandler
		@return a valid cloned FMTdefaulexceptionhandler
		*/
		virtual std::unique_ptr <FMTExceptionHandler> Clone() const;
	private:
		// DocString: FMTDefaultExceptionHandler::serialize
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

BOOST_CLASS_EXPORT_KEY(Exception::FMTDefaultExceptionHandler)

#endif

