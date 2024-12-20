/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTtasklogger_H_INCLUDED
#define FMTtasklogger_H_INCLUDED

#include "FMTlogger.h"
#include "FMTutility.h"
#include <boost/serialization/nvp.hpp>

namespace Logging
{
// DocString: FMTtasklogger
/**
FMTtasklogger will not print any solver informations from osisolverinterface and other defaultlogging
*/
class FMTEXPORT FMTtasklogger final : public FMTlogger
	{
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
			@brief It will do nothing because coinMessagehandler does not support multithread access.
			*/
			void checkSeverity() override;
			// DocString: FMTtasklogger::clone
			/**
			See FMTlogger clone function.
			*/
			FMTlogger* clone() const override;
		#endif
		// DocString: FMTtasklogger::Clone
		/**
		@brief clone the FMTtasklogger
		@return a valid cloned FMTtasklogger
		*/
		virtual std::unique_ptr <FMTlogger> Clone() const;
	private:
		// DocString: FMTtasklogger::serialize
		/**
		Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTlogger", boost::serialization::base_object<FMTlogger>(*this));
		}
	};
}

BOOST_CLASS_EXPORT_KEY(Logging::FMTtasklogger)

#endif