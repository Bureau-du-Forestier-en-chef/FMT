/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTquiettlogger_Hm_included
#define FMTquiettlogger_Hm_included

#include "FMTLogger.h"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
// DocString: FMTQuietLogger
/**
@brief Logger derived from FMTLogger that does not print any solver information from the OSI solver interface.
*/
class FMTEXPORT FMTQuietLogger final : public FMTLogger
	{
	// DocString: FMTQuietLogger::serialize
	/**
	@brief Serialize the FMTQuietLogger through its base FMTLogger for multiprocessing across multiple cpus (pickle in Python).
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
		// DocString: FMTQuietLogger()
		/**
		@brief Default constructor for FMTQuietLogger.
		*/
		FMTQuietLogger();
		// DocString: FMTQuietLogger(const FMTQuietLogger&)
		/**
		@brief Default copy constructor for FMTQuietLogger.
		*/
		FMTQuietLogger(const FMTQuietLogger&) = default;
		// DocString: FMTQuietLogger::operator=
		/**
		@brief Default copy assignment operator for FMTQuietLogger.
		@return a reference to this FMTQuietLogger.
		*/
		FMTQuietLogger& operator = (const FMTQuietLogger&) = default;
		// DocString: FMTQuietLogger::~FMTQuietLogger
		/**
		@brief Default destructor for FMTQuietLogger.
		*/
		~FMTQuietLogger()=default;
		#ifdef FMTWITHOSI
			// DocString: FMTQuietLogger::print
			/**
			@brief Print nothing with the OSI solver interface. See FMTLogger::print.
			@return the value returned by the print function.
			*/
			int print() override;
			// DocString: FMTQuietLogger::checkSeverity
			/**
			@brief Check the severity of the message. See FMTLogger::checkSeverity.
			*/
			void checkSeverity() override;
			// DocString: FMTQuietLogger::clone
			/**
			@brief Clone the logger for the OSI solver interface. See FMTLogger::clone.
			@return a pointer to the cloned logger.
			*/
			FMTLogger* clone() const override;
			#endif
			// DocString: FMTQuietLogger::cout
			/**
			@brief Output nothing. See FMTLogger::cout.
			@param[in] message the message to output.
			*/
			void cout(const char* message) const override;
		// DocString: FMTQuietLogger::Clone
		/**
		@brief clone the FMTQuietLogger
		@return a valid cloned FMTQuietLogger
		*/
		virtual std::unique_ptr <FMTLogger> Clone() const;

	
	};
}

BOOST_CLASS_EXPORT_KEY(Logging::FMTQuietLogger)

#endif