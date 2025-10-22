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
	// DocString: FMTdefaultlogger 
	/**
	FMTdefaultlogger is derived class from FMTlogger standing has the default
	level of log used by FMT.
	*/
	class FMTEXPORT FMTdefaultlogger final: public FMTlogger
	{
		// DocString: FMTdefaultlogger::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("FMTlogger", boost::serialization::base_object<FMTlogger>(*this));
		}
	public:
		// DocString: FMTdefaultlogger()
		/**
		FMTdefaultlogger default constructor.
		*/
		FMTdefaultlogger();
		// DocString: ~FMTdefaultlogger()
		/**
		FMTdefaultlogger default destructor.
		*/
		~FMTdefaultlogger() = default;
		// DocString: FMTdefaultlogger::operator=
		/**
		FMTdefaultlogger default copy assignment operator.
		*/
		FMTdefaultlogger & operator = (const FMTdefaultlogger & rhs) = default;
		// DocString: FMTdefaultlogger(const FMTdefaultlogger&)
		/**
		FMTdefaultlogger default copy constructor.
		*/
		FMTdefaultlogger(const FMTdefaultlogger& rhs) = default;
		#ifdef FMTWITHOSI
			// DocString: FMTdefaultlogger::print
			/**
			FMTdefaultlogger print for osisolverinterface is the default print level used by FMT.
			See FMTlogger print function.
			*/
			int print() override;
			// DocString: FMTdefaultlogger::checkSeverity
			/**
			FMTdefaultlogger checkseverity for osisolverinterface is the default severity check used by FMT.
			See FMTlogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTdefaultlogger::clone
			/**
			See FMTlogger clone function.
			*/
			FMTlogger* clone() const override;
		#endif
		// DocString: FMTdefaultlogger::Clone
		/**
		@brief clone the FMTdefaultlogger
		@return a valid cloned FMTdefaultlogger
		*/
		virtual std::unique_ptr <FMTlogger> Clone() const;
	};

}

BOOST_CLASS_EXPORT_KEY(Logging::FMTdefaultlogger)

#endif