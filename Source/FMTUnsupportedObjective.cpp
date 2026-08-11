#include "FMTUnsupportedObjective.h"

namespace Exception
	{
	FMTUnsupportedObjective::FMTUnsupportedObjective(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTunsupported_objective,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"L'objectif d'optimisation demandé n'est pas supporté.",
				"The requested optimization objective is not supported.")
		{
		}
	}