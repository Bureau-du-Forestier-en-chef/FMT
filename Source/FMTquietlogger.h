/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTquiettlogger_H_INCLUDED
#define FMTquiettlogger_H_INCLUDED

#include "FMTlogger.h"

namespace Logging
{
// DocString: FMTquietlogger
/**
FMTquietlogger will not print any solver informations from osisolverinterface.
*/
class FMTquietlogger final : public FMTlogger
	{
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
			CoinMessageHandler * clone() const override;
	#endif
	};
}



#endif