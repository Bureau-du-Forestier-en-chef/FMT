/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTsolverlogger_H_INCLUDED
#define FMTsolverlogger_H_INCLUDED


#ifdef FMTWITHOSI
	#include <CoinMessageHandler.hpp>
#endif

/// Namespace for the log management, provides different log handlers.
namespace Logging
{
	class FMTlogger;
	// DocString: FMTsolverlogger
	/**
	Abstract class for usage in Osisolverinterface.
	*/
	class FMTsolverlogger: public CoinMessageHandler
		{
		friend class FMTlogger;
		// DocString: FMTsolverlogger::baselogger
		///The base FMTlogger that the abstract class refer to.
		FMTlogger* baselogger;
		// DocString: FMTsolverlogger::ownthelogger
		///If true the logger own the baselogger and need to delete it when destructor called.
		bool ownthelogger;
		// DocString: FMTsolverlogger::cleanup()
		/**
		Check if you can delete baselogger and delete it if so.
		*/
		void cleanup();
		// DocString: FMTsolverlogger::copy()
		/**
		Safely copy from an other solverlogger
		*/
		void copy(const FMTsolverlogger& rhs);
		public:
			// DocString: FMTsolverlogger(const FMTlogger& baselogger)
			/**
			Constuct a logger using an abstrsact FMTlogger.
			*/
			FMTsolverlogger(FMTlogger& baselogger);
			// DocString: FMTsolverlogger()
			/**
			FMTsolverlogger default constructor.
			*/
			FMTsolverlogger();
			// DocString: ~FMTsolverlogger()
			/**
			FMTsolverlogge default destructor.
			*/
			virtual ~FMTsolverlogger();
			// DocString: FMTsolverlogger(const FMTsolverlogge&)
			/**
			FMTsolverlogger copy constructor.
			*/
			FMTsolverlogger(const FMTsolverlogger& rhs);
			// DocString: FMTlogger::operator=
			/**
			FMTsolverlogge copy assignment operator.
			*/
			FMTsolverlogger& operator = (const FMTsolverlogger& rhs);
			// DocString: FMTsolverlogger::print
			/**
			FMTsolverlogge print function if we are using Osisolverinterface the coinmessagehandler
			print function needs to be overloaded.
			*/
			int print() override;
			// DocString: FMTsolverlogger::checkSeverity
			/**
			FMTsolverlogge check the severity of the message to be print by the coinmessagehandler base class.
			*/
			void checkSeverity() override;
			// DocString: FMTsolverlogger::checkcoinSeverity
			/**
			FMTsolverlogge check the severity of the coin message to be print by the coinmessagehandler base class.
			*/
			void checkcoinSeverity();
			// DocString: FMTsolverlogger::clone
			/**
			Clone function needed for the usage of abstract coingmessagehandler class.
			*/
			CoinMessageHandler * clone() const override;
		};
}
#endif
