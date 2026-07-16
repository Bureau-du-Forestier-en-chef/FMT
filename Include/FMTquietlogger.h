/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTquiettlogger_Hm_included
#define FMTquiettlogger_Hm_included

#include "FMTlogger.h"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
// DocString: FMTQuietLogger
/**
FMTQuietLogger will not print any solver informations from osisolverinterface.
*/
class FMTEXPORT FMTQuietLogger final : public FMTLogger
	{
	// DocString: FMTQuietLogger::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
		FMTQuietLogger default constructor.
		*/
		FMTQuietLogger();
		// DocString: FMTQuietLogger(const FMTQuietLogger&)
		/**
		FMTQuietLogger default copy constructor.
		*/
		FMTQuietLogger(const FMTQuietLogger&) = default;
		// DocString: FMTQuietLogger::operator=
		/**
		FMTQuietLogger default copy assignment.
		*/
		FMTQuietLogger& operator = (const FMTQuietLogger&) = default;
		// DocString: FMTQuietLogger::~FMTQuietLogger
		/**
		FMTQuietLogger default destructor.
		*/
		~FMTQuietLogger()=default;
		#ifdef FMTWITHOSI
			// DocString: FMTQuietLogger::print
			/**
			FMTQuietLogger print nothing with osisolverinterface.
			See FMTLogger print function.
			*/
			int print() override;
			// DocString: FMTQuietLogger::checkSeverity
			/**
			See FMTLogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTQuietLogger::clone
			/**
			See FMTLogger clone function.
			*/
			FMTLogger* clone() const override;
			#endif
			// DocString: FMTQuietLogger::cout
			/**
			See FMTLogger cout function that does nothing.
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