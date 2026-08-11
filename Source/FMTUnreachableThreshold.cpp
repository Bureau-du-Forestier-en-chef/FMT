#include "FMTUnreachableThreshold.h"

namespace Exception
	{
	FMTUnreachableThreshold::FMTUnreachableThreshold(
		Core::FMTsection p_section,
		const std::string& p_message,
		const std::string& p_method,
		const std::string& p_file,
		int p_line) :
			FMTWarning(
				FMTexc::FMTunreachable_threshold,
				p_section,
				p_message,
				p_method,
				p_file,
				p_line,
				"Le seuil demandé est inatteignable avec les paramètres actuels.",
				"The requested threshold is unreachable with the current parameters.")
		{
		}
	}