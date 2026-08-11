#include "FMTWrongPartial.h"

namespace Exception
	{
	FMTWrongPartial::FMTWrongPartial(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTwrong_partial,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"L'utilisation de PARTIAL est invalide dans le contexte courant.",
				"The use of PARTIAL is invalid in the current context.")
		{
		}
	}