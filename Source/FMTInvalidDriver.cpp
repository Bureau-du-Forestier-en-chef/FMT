#include "FMTInvalidDriver.h"

namespace Exception
	{
	FMTInvalidDriver::FMTInvalidDriver(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvaliddriver,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le pilote requis est invalide ou indisponible.",
				"The required driver is invalid or unavailable.")
		{
		}
	}