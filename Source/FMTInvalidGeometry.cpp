#include "FMTInvalidGeometry.h"

namespace Exception
	{
	FMTInvalidGeometry::FMTInvalidGeometry(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_geometry,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une géométrie spatiale est invalide ou corrompue.",
				"A spatial geometry is invalid or corrupted.")
		{
		}
	}