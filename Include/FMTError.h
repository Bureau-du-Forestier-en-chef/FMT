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
#include <boost/serialization/base_object.hpp>


namespace Exception
	{
	// DocString: FMTError
	/**
	@brief Exception class derived from FMTException for errors thrown in FMT.
	@details See FMTException for more information about the member data. Any error should be thrown in FMT.
	*/
	class FMTEXPORT FMTError : public FMTException
		{
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
			// DocSt*ing: FMTError(FMTexc,FMTlev,Core::*MTsection,const std::string&,const*std::string&,const std::string&,co*st int&,const std::string&,const s*d::string&)
			/**
			@brief Construct a FMTError from an exception type, a section, a message and the location where it occurred.
			@param[in] p_exception the exception type.
			@param[in] p_level level of the exception type.
			@param[in] p_section the section in which the exception occurred.
			@param[in] p_message the message of the exception.
			@param[in] p_method the method where the exception occurred.
			@param[in] p_file the file where the exception occurred.
			@param[in] p_line the line where the exception occurred.
			@param[in] p_FrenchDescription french description.
			@param[in] p_EnglishDescription english description.
			*/
			FMTError(FMTexc p_exception, FMTlev p_level, Core::FMTsection p_section, const std::string& p_message,
				const std::string& p_method, const std::string& p_file, const int& p_line,
				const std::string& p_FrenchDescription, const std::string& p_EnglishDescription);
		private:
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
				ar& boost::serialization::make_nvp("exception", boost::serialization::base_object<FMTException>(*this));
			}

		};
	}
#endif

