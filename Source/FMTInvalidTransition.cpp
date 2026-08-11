#include "FMTInvalidTransition.h"

namespace Exception
	{
	FMTInvalidTransition::FMTInvalidTransition(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalid_transition,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une transition forestière est invalide ou incohérente.",
				"A forest transition is invalid or inconsistent.")
		{
		}
	}