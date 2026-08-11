#include "FMTRangeError.h"

namespace Exception
	{
	FMTRangeError::FMTRangeError(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTrangeerror,
				FMTlev::FMT_range,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une valeur est Ã  l'extérieur de la plage permise.",
				"A value is outside the permitted range.")
		{
		}
	}