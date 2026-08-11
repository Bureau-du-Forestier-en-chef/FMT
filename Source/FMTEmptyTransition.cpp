#include "FMTEmptyTransition.h"

namespace Exception
	{
	FMTEmptyTransition::FMTEmptyTransition(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTempty_transition,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une transition est déclarée sans règle applicable.",
				"A transition is declared without any applicable rule.")
		{
		}
	}