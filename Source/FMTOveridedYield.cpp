#include "FMTOveridedYield.h"

namespace Exception
	{
	FMTOveridedYield::FMTOveridedYield(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMToveridedyield,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un rendement existant a été remplacé par une nouvelle définition.",
				"An existing yield was replaced by a newer definition.")
		{
		}
	}