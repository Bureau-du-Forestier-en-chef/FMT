#pragma once

#include "FMTExceptionHandler.h"
#include "FMTWrapperCoreExport.h"

namespace FMTWrapperCore
{
	// DocString: WarningExceptionHandler
	/**
	@brief Exception handler used by the graphical interface.

	This handler extends Exception::FMTExceptionHandler and provides
	warning management, error stack formatting and integration with
	the graphical user interface. Fatal exceptions may be rethrown
	while warning-level exceptions are tracked and counted.
	*/
	class FMT_WRAPPER_CORE_EXPORT WarningExceptionHandler :
		public virtual Exception::FMTExceptionHandler
	{
	public:

		// DocString: WarningExceptionHandler(const size_t&)
		/**
		@brief Constructs a warning-aware exception handler.

		@param[in] maxnumberofwarnings Maximum number of warnings that
		may be emitted before warning messages are silenced.
		*/
		WarningExceptionHandler(const size_t& maxnumberofwarnings);

		// DocString: WarningExceptionHandler::printExceptions
		/**
		@brief Processes exception information captured in a catch block.

		The exception information is forwarded to the exception handling
		mechanism for formatting and processing.

		@param[in] text Exception message.
		@param[in] method Method where the exception occurred.
		@param[in] line Source line where the exception occurred.
		@param[in] fil Source file where the exception occurred.
		@param[in] lsection FMT section where the exception occurred.
		*/
		virtual void printExceptions(
			std::string text,
			const std::string& method,
			const int& line,
			const std::string& fil,
			Core::FMTsection lsection) override;

		// DocString: WarningExceptionHandler::tryfileopener
		/**
		@brief Attempts to open the source file associated with an error.

		If the supplied error string contains file and line information,
		Notepad++ is launched at the corresponding location to help with
		diagnostics.

		@param[in] fullerrorstr Complete formatted error string.
		*/
		void tryfileopener(const std::string& fullerrorstr) const;

		// DocString: WarningExceptionHandler::geterrorstack
		/**
		@brief Builds a formatted exception stack trace.

		The exception information is temporarily redirected to the
		graphical logger and returned as a formatted string.

		@param[in] text Exception message.
		@param[in] method Method where the exception occurred.
		@param[in] line Source line where the exception occurred.
		@param[in] fil Source file where the exception occurred.

		@return Formatted exception stack trace.
		*/
		std::string geterrorstack(
			std::string text,
			const std::string& method,
			const int& line,
			const std::string& fil);

		// DocString: WarningExceptionHandler::raise
		/**
		@brief Creates, processes and optionally throws an FMT exception.

		The exception status is updated before returning. Fatal exceptions
		may be rethrown as nested exceptions, while warning-level exceptions
		are tracked through the warning management system.

		@param[in] lexception Exception type.
		@param[in] text Exception message.
		@param[in] method Method where the exception occurred.
		@param[in] line Source line where the exception occurred.
		@param[in] file Source file where the exception occurred.
		@param[in] lsection FMT section where the exception occurred.
		@param[in] throwit If true, the exception may be propagated.

		@return The generated exception object.
		*/
		virtual Exception::FMTException raise(
			Exception::FMTexc lexception,
			std::string text,
			const std::string& method,
			const int& line,
			const std::string& file,
			Core::FMTsection lsection,
			bool throwit) override;

		// DocString: WarningExceptionHandler()
		/**
		@brief Default constructor.
		*/
		WarningExceptionHandler() = default;

		// DocString: ~WarningExceptionHandler()
		/**
		@brief Default destructor.
		*/
		~WarningExceptionHandler() = default;

		// DocString: WarningExceptionHandler::clone
		/**
		@brief Creates a copy of this exception handler.

		@return A unique pointer to the cloned exception handler.
		*/
		std::unique_ptr<Exception::FMTExceptionHandler> clone() const;

		// DocString: WarningExceptionHandler::ResetThread
		/**
		@brief Resets crash tracking information to the main thread.

		This method updates the internal crash tracking state so that
		the main thread becomes the active reference thread. It is
		typically used when rebuilding exception handling resources
		after crash recovery.
		*/
		void ResetThread();
	};
}