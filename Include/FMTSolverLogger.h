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
	@brief Abstract logger used with the OSI solver interface (CoinMessageHandler).
	*/
	class FMTSolverLogger: public CoinMessageHandler
		{
		friend class FMTLogger;
		public:
			// DocString: FMTSolverLogger(const FMTLogger& baselogger)
			/**
			@brief Construct a solver logger from a FMTLogger.
			@param[in] baselogger the base FMTLogger to use.
			*/
			FMTSolverLogger(FMTLogger& baselogger);
			// DocString: FMTSolverLogger()
			/**
			@brief Default constructor for FMTSolverLogger.
			*/
			FMTSolverLogger();
			// DocString: ~FMTSolverLogger()
			/**
			@brief Default destructor for FMTSolverLogger.
			*/
			virtual ~FMTSolverLogger();
			// DocString: FMTSolverLogger(const FMTsolverlogge&)
			/**
			@brief Default copy constructor for FMTSolverLogger.
			@param[in] rhs the FMTSolverLogger to copy.
			*/
			FMTSolverLogger(const FMTSolverLogger& rhs)=default;
			
			// DocString: FMTLogger::operator=
			/**
			@brief Copy assignment operator for FMTSolverLogger.
			@param[in] rhs the FMTSolverLogger to copy.
			@return a reference to this FMTSolverLogger.
			*/
			FMTSolverLogger& operator = (const FMTSolverLogger& rhs)=default;
			// DocString: FMTSolverLogger::print
			/**
			@brief Override the CoinMessageHandler print function used with the OSI solver interface.
			@return the value returned by the print function.
			*/
			int print() override;
			// DocString: FMTSolverLogger::checkSeverity
			/**
			@brief Check the severity of the message to be printed by the CoinMessageHandler base class.
			*/
			void checkSeverity() override;
			// DocString: FMTSolverLogger::checkcoinSeverity
			/**
			@brief Check the severity of the coin message to be printed by the CoinMessageHandler base class.
			*/
			void checkcoinSeverity();
			// DocString: FMTSolverLogger::clone
			/**
			@brief Clone the logger, needed for the abstract CoinMessageHandler class.
			@return a pointer to the cloned CoinMessageHandler.
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
			@brief Delete the base logger if it is owned by this logger.
			*/
			void cleanUp();
			// DocString: FMTSolverLogger::copy()
			/**
			@brief Safely copy from another solver logger.
			@param[in] rhs the FMTSolverLogger to copy from.
			*/
			void copy(const FMTSolverLogger& rhs);
			// DocString: FMTSolverLogger::copyFrom()
			/**
			@brief Copy the members from another solver logger.
			@param[in] rhs the FMTSolverLogger to copy from.
			*/
			void copyFrom(const FMTSolverLogger& rhs);
		};
}
#endif
#endif
