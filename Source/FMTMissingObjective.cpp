#include "FMTMissingObjective.h"

namespace Exception
	{
	FMTMissingObjective::FMTMissingObjective(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTmissingobjective,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Aucun objectif d'optimisation valide n'a été défini.",
				"No valid optimization objective has been defined.")
		{
		}
	}