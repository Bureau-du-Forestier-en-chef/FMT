/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdefaultlogger_Hm_included
#define FMTdefaultlogger_Hm_included
#include "FMTLogger.h"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
	// DocString: FMTDefaultLogger 
	/**
	@brief Default level logger derived from FMTLogger used by FMT.
	*/
	class FMTEXPORT FMTDefaultLogger final: public FMTLogger
	{
		// DocString: FMTDefaultLogger::serialize
		/**
		@brief Serialize the FMTDefaultLogger through its base FMTLogger for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTlogger", boost::serialization::base_object<FMTLogger>(*this));
		}
	public:
		// DocString: FMTDefaultLogger()
		/**
		@brief Default constructor for FMTDefaultLogger.
		*/
		FMTDefaultLogger();
		// DocString: ~FMTDefaultLogger()
		/**
		@brief Default destructor for FMTDefaultLogger.
		*/
		~FMTDefaultLogger() = default;
		// DocString: FMTDefaultLogger::operator=
		/**
		@brief Default copy assignment operator for FMTDefaultLogger.
		@param[in] rhs the FMTDefaultLogger to copy.
		@return a reference to this FMTDefaultLogger.
		*/
		FMTDefaultLogger & operator = (const FMTDefaultLogger & rhs) = default;
		// DocString: FMTDefaultLogger(const FMTDefaultLogger&)
		/**
		@brief Default copy constructor for FMTDefaultLogger.
		@param[in] rhs the FMTDefaultLogger to copy.
		*/
		FMTDefaultLogger(const FMTDefaultLogger& rhs) = default;
		#ifdef FMTWITHOSI
			// DocString: FMTDefaultLogger::print
			/**
			@brief Default print level used by FMT for the OSI solver interface. See FMTLogger::print.
			@return the value returned by the print function.
			*/
			int print() override;
			// DocString: FMTDefaultLogger::checkSeverity
			/**
			@brief Default severity check used by FMT for the OSI solver interface. See FMTLogger::checkSeverity.
			*/
			void checkSeverity() override;
			// DocString: FMTDefaultLogger::clone
			/**
			@brief Clone the logger for the OSI solver interface. See FMTLogger::clone.
			@return a pointer to the cloned logger.
			*/
			FMTLogger* clone() const override;
		#endif
		// DocString: FMTDefaultLogger::Clone
		/**
		@brief clone the FMTDefaultLogger
		@return a valid cloned FMTDefaultLogger
		*/
		virtual std::unique_ptr <FMTLogger> Clone() const;
	};

}

BOOST_CLASS_EXPORT_KEY(Logging::FMTDefaultLogger)

#endif