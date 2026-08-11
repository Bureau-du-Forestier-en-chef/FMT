#include "FMTInvalidMaskrange.h"

namespace Exception
	{
	FMTInvalidMaskrange::FMTInvalidMaskrange(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_maskrange,
				FMTlev::FMT_range,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La plage de valeurs utilisée dans un masque est invalide ou impossible Ã  interpréter.",
				"The value range used in a mask is invalid or cannot be interpreted.")
		{
		}
	}