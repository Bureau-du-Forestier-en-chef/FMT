/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdebugexceptionhandler_Hm_included
#define FMTdebugexceptionhandler_Hm_included
#include "FMTExceptionHandler.h"
#include <string>


namespace Exception
{
	// DocString: FMTDebugExceptionHandler
	/**
	The FMTDebugExceptionHandler is derived from the FMTExceptionHandler base class and is usefull when
	you want to know in which line/file of the source code the exception is comming from.
	*/
	class FMTEXPORT FMTDebugExceptionHandler final : public FMTExceptionHandler
	{
	public:
		// DocString: FMTDebugExceptionHandler()
		/**
		Default constructor for FMTDebugExceptionHandler.
		*/
		FMTDebugExceptionHandler();
		// DocString: ~FMTDebugExceptionHandler()
		/**
		Default destructor for FMTDebugExceptionHandler.
		*/
		~FMTDebugExceptionHandler() = default;
		// DocString: FMTDebugExceptionHandler::raise
		/**
		The function overide the base class raise function to give it a more "debug" style.
		See raise function of FMTExceptionHandler class.
		*/
		FMTException raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;
	#if defined FMTWITHGDAL
        // DocString: FMTDebugExceptionHandler::getCPLdata
		/**
		Used in the handelCPLerror called back by GDAL function reutnr a abstract copy of itselft.
		See getCPLdata of FMTExceptionHandler class.
		*/
		FMTExceptionHandler* getCPLdata() override;
		// DocString: FMTDebugExceptionHandler::handelCPLerror
		/**
		Used has call back in gdal.
		See handelCPLerror of FMTExceptionHandler class.
		*/
		void handelCPLerror(int eErrClass, int nError, const char * pszErrorMsg) override;
	#endif
		// DocString: FMTDebugExceptionHandler::Clone
		/**
		@brief clone the FMTDebugExceptionHandler
		@return a valid cloned FMTDebugExceptionHandler
		*/
		virtual std::unique_ptr <FMTExceptionHandler> Clone() const;
	private:
		// DocString: FMTDebugExceptionHandler::serialize
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

BOOST_CLASS_EXPORT_KEY(Exception::FMTDebugExceptionHandler)

#endif

