#ifndef FMTExceptionFactory_included
#define FMTExceptionFactory_included
#include "FMTException.h"
#include <boost/graph/exception.hpp>
#include <memory>
#include <exception>

#if defined FMTWITHOSI

class CoinError;

#endif


/// Namespace for exceptions and exception handling utilities used by FMT.
namespace Exception
{
#if defined _MSC_VER
	class FMTSeException;
#endif

	// DocString: FMTExceptionFactory
	/**
	@brief Factory class used to create FMT exception objects.

	This factory centralizes the creation of all FMTException-derived
	objects. It provides methods for constructing exceptions from
	FMT exception identifiers as well as wrappers for exceptions
	thrown by external libraries such as Boost Graph, COIN-OR and
	platform-specific system exceptions.
	*/
	class FMTExceptionFactory
	{
	public:

		// DocString: FMTExceptionFactory::create
		/**
		@brief Creates a concrete FMTException instance corresponding to
		the specified FMT exception type.

		The returned object is an instance of the FMTException-derived
		class associated with the supplied exception identifier.

		@param[in] p_exception Type of exception to create.
		@param[in] p_section FMT section in which the exception occurred.
		@param[in] p_message Exception message.
		@param[in] p_method Method in which the exception occurred.
		@param[in] p_file Source file in which the exception occurred.
		@param[in] p_line Source file line at which the exception occurred.

		@return A unique pointer to the created exception object.
		*/
		static std::unique_ptr<FMTException> create(
			FMTexc p_exception,
			Core::FMTsection p_section,
			const std::string& p_message,
			const std::string& p_method,
			const std::string& p_file,
			int p_line);

#if defined FMTWITHOSI

		// DocString: FMTExceptionFactory::createCoinException
		/**
		@brief Creates an FMTCoinError exception from a COIN-OR exception.

		@param[in] p_CoinException The COIN-OR exception to wrap.

		@return A unique pointer to the created FMTCoinError exception.
		*/
		static std::unique_ptr<FMTException> createCoinException(
			const CoinError& p_CoinException);

#endif

		// DocString: FMTExceptionFactory::createBoostGraphException
		/**
		@brief Creates an FMTBoostGraphError exception from a Boost Graph exception.

		@param[in] p_exception The Boost Graph exception to wrap.

		@return A unique pointer to the created FMTBoostGraphError exception.
		*/
		static std::unique_ptr<FMTException> createBoostGraphException(
			const boost::bad_graph& p_exception);

#if defined _MSC_VER

		// DocString: FMTExceptionFactory::createSeException
		/**
		@brief Creates an FMTWIN32Error exception from a structured exception.

		@param[in] p_SeException The structured exception to wrap.

		@return A unique pointer to the created FMTWIN32Error exception.
		*/
		static std::unique_ptr<FMTException> createSeException(
			const FMTSeException& p_SeException);

#endif

		// DocString: FMTExceptionFactory::createUnhandledException
		/**
		@brief Creates an FMTUnhandledError exception from a standard exception.

		This method is typically used to convert exceptions originating
		outside of the FMT exception hierarchy into a managed FMT exception.

		@param[in] p_exception The standard exception to wrap.

		@return A unique pointer to the created FMTUnhandledError exception.
		*/
		static std::unique_ptr<FMTException> createUnhandledException(
			const std::exception& p_exception);
	};
}

#endif