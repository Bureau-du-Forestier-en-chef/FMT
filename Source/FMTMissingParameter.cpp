#include "FMTMissingParameter.h"

namespace Exception
	{
	FMTMissingParameter::FMTMissingParameter(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTmissing_parameter,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un paramètre requis est absent ou invalide.",
				"A required parameter is missing or invalid.")
		{
		}
	}