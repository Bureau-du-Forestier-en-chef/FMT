/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTerror_Hm_included
#define FMTerror_Hm_included

#include "FMTexception.h"
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
	FMTError is derived from the FMTException base class.
	See FMTException class for more information about the member data.
	Any error should throw in FMT.
	*/
	class FMTEXPORT FMTError : public FMTException
		{
		// DocString: FMTError::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
			Default destructor for FMTError.
			*/
			~FMTError() = default;
			// DocString: FMTError::operator=
			/**
			Default assignment operator for FMTError.
			*/
			FMTError& operator=(const FMTError&) = default;
			// DocString: FMTError()
			/**
			Default constructor for FMTError.
			*/
			FMTError();
			// DocString: FMTError(const FMTException&)
			/**
			Default copy constructor for FMTError.
			*/
			FMTError(const FMTException& rhs);
			// DocString: FMTError(const FMTexc,const std::string)
			/**
			Constructor for FMTError taking a exception type and message string as arguments.
			*/
			FMTError(const FMTexc lexception, const std::string message);
			// DocString: FMTError(const FMTexc,const Core::FMTsection,const std::string)
			/**
			Constructor for FMTError taking a exception type, section in which the error occur and message string as arguments.
			*/
			FMTError(const FMTexc lexception, const Core::FMTsection lsection, const std::string message);
			// DocString: FMTError(const FMTexc,const Core::FMTsection,const std::string,const std::string&,const std::string&,const int&)
			/**
			Constructor for FMTError taking a exception type, section in which the error occur and message string as arguments and the location
			where the exception occured (method/file/line).
			*/
			FMTError(const FMTexc lexception,const Core::FMTsection lsection, const std::string message,
					const std::string& lmethod, const std::string& lfile, const int& lline);
			// DocString: FMTError(const FMTexc,const std::string,const std::string&,const std::string&,const int&)
			/**
			Constructor for FMTError taking a exception type, section in which the error occur and message string as arguments and the location
			where the exception occured (method/file/line).
			*/
			FMTError(const FMTexc lexception, const std::string message,
				const std::string& lmethod, const std::string& lfile, const int& lline);
			#if defined FMTWITHOSI
				// DocString: FMTError(const CoinError&)
				/**
				Constructor for FMTError from a CoinError.
				*/
				FMTError(const CoinError& coinexception);
			#endif
			// DocString: FMTError(const boost::bad_graph&)
			/**
			Constructor for FMTError from a boost::bad_graph.
			*/
			FMTError(const boost::bad_graph& boostexception);

		};
	}
#endif

