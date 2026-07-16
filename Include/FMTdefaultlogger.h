/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdefaultlogger_Hm_included
#define FMTdefaultlogger_Hm_included
#include "FMTlogger.h"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
	// DocString: FMTDefaultLogger 
	/**
	FMTDefaultLogger is derived class from FMTLogger standing has the default
	level of log used by FMT.
	*/
	class FMTEXPORT FMTDefaultLogger final: public FMTLogger
	{
		// DocString: FMTDefaultLogger::serialize
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
		// DocString: FMTDefaultLogger()
		/**
		FMTDefaultLogger default constructor.
		*/
		FMTDefaultLogger();
		// DocString: ~FMTDefaultLogger()
		/**
		FMTDefaultLogger default destructor.
		*/
		~FMTDefaultLogger() = default;
		// DocString: FMTDefaultLogger::operator=
		/**
		FMTDefaultLogger default copy assignment operator.
		*/
		FMTDefaultLogger & operator = (const FMTDefaultLogger & rhs) = default;
		// DocString: FMTDefaultLogger(const FMTDefaultLogger&)
		/**
		FMTDefaultLogger default copy constructor.
		*/
		FMTDefaultLogger(const FMTDefaultLogger& rhs) = default;
		#ifdef FMTWITHOSI
			// DocString: FMTDefaultLogger::print
			/**
			FMTDefaultLogger print for osisolverinterface is the default print level used by FMT.
			See FMTLogger print function.
			*/
			int print() override;
			// DocString: FMTDefaultLogger::checkSeverity
			/**
			FMTDefaultLogger checkseverity for osisolverinterface is the default severity check used by FMT.
			See FMTLogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTDefaultLogger::clone
			/**
			See FMTLogger clone function.
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