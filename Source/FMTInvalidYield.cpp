#include "FMTInvalidYield.h"

namespace Exception
	{
	FMTInvalidYield::FMTInvalidYield(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_yield,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La définition du rendement forestier est invalide ou impossible Ã  interpréter.",
				"The forest yield definition is invalid or cannot be interpreted.")
		{
		}
	}