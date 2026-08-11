#include "FMTInvalidOverview.h"

namespace Exception
	{
	FMTInvalidOverview::FMTInvalidOverview(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalidoverview,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La vue d'ensemble raster est invalide ou corrompue.",
				"The raster overview is invalid or corrupted.")
		{
		}
	}