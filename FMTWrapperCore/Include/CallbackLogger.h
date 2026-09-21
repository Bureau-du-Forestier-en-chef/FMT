#pragma once
#include "FMTLogger.h"
#include "FMTWrapperCoreExport.h"
#include <fstream>
#include <iostream>
#include <memory>

namespace FMTWrapper::Backend
{
	class EventPublisher;

	// DocString: CallbackLogger
	/**
	@brief Logger implementation used by the graphical interface.

	This logger extends Logging::FMTLogger and writes its output both to
	a log file and to the events published by the Core.
	*/
	class FMT_WRAPPER_CORE_EXPORT CallbackLogger final :
		public Logging::FMTLogger
	{
	private:
		bool keepprint;
		mutable std::string lastprint;
		const EventPublisher* m_publisher = nullptr;
		bool m_isMainInstance;

	protected:

		// DocString: CallbackLogger::_cout
		/**
		@brief Outputs a message to the configured logging destinations.

		The message is written to the log file when available. If message
		buffering is enabled, the message is stored internally; otherwise
		it is published as a LogEvent.

		@param[in] message Message to output.
		*/
		void _cout(const char* message) const;

	public:

		// DocString: CallbackLogger::logTime
		/**
		@brief Overrides the base timestamp logging behavior.

		This implementation intentionally performs no action. Timestamps
		are written explicitly when required and the interface cannot
		safely be reached from native worker threads.
		*/
		void logTime() override;

		// DocString: CallbackLogger()
		/**
		@brief Default constructor.
		*/
		CallbackLogger() = default;

		// DocString: CallbackLogger(const CallbackLogger&)
		/**
		@brief Copy constructor.

		@param[in] rhs Logger to copy.
		*/
		CallbackLogger(const CallbackLogger& rhs);

		// DocString: ~CallbackLogger()
		/**
		@brief Destructor.

		The log file is closed before the base class destructor executes.
		*/
		~CallbackLogger();

		// DocString: CallbackLogger::dokeepprint
		/**
		@brief Enables message buffering.

		When buffering is enabled, log messages are accumulated internally
		instead of being published.
		*/
		void dokeepprint();

		// DocString: CallbackLogger::closeFile
		/**
		@brief Closes the current log file and releases the associated stream.

		This method safely closes the log file if it is currently open.
		*/
		void closeFile();

		// DocString: CallbackLogger::resetkeepprint
		/**
		@brief Disables message buffering and clears buffered content.
		*/
		void resetkeepprint();

		// DocString: CallbackLogger::getlastprint
		/**
		@brief Returns the buffered log output.

		@return The accumulated log messages captured while buffering was enabled.
		*/
		std::string getlastprint() const;

		// DocString: CallbackLogger(const std::string&,const EventPublisher*)
		/**
		@brief Constructs a logger writing to a log file and publishing its messages.

		@param[in] p_logFilePath Path of the log file.
		@param[in] p_publisher Publisher through which every message is reported. It must
		outlive the logger. A null publisher writes to the file only.
		*/
		CallbackLogger(const std::string& p_logFilePath, const EventPublisher* p_publisher);

		// DocString: CallbackLogger::settasklogginglevel
		/**
		@brief Sets the logging level used for task execution.

		@param[in] taskLogLevel Logging level to apply.
		*/
		void settasklogginglevel(int taskLogLevel);

		// DocString: CallbackLogger::setdefaultlogginglevel
		/**
		@brief Restores the default logging level.
		*/
		void setdefaultlogginglevel();

		// DocString: CallbackLogger::Clone
		/**
		@brief Creates a copy of this logger.

		@return A unique pointer to the cloned logger.
		*/
		virtual std::unique_ptr<FMTLogger> Clone() const;

#ifdef FMTWITHOSI

		// DocString: CallbackLogger::clone
		/**
		@brief Creates a copy of this logger.

		@return A pointer to the cloned logger.
		*/
		virtual FMTLogger* clone() const;

#endif
	};
}
