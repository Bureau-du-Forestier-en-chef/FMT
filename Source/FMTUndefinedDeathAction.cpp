#include "FMTUndefinedDeathAction.h"

namespace Exception
	{
	FMTUndefinedDeathAction::FMTUndefinedDeathAction(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTundefineddeathaction,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"L'action spéciale _death n'est pas définie.",
				"The special _death action is not defined.")
		{
		}
	}