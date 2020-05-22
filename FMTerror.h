/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTerror_H_INCLUDED
#define FMTerror_H_INCLUDED

#include "FMTexception.h"
#include <string>
#include <boost/serialization/serialization.hpp>


namespace Exception
	{
	// DocString: FMTerror
	/**
	FMTerror is derived from the FMTexception base class.
	See FMTexception class for more information about the member data.
	Any error should throw in FMT.
	*/
	class FMTerror : public FMTexception
		{
		// DocString: FMTerror::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar &  boost::serialization::make_nvp("exception", boost::serialization::base_object<FMTexception>(*this));
		}
		public:
			// DocString: FMTerror()
			/**
			Default constructor for FMTerror.
			*/
			FMTerror();
			// DocString: FMTerror(const FMTexception&)
			/**
			Default copy constructor for FMTerror.
			*/
			FMTerror(const FMTexception& rhs);
			// DocString: FMTerror(const FMTexc,const std::string)
			/**
			Constructor for FMTerror taking a exception type and message string as arguments.
			*/
			FMTerror(const FMTexc lexception, const std::string message);
			// DocString: FMTerror(const FMTexc,const std::string)
			/**
			Constructor for FMTerror taking a exception type, section in which the error occur and message string as arguments.
			*/
			FMTerror(const FMTexc lexception, const Core::FMTsection lsection, const std::string message);
		};
	}
#endif

