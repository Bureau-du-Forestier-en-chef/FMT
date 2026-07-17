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
	FMTDebugLogger is derived class from FMTLogger standing has the debug
	level of log used by FMT. It's suppose to print more stuff than the FMTDefaultLogger.
	*/
	class FMTEXPORT FMTDebugLogger final : public FMTLogger
	{
		// DocString: FMTDebugLogger::serialize
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
		// DocString: FMTDebugLogger()
		/**
		FMTDebugLogger default constructor.
		*/
		FMTDebugLogger();
		// DocString: FMTDebugLogger::operator=
		/**
		FMTDebugLogger default copy assignment operator.
		*/
		FMTDebugLogger & operator = (const FMTDebugLogger & rhs) = default;
		// DocString: FMTDebugLogger(const FMTDebugLogger&)
		/**
		FMTDebugLogger default copy constructor.
		*/
		FMTDebugLogger(const FMTDebugLogger& rhs) = default;
		#ifdef FMTWITHOSI
			// DocString: FMTDebugLogger::print
			/**
			FMTDebugLogger print for osisolverinterface is the debug print level used by FMT.
			See FMTLogger print function.
			*/
			int print() override;
			// DocString: FMTDebugLogger::checkSeverity
			/**
			FMTDebugLogger checkseverity for osisolverinterface is the debug severity check used by FMT.
			See FMTLogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTDebugLogger::clone
			/**
			See FMTLogger clone function.
			*/
			FMTLogger* clone() const override;
		#endif
		// DocString: FMTDebugLogger()
		/**
		FMTDebugLogger default destructor.
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

