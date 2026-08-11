#include "FMTInvalidBand.h"

namespace Exception
	{
	FMTInvalidBand::FMTInvalidBand(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalidband,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La bande raster demandée est invalide ou absente.",
				"The requested raster band is invalid or missing.")
		{
		}
	}