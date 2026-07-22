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
	@brief Exception handler derived from FMTExceptionHandler that reports the source line and file where the exception comes from.
	*/
	class FMTEXPORT FMTDebugExceptionHandler final : public FMTExceptionHandler
	{
	public:
		// DocString: FMTDebugExceptionHandler()
		/**
		@brief Default constructor for FMTDebugExceptionHandler.
		*/
		FMTDebugExceptionHandler();
		// DocString: ~FMTDebugExceptionHandler()
		/**
		@brief Default destructor for FMTDebugExceptionHandler.
		*/
		~FMTDebugExceptionHandler() = default;
		// DocString: FMTDebugExceptionHandler::raise
		/**
		@brief Override the base class raise function with a debug style. See FMTExceptionHandler::raise.
		@param[in] lexception the exception type.
		@param[in] text the message of the exception.
		@param[in] method the method where the exception occurred.
		@param[in] line the line where the exception occurred.
		@param[in] file the file where the exception occurred.
		@param[in] lsection the section in which the exception occurred.
		@param[in] throwit if true throws the exception.
		@return the raised FMTException.
		*/
		FMTException raise(FMTexc lexception, std::string text,
			const std::string& method, const int& line, const std::string& file,
			Core::FMTsection lsection = Core::FMTsection::Empty, bool throwit = true) override;
	#if defined FMTWITHGDAL
        // DocString: FMTDebugExceptionHandler::getCPLdata
        /**
        @brief Return an abstract copy of itself, used in handelCPLerror called back by GDAL. See FMTExceptionHandler::getCPLdata.
        @return a pointer to the exception handler.
        */
		FMTExceptionHandler* getCPLdata() override;
		// DocString: FMTDebugExceptionHandler::handelCPLerror
		/**
		@brief Callback used in GDAL. See FMTExceptionHandler::handelCPLerror.
		@param[in] eErrClass the error class.
		@param[in] nError the error number.
		@param[in] pszErrorMsg the error message.
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
		@brief Serialize the FMTDebugExceptionHandler through its base FMTExceptionHandler for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
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

