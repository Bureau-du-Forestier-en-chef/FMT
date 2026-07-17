/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTsolverlogger_Hm_included
#define FMTsolverlogger_Hm_included


#ifdef FMTWITHOSI
	#include <CoinMessageHandler.hpp>


/// Namespace for the log management, provides different log handlers.
namespace Logging
{
	class FMTLogger;
	// DocString: FMTSolverLogger
	/**
	Abstract class for usage in Osisolverinterface.
	*/
	class FMTSolverLogger: public CoinMessageHandler
		{
		friend class FMTLogger;
		public:
			// DocString: FMTSolverLogger(const FMTLogger& baselogger)
			/**
			Constuct a logger using an abstrsact FMTLogger.
			*/
			FMTSolverLogger(FMTLogger& baselogger);
			// DocString: FMTSolverLogger()
			/**
			FMTSolverLogger default constructor.
			*/
			FMTSolverLogger();
			// DocString: ~FMTSolverLogger()
			/**
			FMTsolverlogge default destructor.
			*/
			virtual ~FMTSolverLogger();
			// DocString: FMTSolverLogger(const FMTsolverlogge&)
			/**
			FMTSolverLogger copy constructor.
			*/
			FMTSolverLogger(const FMTSolverLogger& rhs)=default;
			
			// DocString: FMTLogger::operator=
			/**
			FMTsolverlogge copy assignment operator.
			*/
			FMTSolverLogger& operator = (const FMTSolverLogger& rhs)=default;
			// DocString: FMTSolverLogger::print
			/**
			FMTsolverlogge print function if we are using Osisolverinterface the coinmessagehandler
			print function needs to be overloaded.
			*/
			int print() override;
			// DocString: FMTSolverLogger::checkSeverity
			/**
			FMTsolverlogge check the severity of the message to be print by the coinmessagehandler base class.
			*/
			void checkSeverity() override;
			// DocString: FMTSolverLogger::checkcoinSeverity
			/**
			FMTsolverlogge check the severity of the coin message to be print by the coinmessagehandler base class.
			*/
			void checkcoinSeverity();
			// DocString: FMTSolverLogger::clone
			/**
			Clone function needed for the usage of abstract coingmessagehandler class.
			*/
			CoinMessageHandler * clone() const override;
		private:
			// DocString: FMTSolverLogger::baselogger
			///The base FMTLogger that the abstract class refer to.
			FMTLogger* baselogger;
			// DocString: FMTSolverLogger::ownthelogger
			///If true the logger own the baselogger and need to delete it when destructor called.
			bool ownthelogger;
			// DocString: FMTSolverLogger::cleanUp()
			/**
			Check if you can delete baselogger and delete it if so.
			*/
			void cleanUp();
			// DocString: FMTSolverLogger::copy()
			/**
			Safely copy from an other solverlogger
			*/
			void copy(const FMTSolverLogger& rhs);
			// DocString: FMTSolverLogger::copyFrom()
			/**
			FMTSolverLogger copy constructor.
			*/
			void copyFrom(const FMTSolverLogger& rhs);
		};
}
#endif
#endif
