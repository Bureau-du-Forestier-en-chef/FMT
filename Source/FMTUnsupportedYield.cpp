#include "FMTUnsupportedYield.h"

namespace Exception
	{
	FMTUnsupportedYield::FMTUnsupportedYield(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTunsupported_yield,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le type de rendement demandé n'est pas supporté.",
				"The requested yield type is not supported.")
		{
		}
	}