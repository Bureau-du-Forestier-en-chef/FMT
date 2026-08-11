#include "FMTGDALWarning.h"

namespace Exception
	{
	FMTGDALWarning::FMTGDALWarning(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTGDALwarning,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"GDAL a signalé un avertissement pendant le traitement spatial.",
				"GDAL reported a warning during spatial processing.")
		{
		}
	}