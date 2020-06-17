/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTdebuglogger_H_INCLUDED
#define FMTdebuglogger_H_INCLUDED
#include "FMTlogger.h"
namespace Logging
{
	// DocString: FMTdebuglogger 
	/**
	FMTdebuglogger is derived class from FMTlogger standing has the debug
	level of log used by FMT. It's suppose to print more stuff than the FMTdefaultlogger.
	*/
	class FMTdebuglogger final : public FMTlogger
	{
	public:
		// DocString: FMTdebuglogger()
		/**
		FMTdebuglogger default constructor.
		*/
		FMTdebuglogger();
		// DocString: FMTdebuglogger::operator=
		/**
		FMTdebuglogger default copy assignment operator.
		*/
		FMTdebuglogger & operator = (const FMTdebuglogger & rhs) = default;
		// DocString: FMTdebuglogger(const FMTdebuglogger&)
		/**
		FMTdebuglogger default copy constructor.
		*/
		FMTdebuglogger(const FMTdebuglogger& rhs) = default;
		#ifdef FMTWITHOSI
			// DocString: FMTdebuglogger::print
			/**
			FMTdebuglogger print for osisolverinterface is the debug print level used by FMT.
			See FMTlogger print function.
			*/
			int print() override;
			// DocString: FMTdebuglogger::checkSeverity
			/**
			FMTdebuglogger checkseverity for osisolverinterface is the debug severity check used by FMT.
			See FMTlogger checkSeverity function.
			*/
			void checkSeverity() override;
			// DocString: FMTdebuglogger::clone
			/**
			See FMTlogger clone function.
			*/
			CoinMessageHandler* clone() const override;
		#endif
		// DocString: FMTdebuglogger()
		/**
		FMTdebuglogger default destructor.
		*/
		~FMTdebuglogger() = default;
	};
}

#endif

