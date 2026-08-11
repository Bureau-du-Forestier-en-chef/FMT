/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTwarning_Hm_included
#define FMTwarning_Hm_included

#include <unordered_map>
#include "FMTException.h"
#include <string>
#include <memory>
#include "FMTutility.h"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
	class FMTLogger;
}

namespace Exception
{
	// DocString: FMTWarning
	/**
	@brief Exception class derived from FMTException for warnings in FMT.
	@details See FMTException for more information about the member data. Warnings are never thrown but only logged depending on the FMTLogger and FMTExceptionHandler used.
	*/
	class FMTEXPORT FMTWarning : public FMTException
	{
	
	public:
		// DocString: ~FMTWarning()
		/**
		@brief Default destructor for FMTWarning.
		*/
		~FMTWarning()=default;
		// DocString: FMTWarning::operator=
		/**
		@brief Default copy assignment operator for FMTWarning.
		@return a reference to this FMTWarning.
		*/
		FMTWarning& operator = (const FMTWarning&) = default;
		// DocString: FMTWarning()
		/**
		@brief Default constructor for FMTWarning.
		*/
		FMTWarning();
		// DocString: FMTWarning(FMTexc,Core::FMTsection,const std::string&,const std::string&,const std::string&,const int&,const std::string&,const std::string&)
		/**
		@brief Construct a FMTWarning from an exception type, a section, a message and the location where it occurred.
		@param[in] p_exception the exception type.
		@param[in] p_section the section in which the exception occurred.
		@param[in] p_message the message of the exception.
		@param[in] p_method the method where the exception occurred.
		@param[in] p_file the file where the exception occurred.
		@param[in] p_line the line where the exception occurred.
		@param[in] p_FrenchDescription french description.
		@param[in] p_EnglishDescription english description.
		*/
		FMTWarning(FMTexc p_exception, Core::FMTsection p_section, const std::string& p_message,
			const std::string& p_method, const std::string& p_file, const int& p_line,
			const std::string& p_FrenchDescription, const std::string& p_EnglishDescription);
		// DocString: FMTWarning::warn
		/**
		@brief Log the warning using the given logger.
		@param[in,out] logger the logger used to log the warning.
		@param[in,out] specificwarningcount the count of each specific warning already logged.
		@param[in] maxwarning the maximum number of times a warning is logged.
		*/
		void warn(Logging::FMTLogger& logger,std::unordered_map<int,size_t>& specificwarningcount, const size_t& maxwarning) const;
	private:
		// DocString: FMTWarning::serialize
		/**
		@brief Serialize the FMTWarning through its base FMTException for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("parent_exception", boost::serialization::base_object<FMTException>(*this));
		}
	};
}
#endif