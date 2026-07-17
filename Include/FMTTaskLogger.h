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
FMTTaskLogger will not print any solver informations from osisolverinterface and other defaultlogging
*/
class FMTEXPORT FMTTaskLogger final : public FMTLogger
	{
	public:
		// DocString: FMTTaskLogger()
		/**
		FMTTaskLogger default constructor.
		*/
		FMTTaskLogger();
		// DocString: FMTTaskLogger(const FMTTaskLogger&)
		/**
		FMTTaskLogger default copy constructor.
		*/
		FMTTaskLogger(const FMTTaskLogger&) = default;
		// DocString: FMTTaskLogger::operator=
		/**
		FMTTaskLogger default copy assignment.
		*/
		FMTTaskLogger& operator = (const FMTTaskLogger&) = default;
		// DocString: FMTTaskLogger::~FMTTaskLogger
		/**
		FMTTaskLogger default destructor.
		*/
		~FMTTaskLogger()=default;
		#ifdef FMTWITHOSI
			// DocString: FMTTaskLogger::print
			/**
			FMTQuietLogger print nothing with osisolverinterface.
			See FMTLogger print function.
			*/
			int print() override;
			// DocString: FMTTaskLogger::checkSeverity
			/**
			@brief It will do nothing because coinMessagehandler does not support multithread access.
			*/
			void checkSeverity() override;
			// DocString: FMTTaskLogger::clone
			/**
			See FMTLogger clone function.
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
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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