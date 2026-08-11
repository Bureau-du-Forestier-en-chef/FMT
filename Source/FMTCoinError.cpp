#include "FMTCoinError.h"
#if defined FMTWITHOSI
	#include <CoinError.hpp>
#endif

namespace Exception
	{
	FMTCoinError::FMTCoinError(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTcoinerror,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La bibliothèque COIN-OR a signalé une erreur.",
				"The COIN-OR library reported an error.")
		{
		}


	#if defined FMTWITHOSI
		FMTCoinError::FMTCoinError(const CoinError& p_coinException) :
			FMTCoinError(Core::FMTsection::Empty,
				"Coin Function failed: " + p_coinException.message(), p_coinException.className() + "::" + p_coinException.methodName(),
				p_coinException.fileName(), p_coinException.lineNumber())
			{

			}
	#endif



	}