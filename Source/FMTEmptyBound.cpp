#include "FMTEmptyBound.h"

namespace Exception
	{
	FMTEmptyBound::FMTEmptyBound(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTemptybound,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Une borne a été définie sans valeur exploitable.",
				"A bound was defined without a usable value.")
		{
		}
	}