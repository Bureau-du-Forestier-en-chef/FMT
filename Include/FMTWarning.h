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
			ar &  boost::serialization::make_nvp("parent_exception", boost::serialization::base_object<FMTException>(*this));
		}
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
		// DocString: FMTWarning(const FMTException&)
		/**
		@brief Construct a FMTWarning from a FMTException.
		@param[in] rhs the FMTException to copy from.
		*/
		FMTWarning(const FMTException& rhs);
		// DocString: FMTWarning(const FMTexc,const std::string)
		/**
		@brief Construct a FMTWarning from an exception type and a message.
		@param[in] lexception the exception type.
		@param[in] message the message of the warning.
		*/
		FMTWarning(const FMTexc lexception, const std::string message);
		// DocString: FMTWarning(const FMTexc,const Core::FMTsection,const std::string)
		/**
		@brief Construct a FMTWarning from an exception type, a section and a message.
		@param[in] lexception the exception type.
		@param[in] lsection the section in which the warning occurred.
		@param[in] message the message of the warning.
		*/
		FMTWarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message);
		// DocString: FMTWarning(const FMTexc,const Core::FMTsection, const std::string,const std::string&,const std::string&,const std::string&)
		/**
		@brief Construct a FMTWarning from an exception type, a section, a message and the location where it occurred.
		@param[in] lexception the exception type.
		@param[in] lsection the section in which the warning occurred.
		@param[in] message the message of the warning.
		@param[in] lmethod the method where the warning occurred.
		@param[in] lfile the file where the warning occurred.
		@param[in] lline the line where the warning occurred.
		*/
		FMTWarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message,
			const std::string& lmethod, const std::string& lfile, const int& lline);
		// DocString: FMTWarning::warn
		/**
		@brief Log the warning using the given logger.
		@param[in,out] logger the logger used to log the warning.
		@param[in,out] specificwarningcount the count of each specific warning already logged.
		@param[in] maxwarning the maximum number of times a warning is logged.
		*/
		void warn(Logging::FMTLogger& logger,std::unordered_map<int,size_t>& specificwarningcount, const size_t& maxwarning) const;
	};
}
#endif