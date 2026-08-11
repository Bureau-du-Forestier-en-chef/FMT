#include "FMTMSKError.h"

namespace Exception
	{
	FMTMSKError::FMTMSKError(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTmskerror,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le solveur MOSEK a signalé une erreur.",
				"The MOSEK solver reported an error.")
		{
		}
	}