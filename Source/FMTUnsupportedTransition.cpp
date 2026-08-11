#include "FMTUnsupportedTransition.h"

namespace Exception
	{
	FMTUnsupportedTransition::FMTUnsupportedTransition(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTunsupported_transition,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une transition utilise une fonctionnalité non supportée.",
				"A transition uses unsupported functionality.")
		{
		}
	}