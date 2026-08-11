#ifndef FMTExceptionFactory_included
#define FMTExceptionFactory_included

#include "FMTException.h"
#include <boost/graph/exception.hpp>
#include <memory>
#include <exception>

#if defined FMTWITHOSI
	class CoinError;
#endif

namespace Exception
{
	#if defined _MSC_VER
		class FMTSeException;
	#endif
	class FMTExceptionFactory
	{
	public:
		static std::unique_ptr<FMTException> create(
			FMTexc p_exception,
			Core::FMTsection p_section,
			const std::string& p_message,
			const std::string& p_method,
			const std::string& p_file,
			int p_line);
	#if defined FMTWITHOSI
		static std::unique_ptr<FMTException> createCoinException(const CoinError& p_CoinException);
	#endif
		static std::unique_ptr<FMTException> createBoostGraphException(const boost::bad_graph& p_exception);
	#if defined _MSC_VER
		static std::unique_ptr<FMTException> createSeException(const FMTSeException& p_SeException);
	#endif
		static std::unique_ptr<FMTException> createUnhandledException(const std::exception& p_exception);
	};
}

#endif