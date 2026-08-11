#include "FMTMissingRasterAttribute.h"

namespace Exception
	{
	FMTMissingRasterAttribute::FMTMissingRasterAttribute(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTmissingrasterattribute,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Des attributs raster attendus sont absents.",
				"Expected raster attributes are missing.")
		{
		}
	}