#include "FMTInvalidTransitionCase.h"

namespace Exception
	{
	FMTInvalidTransitionCase::FMTInvalidTransitionCase(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_transition_case,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Un cas de transition contient une définition invalide ou ambiguÃ«.",
				"A transition case contains an invalid or ambiguous definition.")
		{
		}
	}