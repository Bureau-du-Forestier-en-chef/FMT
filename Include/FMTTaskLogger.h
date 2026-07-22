/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTtasklogger_Hm_included
#define FMTtasklogger_Hm_included

#include "FMTLogger.h"
#include "FMTutility.h"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
// DocString: FMTTaskLogger
/**
@brief Logger derived from FMTLogger that does not print solver information nor default logging, for use in tasks.
*/
class FMTEXPORT FMTTaskLogger final : public FMTLogger
	{
	public:
		// DocString: FMTTaskLogger()
		/**
		@brief Default constructor for FMTTaskLogger.
		*/
		FMTTaskLogger();
		// DocString: FMTTaskLogger(const FMTTaskLogger&)
		/**
		@brief Default copy constructor for FMTTaskLogger.
		*/
		FMTTaskLogger(const FMTTaskLogger&) = default;
		// DocString: FMTTaskLogger::operator=
		/**
		@brief Default copy assignment operator for FMTTaskLogger.
		@return a reference to this FMTTaskLogger.
		*/
		FMTTaskLogger& operator = (const FMTTaskLogger&) = default;
		// DocString: FMTTaskLogger::~FMTTaskLogger
		/**
		@brief Default destructor for FMTTaskLogger.
		*/
		~FMTTaskLogger()=default;
		#ifdef FMTWITHOSI
			// DocString: FMTTaskLogger::print
			/**
			@brief Print nothing with the OSI solver interface. See FMTLogger::print.
			@return the value returned by the print function.
			*/
			int print() override;
			// DocString: FMTTaskLogger::checkSeverity
			/**
			@brief It will do nothing because coinMessagehandler does not support multithread access.
			*/
			void checkSeverity() override;
			// DocString: FMTTaskLogger::clone
			/**
			@brief Clone the logger for the OSI solver interface. See FMTLogger::clone.
			@return a pointer to the cloned logger.
			*/
			FMTLogger* clone() const override;
		#endif
		// DocString: FMTTaskLogger::Clone
		/**
		@brief clone the FMTTaskLogger
		@return a valid cloned FMTTaskLogger
		*/
		virtual std::unique_ptr <FMTLogger> Clone() const;
	private:
		// DocString: FMTTaskLogger::serialize
		/**
		@brief Serialize the FMTTaskLogger through its base FMTLogger for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTlogger", boost::serialization::base_object<FMTLogger>(*this));
		}
	};
}

BOOST_CLASS_EXPORT_KEY(Logging::FMTTaskLogger)

#endif