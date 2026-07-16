/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTwarning_Hm_included
#define FMTwarning_Hm_included

#include <unordered_map>
#include "FMTexception.h"
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
	FMTWarning is derived from the FMTException base class.
	See FMTException class for more information about the member data.
	In FMT warning are never thrown but only logged depending of the FMTLogger and FMTExceptionHandler used.
	*/
	class FMTEXPORT FMTWarning : public FMTException
	{
	// DocString: FMTWarning::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		Default destructor for FMTWarning.
		*/
		~FMTWarning()=default;
		// DocString: FMTWarning::operator=
		/**
		Default copy assignment for FMTWarning.
		*/
		FMTWarning& operator = (const FMTWarning&) = default;
		// DocString: FMTWarning()
		/**
		Default constructor for FMTWarning.
		*/
		FMTWarning();
		// DocString: FMTWarning(const FMTException&)
		/**
		Default copy constructor for FMTWarning.
		*/
		FMTWarning(const FMTException& rhs);
		// DocString: FMTWarning(const FMTexc,const std::string)
		/**
		Constructor for FMTWarning taking a exception type and message string as arguments.
		*/
		FMTWarning(const FMTexc lexception, const std::string message);
		// DocString: FMTWarning(const FMTexc,const Core::FMTsection,const std::string)
		/**
		Constructor for FMTWarning taking a exception type, section in which the warning occur and message string as arguments.
		*/
		FMTWarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message);
		// DocString: FMTWarning(const FMTexc,const Core::FMTsection, const std::string,const std::string&,const std::string&,const std::string&)
		/**
		Constructor for FMTWarning taking a exception type, section in which the warning occur and message string as arguments and the location
		where the exception occured (method/file/line).
		*/
		FMTWarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message,
			const std::string& lmethod, const std::string& lfile, const int& lline);
		// DocString: FMTWarning::warn
		/**
		Using a given logger the function log the warning with the logger. 
		*/
		void warn(Logging::FMTLogger& logger,std::unordered_map<int,size_t>& specificwarningcount, const size_t& maxwarning) const;
	};
}
#endif