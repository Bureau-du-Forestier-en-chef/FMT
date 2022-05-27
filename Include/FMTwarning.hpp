/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTwarning_H_INCLUDED
#define FMTwarning_H_INCLUDED

#include <unordered_map>
#include "FMTexception.hpp"
#include <string>
#include <memory>
#include "FMTutility.hpp"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
	class FMTlogger;
}

namespace Exception
{
	// DocString: FMTwarning
	/**
	FMTwarning is derived from the FMTexception base class.
	See FMTexception class for more information about the member data.
	In FMT warning are never thrown but only logged depending of the FMTlogger and FMTexceptionhandler used.
	*/
	class FMTEXPORT FMTwarning : public FMTexception
	{
	// DocString: FMTwarning::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
			ar &  boost::serialization::make_nvp("parent_exception", boost::serialization::base_object<FMTexception>(*this));
		}
	public:
		// DocString: ~FMTwarning()
		/**
		Default destructor for FMTwarning.
		*/
		~FMTwarning()=default;
		// DocString: FMTwarning::operator=
		/**
		Default copy assignment for FMTwarning.
		*/
		FMTwarning& operator = (const FMTwarning&) = default;
		// DocString: FMTwarning()
		/**
		Default constructor for FMTwarning.
		*/
		FMTwarning();
		// DocString: FMTwarning(const FMTexception&)
		/**
		Default copy constructor for FMTwarning.
		*/
		FMTwarning(const FMTexception& rhs);
		// DocString: FMTwarning(const FMTexc,const std::string)
		/**
		Constructor for FMTwarning taking a exception type and message string as arguments.
		*/
		FMTwarning(const FMTexc lexception, const std::string message);
		// DocString: FMTwarning(const FMTexc,const Core::FMTsection,const std::string)
		/**
		Constructor for FMTwarning taking a exception type, section in which the warning occur and message string as arguments.
		*/
		FMTwarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message);
		// DocString: FMTwarning(const FMTexc,const Core::FMTsection, const std::string,const std::string&,const std::string&,const std::string&)
		/**
		Constructor for FMTwarning taking a exception type, section in which the warning occur and message string as arguments and the location
		where the exception occured (method/file/line).
		*/
		FMTwarning(const FMTexc lexception, const Core::FMTsection lsection, const std::string message,
			const std::string& lmethod, const std::string& lfile, const int& lline);
		// DocString: FMTwarning::warn
		/**
		Using a given logger the function log the warning with the logger. 
		*/
		void warn(const std::shared_ptr<Logging::FMTlogger>logger,std::unordered_map<int,size_t>& specificwarningcount, const size_t& maxwarning) const;
	};
}
#endif