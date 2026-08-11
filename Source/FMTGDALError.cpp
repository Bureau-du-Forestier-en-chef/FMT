#include "FMTGDALError.h"

namespace Exception
	{
	FMTGDALError::FMTGDALError(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTGDALerror,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"GDAL a signalé une erreur pendant le traitement spatial.",
				"GDAL reported an error during spatial processing.")
		{
		}
	}