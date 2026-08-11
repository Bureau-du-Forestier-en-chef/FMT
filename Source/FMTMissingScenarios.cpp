#include "FMTMissingScenarios.h"

namespace Exception
	{
	FMTMissingScenarios::FMTMissingScenarios(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTmissing_scenarios,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un ou plusieurs scénarios requis sont absents.",
				"One or more required scenarios are missing.")
		{
		}
	}