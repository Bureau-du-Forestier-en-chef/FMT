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
// DocString: FMTquietlogger
/**
FMTquietlogger will not print any solver informations from osisolverinterface.
*/
class FMTEXPORT FMTquietlogger final : public FMTlogger
	{
	// DocString: FMTquietlogger::serialize
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
		// DocString: FMTquietlogger()
		/**
		FMTquietlogger default constructor.
		*/
		FMTquietlogger();
		// DocString: FMTquietlogger(const FMTquietlogger&)
		/**
		FMTquietlogger default copy constructor.
		*/
		FMTquietlogger(const FMTquietlogger&) = default;
		// DocString: FMTquietlogger::operator=
		/**
		FMTquietlogger default copy assignment.
		*/
		FMTquietlogger& operator = (const FMTquietlogger&) = default;
		// DocString: FMTquietlogger::~FMTquietlogger
		/**
		FMTquietlogger default destructor.
		*/
		~FMTquietlogger()=default;
		#ifdef FMTWITHOSI
			// DocString: FMTquietlogger::print
			/**
			FMTquietlogger print nothing with osisolverinterface.
			See FMTlogger print function.
			*/
			int print() override;
			// DocString: FMTquietlogger::checkSeverity
			/**
			See FMTlogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTquietlogger::clone
			/**
			See FMTlogger clone function.
			*/
			FMTlogger* clone() const override;
			#endif
			// DocString: FMTquietlogger::cout
			/**
			See FMTlogger cout function that does nothing.
			*/
			void cout(const char* message) const override;
		// DocString: FMTquietlogger::Clone
		/**
		@brief clone the FMTquietlogger
		@return a valid cloned FMTquietlogger
		*/
		virtual std::unique_ptr <FMTlogger> Clone() const;

	
	};
}

BOOST_CLASS_EXPORT_KEY(Logging::FMTquietlogger)

#endif