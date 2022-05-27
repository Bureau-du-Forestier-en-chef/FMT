/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTtasklogger_H_INCLUDED
#define FMTtasklogger_H_INCLUDED

#include "FMTlogger.hpp"
#include "FMTutility.hpp"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
// DocString: FMTtasklogger
/**
FMTtasklogger will not print any solver informations from osisolverinterface and other defaultlogging
*/
class FMTEXPORT FMTtasklogger final : public FMTlogger
	{
	// DocString: FMTtasklogger::serialize
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
		// DocString: FMTtasklogger()
		/**
		FMTtasklogger default constructor.
		*/
		FMTtasklogger();
		// DocString: FMTtasklogger(const FMTtasklogger&)
		/**
		FMTtasklogger default copy constructor.
		*/
		FMTtasklogger(const FMTtasklogger&) = default;
		// DocString: FMTtasklogger::operator=
		/**
		FMTtasklogger default copy assignment.
		*/
		FMTtasklogger& operator = (const FMTtasklogger&) = default;
		// DocString: FMTtasklogger::~FMTtasklogger
		/**
		FMTtasklogger default destructor.
		*/
		~FMTtasklogger()=default;
		#ifdef FMTWITHOSI
			// DocString: FMTtasklogger::print
			/**
			FMTquietlogger print nothing with osisolverinterface.
			See FMTlogger print function.
			*/
			int print() override;
			// DocString: FMTtasklogger::checkSeverity
			/**
			See FMTlogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTtasklogger::clone
			/**
			See FMTlogger clone function.
			*/
			FMTlogger* clone() const override;
		#endif
	};
}

BOOST_CLASS_EXPORT_KEY(Logging::FMTtasklogger)

#endif