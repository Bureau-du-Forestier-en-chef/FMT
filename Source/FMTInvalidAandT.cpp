#include "FMTInvalidAandT.h"

namespace Exception
	{
	FMTInvalidAandT::FMTInvalidAandT(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTError(
				FMTexc::FMTinvalidAandT,
				FMTlev::FMT_logic,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"La relation entre les actions et les transitions est invalide.",
				"The relationship between actions and transitions is invalid.")
		{
		}
	}