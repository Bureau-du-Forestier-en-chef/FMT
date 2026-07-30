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
@brief Default exception handler used in FMT, derived from FMTExceptionHandler.
*/
class FMTEXPORT FMTDefaultExceptionHandler final : public FMTExceptionHandler
	{
	public:
		// DocString: FMTDefaultExceptionHandler()
		/**
		@brief Default constructor for FMTDefaultExceptionHandler.
		*/
		FMTDefaultExceptionHandler();
		// DocString: FMTDefaultExceptionHandler(const std::unique_ptr<Logging::FMTLogger>&)
		/**
		@brief Construct a FMTDefaultExceptionHandler with a logger.
		@param[in] logger the logger to use.
		*/
		FMTDefaultExceptionHandler(const std::unique_ptr<Logging::FMTLogger>& logger);
		// DocString: ~FMTDefaultExceptionHandler()
		/**
		@brief Default destructor for FMTDefaultExceptionHandler.
		*/
		~FMTDefaultExceptionHandler() = default;
		// DocString: FMTDefaultExceptionHandler::raise
		/**
		@brief Default implementation overriding the base class raise function. See FMTExceptionHandler::raise.
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

		// DocString: FMTDefaultExceptionHandler::getCplData
		/**
		@brief Return an abstract copy of itself, used in handelCplError called back by GDAL. See FMTExceptionHandler::getCplData.
		@return a pointer to the exception handler.
		*/
		FMTExceptionHandler* getCplData() override;
		// DocString: FMTDefaultExceptionHandler::handelCplError
		#if defined FMTWITHGDAL
		/**
		@brief Callback used in GDAL. See FMTExceptionHandler::handelCplError.
		@param[in] eErrClass the error class.
		@param[in] nError the error number.
		@param[in] pszErrorMsg the error message.
		*/
		void handelCplError(int eErrClass, int nError, const char * pszErrorMsg) override;
		#endif
		// DocString: FMTDefaultExceptionHandler::clone
		/**
		@brief clone the FMTdefaulexceptionhandler
		@return a valid cloned FMTdefaulexceptionhandler
		*/
		virtual std::unique_ptr <FMTExceptionHandler> clone() const;
	private:
		// DocString: FMTDefaultExceptionHandler::serialize
		/**
		@brief Serialize the FMTDefaultExceptionHandler through its base FMTExceptionHandler for multiprocessing across multiple cpus (pickle in Python).
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

BOOST_CLASS_EXPORT_KEY(Exception::FMTDefaultExceptionHandler)

#endif

