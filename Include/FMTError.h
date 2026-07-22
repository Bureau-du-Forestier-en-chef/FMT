/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTerror_Hm_included
#define FMTerror_Hm_included

#include "FMTException.h"
#include <string>
#include <boost/serialization/serialization.hpp>

#if defined FMTWITHOSI
	class CoinError;
#endif

#include <boost/graph/exception.hpp>
#include <boost/serialization/nvp.hpp>


namespace Exception
	{
	// DocString: FMTError
	/**
	@brief Exception class derived from FMTException for errors thrown in FMT.
	@details See FMTException for more information about the member data. Any error should be thrown in FMT.
	*/
	class FMTEXPORT FMTError : public FMTException
		{
		// DocString: FMTError::serialize
		/**
		@brief Serialize the FMTError through its base FMTException for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar &  boost::serialization::make_nvp("exception", boost::serialization::base_object<FMTException>(*this));
		}
		public:
			// DocString: ~FMTError()
			/**
			@brief Default destructor for FMTError.
			*/
			~FMTError() = default;
			// DocString: FMTError::operator=
			/**
			@brief Default copy assignment operator for FMTError.
			@return a reference to this FMTError.
			*/
			FMTError& operator=(const FMTError&) = default;
			// DocString: FMTError()
			/**
			@brief Default constructor for FMTError.
			*/
			FMTError();
			// DocString: FMTError(const FMTException&)
			/**
			@brief Construct a FMTError from a FMTException.
			@param[in] rhs the FMTException to copy from.
			*/
			FMTError(const FMTException& rhs);
			// DocString: FMTError(const FMTexc,const std::string)
			/**
			@brief Construct a FMTError from an exception type and a message.
			@param[in] lexception the exception type.
			@param[in] message the message of the error.
			*/
			FMTError(const FMTexc lexception, const std::string message);
			// DocString: FMTError(const FMTexc,const Core::FMTsection,const std::string)
			/**
			@brief Construct a FMTError from an exception type, a section and a message.
			@param[in] lexception the exception type.
			@param[in] lsection the section in which the error occurred.
			@param[in] message the message of the error.
			*/
			FMTError(const FMTexc lexception, const Core::FMTsection lsection, const std::string message);
			// DocString: FMTError(const FMTexc,const Core::FMTsection,const std::string,const std::string&,const std::string&,const int&)
			/**
			@brief Construct a FMTError from an exception type, a section, a message and the location where it occurred.
			@param[in] lexception the exception type.
			@param[in] lsection the section in which the error occurred.
			@param[in] message the message of the error.
			@param[in] lmethod the method where the error occurred.
			@param[in] lfile the file where the error occurred.
			@param[in] lline the line where the error occurred.
			*/
			FMTError(const FMTexc lexception,const Core::FMTsection lsection, const std::string message,
					const std::string& lmethod, const std::string& lfile, const int& lline);
			// DocString: FMTError(const FMTexc,const std::string,const std::string&,const std::string&,const int&)
			/**
			@brief Construct a FMTError from an exception type, a message and the location where it occurred.
			@param[in] lexception the exception type.
			@param[in] message the message of the error.
			@param[in] lmethod the method where the error occurred.
			@param[in] lfile the file where the error occurred.
			@param[in] lline the line where the error occurred.
			*/
			FMTError(const FMTexc lexception, const std::string message,
				const std::string& lmethod, const std::string& lfile, const int& lline);
			#if defined FMTWITHOSI
				// DocString: FMTError(const CoinError&)
				/**
				@brief Construct a FMTError from a CoinError.
				@param[in] coinexception the CoinError to construct from.
				*/
				FMTError(const CoinError& coinexception);
			#endif
			// DocString: FMTError(const boost::bad_graph&)
			/**
			@brief Construct a FMTError from a boost::bad_graph.
			@param[in] boostexception the boost::bad_graph to construct from.
			*/
			FMTError(const boost::bad_graph& boostexception);

		};
	}
#endif

