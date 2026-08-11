#include "FMTUndefinedDeathTransition.h"

namespace Exception
	{
	FMTUndefinedDeathTransition::FMTUndefinedDeathTransition(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTundefineddeathtransition,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La transition spéciale _death n'est pas définie.",
				"The special _death transition is not defined.")
		{
		}
	}