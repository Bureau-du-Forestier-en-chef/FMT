#include "FMTInvalidNumber.h"

namespace Exception
	{
	FMTInvalidNumber::FMTInvalidNumber(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_number,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une valeur numérique est invalide ou hors format.",
				"A numeric value is invalid or improperly formatted.")
		{
		}
	}