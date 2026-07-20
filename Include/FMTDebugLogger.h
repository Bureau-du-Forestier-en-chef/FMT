/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdebuglogger_Hm_included
#define FMTdebuglogger_Hm_included
#include "FMTLogger.h"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
	// DocString: FMTDebugLogger 
	/**
	@brief Debug level logger derived from FMTLogger that prints more than FMTDefaultLogger.
	*/
	class FMTEXPORT FMTDebugLogger final : public FMTLogger
	{
		// DocString: FMTDebugLogger::serialize
		/**
		@brief Serialize the FMTDebugLogger through its base FMTLogger for multiprocessing across multiple cpus (pickle in Python).
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
		// DocString: FMTDebugLogger()
		/**
		@brief Default constructor for FMTDebugLogger.
		*/
		FMTDebugLogger();
		// DocString: FMTDebugLogger::operator=
		/**
		@brief Default copy assignment operator for FMTDebugLogger.
		@param[in] rhs the FMTDebugLogger to copy.
		@return a reference to this FMTDebugLogger.
		*/
		FMTDebugLogger & operator = (const FMTDebugLogger & rhs) = default;
		// DocString: FMTDebugLogger(const FMTDebugLogger&)
		/**
		@brief Default copy constructor for FMTDebugLogger.
		@param[in] rhs the FMTDebugLogger to copy.
		*/
		FMTDebugLogger(const FMTDebugLogger& rhs) = default;
		#ifdef FMTWITHOSI
			// DocString: FMTDebugLogger::print
			/**
			@brief Debug print level used by FMT for the OSI solver interface. See FMTLogger::print.
			@return the value returned by the print function.
			*/
			int print() override;
			// DocString: FMTDebugLogger::checkSeverity
			/**
			@brief Debug severity check used by FMT for the OSI solver interface. See FMTLogger::checkSeverity.
			*/
			void checkSeverity() override;
			// DocString: FMTDebugLogger::clone
			/**
			@brief Clone the logger for the OSI solver interface. See FMTLogger::clone.
			@return a pointer to the cloned logger.
			*/
			FMTLogger* clone() const override;
		#endif
		// DocString: ~FMTDebugLogger()
		/**
		@brief Default destructor for FMTDebugLogger.
		*/
		~FMTDebugLogger() = default;
		// DocString: FMTDebugLogger::Clone
		/**
		@brief clone the FMTDebugLogger
		@return a valid clonedFMTdebuglogger
		*/
		virtual std::unique_ptr <FMTLogger> Clone() const;
	};
}

BOOST_CLASS_EXPORT_KEY(Logging::FMTDebugLogger)

#endif

